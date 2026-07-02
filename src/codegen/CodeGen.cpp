// Code Generator Implementation
// Main pipeline: IR -> Instruction Selection -> Register Allocation -> NASM Emission

#include "codegen/CodeGen.h"
#include "ir/IrParser.h"
#include "codegen/InstructionSelect.h"
#include "codegen/Emitter.h"

#include <iostream>

namespace llvm_i286 {
namespace codegen {

struct CodeGen::Impl {
    CodeGenOptions options;
};

CodeGen::CodeGen(const CodeGenOptions& options) : impl(std::make_unique<Impl>()) {
    impl->options = options;
}

CodeGen::~CodeGen() = default;

std::string CodeGen::generate(const ir::Module& module) {
    Emitter emitter;
    InstructionSelector selector;
    
    // Build function parameter type map for 32-bit argument detection
    std::map<std::string, std::vector<int>> funcParamBitWidths;
    for (const auto& func : module.functions) {
        std::vector<int> paramWidths;
        for (const auto& param : func->params) {
            if (param->type) {
                if (param->type->isInteger()) {
                    paramWidths.push_back(param->type->bitWidth);
                } else if (param->type->isPointer()) {
                    // Pointers are 16-bit in our model
                    paramWidths.push_back(16);
                } else {
                    paramWidths.push_back(0);
                }
            } else {
                paramWidths.push_back(0);
            }
        }
        funcParamBitWidths[func->name] = paramWidths;
    }
    selector.setFunctionDeclarations(funcParamBitWidths);
    
    std::string allFunctions;
    
    // Determine entry function name: "main" if it exists, otherwise first non-declaration function
    std::string entryFuncName;
    for (const auto& func : module.functions) {
        if (!func->isDeclaration) {
            if (func->name == "main") {
                entryFuncName = "main";
                break;
            }
            if (entryFuncName.empty()) {
                entryFuncName = func->name;
            }
        }
    }
    
    // Collect external function declarations
    std::vector<std::string> externFuncs;
    for (const auto& func : module.functions) {
        if (func->isDeclaration) {
            // printf is not available in the runtime library
            if (func->name == "printf") {
                continue;
            }
            externFuncs.push_back(func->name);
        }
    }
    
    // Add runtime library functions (provided by runtime.lib)
    externFuncs.push_back("_MultiplyI32");
    externFuncs.push_back("_DivideI32");
    
    // Generate code for each function
    for (const auto& func : module.functions) {
        if (!func->isDeclaration) {
            // Lower function to 286 instructions
            auto lowered = selector.lowerFunction(*func);
            
            // Emit function with full prologue/epilogue
            allFunctions += emitter.emitFunction(lowered, func->name);
            allFunctions += "\n";
        }
    }
    
    // Generate data segment for global variables (string constants)
    std::string dataSegment;
    for (const auto& gv : module.globals) {
        if (gv->isConstant && gv->initializer) {
            std::string dataLabel = gv->name;
            // NASM treats labels starting with '.' as local labels
            // Prefix with '_' to make them global
            if (!dataLabel.empty() && dataLabel[0] == '.') {
                dataLabel = "_" + dataLabel.substr(1);
            }
            std::string initData = gv->initializer->text;
            
            // Convert LLVM string constant to NASM format
            // LLVM: c"Result: %d\0A\00" -> NASM: db "Result: %d", 0x0A, 0x00, 0
            if (initData.size() >= 2 && initData[0] == 'c' && initData[1] == '"') {
                // Remove c" prefix and " suffix
                std::string strContent = initData.substr(2, initData.size() - 3);
                
                // Split string at escape sequences and output as comma-separated bytes
                std::string parts;
                std::string current;
                
             for (size_t i = 0; i < strContent.size(); i++) {
                    if (strContent[i] == '\\' && i + 1 < strContent.size()) {
                        // Flush current string
                        if (!current.empty()) {
                            if (!parts.empty()) parts += ", ";
                            parts += "\"" + current + "\"";
                            current.clear();
                        }
                        
                        char next = strContent[i + 1];
                        if (i + 3 < strContent.size()) {
                        }
                        if (next == 'n' || (next == '0' && i + 3 < strContent.size() && strContent[i + 2] == 'A')) {
                            if (!parts.empty()) parts += ", ";
                            parts += "0x0A";
                            if (next == '0') i += 1; // Skip extra char
                        } else if (next == 'r' || (next == '0' && i + 3 < strContent.size() && strContent[i + 2] == 'D')) {
                            if (!parts.empty()) parts += ", ";
                            parts += "0x0D";
                            if (next == '0') i += 1;
                        } else if (next == 't') {
                            if (!parts.empty()) parts += ", ";
                            parts += "0x09";
                        } else if (next == '0' && i + 3 <= strContent.size()) {
                            // Hex escape: \0X where X is hex digit
                            char third = strContent[i + 2];
                            if ((third >= '0' && third <= '9') || (third >= 'A' && third <= 'F') || (third >= 'a' && third <= 'f')) {
                                if (!parts.empty()) parts += ", ";
                                std::string hexStr = "0x0";
                                hexStr += third;
                                parts += hexStr;
                                std::cerr << "DEBUG: hex escape \\0" << third << " at pos " << i << std::endl;
                                i += 1; // Skip extra char (the hex digit)
                            } else {
                                if (!parts.empty()) parts += ", ";
                                parts += "0x00";
                            }
                        } else if (next == '0') {
                            if (!parts.empty()) parts += ", ";
                            parts += "0x00";
                        } else if (next == '\\') {
                            current += '\\';
                        } else if (next == '"') {
                            current += '"';
                        } else {
                            current += strContent[i];
                        }
                        i += 1; // Skip next char
                    } else {
                        current += strContent[i];
                    }
                }
                
                // Flush remaining string
                if (!current.empty()) {
                    if (!parts.empty()) parts += ", ";
                    parts += "\"" + current + "\"";
                }
                
                dataSegment += dataLabel + ":\n";
                dataSegment += "\tdb " + parts + ", 0\n\n";
            }
        }
    }
    
    // Wrap in module structure
    // entryFuncName non-empty: ..start: calls entry function, then exits
    // entryFuncName empty: code emitted directly at ..start:
    return emitter.emitModule(
        module.sourceFilename.empty() ? "output" : module.sourceFilename,
        allFunctions,
        dataSegment, // data segment
        "", // bss segment
        entryFuncName,
        externFuncs
    );
}

std::string CodeGen::generateFromFile(const std::string& filename) {
    ir::IrParser parser;
    auto module = parser.parseFile(filename);
    if (!module) {
        return "";
    }
    return generate(*module);
}

} // namespace codegen
} // namespace llvm_i286
