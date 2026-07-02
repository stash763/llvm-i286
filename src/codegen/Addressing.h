// Addressing
// Segment:offset address management for 80286

#ifndef LLVM_I286_CODEGEN_ADDRESSING_H
#define LLVM_I286_CODEGEN_ADDRESSING_H

#include <string>
#include <map>
#include <memory>

#include "ir/IrAst.h"

namespace llvm_i286 {
namespace codegen {

// Segment types
enum class Segment {
    Code,    // _TEXT
    Data,    // _DATA
    BSS,     // _BSS
    Stack,   // STACK
    Extra    // ES segment
};

// Represents a memory operand with segment:offset
struct MemoryOperand {
    Segment segment;
    std::string baseReg;   // e.g., "bx", "bp"
    std::string indexReg;  // e.g., "si", "di"
    int displacement = 0;
    std::string label;     // For direct label references
    bool hasSegmentOverride = false;
};

class Addressing {
public:
    Addressing();
    ~Addressing();
    
    // Get the segment for a global variable
    Segment getSegmentForGlobal(const std::string& name) const;
    
    // Get the segment for a function
    Segment getSegmentForFunction(const std::string& name) const;
    
    // Set the segment for a global variable
    void setSegmentForGlobal(const std::string& name, Segment seg);
    
    // Get NASM segment name from enum
    static std::string segmentToNasm(Segment seg);
    
    // Generate segment override prefix
    static std::string segmentOverride(Segment seg);
    
    // Calculate stack offset for a local variable
    int getStackOffset(const std::string& funcName, const std::string& varName) const;
    
    // Set stack offset for a local variable
    void setStackOffset(const std::string& funcName, const std::string& varName, int offset);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

// Convert register name to NASM format
std::string toNasmReg(const std::string& reg);

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_ADDRESSING_H
