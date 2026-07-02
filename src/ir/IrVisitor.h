// LLVM IR Visitor
// ANTLR4 visitor implementation to build IR AST from parse tree

#ifndef LLVM_I286_IR_VISITOR_H
#define LLVM_I286_IR_VISITOR_H

#include <memory>
#include <string>
#include <map>

#include "ir/IrAst.h"
#include "LLVMIRBaseVisitor.h"

using namespace antlr::generated;

namespace llvm_i286 {
namespace ir {

class IrVisitor : public LLVMIRBaseVisitor {
public:
    IrVisitor();
    ~IrVisitor() override;
    
    std::unique_ptr<Module> visitModule(LLVMIRParser::CompilationUnitContext *ctx);
    
    std::any visitFuncDef(LLVMIRParser::FuncDefContext *ctx) override;
    std::any visitFuncDecl(LLVMIRParser::FuncDeclContext *ctx) override;
    std::any visitBasicBlock(LLVMIRParser::BasicBlockContext *ctx) override;
    
    std::unique_ptr<Type> parseType(LLVMIRParser::TypeContext *ctx);
    std::unique_ptr<Type> parseIntType(LLVMIRParser::IntTypeContext *ctx);
    std::unique_ptr<Value> parseValue(LLVMIRParser::ValueContext *ctx);
    std::unique_ptr<Value> parseConstant(LLVMIRParser::ConstantContext *ctx);
    std::unique_ptr<Instruction> parseInstruction(LLVMIRParser::ValueInstructionContext *ctx);
    std::unique_ptr<Instruction> parseTerminator(LLVMIRParser::TerminatorContext *ctx);
    
private:
    std::unique_ptr<Module> currentModule;
    std::map<std::string, std::unique_ptr<Type>> typeAliases;
    
    // Accumulator for basic block being built
    std::unique_ptr<BasicBlock> currentBB;
};

} // namespace ir
} // namespace llvm_i286

#endif // LLVM_I286_IR_VISITOR_H
