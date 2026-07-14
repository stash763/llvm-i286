// NASM Safe Name Mangling
// Ensures symbol names don't conflict with NASM reserved words

#ifndef LLVM_I286_CODEGEN_NASM_SAFE_H
#define LLVM_I286_CODEGEN_NASM_SAFE_H

#include <string>

namespace llvm_i286 {
namespace codegen {

// Check if a name is a NASM reserved word
bool isNasmReservedWord(const std::string& name);

// Mangle a name if it conflicts with NASM reserved words
std::string safeNasmName(const std::string& name);

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_NASM_SAFE_H
