// Memory Operations: Alloca, Store, Load, GetElementPtr
// Extracted from InstructionSelect.cpp

#include "codegen/InstructionSelectInternal.h"

#include <string>

namespace llvm_i286 {
namespace codegen {

std::vector<LoweredInstruction> lowerAlloca(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Allocate stack space
    // The operand name contains the bit width as a string
    int bitWidth = 32; // default
    if (!irInst.operands.empty()) {
        try {
            bitWidth = std::stoi(irInst.operands[0].name);
        } catch (...) {
            bitWidth = 32;
        }
    }

    int byteSize = bitWidth / 8;
    if (byteSize < 2) byteSize = 2; // Minimum 2 bytes for 16-bit alignment

    // Allocate stack space
    int offset = state.allocateStack(byteSize);

    // Store the stack offset for this variable
    if (!irInst.resultName.empty()) {
        state.vregToStackOffset[irInst.resultName] = offset;
        // Mark as alloca result (this is an ADDRESS, not a value)
        state.allocaVregs.insert(irInst.resultName);
    }

    Instruction286 subSp;
    subSp.mnemonic = "sub";
    subSp.operands.push_back("sp");
    subSp.operands.push_back(std::to_string(byteSize));
    lowered.instructions.push_back(subSp);

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerStore(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Store value to memory
    // Operands: [0] = value to store, [1] = pointer to store to
    if (irInst.operands.size() >= 2) {
        std::string valName = irInst.operands[0].name;
        std::string ptrName = irInst.operands[1].name;
        std::string ptrReg = state.getPhysReg(ptrName);

        // Check if the pointer is an alloca result (ADDRESS, not VALUE)
        bool ptrIsAlloca = state.allocaVregs.find(ptrName) != state.allocaVregs.end();

        // Check if this is a 32-bit store
        // Check the type being stored (resultType) first, then fall back to vreg marking
        bool is32 = false;
        if (irInst.resultType && irInst.resultType->bitWidth == 32) {
            is32 = true;
        } else if (!irInst.resultType) {
            // No resultType available (shouldn't happen for store), check vreg marking
            is32 = state.is32BitReg(valName);
        }
        // Note: We do NOT mark valName as 32-bit here, since the same constant value
        // (e.g., "0") can be stored as different types. We mark the destination pointer.

        // Check if value is in a register (parameter or previously loaded value)
        // A parameter name like "0" or "1" will be in vregToPhys
        std::string valReg = state.getPhysReg(valName);
        bool isRegOrParam = (state.vregToPhys.find(valName) != state.vregToPhys.end());

        // Check if value is a constant (not a parameter or vreg)
        bool isConst = false;
        if (!isRegOrParam) {
            try {
                std::stoi(valName);
                isConst = true;
            } catch (...) {}
        }
        
        // Check if value is a global variable reference (starts with @)
        bool isGlobal = (!valName.empty() && valName[0] == '@');

        if (is32) {
            // 32-bit store: store both low and high words
            // Move value to AX first (low word)
            if (isConst) {
                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back("ax");
                movConst.operands.push_back(valName);
                lowered.instructions.push_back(movConst);
                // High word is 0 for constants
                Instruction286 xorDx;
                xorDx.mnemonic = "xor";
                xorDx.operands.push_back("dx");
                xorDx.operands.push_back("dx");
                lowered.instructions.push_back(xorDx);
           } else if (isGlobal) {
                // Global variable: load its address
                // The global name starts with @, remove it for NASM
                std::string globalName = valName.substr(1); // Remove @ prefix
                // If name starts with '.', replace with '_' for NASM compatibility
                if (!globalName.empty() && globalName[0] == '.') {
                    globalName[0] = '_';
                }
                // For 16-bit protected mode with flat memory model
                Instruction286 movAx;
                movAx.mnemonic = "mov";
                movAx.operands.push_back("ax");
                movAx.operands.push_back(globalName);
                lowered.instructions.push_back(movAx);
                // High word (segment) is DGROUP selector, but for flat model we can use 0
                Instruction286 xorDx;
                xorDx.mnemonic = "xor";
                xorDx.operands.push_back("dx");
                xorDx.operands.push_back("dx");
                lowered.instructions.push_back(xorDx);
            } else if (isRegOrParam) {
                // Check if value is in vregToPhys (register or stack location)
                auto valPhysIt = state.vregToPhys.find(valName);
                if (valPhysIt != state.vregToPhys.end()) {
                    std::string physReg = valPhysIt->second;
                    if (physReg.find("bp") != std::string::npos) {
                        // Value is stored as a stack location in vregToPhys
                        Instruction286 loadLow;
                        loadLow.mnemonic = "mov";
                        loadLow.operands.push_back("ax");
                        loadLow.operands.push_back("[" + physReg + "]");
                        lowered.instructions.push_back(loadLow);
                        Instruction286 loadHigh;
                        loadHigh.mnemonic = "mov";
                        loadHigh.operands.push_back("dx");
                        int offset = 0;
                        std::string offsetStr = physReg.substr(2);
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
                        int newOffset = offset + 2;
                        std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                        loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                        lowered.instructions.push_back(loadHigh);
                    } else {
                        // Value is in a register (e.g., ax from call return)
                        if (physReg != "ax") {
                            Instruction286 movInst;
                            movInst.mnemonic = "mov";
                            movInst.operands.push_back("ax");
                            movInst.operands.push_back(physReg);
                            lowered.instructions.push_back(movInst);
                        }
                        // DX already has the high word from call return or previous operation
                    }
                } else {
                    // Should not happen, but fall back to getPhysReg
                    if (valReg.find("bp") != std::string::npos) {
                        Instruction286 loadLow;
                        loadLow.mnemonic = "mov";
                        loadLow.operands.push_back("ax");
                        loadLow.operands.push_back("[" + valReg + "]");
                        lowered.instructions.push_back(loadLow);
                        Instruction286 loadHigh;
                        loadHigh.mnemonic = "mov";
                        loadHigh.operands.push_back("dx");
                        loadHigh.operands.push_back("[" + valReg + "+2]");
                        lowered.instructions.push_back(loadHigh);
                    }
                }
            } else {
                // Value is on stack (not in vregToPhys, but in vregToStackOffset)
                if (valReg.find("bp") != std::string::npos) {
                    Instruction286 loadLow;
                    loadLow.mnemonic = "mov";
                    loadLow.operands.push_back("ax");
                    loadLow.operands.push_back("[" + valReg + "]");
                    lowered.instructions.push_back(loadLow);
                    Instruction286 loadHigh;
                    loadHigh.mnemonic = "mov";
                    loadHigh.operands.push_back("dx");
                    loadHigh.operands.push_back("[" + valReg + "+2]");
                    lowered.instructions.push_back(loadHigh);
                }
            }

         // For non-alloca pointers, load the address to BX for dereferencing
            // In OS/2 1.x segmented memory model, far pointers have (selector:offset)
            // If selector is 0, use [bx] with DS (assumes DS=DGROUP)
            // If selector is non-zero, load ES with selector and use es:[bx]
            std::string dest;
            if (ptrReg.find("bp") != std::string::npos && ptrIsAlloca) {
                // Alloca result: stack location IS the address
                dest = "[" + ptrReg + "]";
            } else if (ptrReg.find("bp") != std::string::npos) {
                // Non-alloca: load the pointer offset (low word) to BX
                Instruction286 loadAddr;
                loadAddr.mnemonic = "mov";
                loadAddr.operands.push_back("bx");
                loadAddr.operands.push_back("[" + ptrReg + "]");
                lowered.instructions.push_back(loadAddr);

                // Calculate high word offset for the selector
                std::string selectorAddr;
                if (ptrReg.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = ptrReg.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    selectorAddr = (newOffset >= 0) ? ("bp+" + std::to_string(newOffset)) : ("bp" + std::to_string(newOffset));
                } else {
                    selectorAddr = ptrReg + "+2";
                }

                // Load selector value, and load ES if it's non-zero
                // For now, assume DS=DGROUP and just use [bx]
                // (This works for most cases where the selector is 0 or DGROUP)
                dest = "[" + std::string("bx") + "]";
            } else {
                dest = ptrReg;
            }

            // Store low word to [ptr]
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back(dest);
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);

            // Store high word to [ptr+2]
            // Need to calculate the offset properly
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            if (ptrIsAlloca && ptrReg.find("bp") != std::string::npos) {
                // Extract offset from ptrReg (e.g., "bp-10" -> -10)
                int offset = 0;
                std::string offsetStr = ptrReg.substr(2); // Remove "bp"
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                storeHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else if (ptrReg.find("bp") != std::string::npos) {
                // Non-alloca: use bx+2
                storeHigh.operands.push_back("[" + std::string("bx+2") + "]");
            } else {
                storeHigh.operands.push_back("[" + ptrReg + "+2]");
            }
            storeHigh.operands.push_back("dx");
            lowered.instructions.push_back(storeHigh);

            // Mark destination pointer as 32-bit (for subsequent loads)
            state.mark32Bit(ptrName);
        } else {
            // Check if this is an 8-bit store
            bool is8 = irInst.resultType && irInst.resultType->bitWidth == 8;

            // 8/16-bit store
            // Move value to AX/AL first
            if (isConst) {
                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back("ax");
                movConst.operands.push_back(valName);
                lowered.instructions.push_back(movConst);
            } else if (valReg != "ax") {
                Instruction286 movInst;
                movInst.mnemonic = "mov";
                movInst.operands.push_back("ax");
                // Check if valReg is a stack location (contains "bp")
                if (valReg.find("bp") != std::string::npos) {
                    movInst.operands.push_back("[" + valReg + "]");
                } else {
                    movInst.operands.push_back(valReg);
                }
                lowered.instructions.push_back(movInst);
            }

         // For non-alloca pointers, load the address to BX for dereferencing
            // In OS/2 1.x segmented memory model, far pointers have (selector:offset)
            // If selector is 0, use [bx] with DS (assumes DS=DGROUP)
            // If selector is non-zero, load ES with selector and use es:[bx]
            std::string dest;
            if (ptrReg.find("bp") != std::string::npos && ptrIsAlloca) {
                // Alloca result: stack location IS the address
                dest = "[" + ptrReg + "]";
            } else if (ptrReg.find("bp") != std::string::npos) {
                // Non-alloca: load the pointer offset (low word) to BX
                Instruction286 loadAddr;
                loadAddr.mnemonic = "mov";
                loadAddr.operands.push_back("bx");
                loadAddr.operands.push_back("[" + ptrReg + "]");
                lowered.instructions.push_back(loadAddr);

                // Calculate high word offset for the selector
                std::string selectorAddr;
                if (ptrReg.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = ptrReg.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    selectorAddr = (newOffset >= 0) ? ("bp+" + std::to_string(newOffset)) : ("bp" + std::to_string(newOffset));
                } else {
                    selectorAddr = ptrReg + "+2";
                }

                // For now, assume DS=DGROUP and just use [bx]
                // (This works for most cases where the selector is 0 or DGROUP)
                dest = "[" + std::string("bx") + "]";
            } else {
                dest = ptrReg;
            }

            // Store to [ptr]
            Instruction286 storeInst;
            storeInst.mnemonic = "mov";
            if (is8) {
                // 8-bit store: store byte from AL
                storeInst.operands.push_back("byte " + dest);
                storeInst.operands.push_back("al");
            } else {
                // 16-bit store: store word from AX
                storeInst.operands.push_back(dest);
                storeInst.operands.push_back("ax");
            }
            lowered.instructions.push_back(storeInst);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerLoad(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // Load value from memory
    // Operands: [0] = pointer to load from
    if (!irInst.operands.empty()) {
        std::string ptrReg = state.getPhysReg(irInst.operands[0].name);

        // Check if ptrReg is a bp-relative address (contains "bp")
        bool ptrIsMem = (ptrReg.find("bp") != std::string::npos);

        // Check if the pointer is an alloca result (ADDRESS, not VALUE)
        bool ptrIsAlloca = state.allocaVregs.find(irInst.operands[0].name) != state.allocaVregs.end();

        // Check if this is a 32-bit load
        bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;

        if (ptrIsMem) {
            if (is32) {
                // 32-bit load: load both low and high words, store to stack
                std::string stackLoc = ptrReg;

                // For non-alloca pointers, we need to load the pointer VALUE first,
                // then use that value as the address to load from
                std::string actualAddr = stackLoc;
                if (!ptrIsAlloca) {
                    // Load the 16-bit address from the pointer value to BX
                    Instruction286 loadAddr;
                    loadAddr.mnemonic = "mov";
                    loadAddr.operands.push_back("bx");
                    loadAddr.operands.push_back("[" + stackLoc + "]");
                    lowered.instructions.push_back(loadAddr);
                    actualAddr = "bx";
                }

                // Get result stack location
                // If the result doesn't have a stack offset, allocate one
                std::string resultStack = state.getPhysReg(irInst.resultName);
                if (resultStack == "ax" || resultStack == "bx" || resultStack == "cx" || resultStack == "dx") {
                    // Result is not on stack - allocate stack space
                    state.currentStackOffset -= 4; // 32-bit = 4 bytes
                    state.tempSpaceInBlock += 4;   // track temp space for cleanup
                    int stackOffset = state.currentStackOffset;
                    state.vregToStackOffset[irInst.resultName] = stackOffset;
                    resultStack = "bp" + std::to_string(stackOffset);

                    // Emit stack allocation
                    Instruction286 subSp;
                    subSp.mnemonic = "sub";
                    subSp.operands.push_back("sp");
                    subSp.operands.push_back("4");
                    lowered.instructions.push_back(subSp);
                }

                // Load low word from [actualAddr] into AX
                Instruction286 loadLow;
                loadLow.mnemonic = "mov";
                loadLow.operands.push_back("ax");
                if (actualAddr == "bx") {
                    loadLow.operands.push_back("[" + std::string("bx") + "]");
                } else {
                    loadLow.operands.push_back("[" + actualAddr + "]");
                }
                lowered.instructions.push_back(loadLow);

                // Store low word to result
                Instruction286 storeLow;
                storeLow.mnemonic = "mov";
                storeLow.operands.push_back("[" + resultStack + "]");
                storeLow.operands.push_back("ax");
                lowered.instructions.push_back(storeLow);

                // Load high word from [actualAddr+2] into DX
                Instruction286 loadHigh;
                loadHigh.mnemonic = "mov";
                loadHigh.operands.push_back("dx");
                if (actualAddr == "bx") {
                    loadHigh.operands.push_back("[" + std::string("bx+2") + "]");
                } else if (actualAddr.find("bp") != std::string::npos) {
                    int offset = 0;
                    std::string offsetStr = actualAddr.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                } else {
                    loadHigh.operands.push_back("[" + actualAddr + "+2]");
                }
                lowered.instructions.push_back(loadHigh);

                // Store high word to result
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
                storeHigh.operands.push_back("dx");
                lowered.instructions.push_back(storeHigh);

                // Mark result as 32-bit
                state.mark32Bit(irInst.resultName);

                // Update vreg mapping to point to stack (not register)
                // This ensures subsequent operations use the stack location
                state.vregToPhys[irInst.resultName] = resultStack;
            } else {
                // Check if this is an 8-bit load
                bool is8 = irInst.resultType && irInst.resultType->bitWidth == 8;

                // Assign a register for 16-bit loads (8-bit loads use AX then store to stack)
                std::string destReg = state.assignReg(irInst.resultName);

             // For non-alloca pointers, load the address to BX for dereferencing
                // In OS/2 1.x segmented memory model, far pointers have (selector:offset)
                // If selector is 0, use [bx] with DS (assumes DS=DGROUP)
                // If selector is non-zero, load ES with selector and use es:[bx]
                if (!ptrIsAlloca) {
                    Instruction286 loadAddr;
                    loadAddr.mnemonic = "mov";
                    loadAddr.operands.push_back("bx");
                    loadAddr.operands.push_back("[" + ptrReg + "]");
                    lowered.instructions.push_back(loadAddr);

                    // For now, assume DS=DGROUP and just use [bx]
                    // (This works for most cases where the selector is 0 or DGROUP)

                    // Load from [bx] into destReg (or AX then store to stack for 8-bit)
                    if (is8) {
                        // 8-bit load: load into AL, then zero-extend to AX
                        Instruction286 loadInst;
                        loadInst.mnemonic = "mov";
                        loadInst.operands.push_back("al");
                        loadInst.operands.push_back("byte [bx]");
                        lowered.instructions.push_back(loadInst);

                        // Zero-extend: set AH to 0
                        Instruction286 zeroExt;
                        zeroExt.mnemonic = "mov";
                        zeroExt.operands.push_back("ah");
                        zeroExt.operands.push_back("0");
                        lowered.instructions.push_back(zeroExt);

                        // Store AX to stack location for the result vreg
                        std::string resultStack = state.getPhysReg(irInst.resultName);
                        if (resultStack == "ax" || resultStack == "bx" || resultStack == "cx" || resultStack == "dx") {
                            // Result is not on stack - allocate stack space
                            state.currentStackOffset -= 2; // 16-bit = 2 bytes
                            state.tempSpaceInBlock += 2;   // track temp space for cleanup
                            int stackOffset = state.currentStackOffset;
                            state.vregToStackOffset[irInst.resultName] = stackOffset;
                            resultStack = "bp" + std::to_string(stackOffset);

                            // Emit stack allocation
                            Instruction286 subSp;
                            subSp.mnemonic = "sub";
                            subSp.operands.push_back("sp");
                            subSp.operands.push_back("2");
                            lowered.instructions.push_back(subSp);
                        }
                        // Store AX to result stack location
                        Instruction286 storeResult;
                        storeResult.mnemonic = "mov";
                        storeResult.operands.push_back("[" + resultStack + "]");
                        storeResult.operands.push_back("ax");
                        lowered.instructions.push_back(storeResult);

                        // Update vreg mapping to point to stack
                        state.vregToPhys[irInst.resultName] = resultStack;
                    } else {
                        // 16-bit load
                        Instruction286 loadInst;
                        loadInst.mnemonic = "mov";
                        loadInst.operands.push_back(destReg);
                        loadInst.operands.push_back("[" + std::string("bx") + "]");
                        lowered.instructions.push_back(loadInst);
                    }
                } else {
                    // Load from [bp-N] into destReg (alloca result, address is the stack location)
                    if (is8) {
                        // 8-bit load: load into AL, then zero-extend to AX
                        Instruction286 loadInst;
                        loadInst.mnemonic = "mov";
                        loadInst.operands.push_back("al");
                        loadInst.operands.push_back("byte [" + ptrReg + "]");
                        lowered.instructions.push_back(loadInst);

                        // Zero-extend: set AH to 0
                        Instruction286 zeroExt;
                        zeroExt.mnemonic = "mov";
                        zeroExt.operands.push_back("ah");
                        zeroExt.operands.push_back("0");
                        lowered.instructions.push_back(zeroExt);

                        // Store AX to stack location for the result vreg
                        std::string resultStack = state.getPhysReg(irInst.resultName);
                        if (resultStack == "ax" || resultStack == "bx" || resultStack == "cx" || resultStack == "dx") {
                            // Result is not on stack - allocate stack space
                            state.currentStackOffset -= 2; // 16-bit = 2 bytes
                            state.tempSpaceInBlock += 2;   // track temp space for cleanup
                            int stackOffset = state.currentStackOffset;
                            state.vregToStackOffset[irInst.resultName] = stackOffset;
                            resultStack = "bp" + std::to_string(stackOffset);

                            // Emit stack allocation
                            Instruction286 subSp;
                            subSp.mnemonic = "sub";
                            subSp.operands.push_back("sp");
                            subSp.operands.push_back("2");
                            lowered.instructions.push_back(subSp);
                        }
                        // Store AX to result stack location
                        Instruction286 storeResult;
                        storeResult.mnemonic = "mov";
                        storeResult.operands.push_back("[" + resultStack + "]");
                        storeResult.operands.push_back("ax");
                        lowered.instructions.push_back(storeResult);

                        // Update vreg mapping to point to stack
                        state.vregToPhys[irInst.resultName] = resultStack;
                    } else {
                        // 16-bit load
                        Instruction286 loadInst;
                        loadInst.mnemonic = "mov";
                        loadInst.operands.push_back(destReg);
                        loadInst.operands.push_back("[" + ptrReg + "]");
                        lowered.instructions.push_back(loadInst);
                    }
                }
            }
        }
        // else: loading from a register, no-op (value is already there)
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> lowerGetElementPtr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    // GEP: result = getelementptr type, ptr base, i32 idx1, i32 idx2, ...
    // For opaque pointers (ptr), element size = 1 (byte-level arithmetic)
    // For typed pointers, element size = sizeof(elementType)
    if (!irInst.operands.empty()) {
        std::string baseReg = state.getPhysReg(irInst.operands[0].name);

        // Check if this is a 32-bit pointer result
        bool is32 = (irInst.resultType && irInst.resultType->bitWidth == 32);

        // Determine element size from result type
        // For opaque pointers (void*), elemSize = 1
        int elemSize = 1;
        if (irInst.resultType && irInst.resultType->isPointer()) {
            if (irInst.resultType->elementType &&
                irInst.resultType->elementType->isInteger()) {
                elemSize = irInst.resultType->elementType->getBitWidth() / 8;
            }
            // If elementType is void (opaque pointer), elemSize stays 1
        }

        if (is32) {
            // 32-bit GEP: load base pointer (low + high), add offset, store result
            // Check if base is an alloca result (address IS the stack offset, not a value at it)
            bool baseIsAlloca = state.allocaVregs.find(irInst.operands[0].name) != state.allocaVregs.end();

              if (baseIsAlloca && baseReg.find("bp") != std::string::npos) {
                // For alloca results: use lea to get the address (stack offset IS the address)
                Instruction286 leaLow;
                leaLow.mnemonic = "lea";
                leaLow.operands.push_back("ax");
                leaLow.operands.push_back("[" + baseReg + "]");
                lowered.instructions.push_back(leaLow);

                // High word is SS selector (for far pointer in OS/2 1.x segmented model)
                Instruction286 pushSelector;
                pushSelector.mnemonic = "mov";
                pushSelector.operands.push_back("dx");
                pushSelector.operands.push_back("ss");
                lowered.instructions.push_back(pushSelector);
            } else if (baseReg.find("bp") != std::string::npos) {
                // Non-alloca: load 32-bit pointer from stack (low + high words)
                Instruction286 movLow;
                movLow.mnemonic = "mov";
                movLow.operands.push_back("ax");
                movLow.operands.push_back("[" + baseReg + "]");
                lowered.instructions.push_back(movLow);

                // Load high word of base to dx
                Instruction286 movHigh;
                movHigh.mnemonic = "mov";
                movHigh.operands.push_back("dx");
                // Calculate high word offset
                {
                    int offset = 0;
                    std::string offsetStr = baseReg.substr(2);
                    if (!offsetStr.empty()) {
                        try { offset = std::stoi(offsetStr); } catch (...) {}
                    }
                    int newOffset = offset + 2;
                    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                    movHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                }
                lowered.instructions.push_back(movHigh);
            } else if (baseReg != "ax") {
                Instruction286 movLow;
                movLow.mnemonic = "mov";
                movLow.operands.push_back("ax");
                movLow.operands.push_back(baseReg);
                lowered.instructions.push_back(movLow);
                // High word is 0 for register values (near pointer)
                Instruction286 xorHigh;
                xorHigh.mnemonic = "xor";
                xorHigh.operands.push_back("dx");
                xorHigh.operands.push_back("dx");
                lowered.instructions.push_back(xorHigh);
            }

            // Add offset for each index
            for (size_t i = 1; i < irInst.operands.size(); i++) {
                std::string idxName = irInst.operands[i].name;

                // Check if index is a constant (check original name, not register)
                bool isConst = false;
                int constVal = 0;
                try {
                    constVal = std::stoi(idxName);
                    isConst = true;
                } catch (...) {}

                std::string idxReg = state.getPhysReg(irInst.operands[i].name);

                if (isConst && constVal != 0) {
                    // Add constant offset to low word
                    Instruction286 addConst;
                    addConst.mnemonic = "add";
                    addConst.operands.push_back("ax");
                    addConst.operands.push_back(std::to_string(constVal * elemSize));
                    lowered.instructions.push_back(addConst);

                    // Propagate carry to high word
                    Instruction286 adcHigh;
                    adcHigh.mnemonic = "adc";
                    adcHigh.operands.push_back("dx");
                    adcHigh.operands.push_back("0");
                    lowered.instructions.push_back(adcHigh);
                } else if (!isConst) {
                    // Add register offset: ax += idxReg * elemSize
                    if (idxReg != "ax") {
                        Instruction286 movIdx;
                        movIdx.mnemonic = "mov";
                        movIdx.operands.push_back("cx");
                        if (idxReg.find("bp") != std::string::npos && idxReg[0] != '[') {
                            movIdx.operands.push_back("[" + idxReg + "]");
                        } else {
                            movIdx.operands.push_back(idxReg);
                        }
                        lowered.instructions.push_back(movIdx);
                    } else {
                        // Index is already in ax, move to cx
                        Instruction286 movIdx;
                        movIdx.mnemonic = "mov";
                        movIdx.operands.push_back("cx");
                        movIdx.operands.push_back("ax");
                        lowered.instructions.push_back(movIdx);
                    }
                    // Multiply by elemSize (shift left)
                    if (elemSize > 1) {
                        int shiftAmount = 0;
                        int temp = elemSize;
                        while (temp > 1) {
                            temp >>= 1;
                            shiftAmount++;
                        }
                        if (shiftAmount > 0) {
                            Instruction286 shlIdx;
                            shlIdx.mnemonic = "shl";
                            shlIdx.operands.push_back("cx");
                            shlIdx.operands.push_back(std::to_string(shiftAmount));
                            lowered.instructions.push_back(shlIdx);
                        }
                    }

                    Instruction286 addIdx;
                    addIdx.mnemonic = "add";
                    addIdx.operands.push_back("ax");
                    addIdx.operands.push_back("cx");
                    lowered.instructions.push_back(addIdx);

                    // Propagate carry to high word
                    Instruction286 adcHigh;
                    adcHigh.mnemonic = "adc";
                    adcHigh.operands.push_back("dx");
                    adcHigh.operands.push_back("0");
                    lowered.instructions.push_back(adcHigh);
                }
            }

            // Store result to 32-bit stack slot
            if (!irInst.resultName.empty()) {
                // Allocate 32-bit stack slot for result
                state.currentStackOffset -= 4;
                state.tempSpaceInBlock += 4;   // track temp space for cleanup
                int stackOffset = state.currentStackOffset;
                state.vregToStackOffset[irInst.resultName] = stackOffset;
                std::string resultStack = "bp" + std::to_string(stackOffset);
                state.mark32Bit(irInst.resultName);

                Instruction286 subSp;
                subSp.mnemonic = "sub";
                subSp.operands.push_back("sp");
                subSp.operands.push_back("4");
                lowered.instructions.push_back(subSp);

                // Store low word
                Instruction286 storeLow;
                storeLow.mnemonic = "mov";
                storeLow.operands.push_back("[" + resultStack + "]");
                storeLow.operands.push_back("ax");
                lowered.instructions.push_back(storeLow);

                // Store high word
                Instruction286 storeHigh;
                storeHigh.mnemonic = "mov";
                int offset = stackOffset;
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                storeHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                storeHigh.operands.push_back("dx");
                lowered.instructions.push_back(storeHigh);

                state.vregToPhys[irInst.resultName] = resultStack;
                state.physToVreg[resultStack] = irInst.resultName;
            }
        } else {
            // 16-bit GEP
            // Check if base is an alloca result (address IS the stack offset, not a value at it)
            bool baseIsAlloca = state.allocaVregs.find(irInst.operands[0].name) != state.allocaVregs.end();

            // Move/lea base to AX
            if (baseReg != "ax") {
                Instruction286 loadBase;
                if (baseIsAlloca && baseReg.find("bp") != std::string::npos && baseReg[0] != '[') {
                    // Alloca result: use lea to compute the address
                    loadBase.mnemonic = "lea";
                    loadBase.operands.push_back("ax");
                    loadBase.operands.push_back("[" + baseReg + "]");
                } else {
                    // Non-alloca: load the value
                    loadBase.mnemonic = "mov";
                    loadBase.operands.push_back("ax");
                    if (baseReg.find("bp") != std::string::npos && baseReg[0] != '[') {
                        loadBase.operands.push_back("[" + baseReg + "]");
                    } else {
                        loadBase.operands.push_back(baseReg);
                    }
                }
                lowered.instructions.push_back(loadBase);
            }

            // Add offset for each index
            for (size_t i = 1; i < irInst.operands.size(); i++) {
                std::string idxName = irInst.operands[i].name;

                // Check if index is a constant (check original name, not register)
                bool isConst = false;
                int constVal = 0;
                try {
                    constVal = std::stoi(idxName);
                    isConst = true;
                } catch (...) {}

                std::string idxReg = state.getPhysReg(irInst.operands[i].name);

                if (isConst && constVal != 0) {
                    Instruction286 addConst;
                    addConst.mnemonic = "add";
                    addConst.operands.push_back("ax");
                    addConst.operands.push_back(std::to_string(constVal * elemSize));
                    lowered.instructions.push_back(addConst);
                } else if (!isConst) {
                    if (idxReg != "ax") {
                        Instruction286 movIdx;
                        movIdx.mnemonic = "mov";
                        movIdx.operands.push_back("cx");
                        if (idxReg.find("bp") != std::string::npos && idxReg[0] != '[') {
                            movIdx.operands.push_back("[" + idxReg + "]");
                        } else {
                            movIdx.operands.push_back(idxReg);
                        }
                        lowered.instructions.push_back(movIdx);
                    }
                    if (elemSize > 1) {
                        int shiftAmount = 0;
                        int temp = elemSize;
                        while (temp > 1) {
                            temp >>= 1;
                            shiftAmount++;
                        }
                        if (shiftAmount > 0) {
                            Instruction286 shlIdx;
                            shlIdx.mnemonic = "shl";
                            shlIdx.operands.push_back("cx");
                            shlIdx.operands.push_back(std::to_string(shiftAmount));
                            lowered.instructions.push_back(shlIdx);
                        }
                    }

                    Instruction286 addIdx;
                    addIdx.mnemonic = "add";
                    addIdx.operands.push_back("ax");
                    addIdx.operands.push_back("cx");
                    lowered.instructions.push_back(addIdx);
                }
            }

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
