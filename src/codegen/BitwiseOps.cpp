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

    bool skipPush = false;
    if (irInst.operands.size() >= 2) {
        std::string op1Name = irInst.operands[0].name;
        std::string op2Name = irInst.operands[1].name;

        // Check if operands are vregs (exist in state maps)
        bool op1IsVreg = state.frame.hasSlot(op1Name) ||
                         state.frame.hasSlot(op1Name);
        bool op2IsVreg = state.frame.hasSlot(op2Name) ||
                         state.frame.hasSlot(op2Name);

        // Check if operands are constants (only if not vregs)
        bool op2IsConst = !op2IsVreg && isConstantInt(op2Name);
        bool op2IsMem = false;

        op1IsConst = !op1IsVreg && isConstantInt(op1Name);
        op1 = op1IsConst ? op1Name : state.frame.getPhysReg(op1Name);
        std::string op2 = op2IsConst ? op2Name : state.frame.getPhysReg(op2Name);

        // Check if op2 is a memory operand
        op2IsMem = !op2IsConst && op2.find("bp") != std::string::npos;

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
        // If op2 is memory, wrap in brackets
        if (op2IsMem) {
            inst.operands.push_back("[" + op2 + "]");
        } else if (op2IsConst) {
            // Check if this is a 32-bit operation or the constant does not fit in 16 bits
            bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
            int64_t constVal = 0;
            try { constVal = std::stoll(op2Name); } catch (...) {}
            bool needsSplit = is32 || constVal > 0xFFFFLL || constVal < -32768LL;
            if (needsSplit) {
                // Split into low and high word operations
                int16_t lowWord = (int16_t)(constVal & 0xFFFF);
                int16_t highWord = (int16_t)((constVal >> 16) & 0xFFFF);
                Instruction286 lowInst;
                lowInst.mnemonic = mnemonic;
                lowInst.operands.push_back(destReg);
                lowInst.operands.push_back(std::to_string(lowWord));
                lowered.instructions.push_back(lowInst);
                Instruction286 highInst;
                highInst.mnemonic = mnemonic;
                highInst.operands.push_back("dx");
                highInst.operands.push_back(std::to_string(highWord));
                lowered.instructions.push_back(highInst);
                skipPush = true;
            }
            if (!skipPush) {
                inst.operands.push_back(op2);
            }
        } else {
            inst.operands.push_back(op2);
        }

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

    if (!skipPush) {
        lowered.instructions.push_back(inst);
    }

    if (!irInst.resultName.empty()) {
        // Check if result is 32-bit
        bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
        
        if (is32) {
            // For 32-bit results, zero high word (DX) and store to stack
            // This is correct for AND/OR/XOR with constants that fit in 16 bits
            Instruction286 zeroHigh;
            zeroHigh.mnemonic = "xor";
            zeroHigh.operands.push_back("dx");
            zeroHigh.operands.push_back("dx");
            lowered.instructions.push_back(zeroHigh);
            
            // Stack space pre-allocated in prologue
            std::string tempSlot = state.frame.allocTemp(4, true);
            std::string lowStack = tempSlot;
            std::string highStack = state.frame.getHighBpOffset(tempSlot);
            
            // Store low word (AX)
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + lowStack + "]");
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);
            
            // Store high word (DX, zeroed)
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            storeHigh.operands.push_back("[" + highStack + "]");
            storeHigh.operands.push_back("dx");
            lowered.instructions.push_back(storeHigh);
            
            // Update vreg mapping to point to stack slot
            state.frame.setPhysReg(irInst.resultName, tempSlot);
            
        } else {
            state.frame.setPhysReg(irInst.resultName, destReg);
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
        bool op1IsVreg = state.frame.hasSlot(op1Name) ||
                         state.frame.hasSlot(op1Name);
        bool op2IsVreg = state.frame.hasSlot(op2Name) ||
                         state.frame.hasSlot(op2Name);

        // Check if operands are constants (only if not vregs)
        bool op2IsConst = !op2IsVreg && isConstantInt(op2Name);

        op1IsConst = !op1IsVreg && isConstantInt(op1Name);
        op1 = op1IsConst ? op1Name : state.frame.getPhysReg(op1Name);
        op2 = op2IsConst ? op2Name : state.frame.getPhysReg(op2Name);

        // Check if op2 is a memory operand
        bool op2IsMem = !op2IsConst && op2.find("bp") != std::string::npos;

        // Shift count must be in CL for variable shifts, or immediate
        if (!op2IsConst && op2 != "cl" && op2 != "cx") {
            // Move shift count to CL
            Instruction286 movCL;
            movCL.mnemonic = "mov";
            movCL.operands.push_back("cl");
            // If op2 is memory, wrap in brackets
            if (op2IsMem) {
                movCL.operands.push_back("[" + op2 + "]");
            } else {
                movCL.operands.push_back(op2);
            }
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
        state.frame.setPhysReg(irInst.resultName, destReg);
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