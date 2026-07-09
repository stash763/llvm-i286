// Function Analysis - Pre-pass for stack frame layout
//
// This file contains the FunctionAnalysis class, which performs a pre-pass
// over the IR function before instruction lowering begins. It collects:
// - Parameter information (names, types, register assignments)
// - Alloca slots (local variable declarations)
// - Cross-block liveness (which vregs survive across basic blocks)
// - Max temp space needed per block
//
// This information is used to populate a StackFrame before lowering.

#ifndef LLVM_I286_CODEGEN_FUNCTION_ANALYSIS_H
#define LLVM_I286_CODEGEN_FUNCTION_ANALYSIS_H

#include "codegen/StackFrame.h"

#include "ir/IrAst.h"

#include <string>
#include <vector>
#include <map>
#include <set>

namespace llvm_i286 {
namespace codegen {

class FunctionAnalysis {
public:
    // Analyze the function and populate the stack frame
    // func: the IR function to analyze
    // frame: the StackFrame to populate
    // funcParamBitWidths: map of function name to parameter bit widths (from codegen)
    static void analyze(const ir::Function& func, StackFrame& frame,
                        const std::map<std::string, std::vector<int>>& funcParamBitWidths);

private:
    // Per-basic-block analysis results
    struct BlockInfo {
        std::string label;
        std::set<std::string> defined;    // vregs defined in this block (result names)
        std::set<std::string> used;        // vregs used in this block (operand names)
        int tempSpaceNeeded = 0;          // bytes of temp space needed for this block
        int maxTempSpace = 0;             // max temp space across all blocks
    };

    // Analyze parameters
    static void analyzeParams(const ir::Function& func, StackFrame& frame,
                              const std::map<std::string, std::vector<int>>& funcParamBitWidths);

    // Analyze basic blocks (collect defined/used sets, estimate temp space)
    static void analyzeBlocks(const ir::Function& func, StackFrame& frame,
                              std::vector<BlockInfo>& blockInfos);

    // Compute cross-block liveness
    // A vreg is "cross-block" if it's defined in one block and used in another
    static void computeLiveness(const std::vector<BlockInfo>& blockInfos,
                                StackFrame& frame);

    // Compute max temp space needed across all blocks
    static void computeMaxTempSpace(const std::vector<BlockInfo>& blockInfos,
                                     StackFrame& frame);
};

} // namespace codegen
} // namespace llvm_i286

#endif // LLVM_I286_CODEGEN_FUNCTION_ANALYSIS_H
