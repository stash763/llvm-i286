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
    bool destIsMem = false;

    bool skipPush = false;
    if (irInst.operands.size() >= 2) {
        std::string op1Name = irInst.operands[0].name;
        std::string op2Name = irInst.operands[1].name;

        // Check if operands are vregs (exist in state maps)
        bool op1IsVreg = state.frame.hasSlot(op1Name);
        bool op2IsVreg = state.frame.hasSlot(op2Name);

        // Check if operands are constants (only if not vregs)
        bool op2IsConst = !op2IsVreg && irInst.operands[1].ref.isConstant();
        bool op2IsMem = false;

        op1IsConst = !op1IsVreg && irInst.operands[0].ref.isConstant();
        op1 = op1IsConst ? op1Name : state.frame.getPhysReg(op1Name);
        std::string op2 = op2IsConst ? op2Name : state.frame.getPhysReg(op2Name);

        // Check if op2 is a memory operand
        op2IsMem = !op2IsConst && op2.find("bp") != std::string::npos;

        // Check if destReg is a memory operand (bp-relative)
        destIsMem = destReg.find("bp") != std::string::npos;

        // If op1 is a memory operand, load to register first
        op1IsMem = !op1IsConst && op1.find("bp") != std::string::npos;
        if (op1IsMem) {
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back("ax");
            loadInst.operands.push_back("[" + op1 + "]");
            lowered.instructions.push_back(loadInst);
            op1 = "ax";
        }

        // Binary ops (and/or/xor) must operate on registers, not memory
        // Use ax as the working register
        if (op1IsMem || destIsMem) {
            if (!op1IsMem && op1 != "ax") {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back("ax");
                movInst.operands.push_back(op1);
                lowered.instructions.push_back(movInst);
            }
            destReg = "ax";
        }

        inst.operands.push_back(destReg);
        // If op2 is memory, wrap in brackets
        if (op2IsMem) {
            inst.operands.push_back("[" + op2 + "]");
        } else if (op2IsConst) {
            // Check if this is a 32-bit operation or the constant does not fit in 16 bits
            bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
            int64_t constVal = irInst.operands[1].ref.isConstant() ? irInst.operands[1].ref.constValue : 0;
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
            Instruction286 zeroHigh;
            zeroHigh.mnemonic = "xor";
            zeroHigh.operands.push_back("dx");
            zeroHigh.operands.push_back("dx");
            lowered.instructions.push_back(zeroHigh);
            
            // Stack space pre-allocated in prologue
            std::string tempSlot = state.frame.allocResultSlot(irInst.resultName, 4, true);
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
            // If destReg was memory, store result back to the original location
            if (destIsMem) {
                std::string resultLoc = state.frame.getPhysReg(irInst.resultName);
                if (resultLoc.find("bp") != std::string::npos) {
                    Instruction286 storeResult;
                    storeResult.mnemonic = "mov";
                    storeResult.operands.push_back("[" + resultLoc + "]");
                    storeResult.operands.push_back("ax");
                    lowered.instructions.push_back(storeResult);
                    state.frame.setPhysReg(irInst.resultName, resultLoc);
                } else {
                    state.frame.setPhysReg(irInst.resultName, destReg);
                }
            } else {
                state.frame.setPhysReg(irInst.resultName, destReg);
            }
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

    // Check for 64-bit shift
    bool is64 = irInst.resultType && irInst.resultType->bitWidth == 64;

    if (is64 && irInst.operands.size() >= 2) {
        std::string op1Name = irInst.operands[0].name;
        std::string op2Name = irInst.operands[1].name;

        bool op2IsVreg = state.frame.hasSlot(op2Name);
        bool op2IsConst = !op2IsVreg && irInst.operands[1].ref.isConstant();
        int shiftAmt = 0;
        if (op2IsConst) {
            shiftAmt = (int)irInst.operands[1].ref.constValue;
        }

        std::string srcReg = state.frame.getPhysReg(op1Name);

        // If srcReg is a register (not bp-relative), move to bx for addressing
        bool srcIsMem = srcReg.find("bp") != std::string::npos;
        std::string srcBase = srcReg;
        if (!srcIsMem) {
            Instruction286 movBx;
            movBx.mnemonic = "mov";
            movBx.operands.push_back("bx");
            movBx.operands.push_back(srcReg);
            lowered.instructions.push_back(movBx);
            srcBase = "bx";
        }

        // Allocate result on stack (4 words = 8 bytes)
        std::string resultStack = state.frame.allocResultSlot(irInst.resultName, 8, true);

        // Helper to build bp-relative offset string with pre-calculated offset
        auto makeOffset = [](const std::string& base, int offset) -> std::string {
            if (base.find("bp") != std::string::npos) {
                // base is like "bp-32" or "bp+4", parse and add offset
                std::string offsetStr = base.substr(2);
                int baseOffset = 0;
                if (!offsetStr.empty()) {
                    try { baseOffset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = baseOffset + offset;
                if (newOffset >= 0) {
                    return "bp+" + std::to_string(newOffset);
                } else {
                    return "bp" + std::to_string(newOffset);
                }
            } else {
                // Register base like "bx"
                if (offset >= 0) {
                    return base + "+" + std::to_string(offset);
                } else {
                    return base + std::to_string(offset);
                }
            }
        };

        if (op2IsConst && shiftAmt >= 32 && (mnemonic == "shr" || mnemonic == "sar")) {
            // lshr/ashr i64 by >= 32: result_low = src_high >> (shiftAmt - 32), result_high = 0
            int subShift = shiftAmt - 32;

            // Load src word2 (bits 47-32) -> ax
            Instruction286 loadW2;
            loadW2.mnemonic = "mov";
            loadW2.operands.push_back("ax");
            loadW2.operands.push_back("[" + makeOffset(srcBase, 4) + "]");
            lowered.instructions.push_back(loadW2);

            // Load src word3 (bits 63-48) -> dx
            Instruction286 loadW3;
            loadW3.mnemonic = "mov";
            loadW3.operands.push_back("dx");
            loadW3.operands.push_back("[" + makeOffset(srcBase, 6) + "]");
            lowered.instructions.push_back(loadW3);

            if (subShift > 0 && subShift < 16) {
                Instruction286 shrAx;
                shrAx.mnemonic = "shr";
                shrAx.operands.push_back("ax");
                shrAx.operands.push_back(std::to_string(subShift));
                lowered.instructions.push_back(shrAx);

                if (mnemonic == "sar") {
                    Instruction286 shxDx;
                    shxDx.mnemonic = "shr";
                    shxDx.operands.push_back("dx");
                    shxDx.operands.push_back(std::to_string(subShift));
                    lowered.instructions.push_back(shxDx);
                } else {
                    Instruction286 zeroDx;
                    zeroDx.mnemonic = "xor";
                    zeroDx.operands.push_back("dx");
                    zeroDx.operands.push_back("dx");
                    lowered.instructions.push_back(zeroDx);
                }
            } else {
                if (mnemonic == "shr") {
                    Instruction286 zeroDx;
                    zeroDx.mnemonic = "xor";
                    zeroDx.operands.push_back("dx");
                    zeroDx.operands.push_back("dx");
                    lowered.instructions.push_back(zeroDx);
                }
            }

            // Store result: word0 = ax, word1 = dx, word2 = 0, word3 = 0
            Instruction286 storeW0;
            storeW0.mnemonic = "mov";
            storeW0.operands.push_back("[" + resultStack + "]");
            storeW0.operands.push_back("ax");
            lowered.instructions.push_back(storeW0);

            std::string resW1 = makeOffset(resultStack, 2);
            Instruction286 storeW1;
            storeW1.mnemonic = "mov";
            storeW1.operands.push_back("[" + resW1 + "]");
            storeW1.operands.push_back("dx");
            lowered.instructions.push_back(storeW1);

            std::string resW2 = makeOffset(resultStack, 4);
            Instruction286 storeW2;
            storeW2.mnemonic = "mov";
            storeW2.operands.push_back("word [" + resW2 + "]");
            storeW2.operands.push_back("0");
            lowered.instructions.push_back(storeW2);

            std::string resW3 = makeOffset(resultStack, 6);
            Instruction286 storeW3;
            storeW3.mnemonic = "mov";
            storeW3.operands.push_back("word [" + resW3 + "]");
            storeW3.operands.push_back("0");
            lowered.instructions.push_back(storeW3);

        } else if (op2IsConst && shiftAmt == 32 && mnemonic == "shl") {
            // shl i64 by 32: result_low = 0, result_high = src_low
            Instruction286 loadW0;
            loadW0.mnemonic = "mov";
            loadW0.operands.push_back("ax");
            loadW0.operands.push_back("[" + srcBase + "]");
            lowered.instructions.push_back(loadW0);

            Instruction286 loadW1;
            loadW1.mnemonic = "mov";
            loadW1.operands.push_back("dx");
            loadW1.operands.push_back("[" + makeOffset(srcBase, 2) + "]");
            lowered.instructions.push_back(loadW1);

            Instruction286 storeW0;
            storeW0.mnemonic = "mov";
            storeW0.operands.push_back("word [" + resultStack + "]");
            storeW0.operands.push_back("0");
            lowered.instructions.push_back(storeW0);

            std::string resW1 = makeOffset(resultStack, 2);
            Instruction286 storeW1;
            storeW1.mnemonic = "mov";
            storeW1.operands.push_back("word [" + resW1 + "]");
            storeW1.operands.push_back("0");
            lowered.instructions.push_back(storeW1);

            std::string resW2 = makeOffset(resultStack, 4);
            Instruction286 storeW2;
            storeW2.mnemonic = "mov";
            storeW2.operands.push_back("[" + resW2 + "]");
            storeW2.operands.push_back("ax");
            lowered.instructions.push_back(storeW2);

            std::string resW3 = makeOffset(resultStack, 6);
            Instruction286 storeW3;
            storeW3.mnemonic = "mov";
            storeW3.operands.push_back("[" + resW3 + "]");
            storeW3.operands.push_back("dx");
            lowered.instructions.push_back(storeW3);

        } else if (op2IsConst && shiftAmt == 16) {
            if (mnemonic == "shr" || mnemonic == "sar") {
                // lshr/ashr by 16: word0=word1, word1=word2, word2=word3, word3=0
                Instruction286 loadW1;
                loadW1.mnemonic = "mov";
                loadW1.operands.push_back("ax");
                loadW1.operands.push_back("[" + makeOffset(srcBase, 2) + "]");
                lowered.instructions.push_back(loadW1);

                Instruction286 loadW2;
                loadW2.mnemonic = "mov";
                loadW2.operands.push_back("dx");
                loadW2.operands.push_back("[" + makeOffset(srcBase, 4) + "]");
                lowered.instructions.push_back(loadW2);

                Instruction286 storeW0;
                storeW0.mnemonic = "mov";
                storeW0.operands.push_back("[" + resultStack + "]");
                storeW0.operands.push_back("ax");
                lowered.instructions.push_back(storeW0);

                std::string resW1 = makeOffset(resultStack, 2);
                Instruction286 storeW1;
                storeW1.mnemonic = "mov";
                storeW1.operands.push_back("[" + resW1 + "]");
                storeW1.operands.push_back("dx");
                lowered.instructions.push_back(storeW1);

                Instruction286 loadW3;
                loadW3.mnemonic = "mov";
                loadW3.operands.push_back("ax");
                loadW3.operands.push_back("[" + makeOffset(srcBase, 6) + "]");
                lowered.instructions.push_back(loadW3);

                std::string resW2 = makeOffset(resultStack, 4);
                Instruction286 storeW2;
                storeW2.mnemonic = "mov";
                storeW2.operands.push_back("[" + resW2 + "]");
                storeW2.operands.push_back("ax");
                lowered.instructions.push_back(storeW2);

                std::string resW3 = makeOffset(resultStack, 6);
                if (mnemonic == "sar") {
                    Instruction286 cwd;
                    cwd.mnemonic = "cwd";
                    lowered.instructions.push_back(cwd);
                    Instruction286 storeW3;
                    storeW3.mnemonic = "mov";
                    storeW3.operands.push_back("[" + resW3 + "]");
                    storeW3.operands.push_back("dx");
                    lowered.instructions.push_back(storeW3);
                } else {
                    Instruction286 storeW3;
                    storeW3.mnemonic = "mov";
                    storeW3.operands.push_back("word [" + resW3 + "]");
                    storeW3.operands.push_back("0");
                    lowered.instructions.push_back(storeW3);
                }
            } else if (mnemonic == "shl") {
                // shl by 16: word0=0, word1=word0, word2=word1, word3=word2
                Instruction286 loadW0;
                loadW0.mnemonic = "mov";
                loadW0.operands.push_back("ax");
                loadW0.operands.push_back("[" + srcBase + "]");
                lowered.instructions.push_back(loadW0);

                Instruction286 loadW1;
                loadW1.mnemonic = "mov";
                loadW1.operands.push_back("dx");
                loadW1.operands.push_back("[" + makeOffset(srcBase, 2) + "]");
                lowered.instructions.push_back(loadW1);

                Instruction286 storeW0;
                storeW0.mnemonic = "mov";
                storeW0.operands.push_back("word [" + resultStack + "]");
                storeW0.operands.push_back("0");
                lowered.instructions.push_back(storeW0);

                std::string resW1 = makeOffset(resultStack, 2);
                Instruction286 storeW1;
                storeW1.mnemonic = "mov";
                storeW1.operands.push_back("[" + resW1 + "]");
                storeW1.operands.push_back("ax");
                lowered.instructions.push_back(storeW1);

                std::string resW2 = makeOffset(resultStack, 4);
                Instruction286 storeW2;
                storeW2.mnemonic = "mov";
                storeW2.operands.push_back("[" + resW2 + "]");
                storeW2.operands.push_back("dx");
                lowered.instructions.push_back(storeW2);

                Instruction286 loadW2;
                loadW2.mnemonic = "mov";
                loadW2.operands.push_back("ax");
                loadW2.operands.push_back("[" + makeOffset(srcBase, 4) + "]");
                lowered.instructions.push_back(loadW2);

                std::string resW3 = makeOffset(resultStack, 6);
                Instruction286 storeW3;
                storeW3.mnemonic = "mov";
                storeW3.operands.push_back("[" + resW3 + "]");
                storeW3.operands.push_back("ax");
                lowered.instructions.push_back(storeW3);
            }

        } else if (op2IsConst && shiftAmt == 0) {
            for (int i = 0; i < 4; i++) {
                std::string srcOff = makeOffset(srcBase, i * 2);
                std::string resOff = makeOffset(resultStack, i * 2);

                Instruction286 load;
                load.mnemonic = "mov";
                load.operands.push_back("ax");
                load.operands.push_back("[" + srcOff + "]");
                lowered.instructions.push_back(load);

                Instruction286 store;
                store.mnemonic = "mov";
                store.operands.push_back("[" + resOff + "]");
                store.operands.push_back("ax");
                lowered.instructions.push_back(store);
            }

        } else if (op2IsConst && shiftAmt >= 64) {
            for (int i = 0; i < 4; i++) {
                std::string resOff = makeOffset(resultStack, i * 2);
                if (mnemonic == "sar") {
                    if (i == 0) {
                        Instruction286 loadSign;
                        loadSign.mnemonic = "mov";
                        loadSign.operands.push_back("ax");
                        loadSign.operands.push_back("[" + makeOffset(srcBase, 6) + "]");
                        lowered.instructions.push_back(loadSign);
                        Instruction286 cwd;
                        cwd.mnemonic = "cwd";
                        lowered.instructions.push_back(cwd);
                    }
                    Instruction286 store;
                    store.mnemonic = "mov";
                    store.operands.push_back("word [" + resOff + "]");
                    store.operands.push_back(i < 2 ? "ax" : "dx");
                    lowered.instructions.push_back(store);
                } else {
                    Instruction286 store;
                    store.mnemonic = "mov";
                    store.operands.push_back("word [" + resOff + "]");
                    store.operands.push_back("0");
                    lowered.instructions.push_back(store);
                }
            }

        } else {
            // Fallback: treat as 16-bit shift (best effort)
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back("ax");
            loadInst.operands.push_back("[" + srcBase + "]");
            lowered.instructions.push_back(loadInst);

            if (op2IsConst && shiftAmt > 0) {
                Instruction286 shiftInst;
                shiftInst.mnemonic = mnemonic;
                shiftInst.operands.push_back("ax");
                shiftInst.operands.push_back(std::to_string(shiftAmt > 16 ? 16 : shiftAmt));
                lowered.instructions.push_back(shiftInst);
            }

            Instruction286 storeW0;
            storeW0.mnemonic = "mov";
            storeW0.operands.push_back("[" + resultStack + "]");
            storeW0.operands.push_back("ax");
            lowered.instructions.push_back(storeW0);

            for (int i = 1; i < 4; i++) {
                std::string resOff = makeOffset(resultStack, i * 2);
                Instruction286 storeZ;
                storeZ.mnemonic = "mov";
                storeZ.operands.push_back("word [" + resOff + "]");
                storeZ.operands.push_back("0");
                lowered.instructions.push_back(storeZ);
            }
        }

        if (!irInst.resultName.empty()) {
            state.frame.setPhysReg(irInst.resultName, resultStack);
        }

        loweredVec.push_back(lowered);
        return loweredVec;
    }

    // 16-bit and 32-bit shift handling (existing code)
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
        bool op1IsVreg = state.frame.hasSlot(op1Name);
        bool op2IsVreg = state.frame.hasSlot(op2Name);

        // Check if operands are constants (only if not vregs)
        bool op2IsConst = !op2IsVreg && irInst.operands[1].ref.isConstant();

        op1IsConst = !op1IsVreg && irInst.operands[0].ref.isConstant();
        op1 = op1IsConst ? op1Name : state.frame.getPhysReg(op1Name);
        op2 = op2IsConst ? op2Name : state.frame.getPhysReg(op2Name);

        // Check if op2 is a memory operand
        bool op2IsMem = !op2IsConst && op2.find("bp") != std::string::npos;

        // Shift count must be in CL for variable shifts, or immediate
        if (!op2IsConst && op2 != "cl" && op2 != "cx") {
            // Move shift count to CL (8-bit register)
            Instruction286 movCL;
            movCL.mnemonic = "mov";
            movCL.operands.push_back("cl");
            // If op2 is memory, load byte from memory
            if (op2IsMem) {
                movCL.operands.push_back("byte [" + op2 + "]");
            } else if (op2 == "ax") {
                movCL.operands.push_back("al");
            } else if (op2 == "bx") {
                movCL.operands.push_back("bl");
            } else if (op2 == "dx") {
                movCL.operands.push_back("dl");
            } else if (op2 == "si") {
                movCL.operands.push_back("dl");
                // si doesn't have low byte directly, use ax intermediate
                Instruction286 movAx;
                movAx.mnemonic = "mov";
                movAx.operands.push_back("ax");
                movAx.operands.push_back("si");
                lowered.instructions.push_back(movAx);
                movCL.operands.back() = "al";
            } else if (op2 == "di") {
                movCL.operands.push_back("dl");
                Instruction286 movAx;
                movAx.mnemonic = "mov";
                movAx.operands.push_back("ax");
                movAx.operands.push_back("di");
                lowered.instructions.push_back(movAx);
                movCL.operands.back() = "al";
            } else {
                movCL.operands.push_back(op2);
            }
            lowered.instructions.push_back(movCL);
            op2 = "cl";
        }

        // Check if destReg is a memory operand (bp-relative)
        bool destIsMem = destReg.find("bp") != std::string::npos;

        // If op1 is a memory operand, load to register first
        op1IsMem = !op1IsConst && op1.find("bp") != std::string::npos;
        if (op1IsMem) {
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back("ax");
            loadInst.operands.push_back("[" + op1 + "]");
            lowered.instructions.push_back(loadInst);
            op1 = "ax";
        }

        // Shifts must operate on a register, not memory
        // Use ax as the working register (op1 was loaded there if it was memory)
        if (op1IsMem || destIsMem) {
            if (!op1IsMem && op1 != "ax") {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back("ax");
                movInst.operands.push_back(op1);
                lowered.instructions.push_back(movInst);
            }
            destReg = "ax";
        }

        inst.operands.push_back(destReg);
        inst.operands.push_back(op2);

    lowered.instructions.push_back(inst);

        // If result needs to be stored to memory, do it after the shift
        if (!irInst.resultName.empty()) {
            if (destIsMem) {
                // Store result back to memory location
                std::string resultLoc = state.frame.getPhysReg(irInst.resultName);
                if (resultLoc.find("bp") != std::string::npos) {
                    Instruction286 storeResult;
                    storeResult.mnemonic = "mov";
                    storeResult.operands.push_back("[" + resultLoc + "]");
                    storeResult.operands.push_back("ax");
                    lowered.instructions.push_back(storeResult);
                    state.frame.setPhysReg(irInst.resultName, resultLoc);
                } else {
                    state.frame.setPhysReg(irInst.resultName, destReg);
                }
            } else {
                state.frame.setPhysReg(irInst.resultName, destReg);
            }
        }
    } else {
        inst.operands.push_back("ax");
        inst.operands.push_back("cl");
    lowered.instructions.push_back(inst);

    if (!irInst.resultName.empty()) {
        state.frame.setPhysReg(irInst.resultName, "ax");
    }
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