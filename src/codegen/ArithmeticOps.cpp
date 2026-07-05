// Arithmetic Operations: Add, Sub, Mul, Div/Rem
// Extracted from InstructionSelect.cpp

#include "codegen/InstructionSelectInternal.h"

#include <string>
#include <cstdint>

namespace llvm_i286 {
namespace codegen {

std::vector<LoweredInstruction> lowerAdd(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Check if this is a 32-bit operation
    bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;

    if (is32) {
        // 32-bit add: add low words, adc high words
        if (irInst.operands.size() >= 2) {
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            // Mark result as 32-bit
            state.mark32Bit(irInst.resultName);

            // For 32-bit operations, always use stack locations
            std::string op1Stack = state.getPhysReg(op1Name);

            // Check if op2 is a vreg (exists in state maps)
            bool op2IsVreg = state.vregToPhys.find(op2Name) != state.vregToPhys.end() ||
                             state.vregToStackOffset.find(op2Name) != state.vregToStackOffset.end();
            
            // Check if op2 is a constant (only if not a vreg)
            bool op2IsConst = !op2IsVreg && state.isConstant(op2Name);
            std::string op2Stack;
            if (!op2IsConst) {
                op2Stack = state.getPhysReg(op2Name);
            }

            // Load op1 low word to AX, high word to BX
            Instruction286 loadLow;
            loadLow.mnemonic = "mov";
            loadLow.operands.push_back("ax");
            loadLow.operands.push_back("[" + op1Stack + "]");
            lowered.instructions.push_back(loadLow);

            Instruction286 loadHigh;
            loadHigh.mnemonic = "mov";
            loadHigh.operands.push_back("bx");
            loadHigh.operands.push_back("[" + op1Stack + "+2]");
            lowered.instructions.push_back(loadHigh);

            // Load op2 low word to CX, high word to DX
            Instruction286 loadOp2Low;
            loadOp2Low.mnemonic = "mov";
            if (op2IsConst) {
                // Load constant low word
                int64_t constVal = std::stoll(op2Name);
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back(std::to_string(constVal & 0xFFFF));
            } else {
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back("[" + op2Stack + "]");
            }
            lowered.instructions.push_back(loadOp2Low);

            Instruction286 loadOp2High;
            loadOp2High.mnemonic = "mov";
            if (op2IsConst) {
                // Load constant high word (sign-extend for negative constants)
                int64_t constVal = std::stoll(op2Name);
                int16_t highWord = (constVal >> 16) & 0xFFFF;
                loadOp2High.operands.push_back("dx");
                loadOp2High.operands.push_back(std::to_string(highWord));
            } else {
                loadOp2High.operands.push_back("dx");
                loadOp2High.operands.push_back("[" + op2Stack + "+2]");
            }
            lowered.instructions.push_back(loadOp2High);

            // Add low words: AX = AX + CX
            Instruction286 addLow;
            addLow.mnemonic = "add";
            addLow.operands.push_back("ax");
            addLow.operands.push_back("cx");
            lowered.instructions.push_back(addLow);

            // Add high words with carry: BX = BX + DX + CF
            Instruction286 adcHigh;
            adcHigh.mnemonic = "adc";
            adcHigh.operands.push_back("bx");
            adcHigh.operands.push_back("dx");
            lowered.instructions.push_back(adcHigh);

            // Store result to stack (result is in BX:AX)
            if (!irInst.resultName.empty()) {
                std::string resultStack = state.getPhysReg(irInst.resultName);
                // If result doesn't have stack space, allocate it
                if (resultStack.find("bp") == std::string::npos) {
                    state.currentStackOffset -= 4;
                    int stackOffset = state.currentStackOffset;
                    state.vregToStackOffset[irInst.resultName] = stackOffset;
                    resultStack = "bp" + std::to_string(stackOffset);
                    Instruction286 subSp;
                    subSp.mnemonic = "sub";
                    subSp.operands.push_back("sp");
                    subSp.operands.push_back("4");
                    lowered.instructions.push_back(subSp);
                }
                Instruction286 storeLow;
                storeLow.mnemonic = "mov";
                storeLow.operands.push_back("[" + resultStack + "]");
                storeLow.operands.push_back("ax");
                lowered.instructions.push_back(storeLow);

                Instruction286 storeHigh;
                storeHigh.mnemonic = "mov";
                if (resultStack.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = resultStack.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    storeHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    storeHigh.operands.push_back("[" + resultStack + "+2]");
                }
                storeHigh.operands.push_back("bx");
                lowered.instructions.push_back(storeHigh);

                // Update vreg mapping to point to stack
                state.vregToPhys[irInst.resultName] = resultStack;
            }
        }
    } else {
        // 16-bit add (original code)
        Instruction286 addInst;
        addInst.mnemonic = "add";

        if (irInst.operands.size() >= 2) {
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            // Check if operands are vregs/parameters first
            bool op1IsVreg = (state.vregToPhys.find(op1Name) != state.vregToPhys.end());
            bool op2IsVreg = (state.vregToPhys.find(op2Name) != state.vregToPhys.end());

            // Check if operands are constants (not vregs)
            bool op1IsConst = false;
            bool op2IsConst = false;
            if (!op1IsVreg) {
                try { std::stoi(op1Name); op1IsConst = true; } catch (...) {}
            }
            if (!op2IsVreg) {
                try { std::stoi(op2Name); op2IsConst = true; } catch (...) {}
            }

            std::string op1 = op1IsConst ? op1Name : state.getPhysReg(op1Name);
            std::string op2 = op2IsConst ? op2Name : state.getPhysReg(op2Name);

            // Check if operands are memory locations (bp-relative)
            bool op1IsMem = !op1IsConst && (op1.find("bp") != std::string::npos);
            bool op2IsMem = !op2IsConst && (op2.find("bp") != std::string::npos);

            // Load from memory if needed
            if (op1IsMem) {
                Instruction286 loadInst;
                loadInst.mnemonic = "mov";
                loadInst.operands.push_back("ax");
                loadInst.operands.push_back("[" + op1 + "]");
                lowered.instructions.push_back(loadInst);
                op1 = "ax";
            }

            if (op2IsMem) {
                std::string tmpReg = (op1 == "cx") ? "dx" : "cx";
                Instruction286 loadTmp;
                loadTmp.mnemonic = "mov";
                loadTmp.operands.push_back(tmpReg);
                loadTmp.operands.push_back("[" + op2 + "]");
                lowered.instructions.push_back(loadTmp);
                op2 = tmpReg;
            }

            // Move op1 to AX if it's not already there (and not a constant)
            if (!op1IsConst && op1 != "ax") {
                Instruction286 movOp1;
                movOp1.mnemonic = "mov";
                movOp1.operands.push_back("ax");
                movOp1.operands.push_back(op1);
                lowered.instructions.push_back(movOp1);
            }

            // Now op1 is in AX (or is a constant already in op2 position)
            if (op1IsConst) {
                // If op1 is constant, we need to load it into AX first
                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back("ax");
                movConst.operands.push_back(op1Name);
                lowered.instructions.push_back(movConst);
                addInst.operands.push_back("ax");
                addInst.operands.push_back(op2);
            } else {
                addInst.operands.push_back("ax");
                addInst.operands.push_back(op2);
            }

            lowered.instructions.push_back(addInst);

            // Update register mapping - add writes to AX
            if (!irInst.resultName.empty()) {
                state.updateResultReg(irInst.resultName, "ax");
            }
        } else {
            // Fallback: add ax, bx
            addInst.operands.push_back("ax");
            addInst.operands.push_back("bx");
            lowered.instructions.push_back(addInst);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerSub(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Check if this is a 32-bit operation
    bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;

    if (is32) {
        // 32-bit sub: sub low words, sbb high words
        if (irInst.operands.size() >= 2) {
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            // Mark result as 32-bit
            state.mark32Bit(irInst.resultName);

            // Get stack locations for both operands
            std::string op1Stack = state.getPhysReg(op1Name);

            // Check if op2 is a constant
            bool op2IsConst = state.isConstant(op2Name);
            std::string op2Stack;
            if (!op2IsConst) {
                op2Stack = state.getPhysReg(op2Name);
            }

            // Load op1 low word to AX, high word to BX
            Instruction286 loadLow;
            loadLow.mnemonic = "mov";
            loadLow.operands.push_back("ax");
            loadLow.operands.push_back("[" + op1Stack + "]");
            lowered.instructions.push_back(loadLow);

            Instruction286 loadHigh;
            loadHigh.mnemonic = "mov";
            loadHigh.operands.push_back("bx");
            loadHigh.operands.push_back("[" + op1Stack + "+2]");
            lowered.instructions.push_back(loadHigh);

            // Load op2 low word to CX, high word to DX
            Instruction286 loadOp2Low;
            loadOp2Low.mnemonic = "mov";
            if (op2IsConst) {
                // Load constant low word
                int64_t constVal = std::stoll(op2Name);
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back(std::to_string(constVal & 0xFFFF));
            } else {
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back("[" + op2Stack + "]");
            }
            lowered.instructions.push_back(loadOp2Low);

            Instruction286 loadOp2High;
            loadOp2High.mnemonic = "mov";
            if (op2IsConst) {
                // Load constant high word (sign-extend for negative constants)
                int64_t constVal = std::stoll(op2Name);
                int16_t highWord = (constVal >> 16) & 0xFFFF;
                loadOp2High.operands.push_back("dx");
                loadOp2High.operands.push_back(std::to_string(highWord));
            } else {
                loadOp2High.operands.push_back("dx");
                loadOp2High.operands.push_back("[" + op2Stack + "+2]");
            }
            lowered.instructions.push_back(loadOp2High);

            // Sub low words: AX = AX - CX
            Instruction286 subLow;
            subLow.mnemonic = "sub";
            subLow.operands.push_back("ax");
            subLow.operands.push_back("cx");
            lowered.instructions.push_back(subLow);

            // Sub high words with borrow: BX = BX - DX - CF
            Instruction286 sbbHigh;
            sbbHigh.mnemonic = "sbb";
            sbbHigh.operands.push_back("bx");
            sbbHigh.operands.push_back("dx");
            lowered.instructions.push_back(sbbHigh);

            // Store result to stack (result is in BX:AX)
            if (!irInst.resultName.empty()) {
                std::string resultStack = state.getPhysReg(irInst.resultName);
                // Allocate stack space if needed
                if (resultStack.find("bp") == std::string::npos) {
                    state.currentStackOffset -= 4;
                    int stackOffset = state.currentStackOffset;
                    state.vregToStackOffset[irInst.resultName] = stackOffset;
                    resultStack = "bp" + std::to_string(stackOffset);
                    Instruction286 subSp;
                    subSp.mnemonic = "sub";
                    subSp.operands.push_back("sp");
                    subSp.operands.push_back("4");
                    lowered.instructions.push_back(subSp);
                }
                Instruction286 storeLow;
                storeLow.mnemonic = "mov";
                storeLow.operands.push_back("[" + resultStack + "]");
                storeLow.operands.push_back("ax");
                lowered.instructions.push_back(storeLow);

                Instruction286 storeHigh;
                storeHigh.mnemonic = "mov";
                if (resultStack.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = resultStack.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    storeHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    storeHigh.operands.push_back("[" + resultStack + "+2]");
                }
                storeHigh.operands.push_back("bx");
                lowered.instructions.push_back(storeHigh);

                // Update vreg mapping
                state.vregToPhys[irInst.resultName] = resultStack;
            }
        }
    } else {
        // 16-bit sub (original code)
        Instruction286 subInst;
        subInst.mnemonic = "sub";

        if (irInst.operands.size() >= 2) {
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            // Check if operands are vregs/parameters first
            bool op1IsVreg = (state.vregToPhys.find(op1Name) != state.vregToPhys.end());
            bool op2IsVreg = (state.vregToPhys.find(op2Name) != state.vregToPhys.end());

            // Check if operands are constants (not vregs)
            bool op1IsConst = false;
            bool op2IsConst = false;
            if (!op1IsVreg) {
                try { std::stoi(op1Name); op1IsConst = true; } catch (...) {}
            }
            if (!op2IsVreg) {
                try { std::stoi(op2Name); op2IsConst = true; } catch (...) {}
            }

            std::string op1 = op1IsConst ? op1Name : state.getPhysReg(op1Name);
            std::string op2 = op2IsConst ? op2Name : state.getPhysReg(op2Name);

            // Move op1 to resultReg if it's not already there
            if (!op1IsConst && resultReg != op1) {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back(resultReg);
                movInst.operands.push_back(op1);
                lowered.instructions.push_back(movInst);
            } else if (op1IsConst) {
                // Load constant into resultReg
                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back(resultReg);
                movConst.operands.push_back(op1Name);
                lowered.instructions.push_back(movConst);
            }

            subInst.operands.push_back(resultReg);
            subInst.operands.push_back(op2);

            // Update register mapping - sub writes to resultReg
            if (!irInst.resultName.empty()) {
                state.updateResultReg(irInst.resultName, resultReg);
            }
        } else {
            subInst.operands.push_back("ax");
            subInst.operands.push_back("bx");
        }

        lowered.instructions.push_back(subInst);
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerMul(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Lower mul: result = operand1 * operand2
    bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;

    if (is32) {
        // 32-bit multiplication using shift-and-add algorithm
        // Note: This is a simplified version that calls a runtime function
        if (irInst.operands.size() >= 2) {
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            state.mark32Bit(irInst.resultName);

            // Get stack locations
            std::string op1Stack = state.getPhysReg(op1Name);
            std::string op2Stack = state.getPhysReg(op2Name);

            // Allocate result stack space
            std::string resultStack = state.getPhysReg(irInst.resultName);
            if (resultStack.find("bp") == std::string::npos) {
                state.currentStackOffset -= 4;
                int stackOffset = state.currentStackOffset;
                state.vregToStackOffset[irInst.resultName] = stackOffset;
                resultStack = "bp" + std::to_string(stackOffset);
                Instruction286 subSp;
                subSp.mnemonic = "sub";
                subSp.operands.push_back("sp");
                subSp.operands.push_back("4");
                lowered.instructions.push_back(subSp);
            }

            // Load op1 into AX:BX
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

            // Load op2 into CX:DX
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

            // Call multiplication routine
            // Result will be in DI:SI
            Instruction286 callMul;
            callMul.mnemonic = "call";
            callMul.operands.push_back("far _MultiplyI32");
            lowered.instructions.push_back(callMul);

            // Store result to stack
            // _MultiplyI32 returns: si = high word (MSB), di = low word (LSB)
            Instruction286 storeResultLow;
            storeResultLow.mnemonic = "mov";
            storeResultLow.operands.push_back("[" + resultStack + "]");
            storeResultLow.operands.push_back("di");
            lowered.instructions.push_back(storeResultLow);

            Instruction286 storeResultHigh;
            storeResultHigh.mnemonic = "mov";
            if (resultStack.find("bp") != std::string::npos) {
                int offset = 0;
                std::string offsetStr = resultStack.substr(2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                storeResultHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else {
                storeResultHigh.operands.push_back("[" + resultStack + "+2]");
            }
            storeResultHigh.operands.push_back("si");
            lowered.instructions.push_back(storeResultHigh);

            // Update vreg mapping
            state.vregToPhys[irInst.resultName] = resultStack;
        }
    } else {
        // 16-bit multiplication
        Instruction286 imulInst;
        imulInst.mnemonic = "imul";

        if (irInst.operands.size() >= 2) {
            std::string op1 = state.getPhysReg(irInst.operands[0].name);
            std::string op2 = state.getPhysReg(irInst.operands[1].name);

            // Move first operand to AX
            if (op1 != "ax") {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back("ax");
                movInst.operands.push_back(op1);
                lowered.instructions.push_back(movInst);
            }

            // IMUL second operand
            imulInst.operands.push_back(op2);
        } else {
            imulInst.operands.push_back("bx");
        }

        lowered.instructions.push_back(imulInst);

        // Result is in AX (low word) and DX (high word)
        if (resultReg != "ax") {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(resultReg);
            movInst.operands.push_back("ax");
            lowered.instructions.push_back(movInst);
        }

        // Update register mapping - mul writes to AX (low word)
        if (!irInst.resultName.empty()) {
            state.updateResultReg(irInst.resultName, "ax");
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerDivRem(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Division and remainder operations
    // 80286 IDIV/DIV: AX = dividend, divisor in operand
    // Result: AX = quotient, DX = remainder
    bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;

    if (irInst.operands.size() >= 2) {
        std::string dividentName = irInst.operands[0].name;
        std::string divisorName = irInst.operands[1].name;

        if (is32) {
            // 32-bit division using tested runtime helper
            // Tested runtime convention (from runtime/div32.asm):
            //   Input:  ax:bx = dividend (low:high)
            //           cx:dx = divisor (low:high)
            //   Output: di:si = quotient (high:low)
            //   Uses retf (far return)

            // Load dividend low word to AX, high word to BX
            // load32Bit loads to AX:DX, so we need to move DX to BX
            auto dividendLoads = state.load32Bit(dividentName);
            lowered.instructions.insert(lowered.instructions.end(), dividendLoads.begin(), dividendLoads.end());

            // Move dividend low word from AX to... wait, load32Bit loads to AX:DX
            // We need to move DX to BX for the tested runtime convention
            Instruction286 movDivHigh;
            movDivHigh.mnemonic = "mov";
            movDivHigh.operands.push_back("bx");
            movDivHigh.operands.push_back("dx");
            lowered.instructions.push_back(movDivHigh);

            // Load divisor low word to CX, high word to DX
            std::string divisorReg = state.getPhysReg(divisorName);
            if (divisorReg.find("bp") != std::string::npos) {
                Instruction286 loadDivisorLow;
                loadDivisorLow.mnemonic = "mov";
                loadDivisorLow.operands.push_back("cx");
                loadDivisorLow.operands.push_back("[" + divisorReg + "]");
                lowered.instructions.push_back(loadDivisorLow);

                Instruction286 loadDivisorHigh;
                loadDivisorHigh.mnemonic = "mov";
                int offset = 0;
                std::string offsetStr = divisorReg.substr(divisorReg.find("bp") + 2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                loadDivisorHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                loadDivisorHigh.operands.push_back("dx");
                lowered.instructions.push_back(loadDivisorHigh);
            } else if (divisorReg != "cx") {
                Instruction286 movDivisorLow;
                movDivisorLow.mnemonic = "mov";
                movDivisorLow.operands.push_back("cx");
                movDivisorLow.operands.push_back(divisorReg);
                lowered.instructions.push_back(movDivisorLow);

                // Load high word
                Instruction286 movDivisorHigh;
                movDivisorHigh.mnemonic = "mov";
                movDivisorHigh.operands.push_back("dx");
                movDivisorHigh.operands.push_back("[" + divisorReg + "+2]");
                lowered.instructions.push_back(movDivisorHigh);
            }

            // Call division routine (far call, tested runtime)
            Instruction286 callDiv;
            callDiv.mnemonic = "call";
            std::string divFuncName = (irInst.opcode == ir::Opcode::SDiv || irInst.opcode == ir::Opcode::SRem) ? "_DivideI32" : "_DivideU32";
            callDiv.operands.push_back("far " + divFuncName);
            lowered.instructions.push_back(callDiv);

            // Result depends on operation:
            // Division (SDiv/UDiv): DI:SI (DI=high, SI=low) = quotient
            // Remainder (SRem/URem): BX:AX (BX=high, AX=low) = remainder
            if (!irInst.resultName.empty()) {
                std::string resultStack = state.alloc32BitStack(irInst.resultName);
                bool isRemainder = (irInst.opcode == ir::Opcode::SRem || irInst.opcode == ir::Opcode::URem);

                if (isRemainder) {
                    // Store AX (low word of remainder)
                    Instruction286 storeResultLow;
                    storeResultLow.mnemonic = "mov";
                    storeResultLow.operands.push_back("[" + resultStack + "]");
                    storeResultLow.operands.push_back("ax");
                    lowered.instructions.push_back(storeResultLow);

                    // Store BX (high word of remainder)
                    Instruction286 storeResultHigh;
                    storeResultHigh.mnemonic = "mov";
                    if (resultStack.find("bp") != std::string::npos) {
                        int offset = 0;
                        std::string offsetStr = resultStack.substr(2);
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
                        int newOffset = offset + 2;
                        std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                        storeResultHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                    } else {
                        storeResultHigh.operands.push_back("[" + resultStack + "+2]");
                    }
                    storeResultHigh.operands.push_back("bx");
                    lowered.instructions.push_back(storeResultHigh);
                } else {
                    // _DivideI32 returns: si = high word (MSB), di = low word (LSB)
                    // Store DI (low word of quotient)
                    Instruction286 storeResultLow;
                    storeResultLow.mnemonic = "mov";
                    storeResultLow.operands.push_back("[" + resultStack + "]");
                    storeResultLow.operands.push_back("di");
                    lowered.instructions.push_back(storeResultLow);

                    // Store SI (high word of quotient)
                    Instruction286 storeResultHigh;
                    storeResultHigh.mnemonic = "mov";
                    if (resultStack.find("bp") != std::string::npos) {
                        int offset = 0;
                        std::string offsetStr = resultStack.substr(2);
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
                        int newOffset = offset + 2;
                        std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                        storeResultHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                    } else {
                        storeResultHigh.operands.push_back("[" + resultStack + "+2]");
                    }
                    storeResultHigh.operands.push_back("si");
                    lowered.instructions.push_back(storeResultHigh);
                }

                state.vregToPhys[irInst.resultName] = resultStack;
            }
        } else {
            // 16-bit division
            // Load dividend to AX
            std::string dividentReg = state.getPhysReg(dividentName);
            if (dividentReg.find("bp") != std::string::npos) {
                Instruction286 loadDividend;
                loadDividend.mnemonic = "mov";
                loadDividend.operands.push_back("ax");
                loadDividend.operands.push_back("[" + dividentReg + "]");
                lowered.instructions.push_back(loadDividend);
            } else if (dividentReg != "ax") {
                Instruction286 movDividend;
                movDividend.mnemonic = "mov";
                movDividend.operands.push_back("ax");
                movDividend.operands.push_back(dividentReg);
                lowered.instructions.push_back(movDividend);
            }

            // Load divisor to CX
            std::string divisorReg = state.getPhysReg(divisorName);
            if (divisorReg.find("bp") != std::string::npos) {
                Instruction286 loadDivisor;
                loadDivisor.mnemonic = "mov";
                loadDivisor.operands.push_back("cx");
                loadDivisor.operands.push_back("[" + divisorReg + "]");
                lowered.instructions.push_back(loadDivisor);
            } else if (divisorReg != "cx") {
                Instruction286 movDivisor;
                movDivisor.mnemonic = "mov";
                movDivisor.operands.push_back("cx");
                movDivisor.operands.push_back(divisorReg);
                lowered.instructions.push_back(movDivisor);
            }

            // Clear DX for unsigned division, sign-extend for signed
            if (irInst.opcode == ir::Opcode::SDiv || irInst.opcode == ir::Opcode::SRem) {
                Instruction286 cwd;
                cwd.mnemonic = "cwd"; // Sign-extend AX to DX:AX
                lowered.instructions.push_back(cwd);
            } else {
                Instruction286 xorDx;
                xorDx.mnemonic = "xor";
                xorDx.operands.push_back("dx");
                xorDx.operands.push_back("dx");
                lowered.instructions.push_back(xorDx);
            }

            // Perform division
            Instruction286 divInst;
            divInst.mnemonic = irInst.opcode == ir::Opcode::SDiv || irInst.opcode == ir::Opcode::SRem ? "idiv" : "div";
            divInst.operands.push_back("cx");
            lowered.instructions.push_back(divInst);

            // Result: For div/rem, quotient in AX, remainder in DX
            // Store result based on operation type
            std::string divResultReg = (irInst.opcode == ir::Opcode::SDiv || irInst.opcode == ir::Opcode::UDiv) ? "ax" : "dx";

            if (!irInst.resultName.empty()) {
                state.updateResultReg(irInst.resultName, divResultReg);
            }
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286