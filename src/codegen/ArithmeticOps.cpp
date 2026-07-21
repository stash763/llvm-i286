// Arithmetic Operations: Add, Sub, Mul, Div/Rem
// Extracted from InstructionSelect.cpp

#include "codegen/InstructionSelectInternal.h"

#include <string>
#include <cstdint>

namespace llvm_i286 {
namespace codegen {

// Helper: if a vreg is mapped to ax/cx/dx, move it to bx and return "bx".
// In 16-bit mode only bx/si/di/bp may appear in a memory operand.
static std::string safeMemAddr(const std::string& reg, std::vector<Instruction286>& out) {
    if (reg == "ax" || reg == "cx" || reg == "dx") {
        Instruction286 m;
        m.mnemonic = "mov";
        m.operands.push_back("bx");
        m.operands.push_back(reg);
        out.push_back(m);
        return "bx";
    }
    return reg;
}

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
            // 32-bit tracking now in StackFrame

            // Check if op1 is a constant
            bool op1IsConst = irInst.operands[0].ref.isConstant();
            std::string op1Stack;
            if (!op1IsConst) {
                op1Stack = state.frame.getPhysReg(op1Name);
            }

            // Check if op2 is a vreg (exists in state maps)
            bool op2IsVreg = state.frame.hasSlot(op2Name);

            // Check if op2 is a constant (only if not a vreg)
            bool op2IsConst = !op2IsVreg && irInst.operands[1].ref.isConstant();
            std::string op2Stack;
            if (!op2IsConst) {
                op2Stack = state.frame.getPhysReg(op2Name);
            }

            // Load op1 low word to AX, high word to BX
            Instruction286 loadLow;
            loadLow.mnemonic = "mov";
            if (op1IsConst) {
                int64_t constVal = irInst.operands[0].ref.constValue;
                loadLow.operands.push_back("ax");
                loadLow.operands.push_back(std::to_string(constVal & 0xFFFF));
            } else {
                std::string op1Addr = safeMemAddr(op1Stack, lowered.instructions);
                loadLow.operands.push_back("ax");
                loadLow.operands.push_back("[" + op1Addr + "]");
            }
            lowered.instructions.push_back(loadLow);

            Instruction286 loadHigh;
            loadHigh.mnemonic = "mov";
            loadHigh.operands.push_back("bx");
            if (op1IsConst) {
                int64_t constVal = irInst.operands[0].ref.constValue;
                int16_t highWord = (constVal >> 16) & 0xFFFF;
                loadHigh.operands.push_back(std::to_string(highWord));
            } else {
                std::string op1Addr = safeMemAddr(op1Stack, lowered.instructions);
                if (op1Addr.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = op1Addr.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    loadHigh.operands.push_back("[" + op1Addr + "+2]");
                }
            }
            lowered.instructions.push_back(loadHigh);

            // Load op2 low word to CX, high word to DX
            std::string op2Addr = safeMemAddr(op2Stack, lowered.instructions);
            
            Instruction286 loadOp2Low;
            loadOp2Low.mnemonic = "mov";
            if (op2IsConst) {
                int64_t constVal = irInst.operands[1].ref.constValue;
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back(std::to_string(constVal & 0xFFFF));
            } else {
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back("[" + op2Addr + "]");
            }
            lowered.instructions.push_back(loadOp2Low);
            
