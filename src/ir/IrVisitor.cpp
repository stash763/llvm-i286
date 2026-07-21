// LLVM IR Visitor Implementation
// Walks ANTLR4 parse tree to build IR AST

#include "ir/IrVisitor.h"

#include <iostream>
#include <cassert>
#include <sstream>

using namespace antlr::generated;

// WORKAROUND for ANTLR4 lexer greediness.
// The lexer rule is: IntType : 'i' DecimalDigit+
// This is greedy, so for IR like "i32 2147483647" the lexer tokenizes
// "i322147483647" as a single IntType token. The grammar expects
// typeValue to be (firstClassType, value) but the lexer has already
// merged them. This shows up when clang emits things like
// "sub nsw i32 2147483647, %37" where the space between i32 and the
// constant gets consumed into the IntType token.
static int parseIntWidth(const std::string& tokenText) {
    std::string numStr = tokenText.substr(1); // strip leading 'i'
    int bitWidth = 0;
    try {
        bitWidth = std::stoi(numStr);
    } catch (...) {
        return 32;
    }
    if (bitWidth > 2048) {
        // Tokenized as "i<width><value>" by lexer; try to split.
        // Look for a known valid width prefix: i1, i8, i16, i32, i64, i128, etc.
        static const int knownWidths[] = {128, 64, 32, 16, 8, 1};
        for (int w : knownWidths) {
            std::string prefix = std::to_string(w);
            if (numStr.substr(0, prefix.size()) == prefix) {
                return w;
            }
        }
    }
    return bitWidth;
}

