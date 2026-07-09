// Instruction Selection - Shared Utility Functions
// Non-stack-related utilities (stack management now handled by StackFrame)

#include "codegen/InstructionSelectInternal.h"

#include <string>

namespace llvm_i286 {
namespace codegen {

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

} // namespace codegen
} // namespace llvm_i286
