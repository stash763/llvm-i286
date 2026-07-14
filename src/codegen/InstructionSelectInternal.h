// Instruction Selection - Internal Header
// Shared state and utilities for instruction selection handlers
// Not part of the public API - internal to codegen implementation
//
// NOTE: This header now delegates to StackFrame for all stack management.
// The old SelectorState fields (currentStackOffset, tempSpaceInBlock, etc.)
// have been moved into StackFrame.

#ifndef LLVM_I286_CODEGEN_INSTRUCTION_SELECT_INTERNAL_H
#define LLVM_I286_CODEGEN_INSTRUCTION_SELECT_INTERNAL_H

#include "codegen/InstructionSelect.h"
#include "codegen/StackFrame.h"
#include "codegen/FunctionAnalysis.h"

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
    // Central stack frame manager (replaces all old stack tracking fields)
    StackFrame frame;

    // Function declarations: maps function name to parameter bit widths
    std::map<std::string, std::vector<int>> funcParamBitWidths;

    // Alias map: maps alias name to target name (for weak_alias resolution)
    std::map<std::string, std::string> aliasMap;

    // Set of vreg names that are SS-derived pointers (point into SS segment)
    // Propagated through Load (when loading through SS-derived pointer), GEP, inttoptr, ptrtoint
    std::set<std::string> ssDerivedPtrVregs;

    // Counter for generating unique labels
    int labelCounter = 0;

    // Pending trampolines for indirect far calls (emitted at end of function)
    std::vector<LoweredInstruction> pendingTrampolines;

    // Current function being processed
    const ir::Function* currentFunc = nullptr;

    // Type definitions from the module (for GEP offset computation)
    const std::map<std::string, std::unique_ptr<ir::Type>>* typeDefinitions = nullptr;

    // Helper to get next label
    std::string nextLabel(const std::string& prefix = ".L") {
        return prefix + std::to_string(labelCounter++);
    }
};

// ========================================================================
// Shared Utility Functions (non-stack-related, kept here)
// ========================================================================

// Check if a name is a constant integer
bool isConstantInt(const std::string& name);

// Check if a name is a global variable (starts with '.')
bool isGlobalVar(const std::string& name);

// Convert global variable name to NASM format (".foo" -> "_foo")
std::string toNasmGlobal(const std::string& name);

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
std::vector<LoweredInstruction> lowerPtrToInt(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg);
std::vector<LoweredInstruction> lowerIntToPtr(SelectorState& state,
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
