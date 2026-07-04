// Call Operations - Instruction Selection
// Handler for call instruction lowering

#include "codegen/InstructionSelectInternal.h"

#include <string>
#include <vector>
#include <cstdint>

namespace llvm_i286 {
namespace codegen {

std::vector<LoweredInstruction> lowerCall(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Lower call: result = call callee(arg1, arg2, ...)
    // Calling convention: arguments pushed right-to-left, return value in AX
    // Caller cleans up stack after call

    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    std::string callee;
    if (!irInst.calleeName.empty()) {
        callee = irInst.calleeName;
    } else if (!irInst.operands.empty()) {
        callee = irInst.operands[0].name;
    }

    // Push arguments (right-to-left order)
    // Arguments are in irInst.callArgs (if available) or irInst.operands[1..n]
    std::vector<std::string> args;
    if (!irInst.callArgs.empty()) {
        args = irInst.callArgs;
    } else {
        // Fallback: use operands starting from index 1
        for (size_t i = 1; i < irInst.operands.size(); i++) {
            args.push_back(irInst.operands[i].name);
        }
    }

    // Push arguments right-to-left
    for (int i = static_cast<int>(args.size()) - 1; i >= 0; i--) {
        std::string argName = args[i];

        // Check if argument is a global variable (starts with '.')
        bool isGlobal = (!argName.empty() && argName[0] == '.');

        // Convert global variable name to NASM format (remove leading dot, add underscore)
        std::string nasmName = argName;
        if (isGlobal && nasmName.size() > 1) {
            nasmName = "_" + nasmName.substr(1);
        }

        // Check if argument is a vreg/parameter
        bool argIsVreg = (state.vregToPhys.find(argName) != state.vregToPhys.end());

        // Check if argument is a constant (not a vreg)
        bool isConst = false;
        if (!argIsVreg && !isGlobal) {
            try {
                std::stoi(argName);
                isConst = true;
            } catch (...) {}
        }

        // Check if this is a 32-bit argument
        // Look up the function's parameter types to determine if this argument is 32-bit
        bool is32 = false;
        auto declIt = state.funcParamBitWidths.find(callee);
        if (declIt != state.funcParamBitWidths.end()) {
            // Use the function's parameter types
            if (i >= 0 && i < (int)declIt->second.size()) {
                is32 = (declIt->second[i] == 32);
            }
        } else if (irInst.resultType) {
            // Fallback: assume all arguments are the same type as result
            is32 = irInst.resultType->bitWidth == 32;
        }

        if (is32) {
            // 32-bit argument: push high word first, then low word
            if (isGlobal) {
                // Global variable - push both words
                Instruction286 pushHigh;
                pushHigh.mnemonic = "push";
                pushHigh.operands.push_back("word [" + nasmName + "+2]");
                lowered.instructions.push_back(pushHigh);

                Instruction286 pushLow;
                pushLow.mnemonic = "push";
                pushLow.operands.push_back("word [" + nasmName + "]");
                lowered.instructions.push_back(pushLow);
            } else if (isConst) {
                // 32-bit constant: split into low and high words
                // Parse the constant as a 32-bit integer
                int32_t val = 0;
                try {
                    val = std::stoi(args[i]);
                } catch (...) {
                    // If parse fails, treat as 0
                    val = 0;
                }
                // High word: sign-extended upper 16 bits
                int16_t highWord = (int16_t)((uint32_t)val >> 16);
                // Low word: lower 16 bits
                int16_t lowWord = (int16_t)((uint32_t)val & 0xFFFF);

                Instruction286 pushHigh;
                pushHigh.mnemonic = "push";
                pushHigh.operands.push_back(std::to_string((int)highWord));
                lowered.instructions.push_back(pushHigh);

                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back("ax");
                movConst.operands.push_back(std::to_string((int)lowWord));
                lowered.instructions.push_back(movConst);

                Instruction286 pushLow;
                pushLow.mnemonic = "push";
                pushLow.operands.push_back("ax");
                lowered.instructions.push_back(pushLow);
            } else {
                std::string argReg = state.getPhysReg(argName);
                if (argReg.find("bp") != std::string::npos) {
                    // Memory operand - push both words
                    // Compute high word offset properly
                    std::string highAddr;
                    if (argReg.find("bp") != std::string::npos) {
                        int offset = 0;
                        std::string offsetStr = argReg.substr(2); // Remove "bp"
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
                        int newOffset = offset + 2;
                        std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                        highAddr = "[" + std::string("bp") + offsetStr2 + "]";
                    } else {
                        highAddr = "[" + argReg + "+2]";
                    }

                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back("word " + highAddr);
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back("word [" + argReg + "]");
                    lowered.instructions.push_back(pushLow);
                } else {
                    // Register operand - push DX (high), then register (low)
                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back("dx");
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back(argReg);
                    lowered.instructions.push_back(pushLow);
                }
            }
        } else {
            // 16-bit argument (original code)
            if (isGlobal) {
                // Global variable - push its offset/label directly
                Instruction286 pushInst;
                pushInst.mnemonic = "push";
                pushInst.operands.push_back(nasmName);
                lowered.instructions.push_back(pushInst);
            } else if (isConst) {
                // Load constant into AX first, then push
                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back("ax");
                movConst.operands.push_back(args[i]);
                lowered.instructions.push_back(movConst);

                Instruction286 pushInst;
                pushInst.mnemonic = "push";
                pushInst.operands.push_back("ax");
                lowered.instructions.push_back(pushInst);
            } else {
                std::string argReg = state.getPhysReg(argName);
                if (argReg.find("bp") != std::string::npos) {
                    // Memory operand - load into AX first
                    Instruction286 loadInst;
                    loadInst.mnemonic = "mov";
                    loadInst.operands.push_back("ax");
                    loadInst.operands.push_back("[" + argReg + "]");
                    lowered.instructions.push_back(loadInst);

                    Instruction286 pushInst;
                    pushInst.mnemonic = "push";
                    pushInst.operands.push_back("ax");
                    lowered.instructions.push_back(pushInst);
                } else {
                    // Register operand - push directly
                    Instruction286 pushInst;
                    pushInst.mnemonic = "push";
                    pushInst.operands.push_back(argReg);
                    lowered.instructions.push_back(pushInst);
                }
            }
        }
    }

    // Call the function
    Instruction286 callInst;
    callInst.mnemonic = "call";
    callInst.operands.push_back(callee);
    lowered.instructions.push_back(callInst);

    // Clean up stack (caller pays)
    if (!args.empty()) {
        // Calculate stack bytes - need to account for 32-bit args
        int stackBytes = 0;
        for (size_t argIdx = 0; argIdx < args.size(); argIdx++) {
            bool isArg32 = false;
            auto declIt = state.funcParamBitWidths.find(callee);
            if (declIt != state.funcParamBitWidths.end()) {
                if (argIdx < declIt->second.size()) {
                    isArg32 = (declIt->second[argIdx] == 32);
                }
            } else if (irInst.resultType) {
                isArg32 = irInst.resultType->bitWidth == 32;
            }
            stackBytes += isArg32 ? 4 : 2;
        }
        Instruction286 addSp;
        addSp.mnemonic = "add";
        addSp.operands.push_back("sp");
        addSp.operands.push_back(std::to_string(stackBytes));
        lowered.instructions.push_back(addSp);
    }

    // Result is in AX (and DX for 32-bit)
    if (!irInst.resultName.empty()) {
        // Check if this is a 32-bit result
        bool is32 = false;
        if (irInst.resultType) {
            is32 = irInst.resultType->bitWidth == 32;
        }
        
        if (is32) {
            state.mark32Bit(irInst.resultName);
        }
        
        state.updateResultReg(irInst.resultName, "ax");
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286
