// Instruction Selection - Internal Header
// Shared state and utilities for instruction selection handlers
// Not part of the public API - internal to codegen implementation

#ifndef LLVM_I286_CODEGEN_INSTRUCTION_SELECT_INTERNAL_H
#define LLVM_I286_CODEGEN_INSTRUCTION_SELECT_INTERNAL_H

#include "codegen/InstructionSelect.h"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdint>

namespace llvm_i286 {
namespace codegen {

// SelectorState - shared state for instruction selection
// Contains register allocation maps, stack tracking, and helper methods
// extracted from InstructionSelector::Impl so that handler functions
// in separate translation units can access the state.
struct SelectorState {
    // Map virtual register names to physical registers or stack offsets
    std::map<std::string, std::string> vregToPhys;

    // Map virtual register names to stack offsets (negative offsets from BP)
    std::map<std::string, int> vregToStackOffset;

    // Map physical register names to vreg names (to track what's in each register)
    std::map<std::string, std::string> physToVreg;

    // Set of vreg names that are 32-bit values
    std::set<std::string> is32bit;

    // Set of vreg names that are alloca results (addresses, not values)
    std::set<std::string> allocaVregs;

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
            std::string physReg = it->second;
            // For 32-bit vregs, we can't use registers directly for addressing
            // We need to spill to stack and return the stack location
            if (is32BitReg(vreg) && !physReg.empty() && physReg.find("bp") == std::string::npos) {
                // This is a 32-bit vreg in a register - allocate stack space
                int stackOffset = 0;
                if (vregToStackOffset.find(vreg) == vregToStackOffset.end()) {
                    currentStackOffset -= 4;
                    stackOffset = currentStackOffset;
                    vregToStackOffset[vreg] = stackOffset;
                } else {
                    stackOffset = vregToStackOffset[vreg];
                }
                std::string stackResult = "bp" + std::to_string(stackOffset);
                return stackResult;
            }
            return physReg;
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

    // ========================================================================
    // 32-bit Load/Store Helpers
    // ========================================================================

    // Load a 32-bit value from stack/memory into AX:DX (AX=low, DX=high)
    std::vector<Instruction286> load32Bit(const std::string& vregName) {
        std::vector<Instruction286> insts;
        std::string physReg = getPhysReg(vregName);

        Instruction286 loadLow;
        loadLow.mnemonic = "mov";
        loadLow.operands.push_back("ax");
        loadLow.operands.push_back("[" + physReg + "]");
        insts.push_back(loadLow);

        Instruction286 loadHigh;
        loadHigh.mnemonic = "mov";
        loadHigh.operands.push_back("dx");
        // Calculate high word offset
        if (physReg.find("bp") != std::string::npos) {
            int offset = 0;
            std::string offsetStr = physReg.substr(physReg.find("bp") + 2);
            if (!offsetStr.empty()) {
                try { offset = std::stoi(offsetStr); } catch (...) {}
            }
            int newOffset = offset + 2;
            std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
            loadHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
        } else {
            loadHigh.operands.push_back("[" + physReg + "+2]");
        }
        insts.push_back(loadHigh);

        return insts;
    }

    // Store a 32-bit value from AX:DX to a stack/memory location
    std::vector<Instruction286> store32Bit(const std::string& vregName) {
        std::vector<Instruction286> insts;
        std::string physReg = getPhysReg(vregName);

        Instruction286 storeLow;
        storeLow.mnemonic = "mov";
        storeLow.operands.push_back("[" + physReg + "]");
        storeLow.operands.push_back("ax");
        insts.push_back(storeLow);

        Instruction286 storeHigh;
        storeHigh.mnemonic = "mov";
        // Calculate high word offset
        if (physReg.find("bp") != std::string::npos) {
            int offset = 0;
            std::string offsetStr = physReg.substr(2);
            if (!offsetStr.empty()) {
                try { offset = std::stoi(offsetStr); } catch (...) {}
            }
            int newOffset = offset + 2;
            std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
            storeHigh.operands.push_back("[" + std::string("bp") + offsetStr2 + "]");
        } else {
            storeHigh.operands.push_back("[" + physReg + "+2]");
        }
        storeHigh.operands.push_back("dx");
        insts.push_back(storeHigh);

        return insts;
    }

