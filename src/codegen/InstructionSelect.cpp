// Instruction Selection Implementation
// Lowering LLVM IR instructions to 80286 instructions

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

std::vector<LoweredInstruction> InstructionSelector::lowerInstruction(const ir::Instruction& irInst) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;
    
    // Get result register if this instruction produces a value
    std::string resultReg;
    if (!irInst.resultName.empty()) {
        // Check if we already have a mapping for this result
        resultReg = impl->getPhysReg(irInst.resultName);
        
        // If it's still "ax" and not a memory location, we might need to assign a different register
        // For now, just use AX for all results
        if (resultReg == "ax") {
            // Could allocate a different register here if needed
        }
    }
    
    switch (irInst.opcode) {
        case ir::Opcode::Add:
            return lowerAdd(*impl, irInst, resultReg);
        
        case ir::Opcode::Sub:
            return lowerSub(*impl, irInst, resultReg);
        
        case ir::Opcode::Mul:
            return lowerMul(*impl, irInst, resultReg);
        
        case ir::Opcode::And:
            return lowerAnd(*impl, irInst, resultReg);
        
        case ir::Opcode::Or:
            return lowerOr(*impl, irInst, resultReg);
        
        case ir::Opcode::Xor:
            return lowerXor(*impl, irInst, resultReg);
        
        case ir::Opcode::Shl:
            return lowerShl(*impl, irInst, resultReg);
        
        case ir::Opcode::LShr:
            return lowerLShr(*impl, irInst, resultReg);
        
        case ir::Opcode::AShr:
            return lowerAShr(*impl, irInst, resultReg);
        
        case ir::Opcode::RetTerm:
            return lowerRetTerm(*impl, irInst, resultReg);
        
        case ir::Opcode::BrTerm:
            return lowerBrTerm(*impl, irInst, resultReg);
        
        case ir::Opcode::Alloca:
            return lowerAlloca(*impl, irInst, resultReg);
        
        case ir::Opcode::Store:
            return lowerStore(*impl, irInst, resultReg);
        
        case ir::Opcode::Load:
            return lowerLoad(*impl, irInst, resultReg);
        
        case ir::Opcode::Call:
            return lowerCall(*impl, irInst, resultReg);
        
        case ir::Opcode::ICmp:
            return lowerICmp(*impl, irInst, resultReg);
        
        case ir::Opcode::CondBrTerm:
            return lowerCondBrTerm(*impl, irInst, resultReg);
        
         case ir::Opcode::Trunc:
            return lowerTrunc(*impl, irInst, resultReg);
        
        case ir::Opcode::ZExt:
            return lowerZExt(*impl, irInst, resultReg);
        
        case ir::Opcode::SExt:
            return lowerSExt(*impl, irInst, resultReg);
        
        case ir::Opcode::GetElementPtr:
            return lowerGetElementPtr(*impl, irInst, resultReg);
        
        case ir::Opcode::Select:
            return lowerSelect(*impl, irInst, resultReg);
        
        // ====================================================================
        // Division and Remainder Operations
        // ====================================================================
        
        case ir::Opcode::SDiv:
        case ir::Opcode::UDiv:
        case ir::Opcode::SRem:
        case ir::Opcode::URem:
            return lowerDivRem(*impl, irInst, resultReg);
        
        // ====================================================================
        // Floating Point Operations (80287 FPU)
        // ====================================================================
        
        case ir::Opcode::FAdd:
        case ir::Opcode::FSub:
        case ir::Opcode::FMul:
        case ir::Opcode::FDiv:
            return lowerFpuArith(*impl, irInst, resultReg);
        
        case ir::Opcode::FRem:
            return lowerFRem(*impl, irInst, resultReg);
        
        case ir::Opcode::FCmp:
            return lowerFCmp(*impl, irInst, resultReg);
        
        case ir::Opcode::FPTrunc:
        case ir::Opcode::FPExt:
            return lowerFpuConvert(*impl, irInst, resultReg);
        
        case ir::Opcode::FPToUI:
        case ir::Opcode::FPToSI:
            return lowerFPToInt(*impl, irInst, resultReg);
        
        case ir::Opcode::UIToFP:
        case ir::Opcode::SIToFP:
            return lowerIntToFP(*impl, irInst, resultReg);
        
        // ====================================================================
        // Control Flow and Other Operations
        // ====================================================================
        
        case ir::Opcode::Switch:
            return lowerSwitch(*impl, irInst, resultReg);
        
        case ir::Opcode::Phi:
            return lowerPhi(*impl, irInst, resultReg);
        
        case ir::Opcode::Unreachable:
            return lowerUnreachable(*impl, irInst, resultReg);
        
        case ir::Opcode::BitCast:
            return lowerBitCast(*impl, irInst, resultReg);
        
        case ir::Opcode::PtrToInt:
            return lowerPtrToInt(*impl, irInst, resultReg);
        
        case ir::Opcode::IntToPtr:
            return lowerIntToPtr(*impl, irInst, resultReg);
        
        case ir::Opcode::ExtractValue:
            return lowerExtractValue(*impl, irInst, resultReg);
        
        case ir::Opcode::InsertValue:
            return lowerInsertValue(*impl, irInst, resultReg);
        
        case ir::Opcode::Freeze:
            return lowerFreeze(*impl, irInst, resultReg);
        
        default: {
            // Unhandled instruction - emit NOP as placeholder
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

  std::vector<LoweredInstruction> InstructionSelector::lowerBasicBlock(const ir::BasicBlock& bb) {
    std::vector<LoweredInstruction> lowered;
    
    // Emit basic block label if present
    // Scope the label with a prefix from the current function name to avoid collisions across functions
    if (!bb.label.empty()) {
        LoweredInstruction labelInst;
        std::string funcPrefix = "";
        if (impl->currentFunc && !impl->currentFunc->name.empty()) {
            funcPrefix = impl->currentFunc->name + "_";
        }
        labelInst.label = funcPrefix + "bb_" + bb.label;
        lowered.push_back(labelInst);
    }
    
    // Lower each non-terminator instruction
    for (const auto& inst : bb.instructions) {
        auto instLowered = lowerInstruction(*inst);
        lowered.insert(lowered.end(), instLowered.begin(), instLowered.end());
    }
    
    // Before lowering terminator, capture allocaEndOffset for epilogue cleanup.
    // allocaEndOffset should represent only alloca space (no temp space).
    // currentStackOffset includes both alloca and temp space allocations;
    // add tempSpaceInBlock back to get the alloca-only offset.
    impl->allocaEndOffset = impl->currentStackOffset + impl->tempSpaceInBlock;
    
    // Free temp space allocated in this basic block BEFORE terminator
    if (impl->tempSpaceInBlock > 0) {
        // Adjust currentStackOffset to remove temp space from tracking
        impl->currentStackOffset += impl->tempSpaceInBlock;
        
        Instruction286 addSp;
        addSp.mnemonic = "add";
        addSp.operands.push_back("sp");
        addSp.operands.push_back(std::to_string(impl->tempSpaceInBlock));
        LoweredInstruction cleanup;
        cleanup.instructions.push_back(addSp);
        lowered.push_back(cleanup);
        impl->tempSpaceInBlock = 0;
    }
    
    // Lower terminator (uses allocaEndOffset for epilogue)
    if (bb.terminator) {
        auto termLowered = lowerInstruction(*bb.terminator);
        lowered.insert(lowered.end(), termLowered.begin(), termLowered.end());
    }
    
    return lowered;
}

  std::vector<LoweredInstruction> InstructionSelector::lowerFunction(const ir::Function& func) {
    impl->currentFunc = &func;
    impl->vregToPhys.clear();
    impl->physToVreg.clear();  // Also clear the reverse mapping
    impl->vregToStackOffset.clear();
    impl->allocaVregs.clear();
    impl->currentStackOffset = -6;  // Account for 3 saved regs: bx, si, di (2 bytes each)
    impl->tempSpaceInBlock = 0;
    // Note: labelCounter is NOT reset per function to ensure unique labels across functions
    
    std::vector<LoweredInstruction> lowered;
    
    // Add function label
    LoweredInstruction funcLabel;
    funcLabel.label = func.name;
    lowered.push_back(funcLabel);
    
    // Assign physical registers to parameters
    // For OS/2 1.x calling convention, params are on stack
    // [bp+4] = first param, [bp+6] = second param, etc.
    int paramIndex = 0;
    int stackOffset = 4; // Start at [bp+4]
    for (const auto& param : func.params) {
        // Check if this is a 32-bit parameter
        bool is32 = param->type && param->type->bitWidth == 32;
        int paramSize = is32 ? 4 : 2; // 4 bytes for 32-bit, 2 bytes for 16-bit
        
        std::string physReg;
        if (paramIndex < 3 && !is32) {
            // Use BX, SI, DI for first 3 parameters (only for 16-bit params)
            const char* regs[] = {"bx", "si", "di"};
            physReg = regs[paramIndex];
        } else {
            // Spill to stack
            physReg = "[bp+" + std::to_string(stackOffset) + "]";
        }
        
        if (param->name.empty()) {
            paramIndex++;
            stackOffset += paramSize;
            continue;
        }
        
        // Clear any old vreg in this physical register
        auto oldOcc = impl->physToVreg.find(physReg);
        if (oldOcc != impl->physToVreg.end()) {
            impl->vregToPhys.erase(oldOcc->second);
            impl->physToVreg.erase(oldOcc);
        }
        
        // For 32-bit parameters, we need to load both words to stack-based storage
        if (is32) {
            // Allocate stack space for this 32-bit parameter
            impl->currentStackOffset -= 4;
            int localOffset = impl->currentStackOffset;
            impl->vregToStackOffset[param->name] = localOffset;
            std::string stackReg = "bp" + std::to_string(localOffset);
            
            // Emit stack allocation instruction
            LoweredInstruction allocInstr;
            Instruction286 subSp;
            subSp.mnemonic = "sub";
            subSp.operands.push_back("sp");
            subSp.operands.push_back("4");
            allocInstr.instructions.push_back(subSp);
            lowered.push_back(allocInstr);
            
            // Create a lowered instruction block for parameter setup
            LoweredInstruction paramSetup;
            
            // physReg may already have brackets if it's a stack location
            std::string paramLoc = physReg;
            if (paramLoc.find("[") == std::string::npos) {
                paramLoc = "[" + paramLoc + "]";
            }
            
            // Load low word from [bp+stackOffset] to stack
            Instruction286 loadLow;
            loadLow.mnemonic = "mov";
            loadLow.operands.push_back("ax");
            loadLow.operands.push_back(paramLoc);
            paramSetup.instructions.push_back(loadLow);
            
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + stackReg + "]");
            storeLow.operands.push_back("ax");
            paramSetup.instructions.push_back(storeLow);
            
            // Load high word from [bp+stackOffset+2] to stack
            Instruction286 loadHigh;
            loadHigh.mnemonic = "mov";
            loadHigh.operands.push_back("dx");
            // Extract offset and add 2
            if (paramLoc.find("bp") != std::string::npos) {
                int offset = 0;
                std::string offsetStr = paramLoc.substr(paramLoc.find("bp")+2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else {
                loadHigh.operands.push_back("[" + paramLoc + "+2]");
            }
            paramSetup.instructions.push_back(loadHigh);
            
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            // Extract offset and add 2
            if (stackReg.find("bp") != std::string::npos) {
                int offset = 0;
                std::string offsetStr = stackReg.substr(2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                storeHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else {
                storeHigh.operands.push_back("[" + stackReg + "+2]");
            }
            storeHigh.operands.push_back("dx");
            paramSetup.instructions.push_back(storeHigh);
            
            lowered.push_back(paramSetup);
            
            // Point vreg to stack location
            impl->vregToPhys[param->name] = stackReg;
            impl->physToVreg[stackReg] = param->name;
            impl->mark32Bit(param->name);
        } else {
            // For 16-bit parameters, load from stack to register
            LoweredInstruction loadInstr;
            Instruction286 loadReg;
            loadReg.mnemonic = "mov";
            loadReg.operands.push_back(physReg);
            std::string paramLoc = "[bp+" + std::to_string(stackOffset) + "]";
            loadReg.operands.push_back(paramLoc);
            loadInstr.instructions.push_back(loadReg);
            lowered.push_back(loadInstr);
            
            impl->vregToPhys[param->name] = physReg;
            impl->physToVreg[physReg] = param->name;
        }
        
        paramIndex++;
        stackOffset += paramSize;
    }
    
    // Lower each basic block
    for (const auto& bb : func.basicBlocks) {
        auto bbLowered = lowerBasicBlock(*bb);
        lowered.insert(lowered.end(), bbLowered.begin(), bbLowered.end());
        // allocaEndOffset is now captured in lowerBasicBlock before the terminator
    }
    
    return lowered;
}

} // namespace codegen
} // namespace llvm_i286