            Instruction286 loadOp2High;
            loadOp2High.mnemonic = "mov";
            if (op2IsConst) {
                int64_t constVal = irInst.operands[1].ref.constValue;
                int16_t highWord = (constVal >> 16) & 0xFFFF;
                loadOp2High.operands.push_back("dx");
                loadOp2High.operands.push_back(std::to_string(highWord));
            } else {
                loadOp2High.operands.push_back("dx");
                if (op2Addr.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = op2Addr.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    loadOp2High.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    loadOp2High.operands.push_back("[" + op2Addr + "+2]");
                }
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
                std::string resultStack = state.frame.getPhysReg(irInst.resultName);
                // If result doesn't have stack space, allocate it
                if (resultStack.find("bp") == std::string::npos) {
                    resultStack = state.frame.allocResultSlot(irInst.resultName, 4, true);
                }
                Instruction286 storeLow;
                storeLow.mnemonic = "mov";
                storeLow.operands.push_back("[" + resultStack + "]");
                storeLow.operands.push_back("ax");
                lowered.instructions.push_back(storeLow);

                Instruction286 storeHigh;
                storeHigh.mnemonic = "mov";
                std::string highOffset = state.frame.getHighBpOffset(resultStack);
                storeHigh.operands.push_back("[" + highOffset + "]");
                storeHigh.operands.push_back("bx");
                lowered.instructions.push_back(storeHigh);

                // Update vreg mapping to point to stack
                state.frame.setPhysReg(irInst.resultName, resultStack);
                
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
            bool op1IsVreg = (state.frame.hasSlot(op1Name));
            bool op2IsVreg = (state.frame.hasSlot(op2Name));

            // Check if operands are constants (not vregs)
            bool op1IsConst = !op1IsVreg && irInst.operands[0].ref.isConstant();
            bool op2IsConst = !op2IsVreg && irInst.operands[1].ref.isConstant();

            std::string op1 = op1IsConst ? op1Name : state.frame.getPhysReg(op1Name);
            std::string op2 = op2IsConst ? op2Name : state.frame.getPhysReg(op2Name);

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
                state.frame.setPhysReg(irInst.resultName, "ax");
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
            // 32-bit tracking now in StackFrame

            // Get stack locations for both operands
            std::string op1Stack = state.frame.getPhysReg(op1Name);

            // Check if op2 is a constant
            bool op2IsConst = irInst.operands[1].ref.isConstant();
            std::string op2Stack;
            if (!op2IsConst) {
                op2Stack = state.frame.getPhysReg(op2Name);
            }

            // Load op1 low word to AX, high word to BX
            std::string op1Addr = safeMemAddr(op1Stack, lowered.instructions);
            
            Instruction286 loadLow;
            loadLow.mnemonic = "mov";
            loadLow.operands.push_back("ax");
            loadLow.operands.push_back("[" + op1Addr + "]");
            lowered.instructions.push_back(loadLow);

            Instruction286 loadHigh;
            loadHigh.mnemonic = "mov";
            loadHigh.operands.push_back("bx");
            if (op1Addr.find("bp") != std::string::npos) {
                int offset = 0;
                std::string offsetStr = op1Addr.substr(2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else {
                loadHigh.operands.push_back("[" + op1Addr + "+2]");
            }
            lowered.instructions.push_back(loadHigh);

            // Load op2 low word to CX, high word to DX
            std::string op2Addr = safeMemAddr(op2Stack, lowered.instructions);

            Instruction286 loadOp2Low;
            loadOp2Low.mnemonic = "mov";
            if (op2IsConst) {
                int64_t constVal = irInst.operands[1].ref.constValue;
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back(std::to_string(constVal & 0xFFFF));
            } else {
                loadOp2Low.operands.push_back("cx");
                loadOp2Low.operands.push_back("[" + op2Addr + "]");
            }
            lowered.instructions.push_back(loadOp2Low);

            Instruction286 loadOp2High;
            loadOp2High.mnemonic = "mov";
            if (op2IsConst) {
                int64_t constVal = irInst.operands[1].ref.constValue;
                int16_t highWord = (constVal >> 16) & 0xFFFF;
                loadOp2High.operands.push_back("dx");
                loadOp2High.operands.push_back(std::to_string(highWord));
            } else {
                loadOp2High.operands.push_back("dx");
                if (op2Addr.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = op2Addr.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    loadOp2High.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    loadOp2High.operands.push_back("[" + op2Addr + "+2]");
                }
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
                std::string resultStack = state.frame.getPhysReg(irInst.resultName);
                // Allocate stack space if needed
                if (resultStack.find("bp") == std::string::npos) {
                    // Stack space pre-allocated in prologue
                    resultStack = state.frame.allocResultSlot(irInst.resultName, 4, true);
                }
                Instruction286 storeLow;
                storeLow.mnemonic = "mov";
                storeLow.operands.push_back("[" + resultStack + "]");
                storeLow.operands.push_back("ax");
                lowered.instructions.push_back(storeLow);

                Instruction286 storeHigh;
                storeHigh.mnemonic = "mov";
                std::string highOffset = state.frame.getHighBpOffset(resultStack);
                storeHigh.operands.push_back("[" + highOffset + "]");
                storeHigh.operands.push_back("bx");
                lowered.instructions.push_back(storeHigh);

                // Update vreg mapping
                state.frame.setPhysReg(irInst.resultName, resultStack);
                
            }
        }
    } else {
        // 16-bit sub
        Instruction286 subInst;
        subInst.mnemonic = "sub";

        if (irInst.operands.size() >= 2) {
            std::string op1Name = irInst.operands[0].name;
            std::string op2Name = irInst.operands[1].name;

            // Check if operands are vregs/parameters first
            bool op1IsVreg = (state.frame.hasSlot(op1Name));
            bool op2IsVreg = (state.frame.hasSlot(op2Name));

            // Check if operands are constants (not vregs)
            bool op1IsConst = !op1IsVreg && irInst.operands[0].ref.isConstant();
            bool op2IsConst = !op2IsVreg && irInst.operands[1].ref.isConstant();

            std::string op1 = op1IsConst ? op1Name : state.frame.getPhysReg(op1Name);
            std::string op2 = op2IsConst ? op2Name : state.frame.getPhysReg(op2Name);

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

           // Check if resultReg is a memory location
            bool destIsMem = resultReg.find("bp") != std::string::npos;
            std::string destReg = destIsMem ? "ax" : resultReg;

            // Move op1 to destReg if it's not already there
            if (!op1IsConst && destReg != op1) {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back(destReg);
                movInst.operands.push_back(op1);
                lowered.instructions.push_back(movInst);
            } else if (op1IsConst) {
                // Load constant into destReg
                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back(destReg);
                movConst.operands.push_back(op1Name);
                lowered.instructions.push_back(movConst);
            }

            subInst.operands.push_back(destReg);
            subInst.operands.push_back(op2);

            lowered.instructions.push_back(subInst);

            // If destReg was memory, store result back
            if (destIsMem && !irInst.resultName.empty()) {
                Instruction286 storeResult;
                storeResult.mnemonic = "mov";
                storeResult.operands.push_back("[" + resultReg + "]");
                storeResult.operands.push_back("ax");
                lowered.instructions.push_back(storeResult);
                state.frame.setPhysReg(irInst.resultName, resultReg);
            } else {
                // Update register mapping - sub writes to destReg
                if (!irInst.resultName.empty()) {
                    state.frame.setPhysReg(irInst.resultName, destReg);
                }
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

            // 32-bit tracking now in StackFrame

            // Get stack locations
            std::string op1Stack = state.frame.getPhysReg(op1Name);
            std::string op2Stack = state.frame.getPhysReg(op2Name);

            // Allocate result stack space
            std::string resultStack = state.frame.getPhysReg(irInst.resultName);
            if (resultStack.find("bp") == std::string::npos) {
                // Stack space pre-allocated in prologue
                resultStack = state.frame.allocResultSlot(irInst.resultName, 4, true);
            }

            // Use safeMemAddr to handle register operands (ax/cx/dx can't be used for addressing)
            std::string op1Addr = safeMemAddr(op1Stack, lowered.instructions);
            std::string op2Addr = safeMemAddr(op2Stack, lowered.instructions);

            // Load op1 into AX:BX
            Instruction286 loadOp1Low;
            loadOp1Low.mnemonic = "mov";
            loadOp1Low.operands.push_back("ax");
            loadOp1Low.operands.push_back("[" + op1Addr + "]");
            lowered.instructions.push_back(loadOp1Low);

            Instruction286 loadOp1High;
            loadOp1High.mnemonic = "mov";
            loadOp1High.operands.push_back("bx");
            if (op1Addr.find("bp") != std::string::npos) {
                int offset = 0;
                std::string offsetStr = op1Addr.substr(2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                loadOp1High.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else {
                loadOp1High.operands.push_back("[" + op1Addr + "+2]");
            }
            lowered.instructions.push_back(loadOp1High);

            // Load op2 into CX:DX
            Instruction286 loadOp2Low;
            loadOp2Low.mnemonic = "mov";
            loadOp2Low.operands.push_back("cx");
            loadOp2Low.operands.push_back("[" + op2Addr + "]");
            lowered.instructions.push_back(loadOp2Low);

            Instruction286 loadOp2High;
            loadOp2High.mnemonic = "mov";
            loadOp2High.operands.push_back("dx");
            if (op2Addr.find("bp") != std::string::npos) {
                int offset = 0;
                std::string offsetStr = op2Addr.substr(2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                loadOp2High.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else {
                // Calculate correct offset for high word
                if (op2Addr.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = op2Addr.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    loadOp2High.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    loadOp2High.operands.push_back("[" + op2Addr + "+2]");
                }
            }
            lowered.instructions.push_back(loadOp2High);

            // Call multiplication routine (far call to runtime)
            // Result will be in DI:SI
            Instruction286 callMul;
            callMul.mnemonic = "call";
            callMul.operands.push_back("far _MultiplyI32");
            lowered.instructions.push_back(callMul);

       // Store result to stack
            // _MultiplyI32 returns: di = low word (LSB), si = high word (MSB)
            // (di gets adc di,ax where ax is low word; si gets add si,bx where bx is high word)
            Instruction286 storeResultLow;
            storeResultLow.mnemonic = "mov";
            storeResultLow.operands.push_back("[" + resultStack + "]");
            storeResultLow.operands.push_back("di");
            lowered.instructions.push_back(storeResultLow);

            Instruction286 storeResultHigh;
            storeResultHigh.mnemonic = "mov";
            std::string highOffset = state.frame.getHighBpOffset(resultStack);
            storeResultHigh.operands.push_back("[" + highOffset + "]");
            storeResultHigh.operands.push_back("si");
            lowered.instructions.push_back(storeResultHigh);

            // Update vreg mapping
            state.frame.setPhysReg(irInst.resultName, resultStack);
            
        }
    } else {
        // 16-bit multiplication
        // Check if either operand is a constant (getPhysReg returns garbage for constants)
        bool op1IsConst = false, op2IsConst = false;
        std::string op1ConstVal, op2ConstVal;
        if (irInst.operands.size() >= 2) {
            op1IsConst = irInst.operands[0].ref.isConstant();
            op1ConstVal = op1IsConst ? irInst.operands[0].name : "";
            op2IsConst = irInst.operands[1].ref.isConstant();
            op2ConstVal = op2IsConst ? irInst.operands[1].name : "";
        }

        if (op1IsConst && op2IsConst) {
            // Both constants: compute at compile time
            int result = (int)(irInst.operands[0].ref.constValue * irInst.operands[1].ref.constValue);
            Instruction286 movConst;
            movConst.mnemonic = "mov";
            movConst.operands.push_back("ax");
            movConst.operands.push_back(std::to_string(result));
            lowered.instructions.push_back(movConst);
        } else if (op2IsConst || op1IsConst) {
            // One operand is constant: load vreg to AX, constant to CX, then imul cx
            std::string vregName, constVal;
            if (op2IsConst) {
                vregName = irInst.operands[0].name;
                constVal = op2ConstVal;
            } else {
                vregName = irInst.operands[1].name;
                constVal = op1ConstVal;
            }

            // Move vreg operand to AX
            std::string vregPhys = state.frame.getPhysReg(vregName);
            if (vregPhys != "ax") {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back("ax");
                if (vregPhys.find("bp") != std::string::npos) {
                    movInst.operands.push_back("[" + vregPhys + "]");
                } else {
                    movInst.operands.push_back(vregPhys);
                }
                lowered.instructions.push_back(movInst);
            }

            // Move constant to CX
            Instruction286 movConst;
            movConst.mnemonic = "mov";
            movConst.operands.push_back("cx");
            movConst.operands.push_back(constVal);
            lowered.instructions.push_back(movConst);

            // imul cx (one-operand form: AX * CX -> DX:AX, use AX for lower 16 bits)
            Instruction286 imulInst;
            imulInst.mnemonic = "imul";
            imulInst.operands.push_back("cx");
            lowered.instructions.push_back(imulInst);
        } else if (irInst.operands.size() >= 2) {
            // Neither operand is constant: existing logic
            std::string op1 = state.frame.getPhysReg(irInst.operands[0].name);
            std::string op2 = state.frame.getPhysReg(irInst.operands[1].name);

            // Move first operand to AX
            if (op1 != "ax") {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back("ax");
                if (op1.find("bp") != std::string::npos) {
                    movInst.operands.push_back("[" + op1 + "]");
                } else {
                    movInst.operands.push_back(op1);
                }
                lowered.instructions.push_back(movInst);
            }

            // IMUL second operand (one-operand form: AX * op2 -> DX:AX)
            Instruction286 imulInst;
            imulInst.mnemonic = "imul";
            if (op2.find("bp+") != std::string::npos || op2.find("bp-") != std::string::npos) {
                imulInst.operands.push_back("word [" + op2 + "]");
            } else {
                imulInst.operands.push_back(op2);
            }
            lowered.instructions.push_back(imulInst);
        } else {
            Instruction286 imulInst;
            imulInst.mnemonic = "imul";
            imulInst.operands.push_back("bx");
            lowered.instructions.push_back(imulInst);
        }

        // Result is in AX (low word) and DX (high word)
        bool destIsMem = resultReg.find("bp") != std::string::npos;
        if (resultReg != "ax" || destIsMem) {
            if (destIsMem) {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back("[" + resultReg + "]");
                movInst.operands.push_back("ax");
                lowered.instructions.push_back(movInst);
            } else {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back(resultReg);
                movInst.operands.push_back("ax");
                lowered.instructions.push_back(movInst);
            }
        }

        // Update register mapping - mul writes to AX (low word)
        if (!irInst.resultName.empty()) {
            if (destIsMem) {
                state.frame.setPhysReg(irInst.resultName, resultReg);
            } else {
                state.frame.setPhysReg(irInst.resultName, "ax");
            }
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
            std::vector<Instruction286> dividendLoads;
            state.frame.emitLoad32(dividendLoads, dividentName, "ax", "dx");
            lowered.instructions.insert(lowered.instructions.end(), dividendLoads.begin(), dividendLoads.end());

            // Move dividend low word from AX to... wait, load32Bit loads to AX:DX
            // We need to move DX to BX for the tested runtime convention
            Instruction286 movDivHigh;
            movDivHigh.mnemonic = "mov";
            movDivHigh.operands.push_back("bx");
            movDivHigh.operands.push_back("dx");
            lowered.instructions.push_back(movDivHigh);

            // Load divisor low word to CX, high word to DX
            // Check if divisor is a constant
            bool divisorIsConst = irInst.operands[1].ref.isConstant();
            int64_t divisorConstVal = divisorIsConst ? irInst.operands[1].ref.constValue : 0;

            if (divisorIsConst) {
                // Load constant divisor to CX:DX
                Instruction286 loadDivisorLow;
                loadDivisorLow.mnemonic = "mov";
                loadDivisorLow.operands.push_back("cx");
                loadDivisorLow.operands.push_back(std::to_string(divisorConstVal & 0xFFFF));
                lowered.instructions.push_back(loadDivisorLow);

                Instruction286 loadDivisorHigh;
                loadDivisorHigh.mnemonic = "mov";
                int16_t highWord = (divisorConstVal >> 16) & 0xFFFF;
                loadDivisorHigh.operands.push_back("dx");
                loadDivisorHigh.operands.push_back(std::to_string(highWord));
                lowered.instructions.push_back(loadDivisorHigh);
            } else {
                std::string divisorReg = state.frame.getPhysReg(divisorName);
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
                std::string resultStack = state.frame.allocResultSlot(irInst.resultName, 4, true);
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
                    std::string highOffset = state.frame.getHighBpOffset(resultStack);
                    storeResultHigh.operands.push_back("[" + highOffset + "]");
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
                    std::string highOffset = state.frame.getHighBpOffset(resultStack);
                    storeResultHigh.operands.push_back("[" + highOffset + "]");
                    storeResultHigh.operands.push_back("si");
                    lowered.instructions.push_back(storeResultHigh);
                }

                // Update vreg mapping to point to the stack slot
                state.frame.setPhysReg(irInst.resultName, resultStack);
            }
        } else {
            // 16-bit division
            // Load dividend to AX
            std::string dividentReg = state.frame.getPhysReg(dividentName);
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
            std::string divisorReg = state.frame.getPhysReg(divisorName);
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
                state.frame.setPhysReg(irInst.resultName, divResultReg);
            }
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286