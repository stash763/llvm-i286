// Register Allocator Implementation

#include "codegen/RegisterAlloc.h"

namespace llvm_i286 {
namespace codegen {

struct RegisterAllocator::Impl {
    std::map<std::string, Allocation> allocations;
};

RegisterAllocator::RegisterAllocator() : impl(std::make_unique<Impl>()) {}

RegisterAllocator::~RegisterAllocator() = default;

std::vector<LoweredInstruction> RegisterAllocator::allocate(
    const std::vector<LoweredInstruction>& instructions,
    const ir::Function& func) {
    // TODO: Implement linear scan register allocation
    // For now, return instructions unchanged
    return instructions;
}

Register RegisterAllocator::getRegister(const std::string& vregName) const {
    auto it = impl->allocations.find(vregName);
    if (it != impl->allocations.end()) {
        return it->second.reg;
    }
    return Register::None;
}

int RegisterAllocator::getStackSlot(const std::string& vregName) const {
    auto it = impl->allocations.find(vregName);
    if (it != impl->allocations.end()) {
        return it->second.stackSlot;
    }
    return -1;
}

std::string registerToString(Register reg) {
    switch (reg) {
        case Register::AX: return "ax";
        case Register::BX: return "bx";
        case Register::CX: return "cx";
        case Register::DX: return "dx";
        case Register::SI: return "si";
        case Register::DI: return "di";
        case Register::BP: return "bp";
        case Register::SP: return "sp";
        case Register::None: return "none";
    }
    return "ax";
}

std::string registerLow8(Register reg) {
    switch (reg) {
        case Register::AX: return "al";
        case Register::BX: return "bl";
        case Register::CX: return "cl";
        case Register::DX: return "dl";
        default: return "al";
    }
}

std::string registerHigh8(Register reg) {
    switch (reg) {
        case Register::AX: return "ah";
        case Register::BX: return "bh";
        case Register::CX: return "ch";
        case Register::DX: return "dh";
        default: return "ah";
    }
}

} // namespace codegen
} // namespace llvm_i286
