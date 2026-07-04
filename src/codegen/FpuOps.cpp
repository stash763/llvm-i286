// Floating Point Operations - Instruction Selection
// Handlers for FPU IR instructions: FAdd, FSub, FMul, FDiv, FRem,
// FCmp, FPTrunc, FPExt, FPToUI, FPToSI, UIToFP, SIToFP

#include "codegen/InstructionSelectInternal.h"

#include <string>
#include <vector>

namespace llvm_i286 {
namespace codegen {

std::vector<LoweredInstruction> lowerFpuArith(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // FPU arithmetic operations (FAdd, FSub, FMul, FDiv)
    // 80287 FPU uses a register stack ST(0)-ST(7)
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (irInst.operands.size() >= 2) {
        std::string op1Name = irInst.operands[0].name;
        std::string op2Name = irInst.operands[1].name;

        // Load op1 to ST(0) if not already there
        // For now, assume operands are already in FPU registers or memory
        // This is a simplified version - full implementation would track FPU stack

        // Perform operation
        Instruction286 fpuInst;
        switch (irInst.opcode) {
            case ir::Opcode::FAdd: fpuInst.mnemonic = "fadd"; break;
            case ir::Opcode::FSub: fpuInst.mnemonic = "fsub"; break;
            case ir::Opcode::FMul: fpuInst.mnemonic = "fmul"; break;
            case ir::Opcode::FDiv: fpuInst.mnemonic = "fdiv"; break;
            default: break;
        }

        // If both operands are vregs, load second operand and operate
        if (irInst.operands.size() >= 2) {
            std::string op2Reg = state.getPhysReg(op2Name);
            if (op2Reg.find("bp") != std::string::npos) {
                fpuInst.operands.push_back("[" + op2Reg + "]");
            } else {
                fpuInst.operands.push_back(op2Reg);
            }
        }

        lowered.instructions.push_back(fpuInst);

        // Result is in ST(0)
        // For now, we'll store it to memory and reload when needed
        // Full implementation would track FPU register assignments
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerFRem(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // FPU remainder
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string op2Name = irInst.operands[0].name;
        std::string op2Reg = state.getPhysReg(op2Name);

        Instruction286 fprem;
        fprem.mnemonic = "fprem";
        if (op2Reg.find("bp") != std::string::npos) {
            fprem.operands.push_back("[" + op2Reg + "]");
        } else {
            fprem.operands.push_back(op2Reg);
        }
        lowered.instructions.push_back(fprem);
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerFCmp(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // FPU comparison
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (irInst.operands.size() >= 2) {
        std::string op1Name = irInst.operands[0].name;
        std::string op2Name = irInst.operands[1].name;

        // Load op1 to ST(0) if needed
        std::string op1Reg = state.getPhysReg(op1Name);
        if (op1Reg.find("bp") != std::string::npos) {
            Instruction286 fld;
            fld.mnemonic = "fld";
            fld.operands.push_back("[" + op1Reg + "]");
            lowered.instructions.push_back(fld);
        }

        // Compare with op2
        std::string op2Reg = state.getPhysReg(op2Name);
        Instruction286 fcom;
        fcom.mnemonic = "fcom";
        if (op2Reg.find("bp") != std::string::npos) {
            fcom.operands.push_back("[" + op2Reg + "]");
        } else {
            fcom.operands.push_back(op2Reg);
        }
        lowered.instructions.push_back(fcom);

        // Store status word and test
        Instruction286 fstsw;
        fstsw.mnemonic = "fstsw";
        fstsw.operands.push_back("ax");
        lowered.instructions.push_back(fstsw);

        Instruction286 sahf;
        sahf.mnemonic = "sahf";
        lowered.instructions.push_back(sahf);

        // Result: flags set based on comparison
        // This is simplified - full implementation would handle predicate
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerFpuConvert(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // FPU conversion (double->float or float->double)
    // For 80287, all operations use 80-bit extended precision
    // Truncation to float: store as 32-bit
    // Extension to double: store as 64-bit
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string opReg = state.getPhysReg(irInst.operands[0].name);

        // Store to memory with appropriate size
        // This is a placeholder - full implementation would handle FPU stack
        Instruction286 fstp;
        fstp.mnemonic = "fstp";
        if (irInst.opcode == ir::Opcode::FPTrunc) {
            fstp.operands.push_back("dword"); // 32-bit float
        } else {
            fstp.operands.push_back("qword"); // 64-bit double
        }
        if (opReg.find("bp") != std::string::npos) {
            fstp.operands.push_back("[" + opReg + "]");
        } else {
            fstp.operands.push_back(opReg);
        }
        lowered.instructions.push_back(fstp);
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerFPToInt(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // FPU to integer conversion
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string opReg = state.getPhysReg(irInst.operands[0].name);

        Instruction286 fistp;
        fistp.mnemonic = irInst.opcode == ir::Opcode::FPToUI ? "fistp" : "fistp";
        fistp.operands.push_back("dword"); // 32-bit integer
        if (opReg.find("bp") != std::string::npos) {
            fistp.operands.push_back("[" + opReg + "]");
        } else {
            fistp.operands.push_back(opReg);
        }
        lowered.instructions.push_back(fistp);

        // Result in AX
        if (!irInst.resultName.empty()) {
            state.updateResultReg(irInst.resultName, "ax");
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerIntToFP(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Integer to FPU conversion
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string opReg = state.getPhysReg(irInst.operands[0].name);

        Instruction286 fild;
        fild.mnemonic = "fild";
        fild.operands.push_back("dword"); // 32-bit integer
        if (opReg.find("bp") != std::string::npos) {
            fild.operands.push_back("[" + opReg + "]");
        } else {
            fild.operands.push_back(opReg);
        }
        lowered.instructions.push_back(fild);
        // Result in ST(0)
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286
