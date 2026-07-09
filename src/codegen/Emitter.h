// NASM Text Emitter
// Emits 80286 instructions as NASM assembly text

#ifndef LLVM_I286_CODEGEN_EMITTER_H
#define LLVM_I286_CODEGEN_EMITTER_H

#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include "codegen/InstructionSelect.h"

namespace llvm_i286 {
namespace codegen {

class Emitter {
public:
    Emitter();
    ~Emitter();
    
    // Emit a single 286 instruction as NASM text
    std::string emitInstruction(const Instruction286& inst);
    
    // Emit a lowered instruction (possibly multiple 286 instructions)
    std::string emitLoweredInstruction(const LoweredInstruction& lowered);
    
    // Emit a complete function
   std::string emitFunction(const std::vector<LoweredInstruction>& instructions,
                              const std::string& funcName,
                              int frameSize = 0);
    
    // Emit complete NASM file with segments
    // entryFuncName: name of function to call from ..start (empty = emit code directly)
    std::string emitModule(const std::string& moduleName,
                          const std::string& codeSegment,
                          const std::string& dataSegment,
                          const std::string& bssSegment,
                          const std::string& entryFuncName = "",
                          const std::vector<std::string>& externFuncs = std::vector<std::string>());
    
    // Get the output stream for building up output
    std::stringstream& getStream() { return output; }
    std::string getOutput() const { return output.str(); }
    void reset() { output.str(""); output.clear(); }

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
    std::stringstream output;
};

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_EMITTER_H
