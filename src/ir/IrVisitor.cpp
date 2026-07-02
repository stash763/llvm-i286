// LLVM IR Visitor Implementation
// Walks ANTLR4 parse tree to build IR AST

#include "ir/IrVisitor.h"

#include <iostream>
#include <cassert>
#include <sstream>

using namespace antlr::generated;

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
            
            // Get initializer text (use raw text of the constant)
            if (globalDef->constant()) {
                auto *constant = globalDef->constant();
                gv->initializer = std::make_unique<ir::Value>();
                gv->initializer->text = constant->getText();
            }
            
            currentModule->globals.push_back(std::move(gv));
        }
    }
    
    return std::move(currentModule);
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
                    if (!p->name.empty() && p->name[0] == '%') {
                        p->name = p->name.substr(1);
                    }
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
                    if (!instPtr->resultName.empty() && instPtr->resultName[0] == '%') {
                        instPtr->resultName = instPtr->resultName.substr(1);
                    }
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
                // Value to store
                Operand valOp;
                auto *val = storeCtx->typeValue(0)->value();
                if (val) {
                    std::string text = val->getText();
                    if (!text.empty() && text[0] == '%') text = text.substr(1);
                    valOp.name = text;
                }
                instPtr->operands.push_back(std::move(valOp));
                
                // Pointer to store to
                Operand ptrOp;
                auto *ptr = storeCtx->typeValue(1)->value();
                if (ptr) {
                    std::string text = ptr->getText();
                    if (!text.empty() && text[0] == '%') text = text.substr(1);
                    ptrOp.name = text;
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
        return type;
    }
    return Type::makeVoid();
}

