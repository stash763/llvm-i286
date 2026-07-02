// Code Generator
// Main pipeline orchestrator

#ifndef LLVM_I286_CODEGEN_CODEGEN_H
#define LLVM_I286_CODEGEN_CODEGEN_H

#include <string>
#include <memory>

#include "ir/IrAst.h"

namespace llvm_i286 {
namespace codegen {

struct CodeGenOptions {
    std::string outputFormat = "nasm"; // Output format: nasm
    std::string targetOS = "os2"; // Target OS: os2, netbsd
    int pointerSize = 16; // Pointer size in bits
    bool enable32BitEmulation = false; // Enable 32/64-bit emulation
    std::string outputFile;
};

class CodeGen {
public:
    CodeGen(const CodeGenOptions& options = CodeGenOptions());
    ~CodeGen();
    
    // Generate code from an IR module
    // Returns NASM assembly text
    std::string generate(const ir::Module& module);
    
    // Generate code from an IR file
    std::string generateFromFile(const std::string& filename);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_CODEGEN_H
