// Stack Frame Implementation
// Centralized stack frame layout and access

#include "codegen/StackFrame.h"

#include <stdexcept>
#include <iostream>

namespace llvm_i286 {
namespace codegen {

// ============================================================================
// Helper
// ============================================================================

std::string StackFrame::buildBpOffset(int offset) {
    if (offset == 0) {
        return "bp";
    } else if (offset > 0) {
        return "bp+" + std::to_string(offset);
    } else {
        return "bp" + std::to_string(offset);  // negative, e.g., "bp-8"
    }
}

// ============================================================================
// Pass 1: Analysis methods
// ============================================================================

void StackFrame::addParam(const std::string& name, int bitWidth, const std::string& reg) {
    SlotInfo slot;
    slot.vregName = name;
    slot.byteSize = (bitWidth == 32) ? 4 : 2;
    slot.is32bit = (bitWidth == 32);
    slot.kind = SlotKind::Param;
    slot.paramReg = reg;

    // For params, offset is positive (caller-pushed on stack)
    // First 3 16-bit params get registers, rest stay on stack
    if (!reg.empty()) {
        // Register-allocated param - no stack slot needed for the value
        slot.bpOffset = 0;  // placeholder, will be set later if needed
    } else {
        // Stack-based param - need to track its offset
        // Offset will be set in computeLayout based on position
        slot.bpOffset = 0;  // placeholder
    }

    vregToSlotIndex[name] = static_cast<int>(slots.size());
    slots.push_back(slot);

    // Also register track the param
    if (!reg.empty()) {
        vregToPhys_[name] = reg;
        physToVreg_[reg] = name;
    }
}

void StackFrame::addAllocaSlot(const std::string& vregName, int byteSize) {
    SlotInfo slot;
    slot.vregName = vregName;
    slot.byteSize = byteSize;
    slot.is32bit = (byteSize == 4);
    slot.kind = SlotKind::Alloca;
    slot.bpOffset = 0;  // will be set in computeLayout

    vregToSlotIndex[vregName] = static_cast<int>(slots.size());
    slots.push_back(slot);
}

void StackFrame::addSpillSlot(const std::string& vregName, int byteSize, bool is32bit) {
    SlotInfo slot;
    slot.vregName = vregName;
    slot.byteSize = byteSize;
    slot.is32bit = is32bit;
    slot.kind = SlotKind::Spill;
    slot.bpOffset = 0;  // will be set in computeLayout

    vregToSlotIndex[vregName] = static_cast<int>(slots.size());
    slots.push_back(slot);
}

void StackFrame::setMaxTempSpace(int bytes) {
    tempSize = bytes;
}

// ============================================================================
// Pass 2: Layout computation
// ============================================================================

void StackFrame::computeLayout() {
    // Layout:
    // [bp+4, bp+6, ...]  parameters (positive offsets)
    // [bp-2, bp-4, bp-6]  saved regs (bx, si, di)
    // [bp-8, ...]         locals (allocas + spills)
    // [bp-8-L, ...]       temp area (reset per block)

    // Step 1: Compute param offsets (positive)
    int paramOffset = 4;  // start at [bp+4]
    for (auto& slot : slots) {
        if (slot.kind == SlotKind::Param) {
            if (slot.paramReg.empty()) {
                // Stack-based param
                slot.bpOffset = paramOffset;
                paramOffset += slot.byteSize;
            }
            // Register-allocated params don't get stack offsets
        }
    }

    // Step 2: Compute local offsets (negative, growing)
    // Locals = allocas + spills
    nextLocalOffset = -6;  // after saved regs (bx, si, di = 6 bytes)
    localsSize = 0;

    for (auto& slot : slots) {
        if (slot.kind == SlotKind::Alloca || slot.kind == SlotKind::Spill) {
            nextLocalOffset -= slot.byteSize;
            slot.bpOffset = nextLocalOffset;
            localsSize += slot.byteSize;
        }
    }

    // Step 3: Compute temp area
    if (tempSize > 0) {
        // Temp area starts after locals
        tempBase = nextLocalOffset - tempSize;
        tempCurrent = tempBase;  // starts at the beginning
    } else {
        tempBase = nextLocalOffset;
        tempCurrent = nextLocalOffset;
    }

    // Step 4: Compute total frame size
    totalFrameSize = localsSize + tempSize;
}

int StackFrame::getFrameSize() const {
    return totalFrameSize;
}

// ============================================================================
// Pass 3: Accessors
// ============================================================================

bool StackFrame::hasSlot(const std::string& vregName) const {
    return vregToSlotIndex.find(vregName) != vregToSlotIndex.end();
}

std::string StackFrame::getBpOffset(const std::string& vregName) const {
    auto it = vregToSlotIndex.find(vregName);
    if (it == vregToSlotIndex.end()) {
        // Vreg not in slots - check if it's in register tracking
        auto regIt = vregToPhys_.find(vregName);
        if (regIt != vregToPhys_.end()) {
            return regIt->second;  // register name
        }
        return "ax";  // fallback
    }

    const SlotInfo& slot = slots[it->second];

    if (slot.kind == SlotKind::Param) {
        // For params, return the register if available
        if (!slot.paramReg.empty()) {
            return slot.paramReg;
        }
        // Otherwise return stack location
        return buildBpOffset(slot.bpOffset);
    }

    return buildBpOffset(slot.bpOffset);
}

std::string StackFrame::getHighBpOffset(const std::string& vregName) const {
    auto it = vregToSlotIndex.find(vregName);
    if (it == vregToSlotIndex.end()) {
        // Vreg not in slots - check if it's a register or direct bp-relative string
        auto regIt = vregToPhys_.find(vregName);
        std::string regOrAddr = vregName;  // default to vregName (could be bp-relative)
        if (regIt != vregToPhys_.end()) {
            regOrAddr = regIt->second;
        }
        
        if (regOrAddr.find("bp") != std::string::npos) {
            // Stack location or bp-relative string - compute high word offset
            int offset = 0;
            std::string offsetStr = regOrAddr.substr(regOrAddr.find("bp") + 2);
            if (!offsetStr.empty()) {
                try { offset = std::stoi(offsetStr); } catch (...) {}
            }
            return buildBpOffset(offset + 2);
        }
        // Register - return reg+2
        return regOrAddr + "+2";
    }

    const SlotInfo& slot = slots[it->second];

    if (slot.kind == SlotKind::Param) {
        if (!slot.paramReg.empty()) {
            return slot.paramReg + "+2";
        }
        // Stack param - compute high word offset
        return buildBpOffset(slot.bpOffset + 2);
    }

    // Local/spill/temp - compute high word offset
    return buildBpOffset(slot.bpOffset + 2);
}

int StackFrame::getSlotSize(const std::string& vregName) const {
    auto it = vregToSlotIndex.find(vregName);
    if (it == vregToSlotIndex.end()) {
        return 2;  // default
    }
    return slots[it->second].byteSize;
}

bool StackFrame::is32bit(const std::string& vregName) const {
    auto it = vregToSlotIndex.find(vregName);
    if (it == vregToSlotIndex.end()) {
        return false;
    }
    return slots[it->second].is32bit;
}

bool StackFrame::isAlloca(const std::string& vregName) const {
    auto it = vregToSlotIndex.find(vregName);
    if (it == vregToSlotIndex.end()) {
        return false;
    }
    return slots[it->second].kind == SlotKind::Alloca;
}

bool StackFrame::isParam(const std::string& vregName) const {
    auto it = vregToSlotIndex.find(vregName);
    if (it == vregToSlotIndex.end()) {
        return false;
    }
    return slots[it->second].kind == SlotKind::Param;
}

std::string StackFrame::getParamReg(const std::string& vregName) const {
    auto it = vregToSlotIndex.find(vregName);
    if (it == vregToSlotIndex.end()) {
        return "";
    }
    return slots[it->second].paramReg;
}

// ============================================================================
// Temp space management
// ============================================================================

std::string StackFrame::allocTemp(int byteSize, bool is32bit) {
    // Temp space is pre-allocated, just advance the pointer
    std::string offset = buildBpOffset(tempCurrent);

    // Create a slot entry for this temp (so we can track it)
    std::string vregName = "$temp_" + offset;

    SlotInfo slot;
    slot.vregName = vregName;
    slot.byteSize = byteSize;
    slot.is32bit = is32bit;
    slot.kind = SlotKind::Temp;
    slot.bpOffset = tempCurrent;

    vregToSlotIndex[vregName] = static_cast<int>(slots.size());
    slots.push_back(slot);

    tempCurrent += byteSize;

    return offset;
}

void StackFrame::resetTemp() {
    // Reset temp pointer to the beginning of temp area
    tempCurrent = tempBase;
}

// ============================================================================
// Register tracking
// ============================================================================

std::string StackFrame::getPhysReg(const std::string& vregName) const {
    // Check if it's a slot first (could be stack location)
    auto slotIt = vregToSlotIndex.find(vregName);
    if (slotIt != vregToSlotIndex.end()) {
        const SlotInfo& slot = slots[slotIt->second];
        if (slot.kind == SlotKind::Param && !slot.paramReg.empty()) {
            return slot.paramReg;
        }
        // Return stack location
        return buildBpOffset(slot.bpOffset);
    }

    // Check register tracking
    auto regIt = vregToPhys_.find(vregName);
    if (regIt != vregToPhys_.end()) {
        return regIt->second;
    }

    return "ax";  // fallback
}

void StackFrame::setPhysReg(const std::string& vregName, const std::string& reg) {
    // Clear old mapping if exists
    auto oldIt = vregToPhys_.find(vregName);
    if (oldIt != vregToPhys_.end()) {
        std::string oldReg = oldIt->second;
        physToVreg_.erase(oldReg);
    }

    // Clear old occupant of this register
    auto oldOcc = physToVreg_.find(reg);
    if (oldOcc != physToVreg_.end()) {
        vregToPhys_.erase(oldOcc->second);
        physToVreg_.erase(oldOcc);
    }

    // Set new mapping
    vregToPhys_[vregName] = reg;
    physToVreg_[reg] = vregName;
}

std::string StackFrame::getFreeTempReg() const {
    const char* regs[] = {"ax", "cx", "dx"};
    for (const char* reg : regs) {
        if (physToVreg_.find(reg) == physToVreg_.end()) {
            return reg;
        }
    }
    return "cx";  // fallback
}

void StackFrame::freeReg(const std::string& reg) {
    auto it = physToVreg_.find(reg);
    if (it != physToVreg_.end()) {
        vregToPhys_.erase(it->second);
        physToVreg_.erase(it);
    }
}

// ============================================================================
// Operand classification
// ============================================================================

StackFrame::OperandKind StackFrame::classifyOperand(const std::string& name) const {
    if (name.empty()) {
        return OperandKind::Unknown;
    }

    // Check if it's a constant
    if (name[0] == '-' || (name[0] >= '0' && name[0] <= '9')) {
        try {
            std::stoi(name);
            return OperandKind::Constant;
        } catch (...) {
            // Not a constant
        }
    }

    // Check if it's a global variable (starts with '.' or '@')
    if (name[0] == '.' || name[0] == '@') {
        return OperandKind::Global;
    }

    // Check if it's an alloca (address)
    if (isAlloca(name)) {
        return OperandKind::Alloca;
    }

    // Check if it's a vreg (register or stack location)
    if (hasSlot(name) || vregToPhys_.find(name) != vregToPhys_.end()) {
        std::string phys = getPhysReg(name);
        if (phys.find("bp") != std::string::npos) {
            return OperandKind::Memory;
        }
        return OperandKind::Register;
    }

    return OperandKind::Unknown;
}

// ============================================================================
// 32-bit load/store helpers
// ============================================================================

void StackFrame::emitLoad32(std::vector<Instruction286>& output,
                             const std::string& vregName,
                             const std::string& lowReg,
                             const std::string& highReg) const {
    std::string bpOffset = getBpOffset(vregName);

    // Load low word
    Instruction286 loadLow;
    loadLow.mnemonic = "mov";
    loadLow.operands.push_back(lowReg);
    loadLow.operands.push_back("[" + bpOffset + "]");
    output.push_back(loadLow);

    // Load high word
    Instruction286 loadHigh;
    loadHigh.mnemonic = "mov";
    loadHigh.operands.push_back(highReg);

    if (bpOffset.find("bp") != std::string::npos) {
        // Stack location - compute high word offset
        std::string highOffset = getHighBpOffset(vregName);
        loadHigh.operands.push_back("[" + highOffset + "]");
    } else {
        // Register - just use reg+2
        loadHigh.operands.push_back("[" + bpOffset + "+2]");
    }

    output.push_back(loadHigh);
}

void StackFrame::emitStore32(std::vector<Instruction286>& output,
                              const std::string& vregName,
                              const std::string& lowReg,
                              const std::string& highReg) const {
    std::string bpOffset = getBpOffset(vregName);

    // Store low word
    Instruction286 storeLow;
    storeLow.mnemonic = "mov";
    storeLow.operands.push_back("[" + bpOffset + "]");
    storeLow.operands.push_back(lowReg);
    output.push_back(storeLow);

    // Store high word
    Instruction286 storeHigh;
    storeHigh.mnemonic = "mov";
    storeHigh.operands.push_back(highReg);

    if (bpOffset.find("bp") != std::string::npos) {
        // Stack location - compute high word offset
        std::string highOffset = getHighBpOffset(vregName);
        storeHigh.operands.push_back("[" + highOffset + "]");
    } else {
        // Register - just use reg+2
        storeHigh.operands.push_back("[" + bpOffset + "+2]");
    }

    output.push_back(storeHigh);
}

// ============================================================================
// Stack space allocation (for inline allocations, not pre-allocated)
// ============================================================================

std::string StackFrame::allocStack(std::vector<Instruction286>& output,
                                    const std::string& vregName,
                                    int byteSize,
                                    bool is32bit) {
    // This is for cases where a slot wasn't pre-assigned during analysis
    // Emit sub sp, byteSize and return the bp-relative string

    Instruction286 subSp;
    subSp.mnemonic = "sub";
    subSp.operands.push_back("sp");
    subSp.operands.push_back(std::to_string(byteSize));
    output.push_back(subSp);

    // Update nextLocalOffset to track this allocation
    nextLocalOffset -= byteSize;
    int offset = nextLocalOffset;

    // Create a slot entry
    SlotInfo slot;
    slot.vregName = vregName;
    slot.byteSize = byteSize;
    slot.is32bit = is32bit;
    slot.kind = SlotKind::Spill;
    slot.bpOffset = offset;

    vregToSlotIndex[vregName] = static_cast<int>(slots.size());
    slots.push_back(slot);

    return buildBpOffset(offset);
}

} // namespace codegen
} // namespace llvm_i286
