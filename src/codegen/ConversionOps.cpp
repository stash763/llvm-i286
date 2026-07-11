// Conversion Operations - Instruction Selection
// Handlers for type conversion IR instructions: Trunc, ZExt, SExt,
// BitCast, Freeze, ExtractValue, InsertValue

#include "codegen/InstructionSelectInternal.h"

#include <string>
#include <vector>

namespace llvm_i286 {
namespace codegen {

std::vector<LoweredInstruction> lowerTrunc(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Truncate: result = trunc type value to type
    // For 16-bit target: truncating 32-bit to 16-bit is just taking low word
    // For 8-bit: take low byte (AL)
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string srcReg = state.frame.getPhysReg(irInst.operands[0].name);
        std::string destReg = resultReg.empty() ? "ax" : resultReg;

        bool srcIsMem = srcReg.find("bp") != std::string::npos;

        // Load source into destReg if it's a memory location
        if (srcIsMem) {
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back(destReg);
            loadInst.operands.push_back("[" + srcReg + "]");
            lowered.instructions.push_back(loadInst);
        } else if (destReg != srcReg) {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(destReg);
            movInst.operands.push_back(srcReg);
            lowered.instructions.push_back(movInst);
        }

        if (!irInst.resultName.empty()) {
            state.frame.setPhysReg(irInst.resultName, destReg);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerZExt(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Zero extend: result = zext type value to type
    // For 16-bit target: extending 8-bit to 16-bit requires clearing high byte
    // For 32-bit target: extending 8-bit to 32-bit requires clearing high word
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string srcReg = state.frame.getPhysReg(irInst.operands[0].name);
        std::string destReg = resultReg.empty() ? "ax" : resultReg;

        bool srcIsMem = srcReg.find("bp") != std::string::npos;

        // Load source into destReg if it's a memory location
        if (srcIsMem && destReg != srcReg) {
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back(destReg);
            loadInst.operands.push_back("[" + srcReg + "]");
            lowered.instructions.push_back(loadInst);
        } else if (destReg != srcReg) {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(destReg);
            movInst.operands.push_back(srcReg);
            lowered.instructions.push_back(movInst);
        }

        if (!irInst.resultName.empty()) {
            state.frame.setPhysReg(irInst.resultName, destReg);
        }

        // If extending to 32-bit, zero-extend to DX:AX and store to temp slot
        if (irInst.resultType && irInst.resultType->bitWidth == 32) {
            // Clear DX for zero extension
            Instruction286 clearDx;
            clearDx.mnemonic = "xor";
            clearDx.operands.push_back("dx");
            clearDx.operands.push_back("dx");
            lowered.instructions.push_back(clearDx);

            // Allocate temp slot for 32-bit result (pre-allocated in prologue)
            std::string resultStack = state.frame.allocTemp(4, true);

            // Store low word (ax) to temp
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + resultStack + "]");
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);

            // Store high word (dx=0) to temp+2
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            std::string highOffset = state.frame.getHighBpOffset(resultStack);
            storeHigh.operands.push_back("[" + highOffset + "]");
            storeHigh.operands.push_back("dx");
            lowered.instructions.push_back(storeHigh);

            // Update vreg mapping to point to stack
            state.frame.setPhysReg(irInst.resultName, resultStack);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerSExt(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Sign extend: result = sext type value to type
    // For 16-bit target: CBW (AX <- sign-extend AL), CWD (DX:AX <- sign-extend AX)
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string srcReg = state.frame.getPhysReg(irInst.operands[0].name);
        std::string destReg = resultReg.empty() ? "ax" : resultReg;

        bool srcIsMem = srcReg.find("bp") != std::string::npos;

        // Load source into destReg if it's a memory location
        if (srcIsMem && destReg != srcReg) {
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back(destReg);
            loadInst.operands.push_back("[" + srcReg + "]");
            lowered.instructions.push_back(loadInst);
        } else if (destReg != srcReg) {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(destReg);
            movInst.operands.push_back(srcReg);
            lowered.instructions.push_back(movInst);
        }

        // Store result to stack slot if it has a name
        if (!irInst.resultName.empty()) {
            // Allocate stack space for the result (32-bit for sign-extended values)
            // Stack space pre-allocated in prologue
            std::string resultStack = state.frame.allocTemp(4, true);

            // Sign-extend: for 8-bit to 32-bit, use cbw then cwd
            // For 16-bit to 32-bit, use cwd
            {
                // First sign-extend al to ax if source was 8-bit
                Instruction286 cbwInst;
                cbwInst.mnemonic = "cbw";
                lowered.instructions.push_back(cbwInst);

                // Then sign-extend ax to dx:ax
                Instruction286 cwdInst;
                cwdInst.mnemonic = "cwd";
                lowered.instructions.push_back(cwdInst);
            }

            // Store low word (ax)
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + resultStack + "]");
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);

            // Store high word (dx) - getHighBpOffset already returns full bp-relative string
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            std::string hOffsetStr = state.frame.getHighBpOffset(resultStack);
            storeHigh.operands.push_back("[" + hOffsetStr + "]");
            storeHigh.operands.push_back("dx");
            lowered.instructions.push_back(storeHigh);

            // Update vreg mapping to point to stack
            state.frame.setPhysReg(irInst.resultName, resultStack);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerBitCast(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // No-op for same-size types on 80286
    // Just copy the value
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string srcReg = state.frame.getPhysReg(irInst.operands[0].name);

        if (srcReg.find("bp") != std::string::npos) {
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back("ax");
            loadInst.operands.push_back("[" + srcReg + "]");
            lowered.instructions.push_back(loadInst);
        } else if (srcReg != "ax") {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back("ax");
            movInst.operands.push_back(srcReg);
            lowered.instructions.push_back(movInst);
        }

        if (!irInst.resultName.empty()) {
            state.frame.setPhysReg(irInst.resultName, "ax");
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerFreeze(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Freeze is a no-op for initial port
    // Just copy the value
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string srcReg = state.frame.getPhysReg(irInst.operands[0].name);

        if (srcReg.find("bp") != std::string::npos) {
            Instruction286 loadInst;
            loadInst.mnemonic = "mov";
            loadInst.operands.push_back("ax");
            loadInst.operands.push_back("[" + srcReg + "]");
            lowered.instructions.push_back(loadInst);
        } else if (srcReg != "ax") {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back("ax");
            movInst.operands.push_back(srcReg);
            lowered.instructions.push_back(movInst);
        }

        if (!irInst.resultName.empty()) {
            state.frame.setPhysReg(irInst.resultName, "ax");
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerExtractValue(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Extract value from struct
    // For now, treat as a load from an offset
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string aggReg = state.frame.getPhysReg(irInst.operands[0].name);

        // Calculate offset based on indices
        int offset = 0;
        // This is simplified - full implementation would calculate struct offsets
        for (size_t i = 1; i < irInst.operands.size(); i++) {
            try {
                offset += std::stoi(irInst.operands[i].name) * 2; // Assume 16-bit fields
            } catch (...) {}
        }

        Instruction286 loadInst;
        loadInst.mnemonic = "mov";
        loadInst.operands.push_back("ax");
        if (aggReg.find("bp") != std::string::npos) {
            loadInst.operands.push_back("[" + aggReg + "+" + std::to_string(offset) + "]");
        } else {
            loadInst.operands.push_back("[" + aggReg + "+" + std::to_string(offset) + "]");
        }
        lowered.instructions.push_back(loadInst);

        if (!irInst.resultName.empty()) {
            state.frame.setPhysReg(irInst.resultName, "ax");
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerInsertValue(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Insert value into struct
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (irInst.operands.size() >= 2) {
        std::string aggReg = state.frame.getPhysReg(irInst.operands[0].name);
        std::string valReg = state.frame.getPhysReg(irInst.operands[1].name);

        // Calculate offset
        int offset = 0;
        for (size_t i = 2; i < irInst.operands.size(); i++) {
            try {
                offset += std::stoi(irInst.operands[i].name) * 2;
            } catch (...) {}
        }

        // Store value to offset
        Instruction286 storeInst;
        storeInst.mnemonic = "mov";
        if (aggReg.find("bp") != std::string::npos) {
            storeInst.operands.push_back("[" + aggReg + "+" + std::to_string(offset) + "]");
        } else {
            storeInst.operands.push_back("[" + aggReg + "+" + std::to_string(offset) + "]");
        }
        storeInst.operands.push_back(valReg);
        lowered.instructions.push_back(storeInst);
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerPtrToInt(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // PtrToInt: result = ptrtoint ptr value to i32
    // On i286 protected mode, pointers are 32-bit (selector:offset)
    // For flat memory model, selector = 0, so we just need the offset
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string ptrReg = state.frame.getPhysReg(irInst.operands[0].name);
        std::string resultRegName = irInst.resultName;

        // Load the pointer value (32-bit) into AX:DX
        // If the pointer is in memory, load both words
        if (ptrReg.find("bp") != std::string::npos) {
            // Memory operand: use MOV to load the VALUE of the pointer
            // ptrReg is like "bp-18", we need mov ax, [bp-18]
            Instruction286 movLow;
            movLow.mnemonic = "mov";
            movLow.operands.push_back("ax");
            movLow.operands.push_back("[" + ptrReg + "]");
            lowered.instructions.push_back(movLow);

            // High word is 0 for stack addresses in flat model
            Instruction286 xorDx;
            xorDx.mnemonic = "xor";
            xorDx.operands.push_back("dx");
            xorDx.operands.push_back("dx");
            lowered.instructions.push_back(xorDx);
        } else if (ptrReg != "ax") {
            // Register operand: move to AX
            Instruction286 movAx;
            movAx.mnemonic = "mov";
            movAx.operands.push_back("ax");
            movAx.operands.push_back(ptrReg);
            lowered.instructions.push_back(movAx);
            // High word should be 0 for flat model
            Instruction286 xorDx;
            xorDx.mnemonic = "xor";
            xorDx.operands.push_back("dx");
            xorDx.operands.push_back("dx");
            lowered.instructions.push_back(xorDx);
        }

        // Store result in memory (32-bit value in AX:DX)
        // Allocate space on stack for the result (pre-allocated in prologue)
        std::string resultStack = state.frame.allocTemp(4, true);
        std::string highOffsetStr = state.frame.getHighBpOffset(resultStack);

        // Store low word
        Instruction286 storeLow;
        storeLow.mnemonic = "mov";
        storeLow.operands.push_back("[" + resultStack + "]");
        storeLow.operands.push_back("ax");
        lowered.instructions.push_back(storeLow);

        // Store high word
        Instruction286 storeHigh;
        storeHigh.mnemonic = "mov";
        storeHigh.operands.push_back("[" + highOffsetStr + "]");
        storeHigh.operands.push_back("dx");
        lowered.instructions.push_back(storeHigh);

        // Update state: the result is at resultStack
        state.frame.setPhysReg(irInst.resultName, resultStack);
        
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerIntToPtr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // IntToPtr: result = inttoptr i32 value to ptr
    // On i286 protected mode, pointers are 32-bit (selector:offset)
    // For flat memory model, selector = 0, so we just need the offset
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    if (!irInst.operands.empty()) {
        std::string intReg = state.frame.getPhysReg(irInst.operands[0].name);
        std::string resultRegName = irInst.resultName;

        // Load the integer value (32-bit) into AX:DX
        // If the integer is in memory, load both words
        if (intReg.find("bp") != std::string::npos && intReg[0] != '[') {
            // Memory operand: load low word to AX, high word to DX
            Instruction286 loadLow;
            loadLow.mnemonic = "mov";
            loadLow.operands.push_back("ax");
            loadLow.operands.push_back("[" + intReg + "]");
            lowered.instructions.push_back(loadLow);

            Instruction286 loadHigh;
            loadHigh.mnemonic = "mov";
            loadHigh.operands.push_back("dx");
            loadHigh.operands.push_back("[" + intReg + "+2]");
            lowered.instructions.push_back(loadHigh);
        } else if (intReg != "ax") {
            // Register operand: move to AX
            Instruction286 movAx;
            movAx.mnemonic = "mov";
            movAx.operands.push_back("ax");
            movAx.operands.push_back(intReg);
            lowered.instructions.push_back(movAx);
            // High word should be 0 for flat model
            Instruction286 xorDx;
            xorDx.mnemonic = "xor";
            xorDx.operands.push_back("dx");
            xorDx.operands.push_back("dx");
            lowered.instructions.push_back(xorDx);
        }

        // Store result in memory (32-bit value in AX:DX)
        // Allocate space on stack for the result (pre-allocated in prologue)
        std::string resultStack = state.frame.allocTemp(4, true);
        std::string highOffsetStr = state.frame.getHighBpOffset(resultStack);

        // Store low word
        Instruction286 storeLow;
        storeLow.mnemonic = "mov";
        storeLow.operands.push_back("[" + resultStack + "]");
        storeLow.operands.push_back("ax");
        lowered.instructions.push_back(storeLow);

        // Store high word
        Instruction286 storeHigh;
        storeHigh.mnemonic = "mov";
        storeHigh.operands.push_back("[" + highOffsetStr + "]");
        storeHigh.operands.push_back("dx");
        lowered.instructions.push_back(storeHigh);

        // Update state: the result is at resultStack
        state.frame.setPhysReg(irInst.resultName, resultStack);
        
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286
