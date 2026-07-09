// Stack Frame Management - Centralized stack frame layout and access
//
// This file contains the StackFrame class, which is the central manager for
// all stack frame layout knowledge. It replaces the decentralized stack tracking
// in the instruction selectors and eliminates all inline `sub sp`/`add sp` patterns.
//
// The StackFrame is populated during a pre-pass (FunctionAnalysis), then used
// during instruction lowering to provide fixed offsets for all vregs.

#ifndef LLVM_I286_CODEGEN_STACK_FRAME_H
#define LLVM_I286_CODEGEN_STACK_FRAME_H

#include "codegen/InstructionSelect.h"

#include <string>
#include <vector>
#include <map>
#include <set>

namespace llvm_i286 {
namespace codegen {

// Kind of stack slot
enum class SlotKind {
    Param,   // Function parameter (positive bp offset)
    Alloca,  // Local variable (negative bp offset)
    Spill,   // Cross-block spill slot (negative bp offset)
    Temp     // Temp scratch space (negative bp offset, reset per block)
};

// Information about a single stack slot
struct SlotInfo {
    std::string vregName;
    int bpOffset;         // positive for params, negative for locals/spills/temp
    int byteSize;         // 2 for 16-bit, 4 for 32-bit
    bool is32bit;
    SlotKind kind;
    std::string paramReg; // for Param slots: "bx"/"si"/"di" or "" if on stack
};

// Central stack frame manager
// Owns all slot assignments, register tracking, 32-bit helpers, operand classification
class StackFrame {
public:
    // =========================================================================
    // Pass 1: Analysis methods (called by FunctionAnalysis)
    // =========================================================================

    // Add a function parameter
    // name: parameter name
    // bitWidth: parameter bit width (16 or 32)
    // reg: physical register for first 3 16-bit params ("bx"/"si"/"di") or "" if on stack
    void addParam(const std::string& name, int bitWidth, const std::string& reg);

    // Add an alloca slot (local variable)
    void addAllocaSlot(const std::string& vregName, int byteSize);

    // Add a spill slot for a cross-block vreg
    void addSpillSlot(const std::string& vregName, int byteSize, bool is32bit);

    // Set the max temp space needed across all blocks
    void setMaxTempSpace(int bytes);

    // =========================================================================
    // Pass 2: Layout computation
    // =========================================================================

    // Compute final offsets for all slots
    // Must be called after all addParam/addAllocaSlot/addSpillSlot/setMaxTempSpace calls
    void computeLayout();

    // Get the total frame size (L + T) for prologue sub sp
    int getFrameSize() const;

    // =========================================================================
    // Pass 3: Accessors (called by instruction handlers)
    // =========================================================================

    // Check if a vreg has a slot assigned
    bool hasSlot(const std::string& vregName) const;

    // Get bp-relative offset string for a vreg
    // Examples: "bp+4", "bp-8", "bp-10"
    std::string getBpOffset(const std::string& vregName) const;

    // Get bp-relative offset string for the high word of a 32-bit vreg
    // This is bp offset + 2 for stack locations, or just reg+2 for registers
    std::string getHighBpOffset(const std::string& vregName) const;

    // Get the size of a vreg's slot in bytes
    int getSlotSize(const std::string& vregName) const;

    // Check if a vreg is 32-bit
    bool is32bit(const std::string& vregName) const;

    // Check if a vreg is an alloca result (address, not value)
    bool isAlloca(const std::string& vregName) const;

    // Check if a vreg is a function parameter
    bool isParam(const std::string& vregName) const;

    // Get the physical register for a parameter (if register-allocated)
    std::string getParamReg(const std::string& vregName) const;

    // =========================================================================
    // Temp space management (called at block boundaries)
    // =========================================================================

    // Allocate temp space (within pre-allocated area)
    // Returns bp-relative string for the allocated slot
    std::string allocTemp(int byteSize, bool is32bit = false);

    // Reset temp space allocator (called at start of each basic block)
    void resetTemp();

    // =========================================================================
    // Register tracking
    // =========================================================================

    // Get physical register for a vreg (register or stack location)
    std::string getPhysReg(const std::string& vregName) const;

    // Set the physical register for a vreg
    void setPhysReg(const std::string& vregName, const std::string& reg);

    // Get a free temp register (AX, CX, DX)
    std::string getFreeTempReg() const;

    // Free a register
    void freeReg(const std::string& reg);

    // =========================================================================
    // Operand classification
    // =========================================================================

    enum class OperandKind {
        Constant,
        Register,
        Memory,
        Global,
        Alloca,
        Unknown
    };

    // Classify an operand name
    OperandKind classifyOperand(const std::string& name) const;

    // =========================================================================
    // 32-bit load/store helpers (centralized)
    // =========================================================================

    // Emit 32-bit load from stack/memory into lowReg:highReg
    void emitLoad32(std::vector<Instruction286>& output,
                     const std::string& vregName,
                     const std::string& lowReg,
                     const std::string& highReg) const;

    // Emit 32-bit store from lowReg:highReg to stack/memory
    void emitStore32(std::vector<Instruction286>& output,
                      const std::string& vregName,
                      const std::string& lowReg,
                      const std::string& highReg) const;

    // =========================================================================
    // Stack space allocation (for inline allocations, not pre-allocated)
    // =========================================================================

    // Allocate stack space for a vreg (used when slot not pre-assigned)
    // Returns bp-relative string and emits "sub sp, N"
    std::string allocStack(std::vector<Instruction286>& output,
                           const std::string& vregName,
                           int byteSize,
                           bool is32bit = false);

private:
    // Slot storage
    std::vector<SlotInfo> slots;
    std::map<std::string, int> vregToSlotIndex;

    // Register tracking
    std::map<std::string, std::string> vregToPhys_;
    std::map<std::string, std::string> physToVreg_;

    // Frame layout
    int savedRegsSize = 6;        // bx, si, di
    int nextLocalOffset = -6;     // grows negative (after saved regs)
    int localsSize = 0;           // sum of alloca + spill slot sizes
    int tempSize = 0;             // max temp space (pre-allocated)
    int totalFrameSize = 0;       // localsSize + tempSize
    int tempBase = 0;             // bp offset of temp area start
    int tempCurrent = 0;          // current temp offset (reset per block)

    // Helper to build bp-relative offset string
    static std::string buildBpOffset(int offset);
};

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_STACK_FRAME_H