std::unique_ptr<Type> IrVisitor::parseIntType(LLVMIRParser::IntTypeContext *ctx) {
    auto type = std::make_unique<Type>();
    type->kind = TypeKind::Integer;
    
    std::string text = ctx->IntType()->getText();
    if (text.size() > 1 && text[0] == 'i') {
        try {
            type->bitWidth = std::stoi(text.substr(1));
        } catch (...) {
            type->bitWidth = 32;
        }
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
                inst->resultType = Type::makeInteger(std::stoi(typeStr.substr(1)));
            }
            
            Operand op1;
            auto *val = addCtx->typeValue()->value();
            if (val) {
                std::string text = val->getText();
                if (!text.empty() && text[0] == '%') text = text.substr(1);
                op1.name = text;
            }
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            std::string text = addCtx->value()->getText();
            if (!text.empty() && text[0] == '%') text = text.substr(1);
            op2.name = text;
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
                inst->resultType = Type::makeInteger(std::stoi(typeStr.substr(1)));
            }
            
            Operand op1;
            std::string text = subCtx->typeValue()->value() ? subCtx->typeValue()->value()->getText() : "";
            if (!text.empty() && text[0] == '%') text = text.substr(1);
            op1.name = text;
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            text = subCtx->value()->getText();
            if (!text.empty() && text[0] == '%') text = text.substr(1);
            op2.name = text;
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
                inst->resultType = Type::makeInteger(std::stoi(typeStr.substr(1)));
            }
            
            Operand op1;
            std::string text = mulCtx->typeValue()->value() ? mulCtx->typeValue()->value()->getText() : "";
            if (!text.empty() && text[0] == '%') text = text.substr(1);
            op1.name = text;
            inst->operands.push_back(std::move(op1));
            
            Operand op2;
            text = mulCtx->value()->getText();
            if (!text.empty() && text[0] == '%') text = text.substr(1);
            op2.name = text;
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
            if (typeCtx->intType()) {
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
                sizeOp.name = "32"; // default
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
                inst->resultType = Type::makeInteger(std::stoi(typeText.substr(1)));
            }
        }
        
        if (loadCtx->typeValue()) {
            Operand ptrOp;
            auto *ptr = loadCtx->typeValue()->value();
            if (ptr) {
                std::string text = ptr->getText();
                if (!text.empty() && text[0] == '%') text = text.substr(1);
                ptrOp.name = text;
            }
            inst->operands.push_back(std::move(ptrOp));
        }
    }
    // Parse operands for other instructions (simplified)
    else if (ctx->andInst()) inst->opcode = Opcode::And;
    else if (ctx->orInst()) inst->opcode = Opcode::Or;
    else if (ctx->xorInst()) inst->opcode = Opcode::Xor;
    else if (ctx->shlInst()) inst->opcode = Opcode::Shl;
    else if (ctx->lShrInst()) inst->opcode = Opcode::LShr;
    else if (ctx->aShrInst()) inst->opcode = Opcode::AShr;
    else if (ctx->callInst()) {
        inst->opcode = Opcode::Call;
        auto *callCtx = ctx->callInst();
        
        // Parse callee name
        if (callCtx->value()) {
            std::string calleeText = callCtx->value()->getText();
            if (!calleeText.empty() && calleeText[0] == '@') {
                calleeText = calleeText.substr(1); // Remove @ prefix
            }
            inst->calleeName = calleeText;
        }
        
         // Parse arguments
        if (callCtx->args()) {
            auto *argsCtx = callCtx->args();
            // args has multiple arg entries
            auto argVec = argsCtx->arg();
            for (auto *arg : argVec) {
                if (arg->value()) {
                    std::string argText = arg->value()->getText();
                    if (!argText.empty() && argText[0] == '%') {
                        argText = argText.substr(1);
                    } else if (!argText.empty() && argText[0] == '@') {
                        argText = argText.substr(1); // Remove @ prefix for globals
                    }
                    inst->callArgs.push_back(argText);
                }
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
            if (!baseText.empty() && baseText[0] == '%') baseText = baseText.substr(1);
            baseOp.name = baseText;
            inst->operands.push_back(std::move(baseOp));
        }
        // Add remaining indices
        for (size_t i = 1; i < typeValues.size(); i++) {
            Operand idxOp;
            std::string idxText = typeValues[i]->value()->getText();
            if (!idxText.empty() && idxText[0] == '%') idxText = idxText.substr(1);
            idxOp.name = idxText;
            inst->operands.push_back(std::move(idxOp));
        }
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
                inst->resultType = Type::makeInteger(std::stoi(typeStr.substr(1)));
            }
            
            Operand op1;
            std::string val1Text = icmpCtx->typeValue()->value()->getText();
            if (!val1Text.empty() && val1Text[0] == '%') val1Text = val1Text.substr(1);
            op1.name = val1Text;
            inst->operands.push_back(std::move(op1));
        }
        if (icmpCtx->value()) {
            Operand op2;
            std::string val2Text = icmpCtx->value()->getText();
            if (!val2Text.empty() && val2Text[0] == '%') val2Text = val2Text.substr(1);
            op2.name = val2Text;
            inst->operands.push_back(std::move(op2));
        }
    }
    else if (ctx->fCmpInst()) inst->opcode = Opcode::FCmp;
    else if (ctx->truncInst()) {
        inst->opcode = Opcode::Trunc;
        auto *truncCtx = ctx->truncInst();
        if (truncCtx->typeValue()) {
            Operand op;
            std::string valText = truncCtx->typeValue()->value()->getText();
            if (!valText.empty() && valText[0] == '%') valText = valText.substr(1);
            op.name = valText;
            inst->operands.push_back(std::move(op));
        }
    }
    else if (ctx->zExtInst()) {
        inst->opcode = Opcode::ZExt;
        auto *zextCtx = ctx->zExtInst();
        if (zextCtx->typeValue()) {
            Operand op;
            std::string valText = zextCtx->typeValue()->value()->getText();
            if (!valText.empty() && valText[0] == '%') valText = valText.substr(1);
            op.name = valText;
            inst->operands.push_back(std::move(op));
        }
    }
    else if (ctx->sExtInst()) {
        inst->opcode = Opcode::SExt;
        auto *sextCtx = ctx->sExtInst();
        if (sextCtx->typeValue()) {
            Operand op;
            std::string valText = sextCtx->typeValue()->value()->getText();
            if (!valText.empty() && valText[0] == '%') valText = valText.substr(1);
            op.name = valText;
            inst->operands.push_back(std::move(op));
        }
    }
    else if (ctx->bitCastInst()) inst->opcode = Opcode::BitCast;
    else if (ctx->intToPtrInst()) inst->opcode = Opcode::IntToPtr;
    else if (ctx->ptrToIntInst()) inst->opcode = Opcode::PtrToInt;
    else if (ctx->phiInst()) inst->opcode = Opcode::Phi;
    else if (ctx->selectInst()) {
        inst->opcode = Opcode::Select;
        auto *selectCtx = ctx->selectInst();
        // Parse select operands: cond, val1, val2 (all as typeValue)
        auto typeValues = selectCtx->typeValue();
        if (typeValues.size() >= 3) {
            for (int i = 0; i < 3; i++) {
                Operand op;
                std::string valText = typeValues[i]->value()->getText();
                if (!valText.empty() && valText[0] == '%') valText = valText.substr(1);
                op.name = valText;
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
            if (!retVal.empty() && retVal[0] == '%') {
                retVal = retVal.substr(1); // Remove % prefix
            }
            Operand op;
            op.name = retVal;
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
        // Parse condition value
        if (ctx->condBrTerm()->value()) {
            std::string condText = ctx->condBrTerm()->value()->getText();
            if (!condText.empty() && condText[0] == '%') condText = condText.substr(1);
            Operand condOp;
            condOp.name = condText;
            inst->operands.push_back(std::move(condOp));
        }
        // Parse labels
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
    } else if (ctx->unreachableTerm()) {
        inst->opcode = Opcode::UnreachableTerm;
    }
    
    return inst;
}

} // namespace ir
} // namespace llvm_i286
