// Instruction Selection Implementation
// Lowering LLVM IR instructions to 80286 instructions

#include "codegen/InstructionSelect.h"

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <cstdint>

namespace llvm_i286 {
namespace codegen {

struct InstructionSelector::Impl {
    // Map virtual register names to physical registers or stack offsets
    std::map<std::string, std::string> vregToPhys;
    
    // Map virtual register names to stack offsets (negative offsets from BP)
    std::map<std::string, int> vregToStackOffset;
    
    // Map physical register names to vreg names (to track what's in each register)
    std::map<std::string, std::string> physToVreg;
    
    // Set of vreg names that are 32-bit values
    std::set<std::string> is32bit;
    
    // Function declarations: maps function name to parameter bit widths
    std::map<std::string, std::vector<int>> funcParamBitWidths;
    
    // Counter for generating unique labels
    int labelCounter = 0;
    
    // Current function being processed
    const ir::Function* currentFunc = nullptr;
    
    // Current stack offset (grows negative from BP)
    int currentStackOffset = 0;
    
    // Available temp registers (in priority order)
    std::vector<std::string> tempRegs = {"ax", "cx", "dx"};
    
    // Helper to check if a vreg is 32-bit
    bool is32BitReg(const std::string& vreg) {
        return is32bit.find(vreg) != is32bit.end();
    }
    
    // Helper to mark a vreg as 32-bit
    void mark32Bit(const std::string& vreg) {
        if (!vreg.empty()) {
            is32bit.insert(vreg);
        }
    }
    
    // Helper to get physical register for a virtual register
    std::string getPhysReg(const std::string& vreg) {
        // Check if it's a stack variable first
        auto stackIt = vregToStackOffset.find(vreg);
        if (stackIt != vregToStackOffset.end()) {
            int offset = stackIt->second;
            std::string result = "bp";
            if (offset < 0) {
                result += std::to_string(offset);
            } else if (offset > 0) {
                result += "+" + std::to_string(offset);
            }
            return result;
        }
        
        auto it = vregToPhys.find(vreg);
        if (it != vregToPhys.end()) {
            return it->second;
        }
        // Default to AX if not found
        return "ax";
    }
    
    // Helper to get a free temp register
    std::string getFreeTempReg() {
        for (const auto& reg : tempRegs) {
            if (physToVreg.find(reg) == physToVreg.end()) {
                return reg;
            }
        }
        // All temp regs are occupied, use CX (will spill if needed)
        return "cx";
    }
    
    // Helper to assign a vreg to a register, clearing any old occupant
    std::string assignReg(const std::string& vreg) {
        // Check if already assigned to a register (and it's not a stack slot)
        auto it = vregToPhys.find(vreg);
        if (it != vregToPhys.end()) {
            std::string existingReg = it->second;
            // Verify it's not a memory location
            if (existingReg.find("bp") == std::string::npos) {
                return existingReg;
            }
        }
        
        // Get a free register
        std::string reg = getFreeTempReg();
        
        // Clear any old vreg that was in this register
        auto oldOcc = physToVreg.find(reg);
        if (oldOcc != physToVreg.end()) {
            vregToPhys.erase(oldOcc->second);
            physToVreg.erase(oldOcc);
        }
        
        // Assign new mapping
        vregToPhys[vreg] = reg;
        physToVreg[reg] = vreg;
        
        return reg;
    }
    
    // Helper to update the result register after an instruction writes to it.
    // This ensures physToVreg is consistent with vregToPhys.
    void updateResultReg(const std::string& resultVreg, const std::string& physReg) {
        // Clear any old vreg in this physical register
        auto oldOcc = physToVreg.find(physReg);
        if (oldOcc != physToVreg.end() && oldOcc->second != resultVreg) {
            vregToPhys.erase(oldOcc->second);
            physToVreg.erase(oldOcc);
        }
        vregToPhys[resultVreg] = physReg;
        physToVreg[physReg] = resultVreg;
    }
    
    // Helper to free a register
    void freeReg(const std::string& reg) {
        auto it = physToVreg.find(reg);
        if (it != physToVreg.end()) {
            vregToPhys.erase(it->second);
            physToVreg.erase(it);
        }
    }
    
    // Helper to get next label
    std::string nextLabel(const std::string& prefix = ".L") {
        return prefix + std::to_string(labelCounter++);
    }
    
