#!/bin/bash
# Regenerate ANTLR4 parser from LLVM IR grammar

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GRAMMAR_FILE="${SCRIPT_DIR}/grammars-v4/llvm-ir/LLVMIR.g4"
ANTLR_TOOL_JAR="${SCRIPT_DIR}/antlr4/tool/target/antlr4-4.13.3-SNAPSHOT-complete.jar"
OUTPUT_DIR="${SCRIPT_DIR}/antlr"
PACKAGE="antlr::generated"

if [ ! -f "${ANTLR_TOOL_JAR}" ]; then
    echo "Error: ANTLR4 tool jar not found at ${ANTLR_TOOL_JAR}"
    echo "Build ANTLR4 tool: cd antlr4/tool && mvn package"
    exit 1
fi

if [ ! -f "${GRAMMAR_FILE}" ]; then
    echo "Error: Grammar file not found at ${GRAMMAR_FILE}"
    exit 1
fi

echo "Generating ANTLR4 parser from ${GRAMMAR_FILE}"
echo "Output directory: ${OUTPUT_DIR}"
echo "ANTLR4 tool: ${ANTLR_TOOL_JAR}"
echo "Package: ${PACKAGE}"

# Clean old generated files
rm -f "${OUTPUT_DIR}/LLVMIR"*.h "${OUTPUT_DIR}/LLVMIR"*.cpp
rm -f "${OUTPUT_DIR}/LLVMIR".interp "${OUTPUT_DIR}/LLVMIRLexer".interp
rm -f "${OUTPUT_DIR}"/*.tokens
rm -rf "${OUTPUT_DIR}/grammars-v4"

# Generate parser
java -jar "${ANTLR_TOOL_JAR}" \
    -Dlanguage=Cpp \
    -o "${OUTPUT_DIR}" \
    -package "${PACKAGE}" \
    -visitor \
    -no-listener \
    "${GRAMMAR_FILE}"

# Move files from nested directory to output directory
# ANTLR4 creates subdirectory based on grammar relative path
if [ -d "${OUTPUT_DIR}/grammars-v4/llvm-ir" ]; then
    mv "${OUTPUT_DIR}/grammars-v4/llvm-ir/"*.h "${OUTPUT_DIR}/" 2>/dev/null || true
    mv "${OUTPUT_DIR}/grammars-v4/llvm-ir/"*.cpp "${OUTPUT_DIR}/" 2>/dev/null || true
    mv "${OUTPUT_DIR}/grammars-v4/llvm-ir/"*.interp "${OUTPUT_DIR}/" 2>/dev/null || true
    mv "${OUTPUT_DIR}/grammars-v4/llvm-ir/"*.tokens "${OUTPUT_DIR}/" 2>/dev/null || true
    rm -rf "${OUTPUT_DIR}/grammars-v4"
fi

echo ""
echo "Parser generation complete!"
echo "Generated files:"
ls -1 "${OUTPUT_DIR}/LLVMIR"*.h "${OUTPUT_DIR}/LLVMIR"*.cpp 2>/dev/null
