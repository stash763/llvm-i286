// Instruction Selection Implementation
// Lowering LLVM IR instructions to 80286 instructions
//
// NOTE: This file now uses a two-pass approach:
// Pass 1: FunctionAnalysis walks the IR to populate StackFrame
// Pass 2: Instruction handlers query StackFrame for offsets

#include "codegen/InstructionSelect.h"
#include "codegen/InstructionSelectInternal.h"

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <cstdint>

namespace llvm_i286 {
namespace codegen {

struct InstructionSelector::Impl : SelectorState {};

InstructionSelector::InstructionSelector() : impl(std::make_unique<Impl>()) {}

InstructionSelector::~InstructionSelector() = default;

void InstructionSelector::setFunctionDeclarations(const std::map<std::string, std::vector<int>>& decls) {
    impl->funcParamBitWidths = decls;
}

void InstructionSelector::setAliasMap(const std::map<std::string, std::string>& aliases) {
    impl->aliasMap = aliases;
}

void InstructionSelector::setTypeDefinitions(const std::map<std::string, std::unique_ptr<ir::Type>>* typeDefs) {
    impl->typeDefinitions = typeDefs;
}

int InstructionSelector::getFrameSize() const {
    return impl->frame.getFrameSize();
}

// ============================================================================
// Instruction dispatcher
// ============================================================================

std::vector<LoweredInstruction> InstructionSelector::lowerInstruction(const ir::Instruction& irInst) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Get result register if this instruction produces a value
    std::string resultReg;
    if (!irInst.resultName.empty()) {
        resultReg = impl->frame.getPhysReg(irInst.resultName);
    }