    // Allocate stack space for a 32-bit value and return the bp-relative string
    std::string alloc32BitStack(const std::string& vregName) {
        int offset = allocateStack(4);
        vregToStackOffset[vregName] = offset;
        mark32Bit(vregName);

        std::string stackReg = "bp" + std::to_string(offset);

        // Emit sub sp, 4
        Instruction286 subSp;
        subSp.mnemonic = "sub";
        subSp.operands.push_back("sp");
        subSp.operands.push_back("4");

        return stackReg;
    }

    // Check if an operand name is a constant (parseable as integer)
    bool isConstant(const std::string& name) {
        if (name.empty()) return false;
        try {
            std::stoi(name);
            return true;
        } catch (...) {
            return false;
        }
    }
};

// ========================================================================
// Shared Utility Functions
// ========================================================================

// Check if a register string is a memory operand (contains "bp")
bool isMemoryOperand(const std::string& reg);

// Check if a name is a constant integer
bool isConstantInt(const std::string& name);

// Check if a name is a global variable (starts with '.')
bool isGlobalVar(const std::string& name);

// Convert global variable name to NASM format (".foo" -> "_foo")
std::string toNasmGlobal(const std::string& name);

// Check if a name is a vreg (exists in vregToPhys map)
bool isVreg(const std::string& name, const SelectorState& state);

// Build a bp-relative offset string with additional offset
// Given "bp-10" and additionalOffset=2, returns "bp-8"
std::string makeBpOffset(const std::string& bpReg, int additionalOffset);

// Emit a 32-bit store to stack: store lowReg to [stackReg] and highReg to [stackReg+2]
void emit32BitStoreToStack(std::vector<Instruction286>& output,
                           const std::string& stackReg,
                           const std::string& lowReg = "ax",
                           const std::string& highReg = "bx");

// Emit a 32-bit load from stack: load low word from [stackReg] to lowReg
// and high word from [stackReg+2] to highReg
void emit32BitLoadFromStack(std::vector<Instruction286>& output,
                             const std::string& stackReg,
                             const std::string& lowReg = "ax",
                             const std::string& highReg = "dx");

// Ensure a 32-bit stack slot exists for a vreg
// If not already on stack, allocates 4 bytes and emits "sub sp, 4"
// Returns bp-relative string (e.g., "bp-10")
std::string ensure32BitStackSlot(SelectorState& state,
                                 std::vector<Instruction286>& output,
                                 const std::string& vregName);

// ========================================================================
// Opcode Handler Function Declarations
// Each handler returns a vector of LoweredInstruction for the given IR instruction.
// ========================================================================

// Arithmetic operations
std::vector<LoweredInstruction> lowerAdd(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerSub(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerMul(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerDivRem(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);

// Bitwise operations
std::vector<LoweredInstruction> lowerAnd(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerOr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerXor(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerShl(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerLShr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerAShr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);

// Memory operations
std::vector<LoweredInstruction> lowerAlloca(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerLoad(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerStore(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerGetElementPtr(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);

// Control flow
std::vector<LoweredInstruction> lowerRetTerm(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerBrTerm(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerCondBrTerm(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerSwitch(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerPhi(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerUnreachable(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerSelect(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerICmp(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);

// Call
std::vector<LoweredInstruction> lowerCall(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);

// Conversions
std::vector<LoweredInstruction> lowerTrunc(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerZExt(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerSExt(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerBitCast(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerFreeze(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerExtractValue(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerInsertValue(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);

// FPU operations
std::vector<LoweredInstruction> lowerFpuArith(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerFRem(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerFCmp(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerFpuConvert(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerFPToInt(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerIntToFP(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_INSTRUCTION_SELECT_INTERNAL_H