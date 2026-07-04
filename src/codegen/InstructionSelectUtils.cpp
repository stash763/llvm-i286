// Instruction Selection - Shared Utility Functions
// Extracted from duplicated patterns in InstructionSelect.cpp

#include "codegen/InstructionSelectInternal.h"

#include <string>
#include <vector>

namespace llvm_i286 {
namespace codegen {

// Check if a register string is a memory operand (contains "bp")
bool isMemoryOperand(const std::string& reg) {
    return reg.find("bp") != std::string::npos;
}

// Check if a name is a constant integer
bool isConstantInt(const std::string& name) {
    if (name.empty()) return false;
    try {
        std::stoi(name);
        return true;
    } catch (...) {
        return false;
    }
}

// Check if a name is a global variable (starts with '.')
bool isGlobalVar(const std::string& name) {
    return !name.empty() && name[0] == '.';
}

// Convert global variable name to NASM format (".foo" -> "_foo")
std::string toNasmGlobal(const std::string& name) {
    if (isGlobalVar(name) && name.size() > 1) {
        return "_" + name.substr(1);
    }
    return name;
}

// Check if a name is a vreg (exists in vregToPhys map)
bool isVreg(const std::string& name, const SelectorState& state) {
    return state.vregToPhys.find(name) != state.vregToPhys.end();
}

// Build a bp-relative offset string with additional offset
// Given "bp-10" and additionalOffset=2, returns "bp-8"
// Given "bp+4" and additionalOffset=2, returns "bp+6"
// Given "bp" and additionalOffset=2, returns "bp+2"
std::string makeBpOffset(const std::string& bpReg, int additionalOffset) {
    int offset = 0;
    std::string offsetStr = bpReg.substr(bpReg.find("bp") + 2);
    if (!offsetStr.empty()) {
        try { offset = std::stoi(offsetStr); } catch (...) {}
    }
    int newOffset = offset + additionalOffset;
    std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
    return "bp" + offsetStr2;
}

// Emit a 32-bit store to stack: store lowReg to [stackReg] and highReg to [stackReg+2]
void emit32BitStoreToStack(std::vector<Instruction286>& output,
                           const std::string& stackReg,
                           const std::string& lowReg,
                           const std::string& highReg) {
    // Store low word
    Instruction286 storeLow;
    storeLow.mnemonic = "mov";
    storeLow.operands.push_back("[" + stackReg + "]");
    storeLow.operands.push_back(lowReg);
    output.push_back(storeLow);

    // Store high word
    Instruction286 storeHigh;
    storeHigh.mnemonic = "mov";
    if (isMemoryOperand(stackReg)) {
        storeHigh.operands.push_back("[" + makeBpOffset(stackReg, 2) + "]");
    } else {
        storeHigh.operands.push_back("[" + stackReg + "+2]");
    }
    storeHigh.operands.push_back(highReg);
    output.push_back(storeHigh);
}

// Emit a 32-bit load from stack: load low word from [stackReg] to lowReg
// and high word from [stackReg+2] to highReg
void emit32BitLoadFromStack(std::vector<Instruction286>& output,
                             const std::string& stackReg,
                             const std::string& lowReg,
                             const std::string& highReg) {
    // Load low word
    Instruction286 loadLow;
    loadLow.mnemonic = "mov";
    loadLow.operands.push_back(lowReg);
    loadLow.operands.push_back("[" + stackReg + "]");
    output.push_back(loadLow);

    // Load high word
    Instruction286 loadHigh;
    loadHigh.mnemonic = "mov";
    loadHigh.operands.push_back(highReg);
    if (isMemoryOperand(stackReg)) {
        loadHigh.operands.push_back("[" + makeBpOffset(stackReg, 2) + "]");
    } else {
        loadHigh.operands.push_back("[" + stackReg + "+2]");
    }
    output.push_back(loadHigh);
}

// Ensure a 32-bit stack slot exists for a vreg
// If not already on stack, allocates 4 bytes and emits "sub sp, 4"
// Returns bp-relative string (e.g., "bp-10")
std::string ensure32BitStackSlot(SelectorState& state,
                                 std::vector<Instruction286>& output,
                                 const std::string& vregName) {
    std::string resultStack = state.getPhysReg(vregName);
    if (resultStack.find("bp") == std::string::npos) {
        state.currentStackOffset -= 4;
        int stackOffset = state.currentStackOffset;
        state.vregToStackOffset[vregName] = stackOffset;
        resultStack = "bp" + std::to_string(stackOffset);

        Instruction286 subSp;
        subSp.mnemonic = "sub";
        subSp.operands.push_back("sp");
        subSp.operands.push_back("4");
        output.push_back(subSp);
    }
    return resultStack;
}

} // namespace codegen
} // namespace llvm_i286