    switch (irInst.opcode) {
        // Arithmetic
        case ir::Opcode::Add:      return lowerAdd(*impl, irInst, resultReg);
        case ir::Opcode::Sub:      return lowerSub(*impl, irInst, resultReg);
        case ir::Opcode::Mul:      return lowerMul(*impl, irInst, resultReg);
        case ir::Opcode::And:      return lowerAnd(*impl, irInst, resultReg);
        case ir::Opcode::Or:       return lowerOr(*impl, irInst, resultReg);
        case ir::Opcode::Xor:      return lowerXor(*impl, irInst, resultReg);
        case ir::Opcode::Shl:      return lowerShl(*impl, irInst, resultReg);
        case ir::Opcode::LShr:     return lowerLShr(*impl, irInst, resultReg);
        case ir::Opcode::AShr:     return lowerAShr(*impl, irInst, resultReg);
        case ir::Opcode::SDiv:
        case ir::Opcode::UDiv:
        case ir::Opcode::SRem:
        case ir::Opcode::URem:     return lowerDivRem(*impl, irInst, resultReg);

        // Memory
        case ir::Opcode::Alloca:   return lowerAlloca(*impl, irInst, resultReg);
        case ir::Opcode::Load:     return lowerLoad(*impl, irInst, resultReg);
        case ir::Opcode::Store:    return lowerStore(*impl, irInst, resultReg);
        case ir::Opcode::GetElementPtr: return lowerGetElementPtr(*impl, irInst, resultReg);

        // Control flow
        case ir::Opcode::RetTerm:  return lowerRetTerm(*impl, irInst, resultReg);
        case ir::Opcode::BrTerm:   return lowerBrTerm(*impl, irInst, resultReg);
        case ir::Opcode::CondBrTerm: return lowerCondBrTerm(*impl, irInst, resultReg);
        case ir::Opcode::Switch:
        case ir::Opcode::SwitchTerm:   return lowerSwitch(*impl, irInst, resultReg);
        case ir::Opcode::Phi:      return lowerPhi(*impl, irInst, resultReg);
        case ir::Opcode::Unreachable: return lowerUnreachable(*impl, irInst, resultReg);
        case ir::Opcode::Select:   return lowerSelect(*impl, irInst, resultReg);
        case ir::Opcode::ICmp:     return lowerICmp(*impl, irInst, resultReg);

        // Call
        case ir::Opcode::Call:     return lowerCall(*impl, irInst, resultReg);

        // Conversions
        case ir::Opcode::Trunc:    return lowerTrunc(*impl, irInst, resultReg);
        case ir::Opcode::ZExt:     return lowerZExt(*impl, irInst, resultReg);
        case ir::Opcode::SExt:     return lowerSExt(*impl, irInst, resultReg);
        case ir::Opcode::BitCast:
        case ir::Opcode::IntToPtr: return lowerBitCast(*impl, irInst, resultReg);
        case ir::Opcode::PtrToInt: return lowerPtrToInt(*impl, irInst, resultReg);
        case ir::Opcode::Freeze:   return lowerFreeze(*impl, irInst, resultReg);
        case ir::Opcode::ExtractValue: return lowerExtractValue(*impl, irInst, resultReg);
        case ir::Opcode::InsertValue: return lowerInsertValue(*impl, irInst, resultReg);

        // FPU
        case ir::Opcode::FAdd:
        case ir::Opcode::FSub:
        case ir::Opcode::FMul:
        case ir::Opcode::FDiv:     return lowerFpuArith(*impl, irInst, resultReg);
        case ir::Opcode::FRem:     return lowerFRem(*impl, irInst, resultReg);
        case ir::Opcode::FCmp:     return lowerFCmp(*impl, irInst, resultReg);
        case ir::Opcode::FPTrunc:
        case ir::Opcode::FPExt:    return lowerFpuConvert(*impl, irInst, resultReg);
        case ir::Opcode::FPToUI:
        case ir::Opcode::FPToSI:   return lowerFPToInt(*impl, irInst, resultReg);
        case ir::Opcode::UIToFP:
        case ir::Opcode::SIToFP:   return lowerIntToFP(*impl, irInst, resultReg);

        default: {
            Instruction286 nopInst;
            nopInst.mnemonic = "nop";
            nopInst.comment = "TODO: Implement " + std::to_string(static_cast<int>(irInst.opcode));
            lowered.instructions.push_back(nopInst);
            break;
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

// ============================================================================
// Basic block lowering
// ============================================================================

std::vector<LoweredInstruction> InstructionSelector::lowerBasicBlock(const ir::BasicBlock& bb) {
    std::vector<LoweredInstruction> lowered;

    // Emit basic block label if present
    if (!bb.label.empty()) {
        LoweredInstruction labelInst;
        std::string funcPrefix = "";
        if (impl->currentFunc && !impl->currentFunc->name.empty()) {
            funcPrefix = impl->currentFunc->name + "_";
        }
        labelInst.label = funcPrefix + "bb_" + bb.label;
        lowered.push_back(labelInst);
    }

    // PHI elimination: at the start of this block, load each PHI result
    // from its spill slot into ax:dx (or ax for 16-bit)
    for (const auto& inst : bb.instructions) {
        if (inst->opcode == ir::Opcode::Phi && !inst->resultName.empty()) {
            const std::string& phiResult = inst->resultName;
            bool is32 = impl->frame.is32bit(phiResult);

            LoweredInstruction phiLoad;
            Instruction286 loadLow, loadHigh;

            if (impl->frame.hasSlot(phiResult)) {
                std::string offset = impl->frame.getBpOffset(phiResult);
                loadLow.mnemonic = "mov";
                loadLow.operands = {"ax", "[" + offset + "]"};
                loadLow.comment = "phi load " + phiResult;
                phiLoad.instructions.push_back(loadLow);

                if (is32) {
                    std::string highOffset = impl->frame.getHighBpOffset(phiResult);
                    loadHigh.mnemonic = "mov";
                    loadHigh.operands = {"dx", "[" + highOffset + "]"};
                    phiLoad.instructions.push_back(loadHigh);
                }
            }

            // Map PHI result to ax:dx so subsequent code uses registers
            impl->frame.setPhysReg(phiResult, "ax");

            lowered.push_back(phiLoad);
        }
    }

    // Lower each non-terminator instruction
    for (const auto& inst : bb.instructions) {
        auto instLowered = lowerInstruction(*inst);
        lowered.insert(lowered.end(), instLowered.begin(), instLowered.end());
    }

    // PHI elimination: at the end of this block (before terminator),
    // store incoming values to PHI spill slots in successor blocks
    {
        auto predIt = impl->phiStoresByPred.find(bb.label);
        if (predIt != impl->phiStoresByPred.end()) {
            for (const auto& phiInfo : predIt->second) {
                LoweredInstruction phiStore;
                const std::string& incomingValue = phiInfo.incomingValue;
                const std::string& phiResult = phiInfo.phiResult;
                bool is32 = phiInfo.is32bit;

                // Get the PHI result's spill slot
                if (!impl->frame.hasSlot(phiResult)) {
                    continue; // no spill slot for this PHI
                }
                std::string phiOffset = impl->frame.getBpOffset(phiResult);
                std::string phiHighOffset = impl->frame.getHighBpOffset(phiResult);

                // Load the incoming value into ax:dx (or ax for 16-bit)
                // The incoming value is a vreg that should have a slot
                if (incomingValue[0] == '%') {
                    // Vreg: load from its slot if it has one
                    if (impl->frame.hasSlot(incomingValue)) {
                        std::string srcOffset = impl->frame.getBpOffset(incomingValue);
                        Instruction286 loadLow;
                        loadLow.mnemonic = "mov";
                        loadLow.operands = {"ax", "[" + srcOffset + "]"};
                        loadLow.comment = "phi incoming " + incomingValue + " -> " + phiResult;
                        phiStore.instructions.push_back(loadLow);

                        if (is32) {
                            std::string srcHigh = impl->frame.getHighBpOffset(incomingValue);
                            Instruction286 loadHigh;
                            loadHigh.mnemonic = "mov";
                            loadHigh.operands = {"dx", "[" + srcHigh + "]"};
                            phiStore.instructions.push_back(loadHigh);
                        }
                    } else {
                        // Vreg has no slot - assume it's in ax:dx already
                        // No load needed, just add comment
                        Instruction286 comment;
                        comment.mnemonic = "nop";
                        comment.comment = "phi incoming " + incomingValue + " in reg -> " + phiResult;
                        phiStore.instructions.push_back(comment);
                    }
                } else if (incomingValue[0] == '@') {
                    // Global: load address via lea
                    std::string globalName = incomingValue.substr(1);
                    if (!globalName.empty() && globalName[0] == '.')
                        globalName = "_" + globalName.substr(1);

                    Instruction286 leaInst;
                    leaInst.mnemonic = "lea";
                    leaInst.operands = {"ax", "[" + globalName + "]"};
                    leaInst.comment = "phi incoming global " + incomingValue + " -> " + phiResult;
                    phiStore.instructions.push_back(leaInst);

                    if (is32) {
                        Instruction286 segInst;
                        segInst.mnemonic = "mov";
                        segInst.operands = {"dx", "ds"};
                        phiStore.instructions.push_back(segInst);
                    }
                } else {
                    // Constant: load directly
                    // Handle null as 0 (LLVM null constant)
                    std::string constVal = incomingValue;
                    if (constVal == "null") constVal = "0";
                    if (constVal == "false") constVal = "0";
                    if (constVal == "true") constVal = "1";
                    if (constVal == "undef") constVal = "0";
                    // Detect floating-point constants (e.g., "0.000000e+00", "0xK...")
                    // and replace with 0 — FP constants not supported in integer mov
                    bool isFPConst = constVal.find("0xK") != std::string::npos ||
                                     (constVal.find('e') != std::string::npos &&
                                      (constVal.find('+') != std::string::npos || constVal.find('-') != std::string::npos));
                    if (isFPConst) constVal = "0";
                    // Detect complex constant expressions (mul, ptrtoint, etc.)
                    if (constVal.find('(') != std::string::npos) constVal = "0";

                    Instruction286 loadConst;
                    loadConst.mnemonic = "mov";
                    loadConst.operands = {"ax", constVal};
                    loadConst.comment = "phi incoming const " + incomingValue + " -> " + phiResult;
                    phiStore.instructions.push_back(loadConst);

                    if (is32) {
                        Instruction286 zeroHigh;
                        zeroHigh.mnemonic = "xor";
                        zeroHigh.operands = {"dx", "dx"};
                        phiStore.instructions.push_back(zeroHigh);
                    }
                }

                // Store ax:dx to the PHI's spill slot
                Instruction286 storeLow;
                storeLow.mnemonic = "mov";
                storeLow.operands = {"[" + phiOffset + "]", "ax"};
                phiStore.instructions.push_back(storeLow);

                if (is32) {
                    Instruction286 storeHigh;
                    storeHigh.mnemonic = "mov";
                    storeHigh.operands = {"[" + phiHighOffset + "]", "dx"};
                    phiStore.instructions.push_back(storeHigh);
                }

                lowered.push_back(phiStore);
            }
        }
    }

    // NOTE: No tempSpaceInBlock cleanup here.
    // Temp space is pre-allocated in the frame and reset per block
    // in lowerFunction, not freed with add sp.

    // Lower terminator
    if (bb.terminator) {
        auto termLowered = lowerInstruction(*bb.terminator);
        lowered.insert(lowered.end(), termLowered.begin(), termLowered.end());
    }

    return lowered;
}

// ============================================================================
// Function lowering (two-pass approach)
// ============================================================================

std::vector<LoweredInstruction> InstructionSelector::lowerFunction(const ir::Function& func) {
    impl->currentFunc = &func;

    // Reset frame for new function
    impl->frame = StackFrame();

    // Pass 1: Analyze the function to populate the frame
    FunctionAnalysis::analyze(func, impl->frame, impl->funcParamBitWidths);

    // Pass 2: Compute frame layout (assign final offsets)
    impl->frame.computeLayout();

    // Build PHI store map for PHI elimination
    // For each PHI node, register the incoming values keyed by predecessor label
    impl->phiStoresByPred.clear();
    for (const auto& bb : func.basicBlocks) {
        for (const auto& inst : bb->instructions) {
            if (inst->opcode == ir::Opcode::Phi && !inst->resultName.empty()) {
                bool is32 = false;
                if (inst->resultType) {
                    is32 = (inst->resultType->bitWidth == 32);
                }
                for (const auto& phiInc : inst->phiIncrements) {
                    impl->phiStoresByPred[phiInc.label].push_back({
                        inst->resultName,
                        phiInc.value,
                        is32
                    });
                }
            }
        }
    }

    // Map unmatched PHI labels to the entry block
    // In LLVM IR, the entry block is unnamed but PHI nodes reference it
    // by its implicit number (e.g., %2 when params are %0, %1)
    std::set<std::string> blockLabels;
    for (const auto& bb : func.basicBlocks) {
        if (!bb->label.empty()) blockLabels.insert(bb->label);
    }
    std::string entryLabel;
    if (!func.basicBlocks.empty()) {
        entryLabel = func.basicBlocks[0]->label;
    }
    std::vector<std::string> unmatchedLabels;
    for (const auto& [label, _] : impl->phiStoresByPred) {
        if (!label.empty() && blockLabels.find(label) == blockLabels.end()) {
            unmatchedLabels.push_back(label);
        }
    }
    for (const auto& label : unmatchedLabels) {
        auto it = impl->phiStoresByPred.find(label);
        for (const auto& store : it->second) {
            impl->phiStoresByPred[entryLabel].push_back(store);
        }
    }

    std::vector<LoweredInstruction> lowered;

    // Emit function label
    LoweredInstruction funcLabel;
    funcLabel.label = func.name;
    lowered.push_back(funcLabel);

    // Pass 3: Lower each basic block
    for (const auto& bb : func.basicBlocks) {
        // Reset temp space allocator at start of each block
        // (temp area is pre-allocated, this just resets the offset counter)
        impl->frame.resetTemp();

        auto bbLowered = lowerBasicBlock(*bb);
        lowered.insert(lowered.end(), bbLowered.begin(), bbLowered.end());
    }

    // Emit pending trampolines (for indirect far calls) at end of function
    for (const auto& tramp : impl->pendingTrampolines) {
        lowered.push_back(tramp);
    }
    impl->pendingTrampolines.clear();

    return lowered;
}

} // namespace codegen
} // namespace llvm_i286
