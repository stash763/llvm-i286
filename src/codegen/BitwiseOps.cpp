// Bitwise Operations: And, Or, Xor, Shl, LShr, AShr
// Extracted from InstructionSelect.cpp

#include "codegen/InstructionSelectInternal.h"

#include <string>

namespace llvm_i286 {
namespace codegen {

// Shared helper for And, Or, Xor: resultReg = op1 <op> op2
static std::vector<LoweredInstruction> lowerBinaryOp(
    SelectorState& state,
    const ir::Instruction& irInst,
    const std::string& resultReg,
    const std::string& mnemonic) {

    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    Instruction286 inst;
    inst.mnemonic = mnemonic;

    if (irInst.operands.size() >= 2) {
        std::string op1 = state.getPhysReg(irInst.operands[0].name);
        std::string op2 = state.getPhysReg(irInst.operands[1].name);

        inst.operands.push_back(resultReg);
        inst.operands.push_back(op2);

        if (resultReg != op1) {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(resultReg);
            movInst.operands.push_back(op1);
            lowered.instructions.push_back(movInst);
        }
    } else {
        inst.operands.push_back("ax");
        inst.operands.push_back("bx");
    }

    lowered.instructions.push_back(inst);

    if (!irInst.resultName.empty()) {
        state.updateResultReg(irInst.resultName, resultReg);
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

// Shared helper for Shl, LShr, AShr: op1 <shift> op2, result in op1
static std::vector<LoweredInstruction> lowerShiftOp(
    SelectorState& state,
    const ir::Instruction& irInst,
    const std::string& resultReg,
    const std::string& mnemonic) {

    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    Instruction286 inst;
    inst.mnemonic = mnemonic;

    if (irInst.operands.size() >= 2) {
        std::string op1 = state.getPhysReg(irInst.operands[0].name);
        std::string op2 = state.getPhysReg(irInst.operands[1].name);

        inst.operands.push_back(op1);
        inst.operands.push_back(op2);

        if (resultReg != op1) {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(resultReg);
            movInst.operands.push_back(op1);
            lowered.instructions.push_back(movInst);
        }
    } else {
        inst.operands.push_back("ax");
        inst.operands.push_back("cl");
    }

    lowered.instructions.push_back(inst);

    if (!irInst.resultName.empty()) {
        state.updateResultReg(irInst.resultName, resultReg);
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerAnd(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    return lowerBinaryOp(state, irInst, resultReg, "and");
}

std::vector<LoweredInstruction> lowerOr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    return lowerBinaryOp(state, irInst, resultReg, "or");
}

std::vector<LoweredInstruction> lowerXor(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    return lowerBinaryOp(state, irInst, resultReg, "xor");
}

std::vector<LoweredInstruction> lowerShl(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    return lowerShiftOp(state, irInst, resultReg, "shl");
}

std::vector<LoweredInstruction> lowerLShr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    return lowerShiftOp(state, irInst, resultReg, "shr");
}

std::vector<LoweredInstruction> lowerAShr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    return lowerShiftOp(state, irInst, resultReg, "sar");
}

} // namespace codegen
} // namespace llvm_i286