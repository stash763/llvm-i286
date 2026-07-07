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

    std::string destReg = resultReg;
    std::string op1 = "ax";
    bool op1IsConst = false;
    bool op1IsMem = false;

    if (irInst.operands.size() >= 2) {
        std::string op1Name = irInst.operands[0].name;
        std::string op2Name = irInst.operands[1].name;

        // Check if operands are vregs (exist in state maps)
        bool op1IsVreg = state.vregToPhys.find(op1Name) != state.vregToPhys.end() ||
                         state.vregToStackOffset.find(op1Name) != state.vregToStackOffset.end();
        bool op2IsVreg = state.vregToPhys.find(op2Name) != state.vregToPhys.end() ||
                         state.vregToStackOffset.find(op2Name) != state.vregToStackOffset.end();

        // Check if operands are constants (only if not vregs)
        bool op2IsConst = !op2IsVreg && isConstantInt(op2Name);

        op1IsConst = !op1IsVreg && isConstantInt(op1Name);
        op1 = op1IsConst ? op1Name : state.getPhysReg(op1Name);
        std::string op2 = op2IsConst ? op2Name : state.getPhysReg(op2Name);

        // If op1 is a memory operand, we need to load it to a register first
        op1IsMem = !op1IsConst && op1.find("bp") != std::string::npos;
        if (op1IsMem && destReg == op1) {
            // Can't do "and [bp-N], op2" on 286, need to load to register first
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back("ax");
            loadInst.operands.push_back("[" + op1 + "]");
            lowered.instructions.push_back(loadInst);
            destReg = "ax";
            op1 = "ax";
        }

        inst.operands.push_back(destReg);
        inst.operands.push_back(op2);

        if (destReg != op1) {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(destReg);
            // If op1 is memory, add brackets
            if (op1IsMem) {
                movInst.operands.push_back("[" + op1 + "]");
            } else {
                movInst.operands.push_back(op1);
            }
            lowered.instructions.push_back(movInst);
        }
    } else {
        inst.operands.push_back("ax");
        inst.operands.push_back("bx");
    }

   lowered.instructions.push_back(inst);

    if (!irInst.resultName.empty()) {
        // Check if result is 32-bit
        bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
        
        if (is32) {
            // For 32-bit results, store to stack (BX holds high word, AX holds low word)
            state.currentStackOffset -= 4;
            state.tempSpaceInBlock += 4;
            int stackOffset = state.currentStackOffset;
            state.vregToStackOffset[irInst.resultName] = stackOffset;
            
            std::string lowStack = "bp" + std::to_string(stackOffset);
            std::string highStack = "bp" + std::to_string(stackOffset + 2);
            
            // Store low word (AX)
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + lowStack + "]");
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);
            
            // Store high word (BX)
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            storeHigh.operands.push_back("[" + highStack + "]");
            storeHigh.operands.push_back("bx");
            lowered.instructions.push_back(storeHigh);
            
            state.mark32Bit(irInst.resultName);
            state.vregToPhys[irInst.resultName] = lowStack;
        } else {
            state.updateResultReg(irInst.resultName, destReg);
        }
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

    std::string destReg = resultReg;
    std::string op1 = "ax";
    std::string op2 = "cl";
    bool op1IsConst = false;
    bool op1IsMem = false;

    if (irInst.operands.size() >= 2) {
        std::string op1Name = irInst.operands[0].name;
        std::string op2Name = irInst.operands[1].name;

        // Check if operands are vregs
        bool op1IsVreg = state.vregToPhys.find(op1Name) != state.vregToPhys.end() ||
                         state.vregToStackOffset.find(op1Name) != state.vregToStackOffset.end();
        bool op2IsVreg = state.vregToPhys.find(op2Name) != state.vregToPhys.end() ||
                         state.vregToStackOffset.find(op2Name) != state.vregToStackOffset.end();

        // Check if operands are constants (only if not vregs)
        bool op2IsConst = !op2IsVreg && isConstantInt(op2Name);

        op1IsConst = !op1IsVreg && isConstantInt(op1Name);
        op1 = op1IsConst ? op1Name : state.getPhysReg(op1Name);
        op2 = op2IsConst ? op2Name : state.getPhysReg(op2Name);

        // Shift count must be in CL for variable shifts, or immediate
        if (!op2IsConst && op2 != "cl" && op2 != "cx") {
            // Move shift count to CL
            Instruction286 movCL;
            movCL.mnemonic = "mov";
            movCL.operands.push_back("cl");
            movCL.operands.push_back(op2);
            lowered.instructions.push_back(movCL);
            op2 = "cl";
        }

        // If op1 is a memory operand, load to register first
        op1IsMem = !op1IsConst && op1.find("bp") != std::string::npos;
        if (op1IsMem && destReg == op1) {
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back("ax");
            loadInst.operands.push_back("[" + op1 + "]");
            lowered.instructions.push_back(loadInst);
            destReg = "ax";
            op1 = "ax";
        }

        inst.operands.push_back(destReg);
        inst.operands.push_back(op2);

        if (destReg != op1) {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(destReg);
            if (op1IsMem) {
                movInst.operands.push_back("[" + op1 + "]");
            } else {
                movInst.operands.push_back(op1);
            }
            lowered.instructions.push_back(movInst);
        }
    } else {
        inst.operands.push_back("ax");
        inst.operands.push_back("cl");
    }

    lowered.instructions.push_back(inst);

    if (!irInst.resultName.empty()) {
        state.updateResultReg(irInst.resultName, destReg);
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