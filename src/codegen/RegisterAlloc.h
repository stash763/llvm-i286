// Register Allocator
// Linear scan register allocator for 80286

#ifndef LLVM_I286_CODEGEN_REGISTER_ALLOC_H
#define LLVM_I286_CODEGEN_REGISTER_ALLOC_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "ir/IrAst.h"
#include "codegen/InstructionSelect.h"

namespace llvm_i286 {
namespace codegen {

// Available 80286 registers
enum class Register {
    AX, BX, CX, DX,
    SI, DI, BP, SP,
    None // For spilled values
};

// Register allocation for a single value
struct Allocation {
    Register reg;
    int stackSlot = -1; // -1 if not spilled
    std::string vregName; // Virtual register name from IR
};

class RegisterAllocator {
public:
    RegisterAllocator();
    ~RegisterAllocator();
    
    // Allocate registers for a function
    // Input: lowered instructions with virtual registers
    // Output: instructions with physical registers assigned
    std::vector<LoweredInstruction> allocate(const std::vector<LoweredInstruction>& instructions,
                                             const ir::Function& func);
    
    // Get the register assigned to a virtual register
    Register getRegister(const std::string& vregName) const;
    
    // Get the stack slot for a spilled value
    int getStackSlot(const std::string& vregName) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

// Convert register enum to NASM string
std::string registerToString(Register reg);

// Get the 8-bit low part of a register (e.g., AX -> AL)
std::string registerLow8(Register reg);

// Get the 8-bit high part of a register (e.g., AX -> AH)
std::string registerHigh8(Register reg);

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_REGISTER_ALLOC_H
