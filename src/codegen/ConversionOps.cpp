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
        std::string srcReg = state.getPhysReg(irInst.operands[0].name);
        std::string destReg = resultReg.empty() ? "ax" : resultReg;

        // For now, just move the value (16-bit trunc is no-op on 16-bit target)
        if (destReg != srcReg) {
            Instruction286 movInst;
            movInst.mnemonic = "mov";
            movInst.operands.push_back(destReg);
            movInst.operands.push_back(srcReg);
            lowered.instructions.push_back(movInst);
        }

        if (!irInst.resultName.empty()) {
            state.updateResultReg(irInst.resultName, destReg);
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
        std::string srcReg = state.getPhysReg(irInst.operands[0].name);
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
            state.updateResultReg(irInst.resultName, destReg);
        }

        // If extending to 32-bit, zero-extend to DX:AX
        if (irInst.resultType && irInst.resultType->bitWidth == 32) {
            // Clear DX for zero extension
            Instruction286 clearDx;
            clearDx.mnemonic = "xor";
            clearDx.operands.push_back("dx");
            clearDx.operands.push_back("dx");
            lowered.instructions.push_back(clearDx);

            // Allocate stack space and store the 32-bit result
            state.currentStackOffset -= 4;
            state.tempSpaceInBlock += 4;   // track temp space for cleanup
            int stackOffset = state.currentStackOffset;
            state.vregToStackOffset[irInst.resultName] = stackOffset;
            std::string resultStack = "bp" + std::to_string(stackOffset);
            state.mark32Bit(irInst.resultName);

            // Emit stack allocation
            Instruction286 subSp;
            subSp.mnemonic = "sub";
            subSp.operands.push_back("sp");
            subSp.operands.push_back("4");
            lowered.instructions.push_back(subSp);

            // Store low word (ax)
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + resultStack + "]");
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);

            // Store high word (dx = 0)
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            int hNewOffset = stackOffset + 2;
            std::string hOffsetStr2 = (hNewOffset >= 0) ? ("+" + std::to_string(hNewOffset)) : std::to_string(hNewOffset);
            storeHigh.operands.push_back("[" + std::string("bp") + hOffsetStr2 + "]");
            storeHigh.operands.push_back("dx");
            lowered.instructions.push_back(storeHigh);

            // Update vreg mapping to point to stack
            state.vregToPhys[irInst.resultName] = resultStack;
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
        std::string srcReg = state.getPhysReg(irInst.operands[0].name);
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
            state.currentStackOffset -= 4;
            state.tempSpaceInBlock += 4;   // track temp space for cleanup
            int stackOffset = state.currentStackOffset;
            state.vregToStackOffset[irInst.resultName] = stackOffset;
            std::string resultStack = "bp" + std::to_string(stackOffset);
            state.mark32Bit(irInst.resultName);

            // Emit stack allocation
            Instruction286 subSp;
            subSp.mnemonic = "sub";
            subSp.operands.push_back("sp");
            subSp.operands.push_back("4");
            lowered.instructions.push_back(subSp);

            // Store low word (ax)
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + resultStack + "]");
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);

            // Sign-extend: if high word of source was loaded, store it; otherwise sign-extend ax to dx
            if (srcIsMem && srcReg.find("bp") != std::string::npos) {
                // Source is on stack, load high word into dx
                int offset = 0;
                std::string offsetStr = srcReg.substr(2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                Instruction286 loadHigh;
                loadHigh.mnemonic = "mov";
                loadHigh.operands.push_back("dx");
                loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                lowered.instructions.push_back(loadHigh);
            } else {
                // Sign-extend ax to dx (cbw/cwd)
                Instruction286 cwdInst;
                cwdInst.mnemonic = "cwd";
                cwdInst.operands.push_back("dx");
                cwdInst.operands.push_back("ax");
                lowered.instructions.push_back(cwdInst);
            }

            // Store high word (dx)
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            int hOffset = stackOffset;
            int hNewOffset = hOffset + 2;
            std::string hOffsetStr2 = (hNewOffset >= 0) ? ("+" + std::to_string(hNewOffset)) : std::to_string(hNewOffset);
            storeHigh.operands.push_back("[" + std::string("bp") + hOffsetStr2 + "]");
            storeHigh.operands.push_back("dx");
            lowered.instructions.push_back(storeHigh);

            // Update vreg mapping to point to stack
            state.vregToPhys[irInst.resultName] = resultStack;
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
        std::string srcReg = state.getPhysReg(irInst.operands[0].name);

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
            state.updateResultReg(irInst.resultName, "ax");
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
        std::string srcReg = state.getPhysReg(irInst.operands[0].name);

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
            state.updateResultReg(irInst.resultName, "ax");
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
        std::string aggReg = state.getPhysReg(irInst.operands[0].name);

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
            state.updateResultReg(irInst.resultName, "ax");
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
        std::string aggReg = state.getPhysReg(irInst.operands[0].name);
        std::string valReg = state.getPhysReg(irInst.operands[1].name);

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

} // namespace codegen
} // namespace llvm_i286
