// Call Operations - Instruction Selection
// Handler for call instruction lowering

#include "codegen/InstructionSelectInternal.h"
#include "codegen/NasmSafe.h"

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

    bool skipCallAndCleanup = false;
    
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
    
    // Skip inline assembly calls (compiler barriers, etc.)
    if (callee.find("asm") == 0 || callee.find("asm sideeffect") == 0) {
        // Inline asm is a no-op for our purposes - just return empty lowered
        return loweredVec;
    }
    
    // Detect llvm.va_start for inline generation
    bool isLLVAVaStart = (callee == "llvm.va_start");
    
    // Lower llvm.va_end to llvm_va_end (no-op, but keep for now)
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
    
    // For llvm.va_start: add synthetic second argument (address of last named param)
    // Our C implementation expects: llvm_va_start(void *ap, void *last_arg)
    // The last_arg should point to the first parameter of the calling function
    // In the calling function's stack frame: [bp+4] = first param
    if (isLLVAVaStart) {
        // Add synthetic argument: address of last named parameter (bp+4)
        args.push_back("bp_plus_4_sentinel");
        // We need a local copy of callArgKinds to modify (since irInst is const)
        // The synthetic arg is a far pointer (32-bit)
        // We'll handle this in the stack cleanup by checking for synthetic args
        
        // Mark the va_list alloca as SS-derived for pointer loads
        // After va_start, the va_list contains SS-derived pointers to varargs
        if (!args.empty()) {
            std::string apArg = args[0]; // First arg is the ap alloca
            // The ap alloca's contents are SS-derived pointers
            state.ssDerivedPtrVregs.insert(apArg);
        }
    }
    
    // llvm.va_end is a no-op - clear args to skip call and cleanup
    if (isLLVAVaEnd) {
        args.clear();
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
        
        // Handle synthetic bp+4 sentinel for llvm_va_start
        if (argName == "bp_plus_4_sentinel") {
            // Push the far pointer address of the last named parameter (bp+4)
            // For near calls: bp+2 = return IP, bp+4 = first parameter offset
            // For far calls: bp+2 = return IP, bp+4 = return CS, bp+6 = first param offset
            // Our calls are near (within segment), so first param is at bp+4.
            // High word = SS (segment selector for stack), low word = offset
            Instruction286 leaArg;
            leaArg.mnemonic = "lea";
            leaArg.operands.push_back("ax");
            leaArg.operands.push_back("[bp+4]");
            lowered.instructions.push_back(leaArg);
            
            Instruction286 pushHigh;
            pushHigh.mnemonic = "push";
            pushHigh.operands.push_back("SS");
            lowered.instructions.push_back(pushHigh);
            
            Instruction286 pushLow;
            pushLow.mnemonic = "push";
            pushLow.operands.push_back("ax");
            lowered.instructions.push_back(pushLow);
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
        // Mangle NASM reserved words
        if (!nasmName.empty()) {
            nasmName = safeNasmName(nasmName);
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
            } else if (i >= (int)declIt->second.size()) {
                // Argument beyond the fixed parameter count (variadic function)
                // Fall back to result type or assume 32-bit
                is32 = irInst.resultType ? (irInst.resultType->bitWidth == 32) : true;
            }
        } else if (irInst.resultType) {
            // Fallback: assume all arguments are the same type as result
            is32 = irInst.resultType->bitWidth == 32;
        }

        if (is32) {
            // 32-bit argument: push high word first, then low word (little-endian stack layout)
            // Check if this is a pointer argument (push ADDRESS, not DATA)
            bool isPtrArg = (i >= 0 && i < (int)irInst.callArgKinds.size()) ? irInst.callArgKinds[i] : false;
            
            if (isGlobal) {
                if (isPtrArg) {
                    // Pointer argument - push the far pointer ADDRESS (seg:offset)
                    // Push selector (high word) first, then offset (low word)
                    // Stack layout: [offset][selector] (little-endian)
                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back("seg " + nasmName);
                    lowered.instructions.push_back(pushHigh);
                    
                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back(nasmName);
                    lowered.instructions.push_back(pushLow);
                } else if (isPtrToIntExpr) {
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
            } else if (isPtrArg && !isGlobal) {
                // Pointer vreg argument - push the far pointer ADDRESS (seg:offset)
                // Get the physical register for the pointer value
                std::string argReg = state.frame.getPhysReg(vregLookupName);
                
                // Determine if this pointer is SS-derived
                // Note: params are NOT SS-derived because the pointer VALUE could point to DS or SS
                bool argIsSS = state.frame.isAlloca(vregLookupName) ||
                               state.ssDerivedPtrVregs.count(vregLookupName) > 0;
                std::string selector = argIsSS ? "ss" : "0";
                
                if (argReg.find("bp") != std::string::npos) {
                    // Check if this is an alloca: the stack offset IS the pointer value
                    bool argIsAlloca = state.frame.isAlloca(vregLookupName);
                    if (argIsAlloca) {
                        // Alloca result: the stack offset IS the pointer value
                        // Push selector (SS for SS-derived, 0 for DS-derived) and bp+offset (offset)
                        int offset = 0;
                        std::string offsetStr = argReg.substr(2); // Remove "bp"
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
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
                        pushHigh.operands.push_back(selector);
                        lowered.instructions.push_back(pushHigh);

                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back("ax");
                        lowered.instructions.push_back(pushLow);
                    } else {
                        // Pointer value stored on stack - push both words
                        // Push selector (high word) first, then offset (low word)
                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back("word [" + argReg + "+2]");
                        lowered.instructions.push_back(pushHigh);
                        
                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back("word [" + argReg + "]");
                        lowered.instructions.push_back(pushLow);
                    }
                } else {
                    // Pointer value in a register - push selector (high), then register (low)
                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back(selector);
                    lowered.instructions.push_back(pushHigh);
                    
                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back(argReg);
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
                // Determine if this pointer is SS-derived
                // Note: params are NOT SS-derived because the pointer VALUE could point to DS or SS
                bool argIsSS = state.frame.isAlloca(argName) ||
                               state.ssDerivedPtrVregs.count(argName) > 0;
                std::string selector = argIsSS ? "ss" : "0";
                  if (argIsAlloca) {
                    // Alloca result: the stack offset IS the pointer value
                    // We need to compute bp + offset at runtime and push that as the address
                    // Push low word = bp + offset (computed at runtime via lea)
                    // Push high word = selector (SS for SS-derived, 0 for DS-derived)
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
                    pushHigh.operands.push_back(selector);
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
                // Check if this is a pointer argument (push ADDRESS, not DATA)
                bool isPtrArg16 = (i >= 0 && i < (int)irInst.callArgKinds.size()) ? irInst.callArgKinds[i] : false;
                
                if (isPtrArg16) {
                    // Pointer argument - push the far pointer ADDRESS (seg:offset)
                    std::string argReg = state.frame.getPhysReg(vregLookupName);
                    if (argReg.find("bp") != std::string::npos) {
                        // Check if this is an alloca: the stack offset IS the pointer value
                        bool argIsAlloca = state.frame.isAlloca(vregLookupName);
                        if (argIsAlloca) {
                            // Alloca result: the stack offset IS the pointer value
                            int offset = 0;
                            std::string offsetStr = argReg.substr(2); // Remove "bp"
                            if (!offsetStr.empty()) {
                                try { offset = std::stoi(offsetStr); } catch (...) {}
                            }
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
                            // Pointer value stored on stack - push both words
                            // Push selector (high word) first, then offset (low word)
                            Instruction286 pushHigh;
                            pushHigh.mnemonic = "push";
                            pushHigh.operands.push_back("word [" + argReg + "+2]");
                            lowered.instructions.push_back(pushHigh);
                            
                            Instruction286 pushLow;
                            pushLow.mnemonic = "push";
                            pushLow.operands.push_back("word [" + argReg + "]");
                            lowered.instructions.push_back(pushLow);
                        }
                    } else {
                        // Pointer value in a register - push DX (high), then register (low)
                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back("dx");
                        lowered.instructions.push_back(pushHigh);
                        
                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back(argReg);
                        lowered.instructions.push_back(pushLow);
                    }
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
    }

    // For llvm.va_start: generate inline code instead of calling external function
    if (isLLVAVaStart) {
        // We pushed 2 args (ap and last_arg), each 32-bit (4 bytes each = 8 bytes total)
        // Stack layout after pushes: [ap_offset][ap_selector][last_arg_offset][last_arg_selector]
        // After call near pushes return IP: [return_IP][ap_offset][ap_selector][last_arg_offset][last_arg_selector]
        // We can access these from [bp+2], [bp+4], [bp+6], [bp+8] respectively
        // Actually, since we haven't called anything, we can pop them directly
        
        // Pop last_arg (the arg we just pushed last, so it's on top of stack)
        // last_arg was pushed as: push SS (selector), push ax (offset where ax=bp+4)
        // So stack top is: [last_arg_offset][last_arg_selector=SS]
        
        // Pop last_arg_selector into DX
        Instruction286 popLastArgSel;
        popLastArgSel.mnemonic = "pop";
        popLastArgSel.operands.push_back("dx");
        lowered.instructions.push_back(popLastArgSel);
        
        // Pop last_arg_offset into AX
        Instruction286 popLastArgOff;
        popLastArgOff.mnemonic = "pop";
        popLastArgOff.operands.push_back("ax");
        lowered.instructions.push_back(popLastArgOff);
        
        // Pop ap_selector into CX (we'll use this for storing to *ap)
        Instruction286 popApSel;
        popApSel.mnemonic = "pop";
        popApSel.operands.push_back("cx");
        lowered.instructions.push_back(popApSel);
        
        // Pop ap_offset into BX
        Instruction286 popApOff;
        popApOff.mnemonic = "pop";
        popApOff.operands.push_back("bx");
        lowered.instructions.push_back(popApOff);
        
        // Compute first_arg = last_arg + 4
        Instruction286 addFirstArg;
        addFirstArg.mnemonic = "add";
        addFirstArg.operands.push_back("ax");
        addFirstArg.operands.push_back("4");
        lowered.instructions.push_back(addFirstArg);
        // Note: carry flag is set if overflow, but we ignore it for now
        // In practice, last_arg is a stack offset, so adding 4 won't overflow
        
        // Store first_arg to *ap
        // *ap = first_arg offset (low word)
        Instruction286 storeApLow;
        storeApLow.mnemonic = "mov";
        storeApLow.operands.push_back("[ss:bx]");
        storeApLow.operands.push_back("ax");
        lowered.instructions.push_back(storeApLow);
        
        // *(ap+2) = SS (selector for stack)
        Instruction286 storeApHigh;
        storeApHigh.mnemonic = "mov";
        storeApHigh.operands.push_back("[ss:bx+2]");
        storeApHigh.operands.push_back("ss");
        lowered.instructions.push_back(storeApHigh);
        
        // Set flag to skip the normal call and stack cleanup (we already popped the args)
        skipCallAndCleanup = true;
    }
    
    // llvm.va_end is a no-op - skip entirely
    if (isLLVAVaEnd) {
        skipCallAndCleanup = true;
    }
    
    // Call the function (skip for inline intrinsics like llvm.va.start)
    if (!skipCallAndCleanup) {
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
    }

    // Clean up stack (caller pays)
    if (!skipCallAndCleanup && !args.empty()) {
        // Calculate stack bytes - need to account for 32-bit args
        int stackBytes = 0;
        for (size_t argIdx = 0; argIdx < args.size(); argIdx++) {
            bool isArg32 = false;
            // Check callArgKinds first (true = pointer arg, which is 32-bit)
            if (argIdx < irInst.callArgKinds.size() && irInst.callArgKinds[argIdx]) {
                isArg32 = true;
            } else if (isLLVAVaStart && argIdx == args.size() - 1) {
                // Synthetic last_arg argument for llvm_va_start is a far pointer (32-bit)
                isArg32 = true;
            } else {
                // Use funcParamBitWidths
                auto declIt = state.funcParamBitWidths.find(callee);
                if (declIt != state.funcParamBitWidths.end()) {
                    if (argIdx < declIt->second.size()) {
                        isArg32 = (declIt->second[argIdx] == 32);
                    } else {
                        // Beyond fixed param count (variadic) - assume 32-bit
                        isArg32 = irInst.resultType ? (irInst.resultType->bitWidth == 32) : true;
                    }
                } else if (irInst.resultType) {
                    isArg32 = irInst.resultType->bitWidth == 32;
                }
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