    // Helper to allocate stack space and return the offset
    int allocateStack(int byteSize) {
        currentStackOffset -= byteSize;
        return currentStackOffset;
    }
};

InstructionSelector::InstructionSelector() : impl(std::make_unique<Impl>()) {}

InstructionSelector::~InstructionSelector() = default;

void InstructionSelector::setFunctionDeclarations(const std::map<std::string, std::vector<int>>& decls) {
    impl->funcParamBitWidths = decls;
}

std::vector<LoweredInstruction> InstructionSelector::lowerInstruction(const ir::Instruction& irInst) {
    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;
    
    // Get result register if this instruction produces a value
    std::string resultReg;
    if (!irInst.resultName.empty()) {
        // Check if we already have a mapping for this result
        resultReg = impl->getPhysReg(irInst.resultName);
        
        // If it's still "ax" and not a memory location, we might need to assign a different register
        // For now, just use AX for all results
        if (resultReg == "ax") {
            // Could allocate a different register here if needed
        }
    }
    
    switch (irInst.opcode) {
        case ir::Opcode::Add: {
            // Lower add: result = operand1 + operand2
            // Check if this is a 32-bit operation
            bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
            
            if (is32) {
                // 32-bit add: add low words, adc high words
                if (irInst.operands.size() >= 2) {
                    std::string op1Name = irInst.operands[0].name;
                    std::string op2Name = irInst.operands[1].name;
                    
                    // Mark result as 32-bit
                    impl->mark32Bit(irInst.resultName);
                    
                    // For 32-bit operations, always use stack locations
                    // The register allocator can't handle 32-bit values properly on 286
                    // We need to get the stack slot for each operand
                    std::string op1Stack = impl->getPhysReg(op1Name);
                    std::string op2Stack = impl->getPhysReg(op2Name);
                    
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
                    loadOp2Low.operands.push_back("cx");
                    loadOp2Low.operands.push_back("[" + op2Stack + "]");
                    lowered.instructions.push_back(loadOp2Low);
                    
                    Instruction286 loadOp2High;
                    loadOp2High.mnemonic = "mov";
                    loadOp2High.operands.push_back("dx");
                    loadOp2High.operands.push_back("[" + op2Stack + "+2]");
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
                        std::string resultStack = impl->getPhysReg(irInst.resultName);
                        // If result doesn't have stack space, allocate it
                        if (resultStack.find("bp") == std::string::npos) {
                            impl->currentStackOffset -= 4;
                            int stackOffset = impl->currentStackOffset;
                            impl->vregToStackOffset[irInst.resultName] = stackOffset;
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
                        impl->vregToPhys[irInst.resultName] = resultStack;
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
                    bool op1IsVreg = (impl->vregToPhys.find(op1Name) != impl->vregToPhys.end());
                    bool op2IsVreg = (impl->vregToPhys.find(op2Name) != impl->vregToPhys.end());
                    
                    // Check if operands are constants (not vregs)
                    bool op1IsConst = false;
                    bool op2IsConst = false;
                    if (!op1IsVreg) {
                        try { std::stoi(op1Name); op1IsConst = true; } catch (...) {}
                    }
                    if (!op2IsVreg) {
                        try { std::stoi(op2Name); op2IsConst = true; } catch (...) {}
                    }
                    
                    std::string op1 = op1IsConst ? op1Name : impl->getPhysReg(op1Name);
                    std::string op2 = op2IsConst ? op2Name : impl->getPhysReg(op2Name);
                    
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
                        impl->updateResultReg(irInst.resultName, "ax");
                    }
                } else {
                    // Fallback: add ax, bx
                    addInst.operands.push_back("ax");
                    addInst.operands.push_back("bx");
                    lowered.instructions.push_back(addInst);
                }
            }
            
            break;
        }
        
         case ir::Opcode::Sub: {
            // Lower sub: result = operand1 - operand2
            // Check if this is a 32-bit operation
            bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
            
            if (is32) {
                // 32-bit sub: sub low words, sbb high words
                if (irInst.operands.size() >= 2) {
                    std::string op1Name = irInst.operands[0].name;
                    std::string op2Name = irInst.operands[1].name;
                    
                    // Mark result as 32-bit
                    impl->mark32Bit(irInst.resultName);
                    
                    // Get stack locations for both operands
                    std::string op1Stack = impl->getPhysReg(op1Name);
                    std::string op2Stack = impl->getPhysReg(op2Name);
                    
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
                    loadOp2Low.operands.push_back("cx");
                    loadOp2Low.operands.push_back("[" + op2Stack + "]");
                    lowered.instructions.push_back(loadOp2Low);
                    
                    Instruction286 loadOp2High;
                    loadOp2High.mnemonic = "mov";
                    loadOp2High.operands.push_back("dx");
                    loadOp2High.operands.push_back("[" + op2Stack + "+2]");
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
                        std::string resultStack = impl->getPhysReg(irInst.resultName);
                        // Allocate stack space if needed
                        if (resultStack.find("bp") == std::string::npos) {
                            impl->currentStackOffset -= 4;
                            int stackOffset = impl->currentStackOffset;
                            impl->vregToStackOffset[irInst.resultName] = stackOffset;
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
                        impl->vregToPhys[irInst.resultName] = resultStack;
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
                    bool op1IsVreg = (impl->vregToPhys.find(op1Name) != impl->vregToPhys.end());
                    bool op2IsVreg = (impl->vregToPhys.find(op2Name) != impl->vregToPhys.end());
                    
                    // Check if operands are constants (not vregs)
                    bool op1IsConst = false;
                    bool op2IsConst = false;
                    if (!op1IsVreg) {
                        try { std::stoi(op1Name); op1IsConst = true; } catch (...) {}
                    }
                    if (!op2IsVreg) {
                        try { std::stoi(op2Name); op2IsConst = true; } catch (...) {}
                    }
                    
                    std::string op1 = op1IsConst ? op1Name : impl->getPhysReg(op1Name);
                    std::string op2 = op2IsConst ? op2Name : impl->getPhysReg(op2Name);
                
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
                    impl->updateResultReg(irInst.resultName, resultReg);
                }
            } else {
                subInst.operands.push_back("ax");
                subInst.operands.push_back("bx");
            }
            
            lowered.instructions.push_back(subInst);
            } // Close 16-bit else
            break;
        }
        
        case ir::Opcode::Mul: {
            // Lower mul: result = operand1 * operand2
            bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
            
            if (is32) {
                // 32-bit multiplication using shift-and-add algorithm
                // Based on os2_examples/asm_mul/test.asm
                // Note: This is a simplified version that calls a runtime function
                if (irInst.operands.size() >= 2) {
                    std::string op1Name = irInst.operands[0].name;
                    std::string op2Name = irInst.operands[1].name;
                    
                    impl->mark32Bit(irInst.resultName);
                    
                    // Get stack locations
                    std::string op1Stack = impl->getPhysReg(op1Name);
                    std::string op2Stack = impl->getPhysReg(op2Name);
                    
                    // Allocate result stack space
                    std::string resultStack = impl->getPhysReg(irInst.resultName);
                    if (resultStack.find("bp") == std::string::npos) {
                        impl->currentStackOffset -= 4;
                        int stackOffset = impl->currentStackOffset;
                        impl->vregToStackOffset[irInst.resultName] = stackOffset;
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
                    
                    // Store result (DI:SI) to stack
                    Instruction286 storeResultLow;
                    storeResultLow.mnemonic = "mov";
                    storeResultLow.operands.push_back("[" + resultStack + "]");
                    storeResultLow.operands.push_back("si");
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
                    storeResultHigh.operands.push_back("di");
                    lowered.instructions.push_back(storeResultHigh);
                    
                    // Update vreg mapping
                    impl->vregToPhys[irInst.resultName] = resultStack;
                }
            } else {
                // 16-bit multiplication
                Instruction286 imulInst;
                imulInst.mnemonic = "imul";
                
                if (irInst.operands.size() >= 2) {
                    std::string op1 = impl->getPhysReg(irInst.operands[0].name);
                    std::string op2 = impl->getPhysReg(irInst.operands[1].name);
                    
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
                    impl->updateResultReg(irInst.resultName, "ax");
                }
            }
            break;
        }
        
        case ir::Opcode::And: {
            Instruction286 andInst;
            andInst.mnemonic = "and";
            
            if (irInst.operands.size() >= 2) {
                std::string op1 = impl->getPhysReg(irInst.operands[0].name);
                std::string op2 = impl->getPhysReg(irInst.operands[1].name);
                
                andInst.operands.push_back(resultReg);
                andInst.operands.push_back(op2);
                
                if (resultReg != op1) {
                    Instruction286 movInst;
                    movInst.mnemonic = "mov";
                    movInst.operands.push_back(resultReg);
                    movInst.operands.push_back(op1);
                    lowered.instructions.push_back(movInst);
                }
            } else {
                andInst.operands.push_back("ax");
                andInst.operands.push_back("bx");
            }
            
            lowered.instructions.push_back(andInst);
            
            // Update register mapping - and writes to resultReg
            if (!irInst.resultName.empty()) {
                impl->updateResultReg(irInst.resultName, resultReg);
            }
            break;
        }
        
        case ir::Opcode::Or: {
            Instruction286 orInst;
            orInst.mnemonic = "or";
            
            if (irInst.operands.size() >= 2) {
                std::string op1 = impl->getPhysReg(irInst.operands[0].name);
                std::string op2 = impl->getPhysReg(irInst.operands[1].name);
                
                orInst.operands.push_back(resultReg);
                orInst.operands.push_back(op2);
                
                if (resultReg != op1) {
                    Instruction286 movInst;
                    movInst.mnemonic = "mov";
                    movInst.operands.push_back(resultReg);
                    movInst.operands.push_back(op1);
                    lowered.instructions.push_back(movInst);
                }
            } else {
                orInst.operands.push_back("ax");
                orInst.operands.push_back("bx");
            }
            
            lowered.instructions.push_back(orInst);
            
            // Update register mapping - or writes to resultReg
            if (!irInst.resultName.empty()) {
                impl->updateResultReg(irInst.resultName, resultReg);
            }
            break;
        }
        
        case ir::Opcode::Xor: {
            Instruction286 xorInst;
            xorInst.mnemonic = "xor";
            
            if (irInst.operands.size() >= 2) {
                std::string op1 = impl->getPhysReg(irInst.operands[0].name);
                std::string op2 = impl->getPhysReg(irInst.operands[1].name);
                
                xorInst.operands.push_back(resultReg);
                xorInst.operands.push_back(op2);
                
                if (resultReg != op1) {
                    Instruction286 movInst;
                    movInst.mnemonic = "mov";
                    movInst.operands.push_back(resultReg);
                    movInst.operands.push_back(op1);
                    lowered.instructions.push_back(movInst);
                }
            } else {
                xorInst.operands.push_back("ax");
                xorInst.operands.push_back("bx");
            }
            
            lowered.instructions.push_back(xorInst);
            
            // Update register mapping - xor writes to resultReg
            if (!irInst.resultName.empty()) {
                impl->updateResultReg(irInst.resultName, resultReg);
            }
            break;
        }
        
        case ir::Opcode::Shl: {
            Instruction286 shlInst;
            shlInst.mnemonic = "shl";
            
            if (irInst.operands.size() >= 2) {
                std::string op1 = impl->getPhysReg(irInst.operands[0].name);
                std::string op2 = impl->getPhysReg(irInst.operands[1].name);
                
                // On 286, shift count must be in CL or immediate
                // For now, assume immediate or CL
                shlInst.operands.push_back(op1);
                shlInst.operands.push_back(op2);
                
                if (resultReg != op1) {
                    Instruction286 movInst;
                    movInst.mnemonic = "mov";
                    movInst.operands.push_back(resultReg);
                    movInst.operands.push_back(op1);
                    lowered.instructions.push_back(movInst);
                }
            } else {
                shlInst.operands.push_back("ax");
                shlInst.operands.push_back("cl");
            }
            
            lowered.instructions.push_back(shlInst);
            
            // Update register mapping - shl writes to resultReg
            if (!irInst.resultName.empty()) {
                impl->updateResultReg(irInst.resultName, resultReg);
            }
            break;
        }
        
        case ir::Opcode::LShr: {
            Instruction286 shrInst;
            shrInst.mnemonic = "shr";
            
            if (irInst.operands.size() >= 2) {
                std::string op1 = impl->getPhysReg(irInst.operands[0].name);
                std::string op2 = impl->getPhysReg(irInst.operands[1].name);
                
                shrInst.operands.push_back(op1);
                shrInst.operands.push_back(op2);
                
                if (resultReg != op1) {
                    Instruction286 movInst;
                    movInst.mnemonic = "mov";
                    movInst.operands.push_back(resultReg);
                    movInst.operands.push_back(op1);
                    lowered.instructions.push_back(movInst);
                }
            } else {
                shrInst.operands.push_back("ax");
                shrInst.operands.push_back("cl");
            }
            
            lowered.instructions.push_back(shrInst);
            
            // Update register mapping - shr writes to resultReg
            if (!irInst.resultName.empty()) {
                impl->updateResultReg(irInst.resultName, resultReg);
            }
            break;
        }
        
        case ir::Opcode::AShr: {
            Instruction286 sarInst;
            sarInst.mnemonic = "sar";
            
            if (irInst.operands.size() >= 2) {
                std::string op1 = impl->getPhysReg(irInst.operands[0].name);
                std::string op2 = impl->getPhysReg(irInst.operands[1].name);
                
                sarInst.operands.push_back(op1);
                sarInst.operands.push_back(op2);
                
                if (resultReg != op1) {
                    Instruction286 movInst;
                    movInst.mnemonic = "mov";
                    movInst.operands.push_back(resultReg);
                    movInst.operands.push_back(op1);
                    lowered.instructions.push_back(movInst);
                }
            } else {
                sarInst.operands.push_back("ax");
                sarInst.operands.push_back("cl");
            }
            
            lowered.instructions.push_back(sarInst);
            
            // Update register mapping - sar writes to resultReg
            if (!irInst.resultName.empty()) {
                impl->updateResultReg(irInst.resultName, resultReg);
            }
            break;
        }
        
        case ir::Opcode::RetTerm: {
            // Return instruction - emit epilogue then ret
            // If there's a return value operand, load it into AX first
            if (!irInst.operands.empty()) {
                std::string retValName = irInst.operands[0].name;
                
                // Check if return value is a vreg/parameter
                bool isVreg = (impl->vregToPhys.find(retValName) != impl->vregToPhys.end());
                
                // Check if return value is a constant (not a vreg)
                bool isConst = false;
                if (!isVreg) {
                    try {
                        std::stoi(retValName);
                        isConst = true;
                    } catch (...) {}
                }
                
                if (isConst) {
                    // Load constant into AX
                    Instruction286 movConst;
                    movConst.mnemonic = "mov";
                    movConst.operands.push_back("ax");
                    movConst.operands.push_back(retValName);
                    lowered.instructions.push_back(movConst);
                } else {
                    // Move from register/memory to AX
                    std::string retValReg = impl->getPhysReg(retValName);
                    if (retValReg != "ax") {
                        Instruction286 movInst;
                        movInst.mnemonic = "mov";
                        movInst.operands.push_back("ax");
                        // Check if retValReg is a stack location (contains "bp")
                        if (retValReg.find("bp") != std::string::npos) {
                            movInst.operands.push_back("[" + retValReg + "]");
                        } else {
                            movInst.operands.push_back(retValReg);
                        }
                        lowered.instructions.push_back(movInst);
                    }
                }
            }
            
            // Deallocate local stack space before restoring saved registers
            // currentStackOffset started at -6 (for bx/si/di), so total locals = -(currentStackOffset + 6)
            int totalLocals = -(impl->currentStackOffset + 6);
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
            break;
        }
        
        case ir::Opcode::BrTerm: {
            // Unconditional branch
            Instruction286 jmpInst;
            jmpInst.mnemonic = "jmp";
            
            if (!irInst.operands.empty()) {
                jmpInst.operands.push_back("bb_" + irInst.operands[0].name);
            }
            
            lowered.instructions.push_back(jmpInst);
            break;
        }
        
        case ir::Opcode::Alloca: {
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
            int offset = impl->allocateStack(byteSize);
            
            // Store the stack offset for this variable
            if (!irInst.resultName.empty()) {
                impl->vregToStackOffset[irInst.resultName] = offset;
            }
            
            Instruction286 subSp;
            subSp.mnemonic = "sub";
            subSp.operands.push_back("sp");
            subSp.operands.push_back(std::to_string(byteSize));
            lowered.instructions.push_back(subSp);
            break;
        }
        
        case ir::Opcode::Store: {
            // Store value to memory
            // Operands: [0] = value to store, [1] = pointer to store to
            if (irInst.operands.size() >= 2) {
                std::string valName = irInst.operands[0].name;
                std::string ptrReg = impl->getPhysReg(irInst.operands[1].name);
                
                // Check if this is a 32-bit store
                bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
                
                // Check if value is in a register (parameter or previously loaded value)
                // A parameter name like "0" or "1" will be in vregToPhys
                std::string valReg = impl->getPhysReg(valName);
                bool isRegOrParam = (impl->vregToPhys.find(valName) != impl->vregToPhys.end());
                
                // Check if value is a constant (not a parameter or vreg)
                bool isConst = false;
                if (!isRegOrParam) {
                    try {
                        std::stoi(valName);
                        isConst = true;
                    } catch (...) {}
                }
                
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
                    } else if (valReg.find("bp") != std::string::npos) {
                        // Load low word from stack
                        Instruction286 loadLow;
                        loadLow.mnemonic = "mov";
                        loadLow.operands.push_back("ax");
                        loadLow.operands.push_back("[" + valReg + "]");
                        lowered.instructions.push_back(loadLow);
                        // Load high word from stack
                        Instruction286 loadHigh;
                        loadHigh.mnemonic = "mov";
                        loadHigh.operands.push_back("dx");
                        loadHigh.operands.push_back("[" + valReg + "+2]");
                        lowered.instructions.push_back(loadHigh);
                    } else {
                        // Value is in a register (low word in valReg, high word in next register)
                        Instruction286 movInst;
                        movInst.mnemonic = "mov";
                        movInst.operands.push_back("ax");
                        movInst.operands.push_back(valReg);
                        lowered.instructions.push_back(movInst);
                        // High word is in DX or needs to be loaded
                        Instruction286 movHigh;
                        movHigh.mnemonic = "mov";
                        movHigh.operands.push_back("dx");
                        movHigh.operands.push_back("dx"); // Assume high word is in DX
                        lowered.instructions.push_back(movHigh);
                    }
                    
                    // Check if ptrReg is a bp-relative address (contains "bp")
                    std::string dest;
                    if (ptrReg.find("bp") != std::string::npos) {
                        dest = "[" + ptrReg + "]";
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
                    if (ptrReg.find("bp") != std::string::npos) {
                        // Extract offset from ptrReg (e.g., "bp-10" -> -10)
                        int offset = 0;
                        std::string offsetStr = ptrReg.substr(2); // Remove "bp"
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
                        int newOffset = offset + 2;
                        std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                        storeHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                    } else {
                        storeHigh.operands.push_back("[" + ptrReg + "+2]");
                    }
                    storeHigh.operands.push_back("dx");
                    lowered.instructions.push_back(storeHigh);
                    
                    // Mark result as 32-bit
                    impl->mark32Bit(valName);
                } else {
                    // 16-bit store (original code)
                    // Move value to AX first
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
                    
                    // Check if ptrReg is a bp-relative address (contains "bp")
                    std::string dest;
                    if (ptrReg.find("bp") != std::string::npos) {
                        dest = "[" + ptrReg + "]";
                    } else {
                        dest = ptrReg;
                    }
                    
                    // Store AX to [ptr]
                    Instruction286 storeInst;
                    storeInst.mnemonic = "mov";
                    storeInst.operands.push_back(dest);
                    storeInst.operands.push_back("ax");
                    lowered.instructions.push_back(storeInst);
                }
            }
            break;
        }
        
        case ir::Opcode::Load: {
            // Load value from memory
            // Operands: [0] = pointer to load from
              if (!irInst.operands.empty()) {
                std::string ptrReg = impl->getPhysReg(irInst.operands[0].name);
                
                // Check if ptrReg is a bp-relative address (contains "bp")
                bool ptrIsMem = (ptrReg.find("bp") != std::string::npos);
                
                // Check if this is a 32-bit load
                bool is32 = irInst.resultType && irInst.resultType->bitWidth == 32;
                
                if (ptrIsMem) {
                    if (is32) {
                        // 32-bit load: load both low and high words, store to stack
                        std::string stackLoc = ptrReg;
                        
                        // Get result stack location
                        // If the result doesn't have a stack offset, allocate one
                        std::string resultStack = impl->getPhysReg(irInst.resultName);
                        if (resultStack == "ax" || resultStack == "bx" || resultStack == "cx" || resultStack == "dx") {
                            // Result is not on stack - allocate stack space
                            impl->currentStackOffset -= 4; // 32-bit = 4 bytes
                            int stackOffset = impl->currentStackOffset;
                            impl->vregToStackOffset[irInst.resultName] = stackOffset;
                            resultStack = "bp" + std::to_string(stackOffset);
                            
                            // Emit stack allocation
                            Instruction286 subSp;
                            subSp.mnemonic = "sub";
                            subSp.operands.push_back("sp");
                            subSp.operands.push_back("4");
                            lowered.instructions.push_back(subSp);
                        }
                        
                        // Load low word into AX
                        Instruction286 loadLow;
                        loadLow.mnemonic = "mov";
                        loadLow.operands.push_back("ax");
                        loadLow.operands.push_back("[" + stackLoc + "]");
                        lowered.instructions.push_back(loadLow);
                        
                        // Store low word to result
                        Instruction286 storeLow;
                        storeLow.mnemonic = "mov";
                        storeLow.operands.push_back("[" + resultStack + "]");
                        storeLow.operands.push_back("ax");
                        lowered.instructions.push_back(storeLow);
                        
                        // Load high word into DX
                        Instruction286 loadHigh;
                        loadHigh.mnemonic = "mov";
                        loadHigh.operands.push_back("dx");
                        if (stackLoc.find("bp") != std::string::npos) {
                            int offset = 0;
                            std::string offsetStr = stackLoc.substr(2);
                            if (!offsetStr.empty()) {
                                try { offset = std::stoi(offsetStr); } catch (...) {}
                            }
                            int newOffset = offset + 2;
                            std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                            loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
                        } else {
                            loadHigh.operands.push_back("[" + stackLoc + "+2]");
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
                        impl->mark32Bit(irInst.resultName);
                        
                        // Update vreg mapping to point to stack (not register)
                        // This ensures subsequent operations use the stack location
                        impl->vregToPhys[irInst.resultName] = resultStack;
                    } else {
                        // 16-bit load: assign a register and load
                        std::string destReg = impl->assignReg(irInst.resultName);
                        
                        // Load from [bp-N] into destReg
                        Instruction286 loadInst;
                        loadInst.mnemonic = "mov";
                        loadInst.operands.push_back(destReg);
                        loadInst.operands.push_back("[" + ptrReg + "]");
                        lowered.instructions.push_back(loadInst);
                    }
                }
                // else: loading from a register, no-op (value is already there)
            }
            break;
        }
        
        case ir::Opcode::Call: {
            // Lower call: result = call callee(arg1, arg2, ...)
            // Calling convention: arguments pushed right-to-left, return value in AX
            // Caller cleans up stack after call
            
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
                bool argIsVreg = (impl->vregToPhys.find(argName) != impl->vregToPhys.end());
                
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
                auto declIt = impl->funcParamBitWidths.find(callee);
                if (declIt != impl->funcParamBitWidths.end()) {
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
                        pushHigh.operands.push_back("[" + nasmName + "+2]");
                        lowered.instructions.push_back(pushHigh);
                        
                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back("[" + nasmName + "]");
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
                        std::string argReg = impl->getPhysReg(argName);
                        if (argReg.find("bp") != std::string::npos) {
                            // Memory operand - push both words
                            Instruction286 pushHigh;
                            pushHigh.mnemonic = "push";
                            pushHigh.operands.push_back("[" + argReg + "+2]");
                            lowered.instructions.push_back(pushHigh);
                            
                            Instruction286 pushLow;
                            pushLow.mnemonic = "push";
                            pushLow.operands.push_back("[" + argReg + "]");
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
                        std::string argReg = impl->getPhysReg(argName);
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
                    auto declIt = impl->funcParamBitWidths.find(callee);
                    if (declIt != impl->funcParamBitWidths.end()) {
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
            
            // Result is in AX
            if (!irInst.resultName.empty()) {
                impl->updateResultReg(irInst.resultName, "ax");
            }
            
            break;
        }
        
        case ir::Opcode::ICmp: {
            // Comparison: icmp predicate type op1, op2
            // Sets result to 0 or 1 based on comparison
            // On 286: CMP op1, op2; then conditional set
            
            if (irInst.operands.size() >= 2 && !irInst.predicate.empty()) {
                std::string op1Name = irInst.operands[0].name;
                std::string op2Name = irInst.operands[1].name;
                
                // Check if operands are vregs/parameters first
                bool op1IsVreg = (impl->vregToPhys.find(op1Name) != impl->vregToPhys.end());
                bool op2IsVreg = (impl->vregToPhys.find(op2Name) != impl->vregToPhys.end());
                
                // Check if operands are constants (not vregs)
                bool op1IsConst = false;
                bool op2IsConst = false;
                if (!op1IsVreg) {
                    try { std::stoi(op1Name); op1IsConst = true; } catch (...) {}
                }
                if (!op2IsVreg) {
                    try { std::stoi(op2Name); op2IsConst = true; } catch (...) {}
                }
                
                std::string op1Reg = op1IsConst ? op1Name : impl->getPhysReg(op1Name);
                std::string op2Reg = op2IsConst ? op2Name : impl->getPhysReg(op2Name);
                
                // Load operands into registers if they're memory locations
                bool op1IsMem = !op1IsConst && (op1Reg.find("bp") != std::string::npos);
                bool op2IsMem = !op2IsConst && (op2Reg.find("bp") != std::string::npos);
                
                if (op1IsMem) {
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
                std::string trueLabel = impl->nextLabel(".Lcmp_true_");
                std::string falseLabel = impl->nextLabel(".Lcmp_false_");
                std::string endLabel = impl->nextLabel(".Lcmp_end_");
                
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
                    impl->updateResultReg(irInst.resultName, "ax");
                }
            }
            break;
        }
        
        case ir::Opcode::CondBrTerm: {
            // Conditional branch: br i1 cond, label trueBB, label falseBB
            if (irInst.operands.size() >= 3) {
                std::string condName = irInst.operands[0].name;
                std::string trueLabel = "bb_" + irInst.operands[1].name;
                std::string falseLabel = "bb_" + irInst.operands[2].name;
                
                // Check if cond is a vreg/parameter
                bool condIsVreg = (impl->vregToPhys.find(condName) != impl->vregToPhys.end());
                
                // Check if cond is a constant (not a vreg)
                bool isConst = false;
                if (!condIsVreg) {
                    try {
                        std::stoi(condName);
                        isConst = true;
                    } catch (...) {}
                }
                
                std::string condReg = isConst ? condName : impl->getPhysReg(condName);
                
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
                    // Use OR AX, AX to set flags without changing AX
                    Instruction286 testInst;
                    testInst.mnemonic = "or";
                    testInst.operands.push_back("ax");
                    testInst.operands.push_back("ax");
                    lowered.instructions.push_back(testInst);
                    
                    // Jump to false if zero (ZF=1)
                    Instruction286 jmpFalse;
                    jmpFalse.mnemonic = "je";
                    jmpFalse.operands.push_back(falseLabel);
                    lowered.instructions.push_back(jmpFalse);
                    
                    // Fall through to true
                    Instruction286 jmpTrue;
                    jmpTrue.mnemonic = "jmp";
                    jmpTrue.operands.push_back(trueLabel);
                    lowered.instructions.push_back(jmpTrue);
                }
            }
            break;
        }
        
         case ir::Opcode::Trunc: {
            // Truncate: result = trunc type value to type
            // For 16-bit target: truncating 32-bit to 16-bit is just taking low word
            // For 8-bit: take low byte (AL)
            if (!irInst.operands.empty()) {
                std::string srcReg = impl->getPhysReg(irInst.operands[0].name);
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
                    impl->updateResultReg(irInst.resultName, destReg);
                }
            }
            break;
        }
        
        case ir::Opcode::ZExt: {
            // Zero extend: result = zext type value to type
            // For 16-bit target: extending 8-bit to 16-bit requires clearing high byte
            if (!irInst.operands.empty()) {
                std::string srcReg = impl->getPhysReg(irInst.operands[0].name);
                std::string destReg = resultReg.empty() ? "ax" : resultReg;
                
                // For now, just move the value (simple case)
                if (destReg != srcReg) {
                    Instruction286 movInst;
                    movInst.mnemonic = "mov";
                    movInst.operands.push_back(destReg);
                    movInst.operands.push_back(srcReg);
                    lowered.instructions.push_back(movInst);
                }
                
                if (!irInst.resultName.empty()) {
                    impl->updateResultReg(irInst.resultName, destReg);
                }
            }
            break;
        }
        
        case ir::Opcode::SExt: {
            // Sign extend: result = sext type value to type
            // For 16-bit target: CBW (AX <- sign-extend AL), CWD (DX:AX <- sign-extend AX)
            if (!irInst.operands.empty()) {
                std::string srcReg = impl->getPhysReg(irInst.operands[0].name);
                std::string destReg = resultReg.empty() ? "ax" : resultReg;
                
                // For now, just move the value (simple case)
                if (destReg != srcReg) {
                    Instruction286 movInst;
                    movInst.mnemonic = "mov";
                    movInst.operands.push_back(destReg);
                    movInst.operands.push_back(srcReg);
                    lowered.instructions.push_back(movInst);
                }
                
                if (!irInst.resultName.empty()) {
                    impl->updateResultReg(irInst.resultName, destReg);
                }
            }
            break;
        }
        
        case ir::Opcode::GetElementPtr: {
            // GEP: result = getelementptr type, ptr base, i32 idx1, i32 idx2, ...
            // For simple cases (single index): result = base + (index * elemSize)
            if (!irInst.operands.empty()) {
                std::string baseReg = impl->getPhysReg(irInst.operands[0].name);
                
                // Move base to AX
                if (baseReg != "ax") {
                    Instruction286 movBase;
                    movBase.mnemonic = "mov";
                    movBase.operands.push_back("ax");
                    movBase.operands.push_back(baseReg);
                    lowered.instructions.push_back(movBase);
                }
                
                // Add offset for each index (simplified: just first index * elemSize)
                for (size_t i = 1; i < irInst.operands.size(); i++) {
                    std::string idxReg = impl->getPhysReg(irInst.operands[i].name);
                    
                    // Check if index is a constant
                    bool isConst = false;
                    int constVal = 0;
                    try {
                        constVal = std::stoi(idxReg);
                        isConst = true;
                    } catch (...) {}
                    
                    if (isConst && constVal != 0) {
                        // Add constant offset
                        Instruction286 addConst;
                        addConst.mnemonic = "add";
                        addConst.operands.push_back("ax");
                        addConst.operands.push_back(std::to_string(constVal * 2)); // Assume 2-byte elements
                        lowered.instructions.push_back(addConst);
                    } else if (!isConst) {
                        // Add register offset (shift left by 1 to multiply by 2)
                        if (idxReg != "ax") {
                            Instruction286 movIdx;
                            movIdx.mnemonic = "mov";
                            movIdx.operands.push_back("cx");
                            movIdx.operands.push_back(idxReg);
                            lowered.instructions.push_back(movIdx);
                        }
                        Instruction286 shlIdx;
                        shlIdx.mnemonic = "shl";
                        shlIdx.operands.push_back("cx");
                        shlIdx.operands.push_back("1");
                        lowered.instructions.push_back(shlIdx);
                        
                        Instruction286 addIdx;
                        addIdx.mnemonic = "add";
                        addIdx.operands.push_back("ax");
                        addIdx.operands.push_back("cx");
                        lowered.instructions.push_back(addIdx);
                    }
                }
                
                if (!irInst.resultName.empty()) {
                    impl->updateResultReg(irInst.resultName, "ax");
                }
            }
            break;
        }
        
        case ir::Opcode::Select: {
            // Select: result = select i1 cond, type val1, type val2
            // If cond is true, result = val1; else result = val2
            if (irInst.operands.size() >= 3) {
                std::string condReg = impl->getPhysReg(irInst.operands[0].name);
                std::string val1Reg = impl->getPhysReg(irInst.operands[1].name);
                std::string val2Reg = impl->getPhysReg(irInst.operands[2].name);
                
                std::string destReg = resultReg.empty() ? "ax" : resultReg;
                
                // Test condition
                Instruction286 testCond;
                testCond.mnemonic = "or";
                testCond.operands.push_back("ax");
                testCond.operands.push_back("ax");
                lowered.instructions.push_back(testCond);
                
                std::string trueLabel = impl->nextLabel(".Lsel_true_");
                std::string falseLabel = impl->nextLabel(".Lsel_false_");
                std::string endLabel = impl->nextLabel(".Lsel_end_");
                
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
                    impl->updateResultReg(irInst.resultName, destReg);
                }
            }
            break;
        }
        
        default: {
            // Unhandled instruction - emit NOP as placeholder
            Instruction286 nopInst;
            nopInst.mnemonic = "nop";
            nopInst.comment = "TODO: Implement " + std::to_string(static_cast<int>(irInst.opcode));
            lowered.instructions.push_back(nopInst);
            break;
        }
    }
    
    loweredVec.push_back(lowered);
    return loweredVec;
}

std::vector<LoweredInstruction> InstructionSelector::lowerBasicBlock(const ir::BasicBlock& bb) {
    std::vector<LoweredInstruction> lowered;
    
    // Emit basic block label if present (prefix with bb_ to avoid conflicts with reserved labels)
    if (!bb.label.empty()) {
        LoweredInstruction labelInst;
        labelInst.label = "bb_" + bb.label;
        lowered.push_back(labelInst);
    }
    
    // Lower each instruction
    for (const auto& inst : bb.instructions) {
        auto instLowered = lowerInstruction(*inst);
        lowered.insert(lowered.end(), instLowered.begin(), instLowered.end());
    }
    
    // Lower terminator
    if (bb.terminator) {
        auto termLowered = lowerInstruction(*bb.terminator);
        lowered.insert(lowered.end(), termLowered.begin(), termLowered.end());
    }
    
    return lowered;
}

  std::vector<LoweredInstruction> InstructionSelector::lowerFunction(const ir::Function& func) {
    impl->currentFunc = &func;
    impl->vregToPhys.clear();
    impl->physToVreg.clear();  // Also clear the reverse mapping
    impl->vregToStackOffset.clear();
    impl->currentStackOffset = -6;  // Account for 3 saved regs: bx, si, di (2 bytes each)
    impl->labelCounter = 0;
    
    std::vector<LoweredInstruction> lowered;
    
    // Add function label
    LoweredInstruction funcLabel;
    funcLabel.label = func.name;
    lowered.push_back(funcLabel);
    
    // Assign physical registers to parameters
    // For OS/2 1.x calling convention, params are on stack
    // [bp+4] = first param, [bp+6] = second param, etc.
    int paramIndex = 0;
    int stackOffset = 4; // Start at [bp+4]
    for (const auto& param : func.params) {
        // Check if this is a 32-bit parameter
        bool is32 = param->type && param->type->bitWidth == 32;
        int paramSize = is32 ? 4 : 2; // 4 bytes for 32-bit, 2 bytes for 16-bit
        
        std::string physReg;
        if (paramIndex < 3 && !is32) {
            // Use BX, SI, DI for first 3 parameters (only for 16-bit params)
            const char* regs[] = {"bx", "si", "di"};
            physReg = regs[paramIndex];
        } else {
            // Spill to stack
            physReg = "[bp+" + std::to_string(stackOffset) + "]";
        }
        
        if (param->name.empty()) {
            paramIndex++;
            stackOffset += paramSize;
            continue;
        }
        
        // Clear any old vreg in this physical register
        auto oldOcc = impl->physToVreg.find(physReg);
        if (oldOcc != impl->physToVreg.end()) {
            impl->vregToPhys.erase(oldOcc->second);
            impl->physToVreg.erase(oldOcc);
        }
        
        // For 32-bit parameters, we need to load both words to stack-based storage
        if (is32) {
            // Allocate stack space for this 32-bit parameter
            impl->currentStackOffset -= 4;
            int localOffset = impl->currentStackOffset;
            impl->vregToStackOffset[param->name] = localOffset;
            std::string stackReg = "bp" + std::to_string(localOffset);
            
            // Create a lowered instruction block for parameter setup
            LoweredInstruction paramSetup;
            
            // physReg may already have brackets if it's a stack location
            std::string paramLoc = physReg;
            if (paramLoc.find("[") == std::string::npos) {
                paramLoc = "[" + paramLoc + "]";
            }
            
            // Load low word from [bp+stackOffset] to stack
            Instruction286 loadLow;
            loadLow.mnemonic = "mov";
            loadLow.operands.push_back("ax");
            loadLow.operands.push_back(paramLoc);
            paramSetup.instructions.push_back(loadLow);
            
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + stackReg + "]");
            storeLow.operands.push_back("ax");
            paramSetup.instructions.push_back(storeLow);
            
            // Load high word from [bp+stackOffset+2] to stack
            Instruction286 loadHigh;
            loadHigh.mnemonic = "mov";
            loadHigh.operands.push_back("dx");
            // Extract offset and add 2
            if (paramLoc.find("bp") != std::string::npos) {
                int offset = 0;
                std::string offsetStr = paramLoc.substr(paramLoc.find("bp")+2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else {
                loadHigh.operands.push_back("[" + paramLoc + "+2]");
            }
            paramSetup.instructions.push_back(loadHigh);
            
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            // Extract offset and add 2
            if (stackReg.find("bp") != std::string::npos) {
                int offset = 0;
                std::string offsetStr = stackReg.substr(2);
                if (!offsetStr.empty()) {
                    try { offset = std::stoi(offsetStr); } catch (...) {}
                }
                int newOffset = offset + 2;
                std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
                storeHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
            } else {
                storeHigh.operands.push_back("[" + stackReg + "+2]");
            }
            storeHigh.operands.push_back("dx");
            paramSetup.instructions.push_back(storeHigh);
            
            lowered.push_back(paramSetup);
            
            // Point vreg to stack location
            impl->vregToPhys[param->name] = stackReg;
            impl->physToVreg[stackReg] = param->name;
            impl->mark32Bit(param->name);
        } else {
            impl->vregToPhys[param->name] = physReg;
            impl->physToVreg[physReg] = param->name;
        }
        
        paramIndex++;
        stackOffset += paramSize;
    }
    
    // Lower each basic block
    for (const auto& bb : func.basicBlocks) {
        auto bbLowered = lowerBasicBlock(*bb);
        lowered.insert(lowered.end(), bbLowered.begin(), bbLowered.end());
    }
    
    return lowered;
}

} // namespace codegen
} // namespace llvm_i286
