// LLVM IR Parser Header
// Uses ANTLR4 generated parser

#ifndef LLVM_I286_IR_PARSER_H
#define LLVM_I286_IR_PARSER_H

#include <string>
#include <memory>

#include "ir/IrAst.h"

namespace llvm_i286 {
namespace ir {

class IrParser {
public:
    IrParser();
    ~IrParser();
    
    // Parse an LLVM IR file and return the module AST
    std::unique_ptr<Module> parseFile(const std::string& filename);
    
    // Parse an LLVM IR string and return the module AST
    std::unique_ptr<Module> parseString(const std::string& ir);
};

} // namespace ir
} // namespace llvm_i286

#endif // LLVM_I286_IR_PARSER_H
