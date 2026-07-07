// Control Flow Operations - Instruction Selection
// Handlers for control flow IR instructions: RetTerm, BrTerm, CondBrTerm,
// Switch, Phi, Unreachable, Select, ICmp

#include "codegen/InstructionSelectInternal.h"

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

        // Check if return value is a vreg/parameter
        bool isVreg = (state.vregToPhys.find(retValName) != state.vregToPhys.end());

        // Check if return value is a constant (not a vreg)
        bool isConst = false;
        if (!isVreg) {
            try {
                std::stoi(retValName);
                isConst = true;
            } catch (...) {}
        }

        // Check if this is a 32-bit return value
        bool is32 = state.is32BitReg(retValName) ||
                    (irInst.resultType && irInst.resultType->bitWidth == 32);

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
        } else {
            // Check if value is in vregToPhys (could be register or stack location)
            auto physIt = state.vregToPhys.find(retValName);
            bool foundInPhys = physIt != state.vregToPhys.end();
            
            // Also check if value is on stack
            auto stackIt = state.vregToStackOffset.find(retValName);
            bool foundOnStack = stackIt != state.vregToStackOffset.end();
            
            if (foundOnStack) {
                // Value is on stack - load from memory
                std::string retValReg = state.getPhysReg(retValName);
                Instruction286 movLow;
                movLow.mnemonic = "mov";
                movLow.operands.push_back("ax");
                movLow.operands.push_back("[" + retValReg + "]");
                lowered.instructions.push_back(movLow);
                if (is32) {
                    // Load high word from [bp-offset+2]
                    int offset = 0;
                    std::string offsetStr = retValReg.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    Instruction286 movHigh;
                    movHigh.mnemonic = "mov";
                    movHigh.operands.push_back("dx");
                    movHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                    lowered.instructions.push_back(movHigh);
                }
       } else if (foundInPhys) {
            // Value is in vregToPhys (could be register or stack location)
            std::string physReg = physIt->second;
            if (physReg.find("bp") != std::string::npos) {
                // Stack location stored in vregToPhys - load from memory
                Instruction286 movLow;
                movLow.mnemonic = "mov";
                movLow.operands.push_back("ax");
                movLow.operands.push_back("[" + physReg + "]");
                lowered.instructions.push_back(movLow);
                if (is32) {
                    int offset = 0;
                    std::string offsetStr = physReg.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    Instruction286 movHigh;
                    movHigh.mnemonic = "mov";
                    movHigh.operands.push_back("dx");
                    movHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                    lowered.instructions.push_back(movHigh);
                }
            } else if (physReg != "ax") {
                // Register - move to AX
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back("ax");
                movInst.operands.push_back(physReg);
                lowered.instructions.push_back(movInst);
            }
            if (is32) {
                // For 32-bit in register, DX should have high word
            }
        }
        }
    }

    // Deallocate alloca stack space before restoring saved registers
    // allocaEndOffset is the stack offset after all allocas (before any temp space)
    // totalLocals = -(allocaEndOffset + 6) gives the alloca-only space
    int totalLocals = -(state.allocaEndOffset + 6);
    if (totalLocals > 0) {
        Instruction286 addSp;
        addSp.mnemonic = "add";
        addSp.operands.push_back("sp");
        addSp.operands.push_back(std::to_string(totalLocals));
        lowered.instructions.push_back(addSp);
    }

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

    // Emit ret
    Instruction286 retInst;
    retInst.mnemonic = "ret";
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
        std::string prefix = "";
        if (state.currentFunc && !state.currentFunc->name.empty()) {
            prefix = state.currentFunc->name + "_";
        }
        std::string trueLabel = prefix + "bb_" + irInst.operands[1].name;
        std::string falseLabel = prefix + "bb_" + irInst.operands[2].name;

        // Check if cond is a vreg/parameter
        bool condIsVreg = (state.vregToPhys.find(condName) != state.vregToPhys.end());

        // Check if cond is a constant (not a vreg)
        bool isConst = false;
        if (!condIsVreg) {
            try {
                std::stoi(condName);
                isConst = true;
            } catch (...) {}
        }

        std::string condReg = isConst ? condName : state.getPhysReg(condName);

        if (isConst) {
            // Constant condition: always true or always false
            if (condReg == "1") {
                Instruction286 jmpInst;
                jmpInst.mnemonic = "jmp";
                jmpInst.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpInst);
            } else {
                Instruction286 jmpInst;
                jmpInst.mnemonic = "jmp";
                jmpInst.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpInst);
            }
        } else {
            // Test the condition (check if AX is 0 or non-zero)
            // Use TEST AX, AX to set flags without changing AX
            Instruction286 testInst;
            testInst.mnemonic = "test";
            testInst.operands.push_back("ax");
            testInst.operands.push_back("ax");
            lowered.instructions.push_back(testInst);

            // Jump to true if non-zero (ZF=0), use short jnz to avoid je rel16 issues
            std::string nearTrueLabel = state.nextLabel(".Lnear_true_");
            Instruction286 jnzNear;
            jnzNear.mnemonic = "jnz";
            jnzNear.operands.push_back(nearTrueLabel);
            lowered.instructions.push_back(jnzNear);

            // Fall through: condition is false, jump to false label
            Instruction286 jmpFalse;
            jmpFalse.mnemonic = "jmp";
            jmpFalse.operands.push_back(falseLabel);
            lowered.instructions.push_back(jmpFalse);

            // Near true label
            LoweredInstruction nearTrueInst;
            nearTrueInst.label = nearTrueLabel;
            loweredVec.push_back(lowered);
            lowered = LoweredInstruction{};
            loweredVec.push_back(nearTrueInst);

            // Jump to true label (long jump if needed)
            Instruction286 jmpTrue;
            jmpTrue.mnemonic = "jmp";
            jmpTrue.operands.push_back(trueLabel);
            lowered.instructions.push_back(jmpTrue);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerSwitch(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Switch statement - emit as chained comparisons
    if (irInst.operands.size() >= 1) {
        std::string condReg = state.getPhysReg(irInst.operands[0].name);

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

        // Emit case comparisons
        std::string prefix = "";
        if (state.currentFunc && !state.currentFunc->name.empty()) {
            prefix = state.currentFunc->name + "_";
        }
        std::string defaultLabel = prefix + "bb_" + irInst.operands[0].name; // Default case
        std::string endLabel = state.nextLabel(".Lswitch_end_");

        for (size_t i = 1; i < irInst.operands.size(); i += 2) {
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

        // Jump to default
        Instruction286 jmpDefault;
        jmpDefault.mnemonic = "jmp";
        jmpDefault.operands.push_back(defaultLabel);
        lowered.instructions.push_back(jmpDefault);

        // End label
        LoweredInstruction endLabelInst;
        endLabelInst.label = endLabel;
        loweredVec.push_back(lowered);
        lowered = LoweredInstruction{};
        loweredVec.push_back(endLabelInst);
    }

    return loweredVec;
}

std::vector<LoweredInstruction> lowerPhi(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Phi nodes are handled at the basic block level, not here
    // This is a no-op in instruction selection

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerUnreachable(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Emit a halt or trap instruction
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

    // Select: result = select i1 cond, type val1, type val2
    // If cond is true, result = val1; else result = val2
    if (irInst.operands.size() >= 3) {
        std::string condReg = state.getPhysReg(irInst.operands[0].name);
        std::string val1Reg = state.getPhysReg(irInst.operands[1].name);
        std::string val2Reg = state.getPhysReg(irInst.operands[2].name);

        std::string destReg = resultReg.empty() ? "ax" : resultReg;

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

        // False case: load val2
        Instruction286 loadFalse;
        loadFalse.mnemonic = "mov";
        loadFalse.operands.push_back(destReg);
        loadFalse.operands.push_back(val2Reg);
        lowered.instructions.push_back(loadFalse);
        Instruction286 jmpEnd;
        jmpEnd.mnemonic = "jmp";
        jmpEnd.operands.push_back(endLabel);
        lowered.instructions.push_back(jmpEnd);

        // Push current lowered and create new ones for labels
        loweredVec.push_back(lowered);
        lowered = LoweredInstruction{};

        // True case label
        LoweredInstruction trueLabelInst;
        trueLabelInst.label = trueLabel;
        loweredVec.push_back(trueLabelInst);

        // True case: load val1
        Instruction286 loadTrue;
        loadTrue.mnemonic = "mov";
        loadTrue.operands.push_back(destReg);
        loadTrue.operands.push_back(val1Reg);
        lowered.instructions.push_back(loadTrue);

        // Push and create for end label
        loweredVec.push_back(lowered);
        lowered = LoweredInstruction{};

        // End label
        LoweredInstruction endLabelInst;
        endLabelInst.label = endLabel;
        loweredVec.push_back(endLabelInst);

        if (!irInst.resultName.empty()) {
            state.updateResultReg(irInst.resultName, destReg);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerICmp(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Comparison: icmp predicate type op1, op2
    // Sets result to 0 or 1 based on comparison
    // On 286: CMP op1, op2; then conditional set

      if (irInst.operands.size() >= 2 && !irInst.predicate.empty()) {
        // Check if this is a 32-bit comparison
        bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;

        if (is32) {
            // 32-bit comparison: compare high words first, then low words if equal
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            bool op1IsConst = false;
            bool op2IsConst = false;
            try { std::stoi(op1Name); op1IsConst = true; } catch (...) {}
            try { std::stoi(op2Name); op2IsConst = true; } catch (...) {}

            bool op1IsVreg = !op1IsConst && (state.vregToPhys.find(op1Name) != state.vregToPhys.end());
            bool op2IsVreg = !op2IsConst && (state.vregToPhys.find(op2Name) != state.vregToPhys.end());

            std::string op1Stack = op1IsConst ? "" : state.getPhysReg(op1Name);
            std::string op2Stack = op2IsConst ? "" : state.getPhysReg(op2Name);

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
                Instruction286 loadOp1Low;
                loadOp1Low.mnemonic = "mov";
                loadOp1Low.operands.push_back("ax");
                loadOp1Low.operands.push_back("[" + op1Stack + "]");
                lowered.instructions.push_back(loadOp1Low);

                Instruction286 loadOp1High;
                loadOp1High.mnemonic = "mov";
                loadOp1High.operands.push_back("bx");
                if (op1Stack.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = op1Stack.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    loadOp1High.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    loadOp1High.operands.push_back("[" + op1Stack + "+2]");
                }
                lowered.instructions.push_back(loadOp1High);
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
                Instruction286 loadOp2Low;
                loadOp2Low.mnemonic = "mov";
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back("[" + op2Stack + "]");
                lowered.instructions.push_back(loadOp2Low);

                Instruction286 loadOp2High;
                loadOp2High.mnemonic = "mov";
                loadOp2High.operands.push_back("dx");
                if (op2Stack.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = op2Stack.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    loadOp2High.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    loadOp2High.operands.push_back("[" + op2Stack + "+2]");
                }
                lowered.instructions.push_back(loadOp2High);
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

            // For signed comparisons (slt, sle, sgt, sge):
            // - If high words not equal, signed comparison of high words determines result
            // - If high words equal, unsigned comparison of low words determines result

            // Jump to compare low words if high words are equal
            Instruction286 jmpCmpLow;
            jmpCmpLow.mnemonic = "je";
            jmpCmpLow.operands.push_back(cmpLowLabel);
            lowered.instructions.push_back(jmpCmpLow);

            // High words differ - use signed comparison on high words
            std::string jumpMnemonic;
            if (irInst.predicate == "slt") jumpMnemonic = "jl";
            else if (irInst.predicate == "sle") jumpMnemonic = "jle";
            else if (irInst.predicate == "sgt") jumpMnemonic = "jg";
            else if (irInst.predicate == "sge") jumpMnemonic = "jge";
            else if (irInst.predicate == "ult") jumpMnemonic = "jb";
            else if (irInst.predicate == "ule") jumpMnemonic = "jbe";
            else if (irInst.predicate == "ugt") jumpMnemonic = "ja";
            else if (irInst.predicate == "uge") jumpMnemonic = "jae";
            else if (irInst.predicate == "eq") jumpMnemonic = "je";
            else if (irInst.predicate == "ne") jumpMnemonic = "jne";

            if (!jumpMnemonic.empty()) {
                Instruction286 jmpTrue;
                jmpTrue.mnemonic = jumpMnemonic;
                jmpTrue.operands.push_back(trueLabel);
                lowered.instructions.push_back(jmpTrue);

                Instruction286 jmpFalse;
                jmpFalse.mnemonic = "jmp";
                jmpFalse.operands.push_back(falseLabel);
                lowered.instructions.push_back(jmpFalse);
            }

            // Push current lowered and create label for low word comparison
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

            // True label
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

            // False label
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

            // End label
            LoweredInstruction endLabelInst;
            endLabelInst.label = endLabel;
            loweredVec.push_back(endLabelInst);

            // Result is in AX
            if (!irInst.resultName.empty()) {
                state.updateResultReg(irInst.resultName, "ax");
            }
        } else {
            // 16-bit comparison (original code)
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            // Check if operands are constants first (constants take precedence over vregs)
            bool op1IsConst = false;
            bool op2IsConst = false;
            try { std::stoi(op1Name); op1IsConst = true; } catch (...) {}
            try { std::stoi(op2Name); op2IsConst = true; } catch (...) {}

            // Check if operands are vregs/parameters (only if not constants)
            bool op1IsVreg = !op1IsConst && (state.vregToPhys.find(op1Name) != state.vregToPhys.end());
            bool op2IsVreg = !op2IsConst && (state.vregToPhys.find(op2Name) != state.vregToPhys.end());

            std::string op1Reg = op1IsConst ? op1Name : state.getPhysReg(op1Name);
            std::string op2Reg = op2IsConst ? op2Name : state.getPhysReg(op2Name);

            // Load operands into registers if they're memory locations
            bool op1IsMem = !op1IsConst && (op1Reg.find("bp") != std::string::npos);
            bool op2IsMem = !op2IsConst && (op2Reg.find("bp") != std::string::npos);

            // If op1 is already in ax but it's actually a stack location, load it
            if (op1Reg == "ax" && op1IsMem) {
                Instruction286 loadOp1;
                loadOp1.mnemonic = "mov";
                loadOp1.operands.push_back("ax");
                loadOp1.operands.push_back("[" + op1Reg + "]");
                lowered.instructions.push_back(loadOp1);
            } else if (op1IsMem) {
                Instruction286 loadOp1;
                loadOp1.mnemonic = "mov";
                loadOp1.operands.push_back("ax");
                loadOp1.operands.push_back("[" + op1Reg + "]");
                lowered.instructions.push_back(loadOp1);
                op1Reg = "ax";
            }

            if (op2IsMem) {
                // Use CX as temp if AX is already used for op1
                std::string tmpReg = (op1Reg == "cx") ? "dx" : "cx";
                Instruction286 loadOp2;
                loadOp2.mnemonic = "mov";
                loadOp2.operands.push_back(tmpReg);
                loadOp2.operands.push_back("[" + op2Reg + "]");
                lowered.instructions.push_back(loadOp2);
                op2Reg = tmpReg;
            }

            // Move op1 to AX for CMP (unless it's already AX or a constant)
            if (!op1IsConst && op1Reg != "ax") {
                Instruction286 movOp1;
                movOp1.mnemonic = "mov";
                movOp1.operands.push_back("ax");
                movOp1.operands.push_back(op1Reg);
                lowered.instructions.push_back(movOp1);
            }

            // CMP ax, op2
            Instruction286 cmpInst;
            cmpInst.mnemonic = "cmp";
            cmpInst.operands.push_back("ax");
            cmpInst.operands.push_back(op2Reg);
            lowered.instructions.push_back(cmpInst);

        // Set result based on predicate
        // Use conditional jumps to set AX to 0 or 1
        std::string trueLabel = state.nextLabel(".Lcmp_true_");
        std::string falseLabel = state.nextLabel(".Lcmp_false_");
        std::string endLabel = state.nextLabel(".Lcmp_end_");

        // Map LLVM predicate to 286 jump condition
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

            // False case: set ax = 0
            Instruction286 setFalse;
            setFalse.mnemonic = "mov";
            setFalse.operands.push_back("ax");
            setFalse.operands.push_back("0");
            lowered.instructions.push_back(setFalse);
            Instruction286 jmpEnd;
            jmpEnd.mnemonic = "jmp";
            jmpEnd.operands.push_back(endLabel);
            lowered.instructions.push_back(jmpEnd);

            // Push current lowered and create new ones for labels
            loweredVec.push_back(lowered);
            lowered = LoweredInstruction{};

            // True case label
            LoweredInstruction trueLabelInst;
            trueLabelInst.label = trueLabel;
            loweredVec.push_back(trueLabelInst);

            // True case: set ax = 1
            Instruction286 setTrue;
            setTrue.mnemonic = "mov";
            setTrue.operands.push_back("ax");
            setTrue.operands.push_back("1");
            lowered.instructions.push_back(setTrue);

            // Push and create for end label
            loweredVec.push_back(lowered);
            lowered = LoweredInstruction{};

            // End label
            LoweredInstruction endLabelInst;
            endLabelInst.label = endLabel;
            loweredVec.push_back(endLabelInst);
        }

        // Result is in AX
        if (!irInst.resultName.empty()) {
            state.updateResultReg(irInst.resultName, "ax");
        }
    }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286
