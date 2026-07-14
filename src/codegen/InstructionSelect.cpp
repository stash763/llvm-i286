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
        case ir::Opcode::Switch:   return lowerSwitch(*impl, irInst, resultReg);
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

    // Lower each non-terminator instruction
    for (const auto& inst : bb.instructions) {
        auto instLowered = lowerInstruction(*inst);
        lowered.insert(lowered.end(), instLowered.begin(), instLowered.end());
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

    return lowered;
}

} // namespace codegen
} // namespace llvm_i286
