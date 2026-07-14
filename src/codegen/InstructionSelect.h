// Instruction Selection
// Lowering LLVM IR instructions to 80286 instructions

#ifndef LLVM_I286_CODEGEN_INSTRUCTION_SELECT_H
#define LLVM_I286_CODEGEN_INSTRUCTION_SELECT_H

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "ir/IrAst.h"

namespace llvm_i286 {
namespace codegen {

// Represents a 286 instruction for emission
struct Instruction286 {
    std::string mnemonic;
    std::vector<std::string> operands;
    std::string comment;
    int size = 0; // Instruction size in bytes (for code size tracking)
};

// Represents a lowered sequence of 286 instructions
struct LoweredInstruction {
    std::string label; // Optional label for this instruction
    std::vector<Instruction286> instructions;
};

class InstructionSelector {
public:
    InstructionSelector();
    ~InstructionSelector();

    // Set function declarations for parameter type lookup
    void setFunctionDeclarations(const std::map<std::string, std::vector<int>>& decls);

    // Set alias map for weak_alias resolution
    void setAliasMap(const std::map<std::string, std::string>& aliases);

    // Set type definitions for GEP offset computation
    void setTypeDefinitions(const std::map<std::string, std::unique_ptr<ir::Type>>* typeDefs);

    // Get the computed frame size (for prologue emission)
    int getFrameSize() const;

    // Lower an LLVM IR instruction to 286 instruction sequence(s)
    std::vector<LoweredInstruction> lowerInstruction(const ir::Instruction& irInst);

    // Lower a basic block
    std::vector<LoweredInstruction> lowerBasicBlock(const ir::BasicBlock& bb);

    // Lower a function (two-pass: analysis + lowering)
    std::vector<LoweredInstruction> lowerFunction(const ir::Function& func);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_INSTRUCTION_SELECT_H
