// Control Flow Operations - Instruction Selection
// Handlers for control flow IR instructions: RetTerm, BrTerm, CondBrTerm,
// Switch, Phi, Unreachable, Select, ICmp
//
// NOTE: Updated to use StackFrame for all register/stack lookups

#include "codegen/InstructionSelectInternal.h"
#include "codegen/NasmSafe.h"

#include <string>
#include <vector>

namespace llvm_i286 {
namespace codegen {

std::vector<LoweredInstruction> lowerRetTerm(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Return instruction - emit epilogue then ret
    // If there's a return value operand, load it into AX (and DX for 32-bit)
    if (!irInst.operands.empty()) {
        std::string retValName = irInst.operands[0].name;

        // Check if return value is a constant (not a vreg)
        bool isConst = false;
        try {
            std::stoi(retValName);
            isConst = true;
        } catch (...) {}

        // Check if this is a 32-bit return value
        bool is32 = state.frame.is32bit(retValName) ||
                    (irInst.resultType && irInst.resultType->bitWidth == 32);

        // Check if return value is a global reference (starts with @)
        bool isGlobal = (!retValName.empty() && retValName[0] == '@');

        if (isConst) {
            // Load constant into AX
            Instruction286 movConst;
            movConst.mnemonic = "mov";
            movConst.operands.push_back("ax");
            movConst.operands.push_back(retValName);
            lowered.instructions.push_back(movConst);
            if (is32) {
                // High word is 0 for positive constants, or sign-extended
                Instruction286 xorDx;
                xorDx.mnemonic = "xor";
                xorDx.operands.push_back("dx");
                xorDx.operands.push_back("dx");
                lowered.instructions.push_back(xorDx);
            }
        } else if (isGlobal) {
            // Global reference - load address of global into AX:DX
            // Remove @ prefix for NASM
            std::string nasmName = retValName.substr(1);
            // Convert .name to _name for NASM compatibility (matches CodeGen.cpp)
            if (!nasmName.empty() && nasmName[0] == '.') {
                nasmName[0] = '_';
            }
            nasmName = safeNasmName(nasmName);
            // Load offset into AX
            Instruction286 leaAx;
            leaAx.mnemonic = "lea";
            leaAx.operands.push_back("ax");
            leaAx.operands.push_back("[" + nasmName + "]");
            lowered.instructions.push_back(leaAx);
            // Load segment into DX
            Instruction286 movSeg;
            movSeg.mnemonic = "mov";
            movSeg.operands.push_back("dx");
            movSeg.operands.push_back("seg " + nasmName);
            lowered.instructions.push_back(movSeg);
        } else {
            // Value is a vreg - use StackFrame to get its location
            std::string retValReg = state.frame.getPhysReg(retValName);

            // Check if value is in memory (stack) or register
            if (retValReg.find("bp") != std::string::npos) {
                // Stack location - load from memory
                Instruction286 movLow;
                movLow.mnemonic = "mov";
                movLow.operands.push_back("ax");
                movLow.operands.push_back("[" + retValReg + "]");
                lowered.instructions.push_back(movLow);

                if (is32) {
                    // Load high word from [bp-offset+2]
                    std::string highOffset = state.frame.getHighBpOffset(retValName);
                    Instruction286 movHigh;
                    movHigh.mnemonic = "mov";
                    movHigh.operands.push_back("dx");
                    movHigh.operands.push_back("[" + highOffset + "]");
                    lowered.instructions.push_back(movHigh);
                }
            } else {
                // Register - move to AX if needed
                if (retValReg != "ax") {
                    Instruction286 movReg;
                    movReg.mnemonic = "mov";
                    movReg.operands.push_back("ax");
                    movReg.operands.push_back(retValReg);
                    lowered.instructions.push_back(movReg);
                }
                // If is32, DX should already have high word (from calling convention)
            }
        }
    }

    // Deallocate stack frame before restoring saved registers
    // Use lea sp, [bp-6] instead of add sp, frameSize to avoid
    // frame size mismatch when totalFrameSize is extended after
    // early return blocks are lowered.
    Instruction286 leaSp;
    leaSp.mnemonic = "lea";
    leaSp.operands.push_back("sp");
    leaSp.operands.push_back("[bp-6]");
    lowered.instructions.push_back(leaSp);

    // Restore callee-saved registers
    Instruction286 popDi;
    popDi.mnemonic = "pop";
    popDi.operands.push_back("di");
    lowered.instructions.push_back(popDi);

    Instruction286 popSi;
    popSi.mnemonic = "pop";
    popSi.operands.push_back("si");
    lowered.instructions.push_back(popSi);

    Instruction286 popBx;
    popBx.mnemonic = "pop";
    popBx.operands.push_back("bx");
    lowered.instructions.push_back(popBx);

    Instruction286 popBp;
    popBp.mnemonic = "pop";
    popBp.operands.push_back("bp");
    lowered.instructions.push_back(popBp);

    // Emit retf (far return)
    Instruction286 retInst;
    retInst.mnemonic = "retf";
    lowered.instructions.push_back(retInst);

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerBrTerm(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Unconditional branch
    Instruction286 jmpInst;
    jmpInst.mnemonic = "jmp";

    if (!irInst.operands.empty()) {
        std::string prefix = "";
        if (state.currentFunc && !state.currentFunc->name.empty()) {
            prefix = state.currentFunc->name + "_";
        }
        jmpInst.operands.push_back(prefix + "bb_" + irInst.operands[0].name);
    }

    lowered.instructions.push_back(jmpInst);

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerCondBrTerm(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Conditional branch: br i1 cond, label trueBB, label falseBB
    if (irInst.operands.size() >= 3) {
        std::string condName = irInst.operands[0].name;

        // Check if condition is a constant
        auto condKind = state.frame.classifyOperand(condName);
        bool isConst = (condKind == StackFrame::OperandKind::Constant);

        std::string condReg = isConst ? condName : state.frame.getPhysReg(condName);

        // Load condition to CX (use CX instead of AX to preserve AX for GEP results)
        if (!isConst && condReg.find("bp") != std::string::npos) {
            Instruction286 loadCond;
            loadCond.mnemonic = "mov";
            loadCond.operands.push_back("cx");
            loadCond.operands.push_back("[" + condReg + "]");
            lowered.instructions.push_back(loadCond);
            condReg = "cx";
        } else if (!isConst && condReg != "ax" && condReg != "cx") {
            Instruction286 movCond;
            movCond.mnemonic = "mov";
            movCond.operands.push_back("cx");
            movCond.operands.push_back(condReg);
            lowered.instructions.push_back(movCond);
            condReg = "cx";
        } else if (!isConst && condReg == "ax") {
            // Condition is in AX, move to CX to preserve AX
            Instruction286 movCond;
            movCond.mnemonic = "mov";
            movCond.operands.push_back("cx");
            movCond.operands.push_back("ax");
            lowered.instructions.push_back(movCond);
            condReg = "cx";
        }

        // Near true label
        std::string prefix = "";
        if (state.currentFunc && !state.currentFunc->name.empty()) {
            prefix = state.currentFunc->name + "_";
        }
        std::string nearTrueLabel = prefix + "bb_" + irInst.operands[1].name;

        // Test the condition value to set flags
        Instruction286 testCond;
        testCond.mnemonic = "test";
        testCond.operands.push_back(condReg);
        testCond.operands.push_back(condReg);
        lowered.instructions.push_back(testCond);

        Instruction286 nearTrueInst;
        nearTrueInst.mnemonic = "jne";
        nearTrueInst.operands.push_back(nearTrueLabel);
        lowered.instructions.push_back(nearTrueInst);

        // Jump to false label (long jump if needed)
        std::string falseLabel = prefix + "bb_" + irInst.operands[2].name;
        Instruction286 jmpFalse;
        jmpFalse.mnemonic = "jmp";
        jmpFalse.operands.push_back(falseLabel);
        lowered.instructions.push_back(jmpFalse);

        // Near true label (trampoline - use unique name to avoid duplicating the real BB label)
        LoweredInstruction nearTrueLabelInst;
        nearTrueLabelInst.label = prefix + "bb_" + irInst.operands[1].name + "_near";
        loweredVec.push_back(lowered);
        lowered = LoweredInstruction{};
        loweredVec.push_back(nearTrueLabelInst);

        // Jump to true label (long jump if needed)
        std::string trueLabel = prefix + "bb_" + irInst.operands[1].name;
        Instruction286 jmpTrue;
        jmpTrue.mnemonic = "jmp";
        jmpTrue.operands.push_back(trueLabel);
        lowered.instructions.push_back(jmpTrue);
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerSwitch(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Switch statement - emit as chained comparisons
    // Operand layout:
    //   operands[0]: condition value (e.g., "%12")
    //   operands[1]: default label (e.g., "17")
    //   operands[2], operands[3]: case value, case label
    //   operands[4], operands[5]: case value, case label
    //   ...
    if (irInst.operands.size() >= 2) {
        std::string condReg = state.frame.getPhysReg(irInst.operands[0].name);

        // Load condition to AX
        if (condReg.find("bp") != std::string::npos) {
            Instruction286 loadCond;
            loadCond.mnemonic = "mov";
            loadCond.operands.push_back("ax");
            loadCond.operands.push_back("[" + condReg + "]");
            lowered.instructions.push_back(loadCond);
        } else if (condReg != "ax") {
            Instruction286 movCond;
            movCond.mnemonic = "mov";
            movCond.operands.push_back("ax");
            movCond.operands.push_back(condReg);
            lowered.instructions.push_back(movCond);
        }

        // Build prefix for labels
        std::string prefix = "";
        if (state.currentFunc && !state.currentFunc->name.empty()) {
            prefix = state.currentFunc->name + "_";
        }

        // Default label
        std::string defaultLabel = prefix + "bb_" + irInst.operands[1].name;

        // Emit case comparisons (starting from operands[2])
        for (size_t i = 2; i + 1 < irInst.operands.size(); i += 2) {
            std::string caseValue = irInst.operands[i].name;
            std::string caseLabel = irInst.operands[i + 1].name;

            Instruction286 cmpInst;
            cmpInst.mnemonic = "cmp";
            cmpInst.operands.push_back("ax");
            cmpInst.operands.push_back(caseValue);
            lowered.instructions.push_back(cmpInst);

            Instruction286 jmpCase;
            jmpCase.mnemonic = "je";
            jmpCase.operands.push_back(prefix + "bb_" + caseLabel);
            lowered.instructions.push_back(jmpCase);
        }

        // Jump to default (fallthrough)
        Instruction286 jmpDefault;
        jmpDefault.mnemonic = "jmp";
        jmpDefault.operands.push_back(defaultLabel);
        lowered.instructions.push_back(jmpDefault);
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerPhi(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Phi nodes are handled at the basic block level, not here
    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerUnreachable(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    Instruction286 hlt;
    hlt.mnemonic = "hlt";
    hlt.comment = "unreachable";
    lowered.instructions.push_back(hlt);

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerSelect(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (irInst.operands.size() >= 3) {
        std::string condName = irInst.operands[0].name;
        bool isCondConst = false;
        try {
            std::stoi(condName);
            isCondConst = true;
        } catch (...) {}

        std::string condReg = isCondConst ? condName : state.frame.getPhysReg(condName);
        std::string val1Reg = state.frame.getPhysReg(irInst.operands[1].name);
        std::string val2Reg = state.frame.getPhysReg(irInst.operands[2].name);

        std::string destReg = resultReg.empty() ? "ax" : resultReg;
        bool destIsMem = destReg.find("bp") != std::string::npos;
        std::string workReg = destIsMem ? "ax" : destReg;

        // Test condition
        Instruction286 testCond;
        testCond.mnemonic = "or";
        testCond.operands.push_back("ax");
        testCond.operands.push_back("ax");
        lowered.instructions.push_back(testCond);

        std::string trueLabel = state.nextLabel(".Lsel_true_");
        std::string falseLabel = state.nextLabel(".Lsel_false_");
        std::string endLabel = state.nextLabel(".Lsel_end_");

        // Jump to true if non-zero
        Instruction286 jmpTrue;
        jmpTrue.mnemonic = "jne";
        jmpTrue.operands.push_back(trueLabel);
        lowered.instructions.push_back(jmpTrue);

        // False case: load val2 into workReg
        Instruction286 loadFalse;
        loadFalse.mnemonic = "mov";
        loadFalse.operands.push_back(workReg);
        if (val2Reg.find("bp") != std::string::npos) {
            loadFalse.operands.push_back("[" + val2Reg + "]");
        } else {
            loadFalse.operands.push_back(val2Reg);
        }
        lowered.instructions.push_back(loadFalse);
        Instruction286 jmpEnd;
        jmpEnd.mnemonic = "jmp";
        jmpEnd.operands.push_back(endLabel);
        lowered.instructions.push_back(jmpEnd);

        loweredVec.push_back(lowered);
        lowered = LoweredInstruction{};

        LoweredInstruction trueLabelInst;
        trueLabelInst.label = trueLabel;
        loweredVec.push_back(trueLabelInst);

        // True case: load val1 into workReg
        Instruction286 loadTrue;
        loadTrue.mnemonic = "mov";
        loadTrue.operands.push_back(workReg);
        if (val1Reg.find("bp") != std::string::npos) {
            loadTrue.operands.push_back("[" + val1Reg + "]");
        } else {
            loadTrue.operands.push_back(val1Reg);
        }
        lowered.instructions.push_back(loadTrue);

        loweredVec.push_back(lowered);
        lowered = LoweredInstruction{};

        LoweredInstruction endLabelInst;
        endLabelInst.label = endLabel;
        loweredVec.push_back(endLabelInst);

        // If destReg is memory, store workReg to it
        if (destIsMem) {
            Instruction286 storeResult;
            storeResult.mnemonic = "mov";
            storeResult.operands.push_back("[" + destReg + "]");
            storeResult.operands.push_back(workReg);
            lowered.instructions.push_back(storeResult);
        }

        if (!irInst.resultName.empty()) {
            state.frame.setPhysReg(irInst.resultName, destReg);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerICmp(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (irInst.operands.size() >= 2 && !irInst.predicate.empty()) {
        bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;

        if (is32) {
            // 32-bit comparison
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            bool op1IsConst = state.frame.classifyOperand(op1Name) == StackFrame::OperandKind::Constant;
            bool op2IsConst = state.frame.classifyOperand(op2Name) == StackFrame::OperandKind::Constant;

            std::string op1Reg = op1IsConst ? op1Name : state.frame.getPhysReg(op1Name);
            std::string op2Reg = op2IsConst ? op2Name : state.frame.getPhysReg(op2Name);

            // Load op1 low word to AX, high word to BX
            if (op1IsConst) {
                int64_t val = std::stoll(op1Name);
                Instruction286 loadLow;
                loadLow.mnemonic = "mov";
                loadLow.operands.push_back("ax");
                loadLow.operands.push_back(std::to_string(val & 0xFFFF));
                lowered.instructions.push_back(loadLow);

                Instruction286 loadHigh;
                loadHigh.mnemonic = "mov";
                loadHigh.operands.push_back("bx");
                int16_t highWord = (val >> 16) & 0xFFFF;
                loadHigh.operands.push_back(std::to_string(highWord));
                lowered.instructions.push_back(loadHigh);
            } else {
                state.frame.emitLoad32(lowered.instructions, op1Name, "ax", "bx");
            }

            // Load op2 low word to CX, high word to DX
            if (op2IsConst) {
                int64_t val = std::stoll(op2Name);
                Instruction286 loadLow;
                loadLow.mnemonic = "mov";
                loadLow.operands.push_back("cx");
                loadLow.operands.push_back(std::to_string(val & 0xFFFF));
                lowered.instructions.push_back(loadLow);

                Instruction286 loadHigh;
                loadHigh.mnemonic = "mov";
                loadHigh.operands.push_back("dx");
                int16_t highWord = (val >> 16) & 0xFFFF;
                loadHigh.operands.push_back(std::to_string(highWord));
                lowered.instructions.push_back(loadHigh);
            } else {
                state.frame.emitLoad32(lowered.instructions, op2Name, "cx", "dx");
            }

            // Compare high words: BX vs DX
            Instruction286 cmpHigh;
            cmpHigh.mnemonic = "cmp";
            cmpHigh.operands.push_back("bx");
            cmpHigh.operands.push_back("dx");
            lowered.instructions.push_back(cmpHigh);

            // Map LLVM predicate to 286 jump conditions
            std::string trueLabel = state.nextLabel(".Lcmp_true_");
            std::string falseLabel = state.nextLabel(".Lcmp_false_");
            std::string endLabel = state.nextLabel(".Lcmp_end_");
            std::string cmpLowLabel = state.nextLabel(".Lcmp_low_");

            // Jump to compare low words if high words are equal
            Instruction286 jmpCmpLow;
            jmpCmpLow.mnemonic = "je";
            jmpCmpLow.operands.push_back(cmpLowLabel);
            lowered.instructions.push_back(jmpCmpLow);

            // High words differ - use signed comparison on high words
            // After je .Lcmp_low_3, we know BX≠DX (ZF=0).
            // For eq/ne predicates, the result is already determined:
            //   eq: high words differ → not equal → false
            //   ne: high words differ → not equal → true
            // For other predicates, check the actual comparison result.
            if (irInst.predicate == "slt") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "jl"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "sle") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "jle"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "sgt") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "jg"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "sge") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "jge"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "ult") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "jb"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "ule") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "jbe"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "ugt") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "ja"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "uge") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "jae"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "eq") {
                Instruction286 jmpFalse; jmpFalse.mnemonic = "jmp"; jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            } else if (irInst.predicate == "ne") {
                Instruction286 jmpTrue; jmpTrue.mnemonic = "jmp"; jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);
            }

            loweredVec.push_back(lowered);
            lowered = LoweredInstruction{};

            LoweredInstruction cmpLowLabelInst;
            cmpLowLabelInst.label = cmpLowLabel;
            loweredVec.push_back(cmpLowLabelInst);

            // Low words comparison (unsigned since high words are equal)
            Instruction286 cmpLow;
            cmpLow.mnemonic = "cmp";
            cmpLow.operands.push_back("ax");
            cmpLow.operands.push_back("cx");
            lowered.instructions.push_back(cmpLow);

            // For low words when high words equal, use unsigned comparison for signed predicates
            std::string lowJumpMnemonic;
            if (irInst.predicate == "slt") lowJumpMnemonic = "jb";
            else if (irInst.predicate == "sle") lowJumpMnemonic = "jbe";
            else if (irInst.predicate == "sgt") lowJumpMnemonic = "ja";
            else if (irInst.predicate == "sge") lowJumpMnemonic = "jae";
            else if (irInst.predicate == "ult") lowJumpMnemonic = "jb";
            else if (irInst.predicate == "ule") lowJumpMnemonic = "jbe";
            else if (irInst.predicate == "ugt") lowJumpMnemonic = "ja";
            else if (irInst.predicate == "uge") lowJumpMnemonic = "jae";
            else if (irInst.predicate == "eq") lowJumpMnemonic = "je";
            else if (irInst.predicate == "ne") lowJumpMnemonic = "jne";

            if (!lowJumpMnemonic.empty()) {
                Instruction286 jmpTrueLow;
                jmpTrueLow.mnemonic = lowJumpMnemonic;
                jmpTrueLow.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrueLow);

                Instruction286 jmpFalseLow;
                jmpFalseLow.mnemonic = "jmp";
                jmpFalseLow.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalseLow);
            }

            loweredVec.push_back(lowered);
            lowered = LoweredInstruction{};

            LoweredInstruction trueLabelInst;
            trueLabelInst.label = trueLabel;
            loweredVec.push_back(trueLabelInst);

            Instruction286 setTrue;
            setTrue.mnemonic = "mov";
            setTrue.operands.push_back("ax");
            setTrue.operands.push_back("1");
            lowered.instructions.push_back(setTrue);

            Instruction286 jmpToEnd;
            jmpToEnd.mnemonic = "jmp";
            jmpToEnd.operands.push_back(endLabel);
            lowered.instructions.push_back(jmpToEnd);

            loweredVec.push_back(lowered);
            lowered = LoweredInstruction{};

            LoweredInstruction falseLabelInst;
            falseLabelInst.label = falseLabel;
            loweredVec.push_back(falseLabelInst);

            Instruction286 setFalse;
            setFalse.mnemonic = "mov";
            setFalse.operands.push_back("ax");
            setFalse.operands.push_back("0");
            lowered.instructions.push_back(setFalse);
            loweredVec.push_back(lowered);
            lowered = LoweredInstruction{};

            LoweredInstruction endLabelInst;
            endLabelInst.label = endLabel;
            loweredVec.push_back(endLabelInst);

            if (!irInst.resultName.empty()) {
                // Save result to stack temp to prevent clobbering by subsequent ops
                std::string tempSlot = state.frame.allocResultSlot(irInst.resultName, 2, false);
                Instruction286 saveResult;
                saveResult.mnemonic = "mov";
                saveResult.operands.push_back("[" + tempSlot + "]");
                saveResult.operands.push_back("ax");
                lowered.instructions.push_back(saveResult);
                state.frame.setPhysReg(irInst.resultName, tempSlot);
            }
        } else {
            // 16-bit comparison
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            bool op1IsConst = state.frame.classifyOperand(op1Name) == StackFrame::OperandKind::Constant;
            bool op2IsConst = state.frame.classifyOperand(op2Name) == StackFrame::OperandKind::Constant;

            std::string op1Reg = op1IsConst ? op1Name : state.frame.getPhysReg(op1Name);
            std::string op2Reg = op2IsConst ? op2Name : state.frame.getPhysReg(op2Name);

            // Load operands into registers if they're memory locations
            bool op1IsMem = !op1IsConst && (op1Reg.find("bp") != std::string::npos);
            bool op2IsMem = !op2IsConst && (op2Reg.find("bp") != std::string::npos);

            if (op1IsMem) {
                Instruction286 loadOp1;
                loadOp1.mnemonic = "mov";
                loadOp1.operands.push_back("ax");
                loadOp1.operands.push_back("[" + op1Reg + "]");
                lowered.instructions.push_back(loadOp1);
                op1Reg = "ax";
            } else if (!op1IsConst && op1Reg != "ax") {
                Instruction286 movOp1;
                movOp1.mnemonic = "mov";
                movOp1.operands.push_back("ax");
                movOp1.operands.push_back(op1Reg);
                lowered.instructions.push_back(movOp1);
                op1Reg = "ax";
            }

            if (op2IsMem) {
                std::string tmpReg = (op1Reg == "cx") ? "dx" : "cx";
                Instruction286 loadOp2;
                loadOp2.mnemonic = "mov";
                loadOp2.operands.push_back(tmpReg);
                loadOp2.operands.push_back("[" + op2Reg + "]");
                lowered.instructions.push_back(loadOp2);
                op2Reg = tmpReg;
            }

            // CMP ax, op2
            Instruction286 cmpInst;
            cmpInst.mnemonic = "cmp";
            cmpInst.operands.push_back("ax");
            cmpInst.operands.push_back(op2Reg);
            lowered.instructions.push_back(cmpInst);

            // Set result based on predicate
            std::string trueLabel = state.nextLabel(".Lcmp_true_");
            std::string falseLabel = state.nextLabel(".Lcmp_false_");
            std::string endLabel = state.nextLabel(".Lcmp_end_");

            std::string jumpMnemonic;
            if (irInst.predicate == "eq") jumpMnemonic = "je";
            else if (irInst.predicate == "ne") jumpMnemonic = "jne";
            else if (irInst.predicate == "slt") jumpMnemonic = "jl";
            else if (irInst.predicate == "sle") jumpMnemonic = "jle";
            else if (irInst.predicate == "sgt") jumpMnemonic = "jg";
            else if (irInst.predicate == "sge") jumpMnemonic = "jge";
            else if (irInst.predicate == "ult") jumpMnemonic = "jb";
            else if (irInst.predicate == "ule") jumpMnemonic = "jbe";
            else if (irInst.predicate == "ugt") jumpMnemonic = "ja";
            else if (irInst.predicate == "uge") jumpMnemonic = "jae";

            if (!jumpMnemonic.empty()) {
                Instruction286 jmpFalse;
                jmpFalse.mnemonic = jumpMnemonic;
                jmpFalse.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpFalse);

                Instruction286 setFalse;
                setFalse.mnemonic = "mov";
                setFalse.operands.push_back("ax");
                setFalse.operands.push_back("0");
                lowered.instructions.push_back(setFalse);
                Instruction286 jmpEnd;
                jmpEnd.mnemonic = "jmp";
                jmpEnd.operands.push_back(endLabel);
                lowered.instructions.push_back(jmpEnd);

                loweredVec.push_back(lowered);
                lowered = LoweredInstruction{};

                LoweredInstruction trueLabelInst;
                trueLabelInst.label = trueLabel;
                loweredVec.push_back(trueLabelInst);

                Instruction286 setTrue;
                setTrue.mnemonic = "mov";
                setTrue.operands.push_back("ax");
                setTrue.operands.push_back("1");
                lowered.instructions.push_back(setTrue);

                loweredVec.push_back(lowered);
                lowered = LoweredInstruction{};

                LoweredInstruction endLabelInst;
                endLabelInst.label = endLabel;
                loweredVec.push_back(endLabelInst);
            }

            if (!irInst.resultName.empty()) {
                // Save result to stack temp to prevent clobbering by subsequent ops
                std::string tempSlot = state.frame.allocResultSlot(irInst.resultName, 2, false);
                Instruction286 saveResult;
                saveResult.mnemonic = "mov";
                saveResult.operands.push_back("[" + tempSlot + "]");
                saveResult.operands.push_back("ax");
                lowered.instructions.push_back(saveResult);
                state.frame.setPhysReg(irInst.resultName, tempSlot);
            }
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286
