// Function Analysis Implementation
// Pre-pass for stack frame layout

#include "codegen/FunctionAnalysis.h"

#include <iostream>
#include <algorithm>

namespace llvm_i286 {
namespace codegen {

void FunctionAnalysis::analyze(const ir::Function& func, StackFrame& frame,
                                const std::map<std::string, std::vector<int>>& funcParamBitWidths) {
    // Step 1: Analyze parameters
    analyzeParams(func, frame, funcParamBitWidths);

    // Step 2: Analyze basic blocks
    std::vector<BlockInfo> blockInfos;
    analyzeBlocks(func, frame, blockInfos);

    // Step 3: Compute cross-block liveness
    computeLiveness(blockInfos, frame);

    // Step 3.5: Ensure all PHI results get spill slots (needed for PHI elimination)
    for (const auto& bb : func.basicBlocks) {
        for (const auto& inst : bb->instructions) {
            if (inst->opcode == ir::Opcode::Phi && !inst->resultName.empty()) {
                const std::string& phiResult = inst->resultName;
                if (!frame.hasSlot(phiResult) && !frame.isAlloca(phiResult)) {
                    bool is32 = false;
                    if (inst->resultType) {
                        is32 = (inst->resultType->bitWidth == 32);
                    } else {
                        is32 = true; // conservative default
                    }
                    int byteSize = is32 ? 4 : 2;
                    frame.addSpillSlot(phiResult, byteSize, is32);
                }
            }
        }
    }

    // Step 4: Compute max temp space
    computeMaxTempSpace(blockInfos, frame);
}

void FunctionAnalysis::analyzeParams(const ir::Function& func, StackFrame& frame,
                                      const std::map<std::string, std::vector<int>>& funcParamBitWidths) {
    int paramIndex = 0;

    // Check if we have param bit widths from codegen
    auto declIt = funcParamBitWidths.find(func.name);
    bool hasDecl = (declIt != funcParamBitWidths.end());

    for (const auto& param : func.params) {
        if (param->name.empty()) {
            paramIndex++;
            continue;
        }

        // Determine bit width
        int bitWidth = 16;  // default
        if (hasDecl) {
            const std::vector<int>& widths = declIt->second;
            if (paramIndex < static_cast<int>(widths.size())) {
                bitWidth = widths[paramIndex];
            }
        } else if (param->type) {
            if (param->type->isInteger()) {
                bitWidth = param->type->bitWidth;
            } else if (param->type->isPointer()) {
                bitWidth = 32;  // 32-bit pointer on i286
            }
        }

        // Determine register assignment
        // First 3 16-bit params get BX, SI, DI
        std::string reg;
        if (paramIndex < 3 && bitWidth == 16) {
            const char* regs[] = {"bx", "si", "di"};
            reg = regs[paramIndex];
        }

        // Add to frame
        frame.addParam(param->name, bitWidth, reg);

        paramIndex++;
    }
}

void FunctionAnalysis::analyzeBlocks(const ir::Function& func, StackFrame& frame,
                                      std::vector<BlockInfo>& blockInfos) {
    for (const auto& bb : func.basicBlocks) {
        BlockInfo info;
        info.label = bb->label;

        // Collect defined vregs (result names of non-terminator instructions)
        for (const auto& inst : bb->instructions) {
            if (!inst->resultName.empty()) {
                info.defined.insert(inst->resultName);
            }
        }

        // Collect used vregs (operand names of non-terminator instructions)
        for (const auto& inst : bb->instructions) {
            for (const auto& op : inst->operands) {
                if (!op.name.empty() && op.name[0] == '%') {
                    info.used.insert(op.name);
                }
            }

            // For call instructions, check callArgs
            for (const auto& arg : inst->callArgs) {
                if (!arg.empty() && arg[0] == '%') {
                    info.used.insert(arg);
                }
            }
        }

        // Collect used vregs from terminator operands
        if (bb->terminator) {
            for (const auto& op : bb->terminator->operands) {
                if (!op.name.empty() && op.name[0] == '%') {
                    info.used.insert(op.name);
                }
            }
        }

        // Estimate temp space needed for this block
        for (const auto& inst : bb->instructions) {
            bool is32 = false;
            if (inst->resultType) {
                is32 = (inst->resultType->bitWidth == 32);
            }

            if (inst->opcode == ir::Opcode::Alloca) {
                // Detect alloca and create a slot for it
                int bitWidth = 32;
                if (!inst->operands.empty()) {
                    try {
                        bitWidth = std::stoi(inst->operands[0].name);
                    } catch (...) {
                        // Try to extract from type string like "i32"
                        const std::string& opName = inst->operands[0].name;
                        if (opName.size() > 1 && opName[0] == 'i') {
                            try {
                                bitWidth = std::stoi(opName.substr(1));
                            } catch (...) {}
                        }
                        // Also handle array types like [16 x i8]
                        // For now, default to 32 for unrecognized types
                    }
                }
                int byteSize = bitWidth / 8;
                if (byteSize < 2) byteSize = 2;

                // Create a dedicated alloca slot (persists across blocks)
                frame.addAllocaSlot(inst->resultName, byteSize);
            } else {
                // Any instruction with a non-empty result needs temp storage
                // 32-bit results need 4 bytes, others need 2 bytes
                // This covers Load, Call, Add, Sub, Mul, Div, bitwise ops,
                // conversions, GEP, ICmp, Select, Phi, etc.
                if (!inst->resultName.empty()) {
                    if (is32) {
                        info.tempSpaceNeeded += 4;
                    } else if (inst->resultType) {
                        // Known 16-bit (or smaller) result
                        info.tempSpaceNeeded += 2;
                    } else {
                        // Unknown result type - be conservative, assume 32-bit
                        info.tempSpaceNeeded += 4;
                    }
                }
                // Load through register/stack pointer needs extra 4 bytes for pointer save
                if (inst->opcode == ir::Opcode::Load && !inst->resultName.empty()) {
                    info.tempSpaceNeeded += 4;
                }
                // GEP needs extra temp space for intermediate pointer computations
                if (inst->opcode == ir::Opcode::GetElementPtr && !inst->resultName.empty()) {
                    info.tempSpaceNeeded += 4;
                }
                // Call may need extra temp space for argument preparation
                if (inst->opcode == ir::Opcode::Call && !inst->resultName.empty()) {
                    info.tempSpaceNeeded += 4;
                }
            }
        }

        blockInfos.push_back(std::move(info));
    }

    // Compute max temp space across all blocks
    computeMaxTempSpace(blockInfos, frame);
}

void FunctionAnalysis::computeLiveness(const std::vector<BlockInfo>& blockInfos,
                                        StackFrame& frame) {
    // Build a map of vreg -> defining block index
    std::map<std::string, int> vregToDefBlock;
    for (size_t i = 0; i < blockInfos.size(); i++) {
        for (const auto& vreg : blockInfos[i].defined) {
            vregToDefBlock[vreg] = static_cast<int>(i);
        }
    }

    // Find cross-block vregs
    // A vreg is cross-block if it's used in a block != its defining block
    std::set<std::string> crossBlockVregs;
    for (size_t i = 0; i < blockInfos.size(); i++) {
        for (const auto& vreg : blockInfos[i].used) {
            auto defIt = vregToDefBlock.find(vreg);
            if (defIt != vregToDefBlock.end()) {
                if (defIt->second != static_cast<int>(i)) {
                    // This vreg is used in a block different from where it's defined
                    crossBlockVregs.insert(vreg);
                }
            }
        }
    }

    // Add spill slots for cross-block vregs
    for (const auto& vreg : crossBlockVregs) {
        if (frame.isAlloca(vreg)) {
            continue;
        }

        bool is32 = false;
        if (frame.hasSlot(vreg)) {
            is32 = frame.is32bit(vreg);
        } else {
            is32 = true;
        }

        int byteSize = is32 ? 4 : 2;
        frame.addSpillSlot(vreg, byteSize, is32);
    }
}

void FunctionAnalysis::computeMaxTempSpace(const std::vector<BlockInfo>& blockInfos,
                                            StackFrame& frame) {
    int maxTemp = 0;
    for (const auto& info : blockInfos) {
        if (info.tempSpaceNeeded > maxTemp) {
            maxTemp = info.tempSpaceNeeded;
        }
    }

    // Add safety margin to account for any uncounted temp allocations
    // (e.g., pointer saves, intermediate computations, argument prep)
    maxTemp += 16;

    frame.setMaxTempSpace(maxTemp);
}

} // namespace codegen
} // namespace llvm_i286
