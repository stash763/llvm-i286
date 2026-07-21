// Location - Typed location abstraction for register/stack slots
//
// Replaces the string-based location representation where getPhysReg() returns
// strings like "ax", "bp-8", "bp+6" that must be parsed with find("bp") checks.
//
// A Location knows whether it's a register or stack slot, and can compute
// high-word offsets without string parsing.

#ifndef LLVM_I286_CODEGEN_LOCATION_H
#define LLVM_I286_CODEGEN_LOCATION_H

#include <string>
#include <cstdint>

namespace llvm_i286 {
namespace codegen {

class Location {
public:
    enum class Kind {
        Register,     // In a physical register (AX, BX, CX, DX, SI, DI)
        StackSlot,    // At [bp+offset]
        None          // Not yet allocated (error if queried)
    };

    Kind kind = Kind::None;
    std::string regName;  // For Register: "ax", "bx", etc.
    int bpOffset = 0;     // For StackSlot: -8, +6, etc. (0 means [bp])
    bool is32bit = false;  // Whether this location holds a 32-bit value

    // Default constructor → Kind::None
    Location() = default;

    // Construct a register location
    static Location reg(const std::string& r, bool is32 = false) {
        Location loc;
        loc.kind = Kind::Register;
        loc.regName = r;
        loc.is32bit = is32;
        return loc;
    }

    // Construct a stack slot location
    static Location stack(int offset, bool is32 = false) {
        Location loc;
        loc.kind = Kind::StackSlot;
        loc.bpOffset = offset;
        loc.is32bit = is32;
        return loc;
    }

    // Construct a "none" location (unallocated — indicates a bug)
    static Location none() {
        Location loc;
        loc.kind = Kind::None;
        return loc;
    }

    bool isStack() const { return kind == Kind::StackSlot; }
    bool isRegister() const { return kind == Kind::Register; }
    bool isNone() const { return kind == Kind::None; }

    // Returns the address string for the low word of this location.
    // For StackSlot: "[bp-8]" or "[bp+6]" or "[bp]"
    // For Register: "ax", "bx", etc.
    std::string lowWordAddr() const {
        switch (kind) {
            case Kind::Register:
                return regName;
            case Kind::StackSlot:
                return buildBpOffset(bpOffset);
            case Kind::None:
                return "ax";  // fallback — should not happen
        }
        return "ax";
    }

    // Returns the address string for the high word of this location.
    // For StackSlot: "[bp-6]" when low is "[bp-8]" (offset + 2)
    // For Register: "ax+2" (register high word accessed as [reg+2])
    std::string highWordAddr() const {
        switch (kind) {
            case Kind::Register:
                return regName + "+2";
            case Kind::StackSlot:
                return buildBpOffset(bpOffset + 2);
            case Kind::None:
                return "ax+2";  // fallback
        }
        return "ax+2";
    }

    // Returns the bracketed address for memory access.
    // For StackSlot: "[bp-8]"
    // For Register: "[ax]" (dereferenced)
    std::string bracketed() const {
        switch (kind) {
            case Kind::Register:
                return "[" + regName + "]";
            case Kind::StackSlot:
                return "[" + buildBpOffset(bpOffset) + "]";
            case Kind::None:
                return "[ax]";
        }
        return "[ax]";
    }

    // Returns the bracketed high-word address for memory access.
    // For StackSlot: "[bp-6]"
    // For Register: "[ax+2]"
    std::string bracketedHigh() const {
        switch (kind) {
            case Kind::Register:
                return "[" + regName + "+2]";
            case Kind::StackSlot:
                return "[" + buildBpOffset(bpOffset + 2) + "]";
            case Kind::None:
                return "[ax+2]";
        }
        return "[ax+2]";
    }

private:
    static std::string buildBpOffset(int offset) {
        if (offset == 0) return "bp";
        if (offset > 0) return "bp+" + std::to_string(offset);
        return "bp" + std::to_string(offset);  // negative, e.g. "bp-8"
    }
};

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_LOCATION_H