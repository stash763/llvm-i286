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

    // Handle LLVM intrinsics: lower llvm.memcpy to memcpy call
    bool isLLVMMemcpy = (callee.find("llvm.memcpy") != std::string::npos);
    if (isLLVMMemcpy) {
        callee = "memcpy";
    }
    
    // Lower llvm.va_start to llvm_va_start
    bool isLLVAVaStart = (callee == "llvm.va_start");
    if (isLLVAVaStart) {
        callee = "llvm_va_start";
    }
    
    // Lower llvm.va_end to llvm_va_end
    bool isLLVAVaEnd = (callee == "llvm.va_end");
    if (isLLVAVaEnd) {
        callee = "llvm_va_end";
    }
    
    // Resolve alias: if callee is an alias, use the target function
    auto aliasIt = state.aliasMap.find(callee);
    if (aliasIt != state.aliasMap.end()) {
        callee = aliasIt->second;
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

    // For llvm.memcpy intrinsic: drop the last argument (is_volatile, i1 type)
    // llvm.memcpy has 4 args: (dst, src, size, is_volatile) but memcpy has 3: (dst, src, size)
    if (isLLVMMemcpy && args.size() > 3) {
        args.pop_back();
    }

    // Push arguments right-to-left
    for (int i = static_cast<int>(args.size()) - 1; i >= 0; i--) {
        std::string argName = args[i];
        
        // Detect floating-point constants that we can't materialize
        // Skip them by pushing 0 instead (FP operations not fully supported)
        bool argIsFPConst = argName.find("0xK") != std::string::npos ||
                            (argName.find('e') != std::string::npos && 
                             (argName.find('+') != std::string::npos || argName.find('-') != std::string::npos));
        if (argIsFPConst) {
            // Push 0 instead of the FP constant
            Instruction286 pushZero;
            pushZero.mnemonic = "push";
            pushZero.operands.push_back("0");
            lowered.instructions.push_back(pushZero);
            continue;
        }

        // Determine if this is a vreg reference (starts with '%') or a constant/global
        bool argIsVregRef = (!argName.empty() && argName[0] == '%');
        // Use argName directly for vreg lookup (maps use full name with % prefix)
        std::string vregLookupName = argName;

        // Check if argument is a global variable (starts with '.' or '@')
        bool isGlobal = (!argName.empty() && (argName[0] == '.' || argName[0] == '@'));

        // Check if argument is a ptrtoint constant expression: ptrtoint(ptr@global to i32)
        bool isPtrToIntExpr = (!argName.empty() && argName.substr(0, 8) == "ptrtoint");
        std::string ptrToIntGlobalName;
        if (isPtrToIntExpr) {
            // Extract global name from "ptrtoint(ptr@name to i32)" or "ptrtoint(ptr@nametoi32)"
            auto atPos = argName.find('@');
            if (atPos != std::string::npos) {
                // Look for 'to' keyword after the global name
                auto toPos = argName.find("to", atPos + 1);
                if (toPos != std::string::npos) {
                    ptrToIntGlobalName = argName.substr(atPos + 1, toPos - atPos - 1);
                } else {
                    // Fallback: take everything after @ until ')'
                    auto endPos = argName.find(')', atPos + 1);
                    ptrToIntGlobalName = argName.substr(atPos + 1, endPos != std::string::npos ? endPos - atPos - 1 : std::string::npos);
                }
                isGlobal = true; // Treat as global for codegen purposes
            }
        }

        // Convert global variable name to NASM format (remove leading dot or @)
        std::string nasmName = argName;
        if (isGlobal && nasmName.size() > 1 && !isPtrToIntExpr) {
            // Regular global: convert leading . or @ to _ for NASM compatibility
            if (nasmName[0] == '.' || nasmName[0] == '@') {
                nasmName[0] = '_';
            }
        } else if (isGlobal && isPtrToIntExpr && !ptrToIntGlobalName.empty()) {
            // ptrtoint expression: use the extracted global name directly
            nasmName = ptrToIntGlobalName;
        }

        // Check if argument is a vreg/parameter (either register-allocated or stack-allocated)
        // Only look up vreg maps if the argument has a % prefix (vreg reference)
        bool argIsVreg = false;
        if (argIsVregRef) {
            argIsVreg = (state.frame.hasSlot(argName)) ||
                         (state.frame.hasSlot(argName));
        }

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
                if (isPtrToIntExpr) {
                    // ptrtoint constant expression - push address of global
                    // High word = 0, low word = address (offset)
                    Instruction286 leaLow;
                    leaLow.mnemonic = "lea";
                    leaLow.operands.push_back("ax");
                    leaLow.operands.push_back("[" + nasmName + "]");
                    lowered.instructions.push_back(leaLow);

                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back("0");
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back("ax");
                    lowered.instructions.push_back(pushLow);
                } else {
                    // Global variable - push both words of VALUE stored at address
                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back("word [" + nasmName + "+2]");
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back("word [" + nasmName + "]");
                    lowered.instructions.push_back(pushLow);
                }
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
                // Low word: lower 16 bits
                int16_t lowWord = (int16_t)((uint32_t)val & 0xFFFF);
                // High word: sign-extended upper 16 bits
                int16_t highWord = (int16_t)((uint32_t)val >> 16);

                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back("ax");
                movConst.operands.push_back(std::to_string((int)lowWord));
                lowered.instructions.push_back(movConst);

                Instruction286 pushHigh;
                pushHigh.mnemonic = "push";
                pushHigh.operands.push_back(std::to_string((int)highWord));
                lowered.instructions.push_back(pushHigh);

                Instruction286 pushLow;
                pushLow.mnemonic = "push";
                pushLow.operands.push_back("ax");
                lowered.instructions.push_back(pushLow);
              } else {
                // Check if this argument is an alloca result (stack offset IS the address)
                // Use argName (with % prefix) for consistent lookup with other codegen
                bool argIsAlloca = state.frame.isAlloca(argName);
                  if (argIsAlloca) {
                    // Alloca result: the stack offset IS the pointer value
                    // We need to compute bp + offset at runtime and push that as the address
                    // Push low word = bp + offset (computed at runtime via lea)
                    // Push high word = SS selector (for far pointer in OS/2 1.x segmented model)
                    std::string argReg = state.frame.getPhysReg(argName);
                    int offset = 0;
                    if (argReg.find("bp") != std::string::npos) {
                        std::string offsetStr = argReg.substr(2); // Remove "bp"
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
                    }
                    // Compute bp + offset: lea ax, [bp+offset]
                    Instruction286 leaAx;
                    leaAx.mnemonic = "lea";
                    leaAx.operands.push_back("ax");
                    if (offset < 0) {
                        leaAx.operands.push_back("[" + std::string("bp") + std::to_string(offset) + "]");
                    } else if (offset > 0) {
                        leaAx.operands.push_back("[" + std::string("bp+") + std::to_string(offset) + "]");
                    } else {
                        leaAx.operands.push_back("[" + std::string("bp") + "]");
                    }
                    lowered.instructions.push_back(leaAx);

                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back("ss");
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back("ax");
                    lowered.instructions.push_back(pushLow);
                } else {
                    std::string argReg = state.frame.getPhysReg(vregLookupName);
                    if (argReg.find("bp") != std::string::npos) {
                        // Memory operand (value stored at stack location) - push both words
                        // Push high word first, then low word
                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back("word [" + argReg + "+2]");
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
                std::string argReg = state.frame.getPhysReg(vregLookupName);
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
    // Check if callee is an indirect call through a vreg (function pointer)
    if (!callee.empty() && callee[0] == '%') {
        // Indirect call: load function pointer and call through register
        // The callee vreg should map to a stack slot or register
        std::string calleeReg = state.frame.getPhysReg(callee);
        if (calleeReg.find("bp") != std::string::npos) {
            // Value is at a stack location - load into BX and call
            Instruction286 loadPtr;
            loadPtr.mnemonic = "lea";
            loadPtr.operands.push_back("bx");
            loadPtr.operands.push_back("[" + calleeReg + "]");
            lowered.instructions.push_back(loadPtr);
            
            Instruction286 callIndirect;
            callIndirect.mnemonic = "call";
            callIndirect.operands.push_back("bx");
            lowered.instructions.push_back(callIndirect);
        } else {
            // Value is in a register - call through that register
            Instruction286 callIndirect;
            callIndirect.mnemonic = "call";
            callIndirect.operands.push_back(calleeReg);
            lowered.instructions.push_back(callIndirect);
        }
    } else {
        Instruction286 callInst;
        callInst.mnemonic = "call";
        callInst.operands.push_back(callee);
        lowered.instructions.push_back(callInst);
    }

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
            // 32-bit result: store ax (low) and dx (high) to stack
            std::string resultStack = state.frame.getPhysReg(irInst.resultName);
            if (resultStack == "ax" || resultStack == "bx" || resultStack == "cx" || resultStack == "dx") {
                resultStack = state.frame.allocTemp(4, true);
            }
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + resultStack + "]");
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);
            
            std::string highOffset = state.frame.getHighBpOffset(resultStack);
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            storeHigh.operands.push_back("[" + highOffset + "]");
            storeHigh.operands.push_back("dx");
            lowered.instructions.push_back(storeHigh);
            
            state.frame.setPhysReg(irInst.resultName, resultStack);
        } else {
            // 16-bit result: store ax to stack
            std::string resultStack = state.frame.getPhysReg(irInst.resultName);
            if (resultStack == "ax" || resultStack == "bx" || resultStack == "cx" || resultStack == "dx") {
                resultStack = state.frame.allocTemp(2, false);
            }
            Instruction286 storeResult;
            storeResult.mnemonic = "mov";
            storeResult.operands.push_back("[" + resultStack + "]");
            storeResult.operands.push_back("ax");
            lowered.instructions.push_back(storeResult);
            
            state.frame.setPhysReg(irInst.resultName, resultStack);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286
