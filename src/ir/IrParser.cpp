// LLVM IR Parser Implementation
// Uses ANTLR4 generated parser and our visitor

#include "ir/IrParser.h"
#include "ir/IrVisitor.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "LLVMIRLexer.h"
#include "LLVMIRParser.h"

namespace llvm_i286 {
namespace ir {

IrParser::IrParser() = default;

IrParser::~IrParser() = default;

std::unique_ptr<Module> IrParser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return nullptr;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseString(buffer.str());
}

std::unique_ptr<Module> IrParser::parseString(const std::string& ir) {
    std::cout << "Parsing LLVM IR (" << ir.size() << " bytes)" << std::endl;

    // Strip metadata attachments (!llvm.loop, !dbg, etc.) that confuse the parser
    std::string cleaned;
    cleaned.reserve(ir.size());
    {
        std::stringstream ss(ir);
        std::string line;
        while (std::getline(ss, line)) {
            // Remove trailing ", !... metadata" from instruction lines
            // Also remove standalone metadata lines (lines starting with !)
            auto comma_bang = line.find(", !");
            if (comma_bang != std::string::npos) {
                line = line.substr(0, comma_bang);
            }
            // Skip standalone metadata definition lines: !N = !{...}
            // but keep !dbg references etc. that are already stripped above
            if (!line.empty() && line[0] == '!' && line.find('=') != std::string::npos) {
                continue;
            }
            cleaned += line;
            cleaned += '\n';
        }
    }

    try {
        // Create ANTLR4 input stream
        antlr4::ANTLRInputStream input(cleaned);
        
        // Create lexer and tokenize
        antlr::generated::LLVMIRLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);
        
        // Create parser
        antlr::generated::LLVMIRParser parser(&tokens);
        
        // Parse compilation unit
        auto *tree = parser.compilationUnit();
        
        // Use our visitor to build AST
        IrVisitor visitor;
        return visitor.visitModule(tree);
        
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return nullptr;
    }
}

} // namespace ir
} // namespace llvm_i286