namespace llvm_i286 {
namespace ir {

IrVisitor::IrVisitor() {}

IrVisitor::~IrVisitor() = default;

std::unique_ptr<Module> IrVisitor::visitModule(LLVMIRParser::CompilationUnitContext *ctx) {
    currentModule = std::make_unique<Module>();
    typeAliases.clear();
    
    for (auto *entity : ctx->topLevelEntity()) {
        if (entity->funcDef()) {
            visitFuncDef(entity->funcDef());
        } else if (entity->funcDecl()) {
            visitFuncDecl(entity->funcDecl());
        } else if (entity->targetDef()) {
            auto *targetDef = entity->targetDef();
            if (targetDef->targetTriple()) {
                currentModule->targetTriple = targetDef->targetTriple()->StringLit()->getText();
                if (currentModule->targetTriple.size() >= 2 && 
                    currentModule->targetTriple.front() == '"' && 
                    currentModule->targetTriple.back() == '"') {
                    currentModule->targetTriple = currentModule->targetTriple.substr(1, currentModule->targetTriple.size() - 2);
                }
            } else if (targetDef->targetDataLayout()) {
                currentModule->dataLayout = targetDef->targetDataLayout()->StringLit()->getText();
                if (currentModule->dataLayout.size() >= 2 && 
                    currentModule->dataLayout.front() == '"' && 
                    currentModule->dataLayout.back() == '"') {
                    currentModule->dataLayout = currentModule->dataLayout.substr(1, currentModule->dataLayout.size() - 2);
                }
            }
        } else if (entity->sourceFilename()) {
            currentModule->sourceFilename = entity->sourceFilename()->StringLit()->getText();
            if (currentModule->sourceFilename.size() >= 2 && 
                currentModule->sourceFilename.front() == '"' && 
                currentModule->sourceFilename.back() == '"') {
                currentModule->sourceFilename = currentModule->sourceFilename.substr(1, currentModule->sourceFilename.size() - 2);
            }
         } else if (entity->globalDef()) {
            auto *globalDef = entity->globalDef();
            auto gv = std::make_unique<ir::GlobalVariable>();
            
            // Get global variable name
            if (globalDef->GlobalIdent()) {
                std::string name = globalDef->GlobalIdent()->getText();
                if (!name.empty() && name[0] == '@') name = name.substr(1);
                gv->name = name;
            }
            
            // Get type
            if (globalDef->type()) {
                gv->type = parseType(globalDef->type());
            }
            
            // Check if constant
            if (globalDef->constant()) {
                gv->isConstant = true;
            }
            
            // Get initializer text and kind (for constant globals)
            if (globalDef->constant()) {
                auto *constant = globalDef->constant();
                gv->initializer = std::make_unique<ir::Value>();
                gv->initializer->text = constant->getText();
                
                // Determine the kind of constant
                if (constant->nullConst()) {
                    gv->initializer->kind = ir::ValueKind::ConstantNull;
                } else if (constant->zeroInitializerConst()) {
                    gv->initializer->kind = ir::ValueKind::ConstantZeroInitializer;
                } else if (constant->undefConst()) {
                    gv->initializer->kind = ir::ValueKind::ConstantUndef;
                } else if (constant->GlobalIdent()) {
                    gv->initializer->kind = ir::ValueKind::GlobalReference;
                } else {
                    gv->initializer->kind = ir::ValueKind::ConstantInt;
                }
            }
            
            currentModule->globals.push_back(std::move(gv));
        } else if (entity->globalDecl()) {
            // Non-constant global declaration (e.g., "@x = external constant ptr @foo")
            auto *globalDecl = entity->globalDecl();
            auto gv = std::make_unique<ir::GlobalVariable>();
            
            // Get global variable name
            if (globalDecl->GlobalIdent()) {
                std::string name = globalDecl->GlobalIdent()->getText();
                if (!name.empty() && name[0] == '@') name = name.substr(1);
                gv->name = name;
            }
            
            // Get type
            if (globalDecl->type()) {
                gv->type = parseType(globalDecl->type());
            }
            
            // Global declarations with externalLinkage are external
            gv->linkage = "external";
            
            // Check if the raw text contains "constant" keyword
            // (grammar doesn't have constant in globalDecl, but ANTLR error recovery
            //  allows parsing "@x = external constant ptr" through globalDecl)
            {
                std::string declText = globalDecl->getText();
                gv->isConstant = declText.find(" constant ") != std::string::npos;
            }
            
            // Try to extract initializer from the text
            // The text looks like: "@__stdout_used = hidden global ptr @stdout_file, align 4"
            // We need to extract "@stdout_file"
            {
                std::string declText = globalDecl->getText();
                // Find the pattern " @identifier" which is the initializer
                // Look for "@" that comes after the type declaration
                // Simple approach: find last "@@" in the text
                size_t atPos = declText.rfind('@');
                if (atPos != std::string::npos && atPos > 0) {
                    // Extract from @ to end, stopping at comma or space
                    std::string ident;
                    for (size_t i = atPos; i < declText.size(); i++) {
                        char c = declText[i];
                        if (c == ',' || c == ' ' || c == ')' || c == ']' || c == '\n' || c == '\r') break;
                        ident += c;
                    }
                    if (ident.size() > 1) { // Should be at least "@x"
                        gv->initializer = std::make_unique<ir::Value>();
                        gv->initializer->text = ident;
                        gv->initializer->kind = ir::ValueKind::GlobalReference;
                    }
                }
            }
            
            currentModule->globals.push_back(std::move(gv));
        } else if (entity->indirectSymbolDef()) {
            // Handle alias declarations: @alias = alias ... @target
            visitIndirectSymbolDef(entity->indirectSymbolDef());
        } else if (entity->typeDef()) {
            // Handle type definitions: %struct.NAME = type { i32, ptr, ... }
            auto* td = entity->typeDef();
            if (td->LocalIdent() && td->type()) {
                std::string typeName = td->LocalIdent()->getText();
                auto type = parseType(td->type());
                currentModule->typeDefinitions[typeName] = std::move(type);
            }
        } else if (entity->moduleAsm()) {
            // Handle module asm: collect raw assembly strings
            auto* ma = entity->moduleAsm();
            if (ma->StringLit()) {
                std::string asmStr = ma->StringLit()->getText();
                // Strip surrounding quotes
                if (asmStr.size() >= 2 && asmStr.front() == '"' && asmStr.back() == '"') {
                    asmStr = asmStr.substr(1, asmStr.size() - 2);
                }
                currentModule->moduleAsmStrings.push_back(asmStr);
            }
        }
    }

    // Post-processing pass: populate OperandRef from raw name strings
    // This is the single source of truth for operand classification,
    // replacing 180+ string prefix checks scattered across codegen handlers.
    for (auto& func : currentModule->functions) {
        for (auto& bb : func->basicBlocks) {
            for (auto& inst : bb->instructions) {
                // Classify operands
                for (auto& op : inst->operands) {
                    if (!op.name.empty()) {
                        op.ref = classifyOperand(op.name);
                    }
                }
                // Classify call args
                for (size_t i = 0; i < inst->callArgs.size(); i++) {
                    if (i < inst->callArgRefs.size()) continue;
                    inst->callArgRefs.push_back(classifyOperand(inst->callArgs[i]));
                }
                // Classify callee
                if (!inst->calleeName.empty() && inst->calleeRef.kind == OperandRef::Kind::Unknown) {
                    inst->calleeRef = classifyOperand(inst->calleeName);
                }
                // Classify phi increments
                for (auto& inc : inst->phiIncrements) {
                    if (!inc.value.empty() && inc.valueRef.kind == OperandRef::Kind::Unknown) {
                        inc.valueRef = classifyOperand(inc.value);
                    }
                }
                // Classify switch cases
                for (auto& sc : inst->switchCases) {
                    if (!sc.value.empty() && sc.valueRef.kind == OperandRef::Kind::Unknown) {
                        sc.valueRef = classifyOperand(sc.value);
                    }
                }
            }
            // Also classify terminator operands if present
            if (bb->terminator) {
                for (auto& op : bb->terminator->operands) {
                    if (!op.name.empty()) {
                        op.ref = classifyOperand(op.name);
                    }
                }
            }
        }
    }

    return std::move(currentModule);
}

std::any IrVisitor::visitIndirectSymbolDef(LLVMIRParser::IndirectSymbolDefContext *ctx) {
    // Parse alias name
    std::string aliasName;
    if (ctx->GlobalIdent()) {
        aliasName = ctx->GlobalIdent()->getText();
        if (!aliasName.empty() && aliasName[0] == '@') {
            aliasName = aliasName.substr(1);
        }
    }
    
    // Parse target name from indirectSymbol
    // The indirectSymbol can be typeConst, bitCastExpr, etc.
    // For simple aliases, it's usually a typeConst with a GlobalIdent reference
    std::string targetName;
    if (ctx->indirectSymbol()) {
        // Get the text of the indirectSymbol and look for @name pattern
        std::string symbolText = ctx->indirectSymbol()->getText();
        // Find the last @ in the text (the target function/global name)
        size_t atPos = symbolText.rfind('@');
        if (atPos != std::string::npos) {
            targetName = symbolText.substr(atPos + 1);
            // Remove any trailing characters (like function signature)
            size_t parenPos = targetName.find('(');
            if (parenPos != std::string::npos) {
                targetName = targetName.substr(0, parenPos);
            }
        }
    }
    
    if (!aliasName.empty() && !targetName.empty()) {
        currentModule->aliases[aliasName] = targetName;
    }
    
    return std::any();
}

std::any IrVisitor::visitFuncDef(LLVMIRParser::FuncDefContext *ctx) {
    auto func = std::make_unique<Function>();
    func->isDeclaration = false;
    
    auto *header = ctx->funcHeader();
    if (header) {
        if (header->GlobalIdent()) {
            func->name = header->GlobalIdent()->getText();
            if (!func->name.empty() && func->name[0] == '@') {
                func->name = func->name.substr(1);
            }
        }
        
        if (header->type()) {
            func->returnType = parseType(header->type());
        }
        
        if (header->params()) {
            for (auto *param : header->params()->param()) {
                auto p = std::make_unique<Parameter>();
                if (param->type()) {
                    p->type = parseType(param->type());
                }
                         if (param->LocalIdent()) {
                    p->name = param->LocalIdent()->getText();
                    // Preserve % prefix for vreg lookup
                }
                func->params.push_back(std::move(p));
            }
        }
        
        if (header->linkage()) {
            if (header->linkage()->externalLinkage()) {
                func->linkage = "external";
            } else if (header->linkage()->internalLinkage()) {
                func->linkage = "internal";
            }
        }
    }
    
    // Visit basic blocks
    if (ctx->funcBody()) {
        for (auto *bbCtx : ctx->funcBody()->basicBlock()) {
            visitBasicBlock(bbCtx);
            if (currentBB) {
                func->basicBlocks.push_back(std::move(currentBB));
            }
        }
    }
    
    currentModule->functions.push_back(std::move(func));
    return std::any();
}

std::any IrVisitor::visitFuncDecl(LLVMIRParser::FuncDeclContext *ctx) {
    auto func = std::make_unique<Function>();
    func->isDeclaration = true;
    
    auto *header = ctx->funcHeader();
    if (header) {
        if (header->GlobalIdent()) {
            func->name = header->GlobalIdent()->getText();
            if (!func->name.empty() && func->name[0] == '@') {
                func->name = func->name.substr(1);
            }
        }
        
        if (header->type()) {
            func->returnType = parseType(header->type());
        }
        
        if (header->params()) {
            for (auto *param : header->params()->param()) {
                auto p = std::make_unique<Parameter>();
                if (param->type()) {
                    p->type = parseType(param->type());
                }
                func->params.push_back(std::move(p));
            }
        }
    }
    
    currentModule->functions.push_back(std::move(func));
    return std::any();
}

std::any IrVisitor::visitBasicBlock(LLVMIRParser::BasicBlockContext *ctx) {
    currentBB = std::make_unique<BasicBlock>();
    
    if (ctx->LabelIdent()) {
        currentBB->label = ctx->LabelIdent()->getText();
        if (!currentBB->label.empty() && currentBB->label.back() == ':') {
            currentBB->label = currentBB->label.substr(0, currentBB->label.size() - 1);
        }
    }
    
    for (auto *inst : ctx->instruction()) {
        if (inst->localDefInst()) {
                       auto *localDef = inst->localDefInst();
            if (localDef->LocalIdent() && localDef->valueInstruction()) {
                auto instPtr = parseInstruction(localDef->valueInstruction());
                if (instPtr) {
                    instPtr->resultName = localDef->LocalIdent()->getText();
                    // Preserve % prefix for vreg lookup
                }
                currentBB->instructions.push_back(std::move(instPtr));
            }
        } else if (inst->valueInstruction()) {
            auto instPtr = parseInstruction(inst->valueInstruction());
            currentBB->instructions.push_back(std::move(instPtr));
                 } else if (inst->storeInst()) {
            // Store instruction is separate from value instructions
            auto instPtr = std::make_unique<Instruction>();
            instPtr->opcode = ir::Opcode::Store;
            
            auto *storeCtx = inst->storeInst();
            // store typeValue, typeValue
            if (storeCtx->typeValue().size() >= 2) {
                // Parse the type of the value being stored
                if (storeCtx->typeValue(0)->firstClassType()) {
                    std::string typeText = storeCtx->typeValue(0)->firstClassType()->getText();
                    if (typeText.substr(0, 1) == "i") {
                        instPtr->resultType = Type::makeInteger(parseIntWidth(typeText));
                    } else if (typeText == "ptr") {
                        instPtr->resultType = std::make_unique<Type>();
                        instPtr->resultType->kind = TypeKind::Pointer;
                        instPtr->resultType->elementType = Type::makeVoid();
                        instPtr->resultType->bitWidth = 32;
                    }
                }
                // Value to store
                Operand valOp;
                auto *val = storeCtx->typeValue(0)->value();
                if (val) {
                    valOp.name = val->getText(); // Preserve % prefix for vreg lookup
                }
                instPtr->operands.push_back(std::move(valOp));

                // Pointer to store to
                Operand ptrOp;
                auto *ptr = storeCtx->typeValue(1)->value();
                if (ptr) {
                    ptrOp.name = ptr->getText(); // Preserve % prefix for vreg lookup
                }
                instPtr->operands.push_back(std::move(ptrOp));
            }
            
            currentBB->instructions.push_back(std::move(instPtr));
        }
    }
    
    if (ctx->terminator()) {
        currentBB->terminator = parseTerminator(ctx->terminator());
    }
    
    // Don't move currentBB here - let the caller (visitFuncDef) move it
    return std::any();
}

std::unique_ptr<Type> IrVisitor::parseType(LLVMIRParser::TypeContext *ctx) {
    if (ctx->intType()) {
        return parseIntType(ctx->intType());
    } else if (ctx->getText() == "void") {
        return Type::makeVoid();
    } else if (ctx->opaquePointerType()) {
        auto type = std::make_unique<Type>();
        type->kind = TypeKind::Pointer;
        type->elementType = Type::makeVoid();
        type->bitWidth = 32;  // 32-bit pointers (i386 target)
        return type;
    } else if (ctx->structType()) {
        auto type = std::make_unique<Type>();
        type->kind = TypeKind::Struct;
        for (auto* fieldCtx : ctx->structType()->type()) {
            type->structElements.push_back(parseType(fieldCtx));
        }
        return type;
    } else if (ctx->namedType()) {
        auto type = std::make_unique<Type>();
        type->kind = TypeKind::Named;
        std::string name = ctx->namedType()->LocalIdent()->getText();
        type->name = name;
        return type;
    } else if (ctx->arrayType()) {
        auto type = std::make_unique<Type>();
        type->kind = TypeKind::Array;
        type->elementType = parseType(ctx->arrayType()->type());
        std::string numStr = ctx->arrayType()->IntLit()->getText();
        type->numElements = std::stoi(numStr);
        return type;
    }
    return Type::makeVoid();
}

std::unique_ptr<Type> IrVisitor::parseIntType(LLVMIRParser::IntTypeContext *ctx) {
    auto type = std::make_unique<Type>();
    type->kind = TypeKind::Integer;
    
    std::string text = ctx->IntType()->getText();
    if (text.size() > 1 && text[0] == 'i') {
        // WORKAROUND for ANTLR4 lexer greediness.
        // The lexer rule is: IntType : 'i' DecimalDigit+
        // This is greedy, so for IR like "i32 2147483647" the lexer tokenizes
        // "i322147483647" as a single IntType token. The grammar expects
        // typeValue to be (firstClassType, value) but the lexer has already
        // merged them. This shows up with clang emitting things like
        // "sub nsw i32 2147483647, %37" where the space between i32 and the
        // constant gets consumed into the IntType token.
        // We detect this by checking if the bit width is unreasonably large
        // (anything above i2048 is clearly a mis-tokenization).
        int bitWidth = 0;
        std::string numStr = text.substr(1);
        try {
            bitWidth = std::stoi(numStr);
        } catch (...) {
            bitWidth = 32;
        }
        if (bitWidth > 2048) {
            // Tokenized as "i<width><value>" by lexer; try to split.
            // Look for a known valid width prefix: i1, i8, i16, i32, i64, i128, etc.
            static const int knownWidths[] = {128, 64, 32, 16, 8, 1};
            for (int w : knownWidths) {
                std::string prefix = std::to_string(w);
                if (numStr.substr(0, prefix.size()) == prefix) {
                    bitWidth = w;
                    break;
                }
            }
        }
        type->bitWidth = bitWidth;
    } else {
        type->bitWidth = 32;
    }
    
    return type;
}

std::unique_ptr<Value> IrVisitor::parseValue(LLVMIRParser::ValueContext *ctx) {
    if (ctx->constant()) {
        return parseConstant(ctx->constant());
    } else if (ctx->LocalIdent()) {
        return Value::makeLocalRef(ctx->LocalIdent()->getText());
    }
    return Value::makeInt(0, 32);
}

std::unique_ptr<Value> IrVisitor::parseConstant(LLVMIRParser::ConstantContext *ctx) {
    if (ctx->intConst()) {
        std::string text = ctx->intConst()->IntLit()->getText();
        int64_t val = 0;
        try {
            val = std::stoll(text);
        } catch (...) {
            val = 0;
        }
        return Value::makeInt(val, 32);
    } else if (ctx->nullConst()) {
        auto v = Value::makeInt(0, 32);
        v->kind = ValueKind::ConstantNull;
        return v;
    } else if (ctx->boolConst()) {
        auto v = Value::makeInt(ctx->boolConst()->getText() == "true" ? 1 : 0, 1);
        v->kind = ctx->boolConst()->getText() == "true" ? ValueKind::ConstantTrue : ValueKind::ConstantFalse;
        return v;
    }
    return Value::makeInt(0, 32);
}

std::unique_ptr<Instruction> IrVisitor::parseInstruction(LLVMIRParser::ValueInstructionContext *ctx) {
    auto inst = std::make_unique<Instruction>();
    
    // Parse operands for add instruction
    if (ctx->addInst()) {
        inst->opcode = Opcode::Add;
        auto *addCtx = ctx->addInst();
        // add has: typeValue, value
        if (addCtx->typeValue() && addCtx->value()) {
            // Extract type from typeValue (e.g., "i32" from "i32 %val")
            std::string typeText = addCtx->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                // Parse the type (e.g., "i32")
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            
            Operand op1;
            auto *val = addCtx->typeValue()->value();
            if (val) {
                op1.name = val->getText(); // Preserve % prefix for vreg lookup
            }
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            op2.name = addCtx->value()->getText(); // Preserve % prefix for vreg lookup
            inst->operands.push_back(std::move(op2));
        }
    }
    // Parse operands for sub instruction
    else if (ctx->subInst()) {
        inst->opcode = Opcode::Sub;
        auto *subCtx = ctx->subInst();
        if (subCtx->typeValue() && subCtx->value()) {
            // Extract type
            std::string typeText = subCtx->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            
            Operand op1;
            op1.name = subCtx->typeValue()->value() ? subCtx->typeValue()->value()->getText() : ""; // Preserve % prefix
            inst->operands.push_back(std::move(op1));
            
                                 
            
            Operand op2;
            op2.name = subCtx->value()->getText(); // Preserve % prefix
            inst->operands.push_back(std::move(op2));
        }
    }
         // Parse operands for mul instruction
    else if (ctx->mulInst()) {
        inst->opcode = Opcode::Mul;
        auto *mulCtx = ctx->mulInst();
        if (mulCtx->typeValue() && mulCtx->value()) {
            // Extract type
            std::string typeText = mulCtx->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            
            Operand op1;
            op1.name = mulCtx->typeValue()->value() ? mulCtx->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            op2.name = mulCtx->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    // Parse operands for sdiv instruction
    else if (ctx->sDivInst()) {
        inst->opcode = Opcode::SDiv;
        auto *divCtx = ctx->sDivInst();
        if (divCtx->typeValue() && divCtx->value()) {
            std::string typeText = divCtx->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            
            Operand op1;
            op1.name = divCtx->typeValue()->value() ? divCtx->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            op2.name = divCtx->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    // Parse operands for udiv instruction
    else if (ctx->uDivInst()) {
        inst->opcode = Opcode::UDiv;
        auto *divCtx = ctx->uDivInst();
        if (divCtx->typeValue() && divCtx->value()) {
            std::string typeText = divCtx->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            
            Operand op1;
            op1.name = divCtx->typeValue()->value() ? divCtx->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            op2.name = divCtx->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    // Parse operands for srem instruction
    else if (ctx->sRemInst()) {
        inst->opcode = Opcode::SRem;
        auto *remCtx = ctx->sRemInst();
        if (remCtx->typeValue() && remCtx->value()) {
            std::string typeText = remCtx->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            
            Operand op1;
            op1.name = remCtx->typeValue()->value() ? remCtx->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            op2.name = remCtx->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    // Parse operands for urem instruction
    else if (ctx->uRemInst()) {
        inst->opcode = Opcode::URem;
        auto *remCtx = ctx->uRemInst();
        if (remCtx->typeValue() && remCtx->value()) {
            std::string typeText = remCtx->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            
            Operand op1;
            op1.name = remCtx->typeValue()->value() ? remCtx->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            op2.name = remCtx->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    // Parse alloca instruction
    else if (ctx->allocaInst()) {
        inst->opcode = Opcode::Alloca;
        auto *allocaCtx = ctx->allocaInst();
        // Get the allocated type to determine size
        if (allocaCtx->type()) {
            Operand sizeOp;
            // Parse type to get bit width
            auto *typeCtx = allocaCtx->type();
            std::string typeText = typeCtx->getText();
            // Check if it's an array type like "[16 x i8]"
            if (typeText.find("[") != std::string::npos && typeText.find("x") != std::string::npos) {
                // Parse array dimensions: [count x elementType]
                // Extract count (between [ and x)
                size_t bracketPos = typeText.find('[');
                size_t xPos = typeText.find('x');
                if (bracketPos != std::string::npos && xPos != std::string::npos && bracketPos < xPos) {
                    std::string countStr = typeText.substr(bracketPos + 1, xPos - bracketPos - 1);
                    // Trim whitespace
                    countStr.erase(0, countStr.find_first_not_of(" \t"));
                    countStr.erase(countStr.find_last_not_of(" \t") + 1);
                    int count = 0;
                    try { count = std::stoi(countStr); } catch (...) { count = 1; }
                    // Extract element type bit width (after x)
                    size_t afterX = xPos + 1;
                    // Skip whitespace after x
                    while (afterX < typeText.size() && (typeText[afterX] == ' ' || typeText[afterX] == '\t')) afterX++;
                    // Check if element type starts with 'i' (like i8, i32, etc.)
                    int elemBitWidth = 8; // default to i8 (1 byte)
                    if (afterX < typeText.size() && typeText[afterX] == 'i') {
                        size_t numStart = afterX + 1;
                        std::string numStr;
                        while (numStart < typeText.size() && std::isdigit(typeText[numStart])) {
                            numStr += typeText[numStart];
                            numStart++;
                        }
                        if (!numStr.empty()) {
                            try { elemBitWidth = std::stoi(numStr); } catch (...) { elemBitWidth = 8; }
                        }
                    }
                    // Total bit width = count * element bit width
                    int totalBitWidth = count * elemBitWidth;
                    // Ensure minimum 2 bytes (16 bits)
                    if (totalBitWidth < 16) totalBitWidth = 16;
                    sizeOp.name = std::to_string(totalBitWidth);
                } else {
                    sizeOp.name = "32"; // default
                }
            } else if (typeCtx->intType()) {
                std::string text = typeCtx->intType()->IntType()->getText();
                if (text.size() > 1 && text[0] == 'i') {
                    try {
                        sizeOp.name = text.substr(1); // Store bit width as string
                    } catch (...) {
                        sizeOp.name = "32";
                    }
                } else {
                    sizeOp.name = "32";
                }
            } else {
                sizeOp.name = "32"; // default for ptr types
            }
            inst->operands.push_back(std::move(sizeOp));
        }
    }
    // Parse load instruction
    else if (ctx->loadInst()) {
        inst->opcode = Opcode::Load;
        auto *loadCtx = ctx->loadInst();
             // load type, typeValue
        if (loadCtx->type()) {
            // Extract type (e.g., "i32")
            std::string typeText = loadCtx->type()->getText();
            if (typeText.substr(0, 1) == "i") {
                // Parse the type (e.g., "i32")
                inst->resultType = Type::makeInteger(parseIntWidth(typeText));
            } else if (typeText == "ptr") {
                // Pointer type (32-bit)
                inst->resultType = std::make_unique<Type>();
                inst->resultType->kind = TypeKind::Pointer;
                inst->resultType->elementType = Type::makeVoid();
                inst->resultType->bitWidth = 32;
            }
        }
        
        if (loadCtx->typeValue()) {
            Operand ptrOp;
            auto *ptr = loadCtx->typeValue()->value();
            if (ptr) {
                ptrOp.name = ptr->getText(); // Preserve % prefix for vreg lookup
            }
            // Set the operand type to match the result type (ptr)
            ptrOp.type = std::make_unique<Type>();
            ptrOp.type->kind = TypeKind::Pointer;
            ptrOp.type->elementType = Type::makeVoid();
            ptrOp.type->bitWidth = 32;
            inst->operands.push_back(std::move(ptrOp));
        }
    }
    // Parse operands for bitwise instructions
         else if (ctx->andInst()) {
        inst->opcode = Opcode::And;
        auto *ctx_ = ctx->andInst();
        if (ctx_->typeValue() && ctx_->value()) {
            std::string typeText = ctx_->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            Operand op1;
            op1.name = ctx_->typeValue()->value() ? ctx_->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            Operand op2;
            op2.name = ctx_->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    else if (ctx->orInst()) {
        inst->opcode = Opcode::Or;
        auto *ctx_ = ctx->orInst();
        if (ctx_->typeValue() && ctx_->value()) {
            std::string typeText = ctx_->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            Operand op1;
            op1.name = ctx_->typeValue()->value() ? ctx_->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            Operand op2;
            op2.name = ctx_->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    else if (ctx->xorInst()) {
        inst->opcode = Opcode::Xor;
        auto *ctx_ = ctx->xorInst();
        if (ctx_->typeValue() && ctx_->value()) {
            std::string typeText = ctx_->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            Operand op1;
            op1.name = ctx_->typeValue()->value() ? ctx_->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            Operand op2;
            op2.name = ctx_->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
         else if (ctx->shlInst()) {
        inst->opcode = Opcode::Shl;
        auto *ctx_ = ctx->shlInst();
        if (ctx_->typeValue() && ctx_->value()) {
            std::string typeText = ctx_->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            Operand op1;
            op1.name = ctx_->typeValue()->value() ? ctx_->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            Operand op2;
            op2.name = ctx_->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    else if (ctx->lShrInst()) {
        inst->opcode = Opcode::LShr;
        auto *ctx_ = ctx->lShrInst();
        if (ctx_->typeValue() && ctx_->value()) {
            std::string typeText = ctx_->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            Operand op1;
            op1.name = ctx_->typeValue()->value() ? ctx_->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            Operand op2;
            op2.name = ctx_->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    else if (ctx->aShrInst()) {
        inst->opcode = Opcode::AShr;
        auto *ctx_ = ctx->aShrInst();
        if (ctx_->typeValue() && ctx_->value()) {
            std::string typeText = ctx_->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            Operand op1;
            op1.name = ctx_->typeValue()->value() ? ctx_->typeValue()->value()->getText() : "";
            inst->operands.push_back(std::move(op1));
            Operand op2;
            op2.name = ctx_->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    else if (ctx->callInst()) {
        inst->opcode = Opcode::Call;
        auto *callCtx = ctx->callInst();
        
        // Parse return type (the type before the callee value)
        if (callCtx->type()) {
            std::string typeText = callCtx->type()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            } else if (typeText == "ptr") {
                // Pointer return type (32-bit)
                inst->resultType = std::make_unique<Type>();
                inst->resultType->kind = TypeKind::Pointer;
                inst->resultType->elementType = Type::makeVoid();
                inst->resultType->bitWidth = 32;
            }
        }
        
        // Parse callee name
        if (callCtx->value()) {
            // Check if callee is an inline asm expression
            if (callCtx->value()->inlineAsm()) {
                // Inline asm - will be resolved after args are parsed
                // (need to check if first arg is @global for load-address pattern)
            } else {
                std::string calleeText = callCtx->value()->getText();
                if (!calleeText.empty() && calleeText[0] == '@') {
                    calleeText = calleeText.substr(1); // Remove @ prefix
                }
                inst->calleeName = calleeText;
            }
        }
        
        // Parse result name (from the localDef that wraps this instruction)
        // The result name is set by the caller (parseInstruction) from localDefTerm
        
         // Parse arguments
        if (callCtx->args()) {
            auto *argsCtx = callCtx->args();
            // args has multiple arg entries
            auto argVec = argsCtx->arg();
            for (auto *arg : argVec) {
                if (arg->value()) {
                    std::string argText = arg->value()->getText();
                    // Preserve @ and % prefixes so CallOps can distinguish
                    // globals (@), vregs (%), and constants (numeric)
                    inst->callArgs.push_back(argText);
                    
                    // Track whether this argument is a pointer type
                    bool isPtr = false;
                    if (arg->concreteType()) {
                        isPtr = (arg->concreteType()->pointerType() != nullptr);
                    }
                    inst->callArgKinds.push_back(isPtr);
                }
            }
        }
        
        // Handle inline asm: check if it's a load-address pattern or compiler barrier
        if (callCtx->value() && callCtx->value()->inlineAsm()) {
            if (!inst->callArgs.empty()) {
                std::string argText = inst->callArgs[0];
                if (!argText.empty() && argText[0] == '@') {
                    // Load address pattern: asm "", "=r,0,..."(@function)
                    inst->isInlineAsm = true;
                    inst->calleeName = argText.substr(1); // Remove @ prefix
                } else {
                    // Compiler barrier pattern: asm sideeffect ""(...%vreg)
                    inst->calleeName = "asm";
                }
            } else {
                // No args: asm sideeffect "fnclex", ...()
                inst->calleeName = "asm";
            }
        }
    }
    else if (ctx->getElementPtrInst()) {
        inst->opcode = Opcode::GetElementPtr;
        // Parse GEP operands: base pointer + indices (all typeValue)
        auto *gepCtx = ctx->getElementPtrInst();
        auto typeValues = gepCtx->typeValue();
        if (!typeValues.empty()) {
            Operand baseOp;
            std::string baseText = typeValues[0]->value()->getText();
            baseOp.name = baseText; // Preserve % prefix for vreg lookup
            inst->operands.push_back(std::move(baseOp));
        }
        // Add remaining indices
        for (size_t i = 1; i < typeValues.size(); i++) {
            Operand idxOp;
            std::string idxText = typeValues[i]->value()->getText();
            idxOp.name = idxText; // Preserve % prefix for vreg lookup
            inst->operands.push_back(std::move(idxOp));
        }
        // GEP result is always a pointer (32-bit)
        inst->resultType = std::make_unique<Type>();
        inst->resultType->kind = TypeKind::Pointer;
        // Parse source element type from GEP instruction (e.g., "i32" in
        // "getelementptr inbounds i32, ptr %0, i32 1") to set elementType
        // so that lowerGetElementPtr can compute correct element size.
        if (gepCtx->type()) {
            std::string elemTypeStr = gepCtx->type()->getText();
            if (elemTypeStr.substr(0, 1) == "i") {
                int bitWidth = parseIntWidth(elemTypeStr);
                if (bitWidth > 0) {
                    inst->resultType->elementType = Type::makeInteger(bitWidth);
                } else {
                    inst->resultType->elementType = Type::makeVoid();
                }
            } else if (elemTypeStr == "ptr") {
                inst->resultType->elementType = Type::makeInteger(32);
            } else if (elemTypeStr.find("[") == 0) {
                // Array type like [256xptr] or [16xi8] (ANTLR getText strips spaces)
                // Extract the element type after "x"
                // Format: [NxELEMTYPE]
                auto xPos = elemTypeStr.find('x');
                if (xPos != std::string::npos) {
                    std::string arrElemType = elemTypeStr.substr(xPos + 1);
                    // Trim trailing ]
                    while (!arrElemType.empty() && arrElemType.back() == ']') {
                        arrElemType.pop_back();
                    }
                    if (arrElemType.substr(0, 1) == "i") {
                        int bitWidth = parseIntWidth(arrElemType);
                        if (bitWidth > 0) {
                            inst->resultType->elementType = Type::makeInteger(bitWidth);
                        } else {
                            inst->resultType->elementType = Type::makeVoid();
                        }
                    } else if (arrElemType == "ptr") {
                        inst->resultType->elementType = Type::makeInteger(32);
                    } else {
                        inst->resultType->elementType = Type::makeVoid();
                    }
                } else {
                    inst->resultType->elementType = Type::makeVoid();
                }
            } else {
                inst->resultType->elementType = Type::makeVoid();
            }
        } else {
            inst->resultType->elementType = Type::makeVoid();
        }
        inst->resultType->bitWidth = 32;
    }
    else if (ctx->iCmpInst()) {
        inst->opcode = Opcode::ICmp;
        auto *icmpCtx = ctx->iCmpInst();
        // Parse predicate from iPred
        if (icmpCtx->iPred()) {
            inst->predicate = icmpCtx->iPred()->getText();
        }
        // Parse type from typeValue
        if (icmpCtx->typeValue()) {
            std::string typeText = icmpCtx->typeValue()->getText();
            if (typeText.substr(0, 1) == "i") {
                size_t spacePos = typeText.find(' ');
                std::string typeStr = spacePos != std::string::npos ? typeText.substr(0, spacePos) : typeText;
                inst->resultType = Type::makeInteger(parseIntWidth(typeStr));
            }
            
                                    
            
            Operand op1;
            op1.name = icmpCtx->typeValue()->value()->getText();
            inst->operands.push_back(std::move(op1));
        }
        if (icmpCtx->value()) {
            Operand op2;
            op2.name = icmpCtx->value()->getText();
            inst->operands.push_back(std::move(op2));
        }
    }
    else if (ctx->fCmpInst()) inst->opcode = Opcode::FCmp;
    else if (ctx->truncInst()) {
        inst->opcode = Opcode::Trunc;
        auto *truncCtx = ctx->truncInst();
        if (truncCtx->typeValue()) {
            Operand op;
            op.name = truncCtx->typeValue()->value()->getText();
            inst->operands.push_back(std::move(op));
        }
        if (truncCtx->type()) {
            std::string typeText = truncCtx->type()->getText();
            if (typeText.substr(0, 1) == "i") {
                inst->resultType = Type::makeInteger(parseIntWidth(typeText));
            }
        }
    }
           else if (ctx->zExtInst()) {
        inst->opcode = Opcode::ZExt;
        auto *zextCtx = ctx->zExtInst();
        // Set result type from the destination type
        if (zextCtx->type()) {
            std::string typeText = zextCtx->type()->getText();
            if (typeText.substr(0, 1) == "i") {
                inst->resultType = Type::makeInteger(parseIntWidth(typeText));
            }
        }
        if (zextCtx->typeValue()) {
            Operand op;
            op.name = zextCtx->typeValue()->value()->getText();
            inst->operands.push_back(std::move(op));
        }
    }
    else if (ctx->sExtInst()) {
        inst->opcode = Opcode::SExt;
        auto *sextCtx = ctx->sExtInst();
        if (sextCtx->type()) {
            std::string typeText = sextCtx->type()->getText();
            if (typeText.substr(0, 1) == "i") {
                inst->resultType = Type::makeInteger(parseIntWidth(typeText));
            }
        }
        if (sextCtx->typeValue()) {
            Operand op;
            op.name = sextCtx->typeValue()->value()->getText();
            inst->operands.push_back(std::move(op));
        }
    }
    else if (ctx->bitCastInst()) {
        inst->opcode = Opcode::BitCast;
        auto *bcCtx = ctx->bitCastInst();
        if (bcCtx->typeValue()) {
            Operand op;
            op.name = bcCtx->typeValue()->value()->getText();
            inst->operands.push_back(std::move(op));
        }
        if (bcCtx->type()) {
            std::string typeText = bcCtx->type()->getText();
            if (typeText.substr(0, 1) == "i") {
                inst->resultType = Type::makeInteger(parseIntWidth(typeText));
            } else if (typeText == "ptr") {
                inst->resultType = std::make_unique<Type>();
                inst->resultType->kind = TypeKind::Pointer;
                inst->resultType->elementType = Type::makeVoid();
                inst->resultType->bitWidth = 32;
            }
        }
    }
    else if (ctx->intToPtrInst()) {
        inst->opcode = Opcode::IntToPtr;
        auto *intToPtrCtx = ctx->intToPtrInst();
        if (intToPtrCtx->typeValue()) {
            Operand op;
            op.name = intToPtrCtx->typeValue()->value()->getText();
            inst->operands.push_back(std::move(op));
        }
        if (intToPtrCtx->type()) {
            std::string typeText = intToPtrCtx->type()->getText();
            if (typeText == "ptr") {
                inst->resultType = std::make_unique<Type>();
                inst->resultType->kind = TypeKind::Pointer;
                inst->resultType->elementType = Type::makeVoid();
                inst->resultType->bitWidth = 32;
            } else if (typeText.substr(0, 1) == "i") {
                inst->resultType = Type::makeInteger(parseIntWidth(typeText));
            }
        }
    }
    else if (ctx->ptrToIntInst()) {
        inst->opcode = Opcode::PtrToInt;
        auto *ptrToIntCtx = ctx->ptrToIntInst();
        if (ptrToIntCtx->typeValue()) {
            Operand op;
            op.name = ptrToIntCtx->typeValue()->value()->getText();
            inst->operands.push_back(std::move(op));
        }
        if (ptrToIntCtx->type()) {
            std::string typeText = ptrToIntCtx->type()->getText();
            if (typeText.substr(0, 1) == "i") {
                inst->resultType = Type::makeInteger(parseIntWidth(typeText));
            } else if (typeText == "ptr") {
                inst->resultType = std::make_unique<Type>();
                inst->resultType->kind = TypeKind::Pointer;
                inst->resultType->elementType = Type::makeVoid();
                inst->resultType->bitWidth = 32;
            }
        }
    }
    else if (ctx->phiInst()) {
        inst->opcode = Opcode::Phi;
        auto *phiCtx = ctx->phiInst();
        // Parse phi type for resultType
        if (phiCtx->type()) {
            std::string typeText = phiCtx->type()->getText();
            if (typeText.substr(0, 1) == "i") {
                inst->resultType = Type::makeInteger(parseIntWidth(typeText));
            } else if (typeText == "ptr") {
                inst->resultType = std::make_unique<Type>();
                inst->resultType->kind = TypeKind::Pointer;
                inst->resultType->elementType = Type::makeVoid();
                inst->resultType->bitWidth = 32;
            }
        }
        // Parse incoming values and predecessor labels
        // phiInst: 'phi' type (inc (',' inc)*)
        // inc: '[' value ',' LocalIdent ']'
        auto incs = phiCtx->inc();
        for (auto *incCtx : incs) {
            if (incCtx->value() && incCtx->LocalIdent()) {
                Instruction::PhiInc inc;
                inc.value = incCtx->value()->getText();
                inc.label = incCtx->LocalIdent()->getText();
                // Strip % prefix to match block labels
                if (!inc.label.empty() && inc.label[0] == '%') {
                    inc.label = inc.label.substr(1);
                }
                inst->phiIncrements.push_back(std::move(inc));
            }
        }
    }
             else if (ctx->selectInst()) {
        inst->opcode = Opcode::Select;
        auto *selectCtx = ctx->selectInst();
        // Parse select operands: cond, val1, val2 (all as typeValue)
        auto typeValues = selectCtx->typeValue();
        if (typeValues.size() >= 3) {
            for (int i = 0; i < 3; i++) {
                Operand op;
                op.name = typeValues[i]->value()->getText();
                inst->operands.push_back(std::move(op));
            }
        }
    }
    
    return inst;
}

                 std::unique_ptr<Instruction> IrVisitor::parseTerminator(LLVMIRParser::TerminatorContext *ctx) {
    auto inst = std::make_unique<Instruction>();
    
    if (ctx->retTerm()) {
        inst->opcode = Opcode::RetTerm;
                       // Parse return value if present
        if (ctx->retTerm()->value()) {
            std::string retVal = ctx->retTerm()->value()->getText();
            Operand op;
            op.name = retVal; // Preserve % prefix for vreg lookup
            inst->operands.push_back(std::move(op));
        }
    } else if (ctx->brTerm()) {
        inst->opcode = Opcode::BrTerm;
        if (ctx->brTerm()->label()) {
            std::string target = ctx->brTerm()->label()->LocalIdent()->getText();
            if (!target.empty() && target[0] == '%') target = target.substr(1);
            Operand op;
            op.name = target;
            inst->operands.push_back(std::move(op));
        }
    } else if (ctx->condBrTerm()) {
        inst->opcode = Opcode::CondBrTerm;
        // Parse condition value (preserve % for vreg lookup)
        if (ctx->condBrTerm()->value()) {
            std::string condText = ctx->condBrTerm()->value()->getText();
            Operand condOp;
            condOp.name = condText; // Keep % prefix for vreg lookup
            inst->operands.push_back(std::move(condOp));
        }
        // Parse labels (strip % prefix)
        auto labels = ctx->condBrTerm()->label();
        if (labels.size() >= 2) {
            for (size_t i = 0; i < 2; i++) {
                std::string label = labels[i]->LocalIdent()->getText();
                if (!label.empty() && label[0] == '%') label = label.substr(1);
                Operand op;
                op.name = label;
                inst->operands.push_back(std::move(op));
            }
        }
    } else if (ctx->switchTerm()) {
        inst->opcode = Opcode::SwitchTerm;
        auto sw = ctx->switchTerm();
        if (sw->typeValue() && sw->typeValue()->value()) {
            std::string condText = sw->typeValue()->value()->getText();
            Operand condOp;
            condOp.name = condText;
            inst->operands.push_back(std::move(condOp));
        }
        if (sw->label()) {
            std::string defLabel = sw->label()->LocalIdent()->getText();
            if (!defLabel.empty() && defLabel[0] == '%') defLabel = defLabel.substr(1);
            Operand op;
            op.name = defLabel;
            inst->operands.push_back(std::move(op));
        }
        auto cases = sw->case_();
        for (auto* caseCtx : cases) {
            if (caseCtx->typeConst() && caseCtx->typeConst()->constant()) {
                std::string caseVal = caseCtx->typeConst()->constant()->getText();
                Operand valOp;
                valOp.name = caseVal;
                inst->operands.push_back(std::move(valOp));
            }
            if (caseCtx->label()) {
                std::string caseLabel = caseCtx->label()->LocalIdent()->getText();
                if (!caseLabel.empty() && caseLabel[0] == '%') caseLabel = caseLabel.substr(1);
                Operand op;
                op.name = caseLabel;
                inst->operands.push_back(std::move(op));
            }
        }
    } else if (ctx->unreachableTerm()) {
        inst->opcode = Opcode::UnreachableTerm;
    }
    
    return inst;
}

} // namespace ir
} // namespace llvm_i286
