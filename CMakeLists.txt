cmake_minimum_required(VERSION 3.16)
project(llvm-i286 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# ANTLR4 C++ Runtime
# Build from in-tree source
set(ANTLR4_RUNTIME_ROOT "${CMAKE_SOURCE_DIR}/antlr4/runtime/Cpp")
set(ANTLR4_RUNTIME_INCLUDE "${ANTLR4_RUNTIME_ROOT}/runtime/src")
set(ANTLR4_RUNTIME_BUILD_DIR "${ANTLR4_RUNTIME_ROOT}/build")

# ANTLR4 Tool (Java) for generating parsers
# Path to the complete jar (built from antlr4/tool)
set(ANTLR4_TOOL_JAR "${CMAKE_SOURCE_DIR}/antlr4/tool/target/antlr4-4.13.3-SNAPSHOT-complete.jar")

# Generated parser sources directory
set(GENERATED_PARSER_DIR "${CMAKE_SOURCE_DIR}/antlr")

# Check if ANTLR4 tool is available
if(EXISTS "${ANTLR4_TOOL_JAR}")
    set(ANTLR4_TOOL_AVAILABLE TRUE)
    message(STATUS "ANTLR4 Tool: available")
else()
    set(ANTLR4_TOOL_AVAILABLE FALSE)
    message(STATUS "ANTLR4 Tool: NOT available (will use stub parsers)")
    message(STATUS "To build ANTLR4 tool: cd antlr4/tool && mvn package")
endif()

# Function to generate ANTLR4 parser from grammar
function(generate_antlr_grammar GRAMMAR_FILE OUTPUT_DIR)
    get_filename_component(GRAMMAR_NAME ${GRAMMAR_FILE} NAME_WE)
    
    set(GENERATED_SOURCES
        "${OUTPUT_DIR}/${GRAMMAR_NAME}Lexer.h"
        "${OUTPUT_DIR}/${GRAMMAR_NAME}Lexer.cpp"
        "${OUTPUT_DIR}/${GRAMMAR_NAME}Parser.h"
        "${OUTPUT_DIR}/${GRAMMAR_NAME}Parser.cpp"
        "${OUTPUT_DIR}/${GRAMMAR_NAME}BaseVisitor.h"
        "${OUTPUT_DIR}/${GRAMMAR_NAME}BaseVisitor.cpp"
        "${OUTPUT_DIR}/${GRAMMAR_NAME}Visitor.h"
        "${OUTPUT_DIR}/${GRAMMAR_NAME}Visitor.cpp"
    )
    
    if(NOT ANTLR4_TOOL_AVAILABLE)
        # Tool not available, use existing generated files
        if(NOT EXISTS "${OUTPUT_DIR}/${GRAMMAR_NAME}Parser.cpp")
            message(FATAL_ERROR "ANTLR4 tool not available and generated parser files not found in ${OUTPUT_DIR}")
        endif()
        message(STATUS "Using existing generated parser files from ${OUTPUT_DIR}")
    else()
        # Tool available, regenerate parser
        # Copy grammar to output dir so ANTLR doesn't create subdirectories
        set(GRAMMAR_COPY "${OUTPUT_DIR}/${GRAMMAR_NAME}.g4")
        add_custom_command(
            OUTPUT ${GENERATED_SOURCES}
            COMMAND ${CMAKE_COMMAND} -E copy ${GRAMMAR_FILE} ${GRAMMAR_COPY}
            COMMAND java -jar ${ANTLR4_TOOL_JAR}
                    -Dlanguage=Cpp
                    -o ${OUTPUT_DIR}
                    -package antlr::generated
                    -visitor
                    -no-listener
                    ${GRAMMAR_COPY}
            COMMAND ${CMAKE_COMMAND} -E remove ${GRAMMAR_COPY}
            DEPENDS ${GRAMMAR_FILE}
            COMMENT "Generating ANTLR4 parser for ${GRAMMAR_NAME}"
            VERBATIM
        )
    endif()
    
    # Create an interface library for the generated parser
    add_library(${GRAMMAR_NAME}_parser STATIC ${GENERATED_SOURCES})
    target_include_directories(${GRAMMAR_NAME}_parser PUBLIC ${OUTPUT_DIR} ${ANTLR4_RUNTIME_INCLUDE})
    target_link_libraries(${GRAMMAR_NAME}_parser PUBLIC ${ANTLR4_RUNTIME_LIB_TARGET})
endfunction()

# ANTLR4 Runtime Library - build from source
# Disable tests and shared library, only build static
set(ANTLR_BUILD_CPP_TESTS OFF CACHE BOOL "Disable ANTLR4 tests" FORCE)
set(ANTLR_BUILD_SHARED OFF CACHE BOOL "Disable shared library" FORCE)
set(ANTLR_BUILD_STATIC ON CACHE BOOL "Enable static library" FORCE)

# Suppress FetchContent warnings
set(FETCHCONTENT_QUIET ON CACHE BOOL "" FORCE)

add_subdirectory(${ANTLR4_RUNTIME_ROOT} ${ANTLR4_RUNTIME_BUILD_DIR})

# The static library target is named antlr4_static
set(ANTLR4_RUNTIME_LIB_TARGET antlr4_static)

# Generate LLVM IR Parser from local grammar copy
# Grammar is at src/ir/grammar/LLVMIR.g4 (copied from grammars-v4/llvm-ir/)
# Regenerate parser: java -jar ${ANTLR4_TOOL_JAR} -Dlanguage=Cpp -o ${GENERATED_PARSER_DIR} -package antlr::generated -visitor -no-listener ${CMAKE_SOURCE_DIR}/src/ir/grammar/LLVMIR.g4
generate_antlr_grammar(
    "${CMAKE_SOURCE_DIR}/src/ir/grammar/LLVMIR.g4"
    "${GENERATED_PARSER_DIR}"
)

# LLVM IR Parser and AST
add_library(ir_parser
    src/ir/IrParser.cpp
    src/ir/IrAst.cpp
    src/ir/IrVisitor.cpp
)
target_include_directories(ir_parser PUBLIC ${CMAKE_SOURCE_DIR}/src ${GENERATED_PARSER_DIR} ${ANTLR4_RUNTIME_INCLUDE})
target_link_libraries(ir_parser PUBLIC LLVMIR_parser ${ANTLR4_RUNTIME_LIB_TARGET})

# 286 Target Definitions
add_library(target_286
    src/target/x86_286.cpp
    src/target/CallingConv.cpp
)
target_include_directories(target_286 PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Code Generator
add_library(codegen
    src/codegen/CodeGen.cpp
    src/codegen/InstructionSelect.cpp
    src/codegen/InstructionSelectUtils.cpp
    src/codegen/StackFrame.cpp
    src/codegen/FunctionAnalysis.cpp
    src/codegen/ControlFlowOps.cpp
    src/codegen/ArithmeticOps.cpp
    src/codegen/BitwiseOps.cpp
    src/codegen/MemoryOps.cpp
    src/codegen/RegisterAlloc.cpp
    src/codegen/Emitter.cpp
    src/codegen/Addressing.cpp
    src/codegen/CallOps.cpp
    src/codegen/ConversionOps.cpp
    src/codegen/FpuOps.cpp
)
target_include_directories(codegen PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(codegen PUBLIC ir_parser target_286)

# Main executable
add_executable(llvm-i286 src/main.cpp)
target_link_libraries(llvm-i286 PRIVATE codegen)

# Test targets
enable_testing()

# Integration tests
add_test(
    NAME integration_test
    COMMAND ${CMAKE_SOURCE_DIR}/tests/run_tests.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Print configuration summary
message(STATUS "=== LLVM i286 Code Generator Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "ANTLR4 Runtime: building from source (${ANTLR4_RUNTIME_ROOT})")
message(STATUS "ANTLR4 Tool Available: ${ANTLR4_TOOL_AVAILABLE}")
message(STATUS "Generated parsers: ${GENERATED_PARSER_DIR}")
message(STATUS "==============================================")
