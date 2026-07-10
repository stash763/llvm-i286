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
                    // Pointers are 32-bit (i386 target)
                    paramWidths.push_back(32);
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
    
    // Pass alias map to selector for weak_alias resolution
    selector.setAliasMap(module.aliases);
    
    std::string allFunctions;
    
    // Determine entry function name: only "main" gets a start entry point.
    // Helper/library modules (without main) should not emit a start label,
    // so they can be linked alongside a main module.
    std::string entryFuncName;
    for (const auto& func : module.functions) {
        if (!func->isDeclaration && func->name == "main") {
            entryFuncName = "main";
            break;
        }
    }
    
    // Collect external function declarations
    std::vector<std::string> externFuncs;
    for (const auto& func : module.functions) {
        if (func->isDeclaration) {
            // LLVM intrinsics are lowered to runtime calls
            // Rename llvm.memcpy -> memcpy, llvm.va_start -> llvm_va_start, llvm.va_end -> llvm_va_end
            std::string funcName = func->name;
            if (funcName.find("llvm.memcpy") != std::string::npos) {
                continue; // Skip llvm.memcpy entirely
            }
            if (funcName == "llvm.va_start") {
                funcName = "llvm_va_start";
            } else if (funcName == "llvm.va_end") {
                funcName = "llvm_va_end";
            }
            externFuncs.push_back(funcName);
        }
    }
    
    // Collect external global declarations (declared but not defined in this module)
    std::vector<std::string> externGlobals;
    for (const auto& gv : module.globals) {
        if (gv->linkage == "external") {
            externGlobals.push_back(gv->name);
        }
    }
    
    // Note: Runtime library functions are added below after globals processing
    
    // Generate code for each function
    for (const auto& func : module.functions) {
        if (!func->isDeclaration) {
            // Lower function to 286 instructions
            auto lowered = selector.lowerFunction(*func);
            
            // Emit function with full prologue/epilogue
            int frameSize = selector.getFrameSize();
            allFunctions += emitter.emitFunction(lowered, func->name, frameSize);
            allFunctions += "\n";
        }
    }
    
    // Generate data segment for global variables (initialized data)
    std::string dataSegment;
    std::string bssSegment;
    
    for (const auto& gv : module.globals) {
        // Skip external globals - they are declared elsewhere and linked in
        if (gv->linkage == "external") continue;
        
        std::string dataLabel = gv->name;
        // NASM treats labels starting with '.' as local labels
        // Prefix with '_' to make them global
        if (!dataLabel.empty() && dataLabel[0] == '.') {
            dataLabel = "_" + dataLabel.substr(1);
        }
        
        // Check if this is an uninitialized global (BSS)
        bool isBSS = !gv->initializer || 
                     (gv->initializer && (gv->initializer->kind == ir::ValueKind::ConstantZeroInitializer || 
                                          gv->initializer->kind == ir::ValueKind::ConstantNull));
        
        if (isBSS) {
            // BSS segment: uninitialized data
            int byteSize = 2; // Default to 2 bytes (16-bit)
            if (gv->type) {
                if (gv->type->isInteger()) {
                    byteSize = gv->type->bitWidth / 8;
                    if (byteSize < 2) byteSize = 2; // Minimum 2 bytes
                } else if (gv->type->isPointer()) {
                    byteSize = 4; // 32-bit pointer
                }
            }
            
            bssSegment += dataLabel + ":\n";
            bssSegment += "\tresb " + std::to_string(byteSize) + "\n\n";
        } else if (gv->isConstant && gv->initializer) {
            // Initialized constant data (e.g., string constants)
            std::string initData = gv->initializer->text;
            
            // Convert LLVM string constant to NASM format
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
            } else {
                // Other initialized constants (integers, etc.)
                // Check for complex LLVM IR initializers we can't handle directly
                // (structs, arrays, concatenated strings, zeroinitializer, etc.)
                std::string initValue = gv->initializer->text;
                bool isComplex = initValue.find("<{") != std::string::npos ||
                                 initValue.find("zeroinitializer") != std::string::npos ||
                                 initValue.find("undef") != std::string::npos ||
                                 initValue.find("[") != std::string::npos;
                
                int byteSize = 2; // Default to 2 bytes
                if (gv->type) {
                    if (gv->type->isInteger()) {
                        byteSize = gv->type->bitWidth / 8;
                        if (byteSize < 2) byteSize = 2;
                    } else if (gv->type->isPointer()) {
                        byteSize = 4;
                    }
                }
                
                if (isComplex) {
                    // Complex initializer: emit as zero-initialized BSS data
                    bssSegment += dataLabel + ":\n";
                    bssSegment += "\tresb " + std::to_string(byteSize) + "\n\n";
                } else {
                    // Strip @ prefix from global references for NASM compatibility
                    if (!initValue.empty() && initValue[0] == '@') {
                        initValue = initValue.substr(1);
                    }
                    // Also strip leading '.' and add '_' for NASM compatibility
                    if (!initValue.empty() && initValue[0] == '.') {
                        initValue = "_" + initValue.substr(1);
                    }
                    
                    dataSegment += dataLabel + ":\n";
                    if (byteSize == 4) {
                        dataSegment += "\tdd " + initValue + "\n\n"; // 32-bit dword
                    } else {
                        dataSegment += "\tdw " + initValue + "\n\n"; // 16-bit word
                    }
                }
            }
        } else if (gv->initializer) {
            // Initialized non-constant global (data segment)
            // Check if initializer is null or zero (should go in BSS)
            if (gv->initializer->kind == ir::ValueKind::ConstantNull || 
                gv->initializer->kind == ir::ValueKind::ConstantZeroInitializer) {
                // Treat as BSS (uninitialized)
                int byteSize = 2;
                if (gv->type) {
                    if (gv->type->isInteger()) {
                        byteSize = gv->type->bitWidth / 8;
                        if (byteSize < 2) byteSize = 2;
                    } else if (gv->type->isPointer()) {
                        byteSize = 4;
                    }
                }
                
                bssSegment += dataLabel + ":\n";
                bssSegment += "\tresb " + std::to_string(byteSize) + "\n\n";
            } else {
                std::string initValue = gv->initializer->text;
                bool isComplex = initValue.find("<{") != std::string::npos ||
                                 initValue.find("zeroinitializer") != std::string::npos ||
                                 initValue.find("undef") != std::string::npos ||
                                 initValue.find("[") != std::string::npos;
                
                int byteSize = 2;
                if (gv->type) {
                    if (gv->type->isInteger()) {
                        byteSize = gv->type->bitWidth / 8;
                        if (byteSize < 2) byteSize = 2;
                    } else if (gv->type->isPointer()) {
                        byteSize = 4;
                    }
                }
                
                if (isComplex) {
                    // Complex initializer: emit as zero-initialized BSS data
                    bssSegment += dataLabel + ":\n";
                    bssSegment += "\tresb " + std::to_string(byteSize) + "\n\n";
                } else {
                    // Strip @ prefix from global references for NASM compatibility
                    if (!initValue.empty() && initValue[0] == '@') {
                        initValue = initValue.substr(1);
                    }
                    // Also strip leading '.' and add '_' for NASM compatibility
                    if (!initValue.empty() && initValue[0] == '.') {
                        initValue = "_" + initValue.substr(1);
                    }
                    
                    dataSegment += dataLabel + ":\n";
                    if (byteSize == 4) {
                        dataSegment += "\tdd " + initValue + "\n\n";
                    } else {
                        dataSegment += "\tdw " + initValue + "\n\n";
                    }
                }
            }
        }
    }
    
    // Add runtime library functions to extern list
    // These must match the tested runtime in ./runtime/
    externFuncs.push_back("_MultiplyI32");
    externFuncs.push_back("_DivideI32");
    externFuncs.push_back("printnum");
    externFuncs.push_back("itoa");
    
    // Wrap in module structure
    return emitter.emitModule(
        module.sourceFilename.empty() ? "output" : module.sourceFilename,
        allFunctions,
        dataSegment, // data segment
        bssSegment, // bss segment
        entryFuncName,
        externFuncs,
        externGlobals
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
