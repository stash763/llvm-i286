// LLVM i286 Code Generator
// Main entry point

#include <iostream>
#include <fstream>
#include <string>

#include "codegen/CodeGen.h"

using namespace llvm_i286;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: llvm-i286 <input.ll> [-o output.asm]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "output.asm";
    
    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }
    
    std::cout << "LLVM i286 Code Generator" << std::endl;
    std::cout << "Input: " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;
    
    // Run code generation pipeline
    codegen::CodeGen gen;
    std::string asmOutput = gen.generateFromFile(inputFile);
    
    if (asmOutput.empty()) {
        std::cerr << "Error: Code generation failed" << std::endl;
        return 1;
    }
    
    // Write output to file
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot open output file " << outputFile << std::endl;
        return 1;
    }
    
    outFile << asmOutput;
    outFile.close();
    
    std::cout << "Code generation complete." << std::endl;
    std::cout << "Generated " << asmOutput.size() << " bytes of NASM assembly." << std::endl;
    
    return 0;
}
