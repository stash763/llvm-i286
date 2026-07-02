
// Generated from /home/stash/code/llvm_i286/grammars-v4/llvm-ir/LLVMIR.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "LLVMIRVisitor.h"


namespace antlr::generated {

/**
 * This class provides an empty implementation of LLVMIRVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  LLVMIRBaseVisitor : public LLVMIRVisitor {
public:

  virtual std::any visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTargetDef(LLVMIRParser::TargetDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSourceFilename(LLVMIRParser::SourceFilenameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTargetDataLayout(LLVMIRParser::TargetDataLayoutContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTargetTriple(LLVMIRParser::TargetTripleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTopLevelEntity(LLVMIRParser::TopLevelEntityContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModuleAsm(LLVMIRParser::ModuleAsmContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeDef(LLVMIRParser::TypeDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComdatDef(LLVMIRParser::ComdatDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobalDecl(LLVMIRParser::GlobalDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobalDef(LLVMIRParser::GlobalDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIndirectSymbolDef(LLVMIRParser::IndirectSymbolDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncDecl(LLVMIRParser::FuncDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncDef(LLVMIRParser::FuncDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttrGroupDef(LLVMIRParser::AttrGroupDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamedMetadataDef(LLVMIRParser::NamedMetadataDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMetadataDef(LLVMIRParser::MetadataDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUseListOrder(LLVMIRParser::UseListOrderContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUseListOrderBB(LLVMIRParser::UseListOrderBBContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncHeader(LLVMIRParser::FuncHeaderContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIndirectSymbol(LLVMIRParser::IndirectSymbolContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCallingConv(LLVMIRParser::CallingConvContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCallingConvInt(LLVMIRParser::CallingConvIntContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncHdrField(LLVMIRParser::FuncHdrFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGc(LLVMIRParser::GcContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrefix(LLVMIRParser::PrefixContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrologue(LLVMIRParser::PrologueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPersonality(LLVMIRParser::PersonalityContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReturnAttribute(LLVMIRParser::ReturnAttributeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncBody(LLVMIRParser::FuncBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBasicBlock(LLVMIRParser::BasicBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInstruction(LLVMIRParser::InstructionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTerminator(LLVMIRParser::TerminatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLocalDefTerm(LLVMIRParser::LocalDefTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValueTerminator(LLVMIRParser::ValueTerminatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRetTerm(LLVMIRParser::RetTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBrTerm(LLVMIRParser::BrTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCondBrTerm(LLVMIRParser::CondBrTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSwitchTerm(LLVMIRParser::SwitchTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIndirectBrTerm(LLVMIRParser::IndirectBrTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitResumeTerm(LLVMIRParser::ResumeTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchRetTerm(LLVMIRParser::CatchRetTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCleanupRetTerm(LLVMIRParser::CleanupRetTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnreachableTerm(LLVMIRParser::UnreachableTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInvokeTerm(LLVMIRParser::InvokeTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCallBrTerm(LLVMIRParser::CallBrTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchSwitchTerm(LLVMIRParser::CatchSwitchTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLabel(LLVMIRParser::LabelContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase(LLVMIRParser::CaseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnwindTarget(LLVMIRParser::UnwindTargetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHandlers(LLVMIRParser::HandlersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMetadataNode(LLVMIRParser::MetadataNodeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiExpression(LLVMIRParser::DiExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiExpressionField(LLVMIRParser::DiExpressionFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobalField(LLVMIRParser::GlobalFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSection(LLVMIRParser::SectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComdat(LLVMIRParser::ComdatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPartition(LLVMIRParser::PartitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstant(LLVMIRParser::ConstantContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBoolConst(LLVMIRParser::BoolConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIntConst(LLVMIRParser::IntConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFloatConst(LLVMIRParser::FloatConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNullConst(LLVMIRParser::NullConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNoneConst(LLVMIRParser::NoneConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStructConst(LLVMIRParser::StructConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayConst(LLVMIRParser::ArrayConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVectorConst(LLVMIRParser::VectorConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitZeroInitializerConst(LLVMIRParser::ZeroInitializerConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUndefConst(LLVMIRParser::UndefConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPoisonConst(LLVMIRParser::PoisonConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockAddressConst(LLVMIRParser::BlockAddressConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDsoLocalEquivalentConst(LLVMIRParser::DsoLocalEquivalentConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNoCFIConst(LLVMIRParser::NoCFIConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstantExpr(LLVMIRParser::ConstantExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeConst(LLVMIRParser::TypeConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMetadataAttachment(LLVMIRParser::MetadataAttachmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMdNode(LLVMIRParser::MdNodeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMdTuple(LLVMIRParser::MdTupleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMetadata(LLVMIRParser::MetadataContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiArgList(LLVMIRParser::DiArgListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeValue(LLVMIRParser::TypeValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValue(LLVMIRParser::ValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInlineAsm(LLVMIRParser::InlineAsmContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMdString(LLVMIRParser::MdStringContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMdFieldOrInt(LLVMIRParser::MdFieldOrIntContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiSPFlag(LLVMIRParser::DiSPFlagContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncAttribute(LLVMIRParser::FuncAttributeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType(LLVMIRParser::TypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParams(LLVMIRParser::ParamsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParam(LLVMIRParser::ParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParamAttribute(LLVMIRParser::ParamAttributeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttrString(LLVMIRParser::AttrStringContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttrPair(LLVMIRParser::AttrPairContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlign(LLVMIRParser::AlignContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlignPair(LLVMIRParser::AlignPairContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlignStack(LLVMIRParser::AlignStackContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlignStackPair(LLVMIRParser::AlignStackPairContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAllocKind(LLVMIRParser::AllocKindContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAllocSize(LLVMIRParser::AllocSizeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnwindTable(LLVMIRParser::UnwindTableContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVectorScaleRange(LLVMIRParser::VectorScaleRangeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitByRefAttr(LLVMIRParser::ByRefAttrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitByval(LLVMIRParser::ByvalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDereferenceable(LLVMIRParser::DereferenceableContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementType(LLVMIRParser::ElementTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInAlloca(LLVMIRParser::InAllocaContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParamAttr(LLVMIRParser::ParamAttrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPreallocated(LLVMIRParser::PreallocatedContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStructRetAttr(LLVMIRParser::StructRetAttrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFirstClassType(LLVMIRParser::FirstClassTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConcreteType(LLVMIRParser::ConcreteTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIntType(LLVMIRParser::IntTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFloatType(LLVMIRParser::FloatTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPointerType(LLVMIRParser::PointerTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVectorType(LLVMIRParser::VectorTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLabelType(LLVMIRParser::LabelTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayType(LLVMIRParser::ArrayTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStructType(LLVMIRParser::StructTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamedType(LLVMIRParser::NamedTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMmxType(LLVMIRParser::MmxTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTokenType(LLVMIRParser::TokenTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOpaquePointerType(LLVMIRParser::OpaquePointerTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAddrSpace(LLVMIRParser::AddrSpaceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThreadLocal(LLVMIRParser::ThreadLocalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMetadataType(LLVMIRParser::MetadataTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitCastExpr(LLVMIRParser::BitCastExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGetElementPtrExpr(LLVMIRParser::GetElementPtrExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGepIndex(LLVMIRParser::GepIndexContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAddrSpaceCastExpr(LLVMIRParser::AddrSpaceCastExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIntToPtrExpr(LLVMIRParser::IntToPtrExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitICmpExpr(LLVMIRParser::ICmpExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFCmpExpr(LLVMIRParser::FCmpExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectExpr(LLVMIRParser::SelectExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTruncExpr(LLVMIRParser::TruncExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitZExtExpr(LLVMIRParser::ZExtExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSExtExpr(LLVMIRParser::SExtExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFpTruncExpr(LLVMIRParser::FpTruncExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFpExtExpr(LLVMIRParser::FpExtExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFpToUiExpr(LLVMIRParser::FpToUiExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFpToSiExpr(LLVMIRParser::FpToSiExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUiToFpExpr(LLVMIRParser::UiToFpExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSiToFpExpr(LLVMIRParser::SiToFpExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPtrToIntExpr(LLVMIRParser::PtrToIntExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExtractElementExpr(LLVMIRParser::ExtractElementExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInsertElementExpr(LLVMIRParser::InsertElementExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShuffleVectorExpr(LLVMIRParser::ShuffleVectorExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShlExpr(LLVMIRParser::ShlExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLShrExpr(LLVMIRParser::LShrExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAShrExpr(LLVMIRParser::AShrExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAndExpr(LLVMIRParser::AndExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrExpr(LLVMIRParser::OrExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitXorExpr(LLVMIRParser::XorExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAddExpr(LLVMIRParser::AddExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSubExpr(LLVMIRParser::SubExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMulExpr(LLVMIRParser::MulExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFNegExpr(LLVMIRParser::FNegExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLocalDefInst(LLVMIRParser::LocalDefInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValueInstruction(LLVMIRParser::ValueInstructionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStoreInst(LLVMIRParser::StoreInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSyncScope(LLVMIRParser::SyncScopeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFenceInst(LLVMIRParser::FenceInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFNegInst(LLVMIRParser::FNegInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAddInst(LLVMIRParser::AddInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFAddInst(LLVMIRParser::FAddInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSubInst(LLVMIRParser::SubInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFSubInst(LLVMIRParser::FSubInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMulInst(LLVMIRParser::MulInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFMulInst(LLVMIRParser::FMulInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUDivInst(LLVMIRParser::UDivInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSDivInst(LLVMIRParser::SDivInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFDivInst(LLVMIRParser::FDivInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitURemInst(LLVMIRParser::URemInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSRemInst(LLVMIRParser::SRemInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFRemInst(LLVMIRParser::FRemInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShlInst(LLVMIRParser::ShlInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLShrInst(LLVMIRParser::LShrInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAShrInst(LLVMIRParser::AShrInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAndInst(LLVMIRParser::AndInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrInst(LLVMIRParser::OrInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitXorInst(LLVMIRParser::XorInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExtractElementInst(LLVMIRParser::ExtractElementInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInsertElementInst(LLVMIRParser::InsertElementInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShuffleVectorInst(LLVMIRParser::ShuffleVectorInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExtractValueInst(LLVMIRParser::ExtractValueInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInsertValueInst(LLVMIRParser::InsertValueInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAllocaInst(LLVMIRParser::AllocaInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLoadInst(LLVMIRParser::LoadInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCmpXchgInst(LLVMIRParser::CmpXchgInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtomicRMWInst(LLVMIRParser::AtomicRMWInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGetElementPtrInst(LLVMIRParser::GetElementPtrInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTruncInst(LLVMIRParser::TruncInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitZExtInst(LLVMIRParser::ZExtInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSExtInst(LLVMIRParser::SExtInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFpTruncInst(LLVMIRParser::FpTruncInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFpExtInst(LLVMIRParser::FpExtInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFpToUiInst(LLVMIRParser::FpToUiInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFpToSiInst(LLVMIRParser::FpToSiInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUiToFpInst(LLVMIRParser::UiToFpInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSiToFpInst(LLVMIRParser::SiToFpInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPtrToIntInst(LLVMIRParser::PtrToIntInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIntToPtrInst(LLVMIRParser::IntToPtrInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitCastInst(LLVMIRParser::BitCastInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAddrSpaceCastInst(LLVMIRParser::AddrSpaceCastInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitICmpInst(LLVMIRParser::ICmpInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFCmpInst(LLVMIRParser::FCmpInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPhiInst(LLVMIRParser::PhiInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectInst(LLVMIRParser::SelectInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFreezeInst(LLVMIRParser::FreezeInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCallInst(LLVMIRParser::CallInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVaargInst(LLVMIRParser::VaargInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLandingPadInst(LLVMIRParser::LandingPadInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchPadInst(LLVMIRParser::CatchPadInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCleanupPadInst(LLVMIRParser::CleanupPadInstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInc(LLVMIRParser::IncContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperandBundle(LLVMIRParser::OperandBundleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClause(LLVMIRParser::ClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgs(LLVMIRParser::ArgsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArg(LLVMIRParser::ArgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExceptionArg(LLVMIRParser::ExceptionArgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExceptionPad(LLVMIRParser::ExceptionPadContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExternalLinkage(LLVMIRParser::ExternalLinkageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInternalLinkage(LLVMIRParser::InternalLinkageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLinkage(LLVMIRParser::LinkageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPreemption(LLVMIRParser::PreemptionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVisibility(LLVMIRParser::VisibilityContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDllStorageClass(LLVMIRParser::DllStorageClassContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTlsModel(LLVMIRParser::TlsModelContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnnamedAddr(LLVMIRParser::UnnamedAddrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExternallyInitialized(LLVMIRParser::ExternallyInitializedContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImmutable(LLVMIRParser::ImmutableContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncAttr(LLVMIRParser::FuncAttrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDistinct(LLVMIRParser::DistinctContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInBounds(LLVMIRParser::InBoundsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReturnAttr(LLVMIRParser::ReturnAttrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOverflowFlag(LLVMIRParser::OverflowFlagContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIPred(LLVMIRParser::IPredContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFPred(LLVMIRParser::FPredContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtomicOrdering(LLVMIRParser::AtomicOrderingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCallingConvEnum(LLVMIRParser::CallingConvEnumContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFastMathFlag(LLVMIRParser::FastMathFlagContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtomicOp(LLVMIRParser::AtomicOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFloatKind(LLVMIRParser::FloatKindContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecializedMDNode(LLVMIRParser::SpecializedMDNodeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiBasicType(LLVMIRParser::DiBasicTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiCommonBlock(LLVMIRParser::DiCommonBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiCompileUnit(LLVMIRParser::DiCompileUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiCompositeType(LLVMIRParser::DiCompositeTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiCompositeTypeField(LLVMIRParser::DiCompositeTypeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiDerivedType(LLVMIRParser::DiDerivedTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiDerivedTypeField(LLVMIRParser::DiDerivedTypeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiEnumerator(LLVMIRParser::DiEnumeratorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiEnumeratorField(LLVMIRParser::DiEnumeratorFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiFile(LLVMIRParser::DiFileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiFileField(LLVMIRParser::DiFileFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiGlobalVariable(LLVMIRParser::DiGlobalVariableContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiGlobalVariableField(LLVMIRParser::DiGlobalVariableFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiGlobalVariableExpression(LLVMIRParser::DiGlobalVariableExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiGlobalVariableExpressionField(LLVMIRParser::DiGlobalVariableExpressionFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiImportedEntity(LLVMIRParser::DiImportedEntityContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiImportedEntityField(LLVMIRParser::DiImportedEntityFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLabel(LLVMIRParser::DiLabelContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLabelField(LLVMIRParser::DiLabelFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLexicalBlock(LLVMIRParser::DiLexicalBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLexicalBlockField(LLVMIRParser::DiLexicalBlockFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLexicalBlockFile(LLVMIRParser::DiLexicalBlockFileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLexicalBlockFileField(LLVMIRParser::DiLexicalBlockFileFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLocalVariable(LLVMIRParser::DiLocalVariableContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLocalVariableField(LLVMIRParser::DiLocalVariableFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLocation(LLVMIRParser::DiLocationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiLocationField(LLVMIRParser::DiLocationFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiMacro(LLVMIRParser::DiMacroContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiMacroField(LLVMIRParser::DiMacroFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiMacroFile(LLVMIRParser::DiMacroFileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiMacroFileField(LLVMIRParser::DiMacroFileFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiModule(LLVMIRParser::DiModuleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiModuleField(LLVMIRParser::DiModuleFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiNamespace(LLVMIRParser::DiNamespaceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiNamespaceField(LLVMIRParser::DiNamespaceFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiObjCProperty(LLVMIRParser::DiObjCPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiObjCPropertyField(LLVMIRParser::DiObjCPropertyFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiStringType(LLVMIRParser::DiStringTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiStringTypeField(LLVMIRParser::DiStringTypeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiSubprogram(LLVMIRParser::DiSubprogramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiSubprogramField(LLVMIRParser::DiSubprogramFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiSubrange(LLVMIRParser::DiSubrangeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiSubrangeField(LLVMIRParser::DiSubrangeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiSubroutineType(LLVMIRParser::DiSubroutineTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiTemplateTypeParameter(LLVMIRParser::DiTemplateTypeParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiTemplateValueParameter(LLVMIRParser::DiTemplateValueParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenericDiNode(LLVMIRParser::GenericDiNodeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiTemplateTypeParameterField(LLVMIRParser::DiTemplateTypeParameterFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiCompileUnitField(LLVMIRParser::DiCompileUnitFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiCommonBlockField(LLVMIRParser::DiCommonBlockFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiBasicTypeField(LLVMIRParser::DiBasicTypeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenericDINodeField(LLVMIRParser::GenericDINodeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTagField(LLVMIRParser::TagFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHeaderField(LLVMIRParser::HeaderFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperandsField(LLVMIRParser::OperandsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiTemplateValueParameterField(LLVMIRParser::DiTemplateValueParameterFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNameField(LLVMIRParser::NameFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeField(LLVMIRParser::TypeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefaultedField(LLVMIRParser::DefaultedFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValueField(LLVMIRParser::ValueFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMdField(LLVMIRParser::MdFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiSubroutineTypeField(LLVMIRParser::DiSubroutineTypeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFlagsField(LLVMIRParser::FlagsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiFlags(LLVMIRParser::DiFlagsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCcField(LLVMIRParser::CcFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlignField(LLVMIRParser::AlignFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAllocatedField(LLVMIRParser::AllocatedFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnnotationsField(LLVMIRParser::AnnotationsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgField(LLVMIRParser::ArgFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssociatedField(LLVMIRParser::AssociatedFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttributesField(LLVMIRParser::AttributesFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBaseTypeField(LLVMIRParser::BaseTypeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChecksumField(LLVMIRParser::ChecksumFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChecksumkindField(LLVMIRParser::ChecksumkindFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitColumnField(LLVMIRParser::ColumnFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfigMacrosField(LLVMIRParser::ConfigMacrosFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitContainingTypeField(LLVMIRParser::ContainingTypeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCountField(LLVMIRParser::CountFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDebugInfoForProfilingField(LLVMIRParser::DebugInfoForProfilingFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclarationField(LLVMIRParser::DeclarationFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDirectoryField(LLVMIRParser::DirectoryFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiscriminatorField(LLVMIRParser::DiscriminatorFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDataLocationField(LLVMIRParser::DataLocationFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDiscriminatorIntField(LLVMIRParser::DiscriminatorIntFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDwarfAddressSpaceField(LLVMIRParser::DwarfAddressSpaceFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDwoIdField(LLVMIRParser::DwoIdFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementsField(LLVMIRParser::ElementsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEmissionKindField(LLVMIRParser::EmissionKindFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEncodingField(LLVMIRParser::EncodingFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEntityField(LLVMIRParser::EntityFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnumsField(LLVMIRParser::EnumsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportSymbolsField(LLVMIRParser::ExportSymbolsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExprField(LLVMIRParser::ExprFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExtraDataField(LLVMIRParser::ExtraDataFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFileField(LLVMIRParser::FileFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFilenameField(LLVMIRParser::FilenameFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFlagsStringField(LLVMIRParser::FlagsStringFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGetterField(LLVMIRParser::GetterFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobalsField(LLVMIRParser::GlobalsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifierField(LLVMIRParser::IdentifierFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportsField(LLVMIRParser::ImportsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIncludePathField(LLVMIRParser::IncludePathFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInlinedAtField(LLVMIRParser::InlinedAtFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIsDeclField(LLVMIRParser::IsDeclFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIsDefinitionField(LLVMIRParser::IsDefinitionFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIsImplicitCodeField(LLVMIRParser::IsImplicitCodeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIsLocalField(LLVMIRParser::IsLocalFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIsOptimizedField(LLVMIRParser::IsOptimizedFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIsUnsignedField(LLVMIRParser::IsUnsignedFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitApiNotesField(LLVMIRParser::ApiNotesFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLanguageField(LLVMIRParser::LanguageFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLineField(LLVMIRParser::LineFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLinkageNameField(LLVMIRParser::LinkageNameFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLowerBoundField(LLVMIRParser::LowerBoundFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMacrosField(LLVMIRParser::MacrosFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNameTableKindField(LLVMIRParser::NameTableKindFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNodesField(LLVMIRParser::NodesFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOffsetField(LLVMIRParser::OffsetFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProducerField(LLVMIRParser::ProducerFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRangesBaseAddressField(LLVMIRParser::RangesBaseAddressFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRankField(LLVMIRParser::RankFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRetainedNodesField(LLVMIRParser::RetainedNodesFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRetainedTypesField(LLVMIRParser::RetainedTypesFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRuntimeLangField(LLVMIRParser::RuntimeLangFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRuntimeVersionField(LLVMIRParser::RuntimeVersionFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScopeField(LLVMIRParser::ScopeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScopeLineField(LLVMIRParser::ScopeLineFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSdkField(LLVMIRParser::SdkFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSetterField(LLVMIRParser::SetterFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSizeField(LLVMIRParser::SizeFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSourceField(LLVMIRParser::SourceFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpFlagsField(LLVMIRParser::SpFlagsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSplitDebugFilenameField(LLVMIRParser::SplitDebugFilenameFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSplitDebugInliningField(LLVMIRParser::SplitDebugInliningFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStrideField(LLVMIRParser::StrideFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStringLengthField(LLVMIRParser::StringLengthFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStringLengthExpressionField(LLVMIRParser::StringLengthExpressionFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStringLocationExpressionField(LLVMIRParser::StringLocationExpressionFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSysrootField(LLVMIRParser::SysrootFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTargetFuncNameField(LLVMIRParser::TargetFuncNameFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateParamsField(LLVMIRParser::TemplateParamsFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThisAdjustmentField(LLVMIRParser::ThisAdjustmentFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThrownTypesField(LLVMIRParser::ThrownTypesFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeMacinfoField(LLVMIRParser::TypeMacinfoFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypesField(LLVMIRParser::TypesFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnitField(LLVMIRParser::UnitFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUpperBoundField(LLVMIRParser::UpperBoundFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValueIntField(LLVMIRParser::ValueIntFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValueStringField(LLVMIRParser::ValueStringFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarField(LLVMIRParser::VarFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVirtualIndexField(LLVMIRParser::VirtualIndexFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVirtualityField(LLVMIRParser::VirtualityFieldContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVtableHolderField(LLVMIRParser::VtableHolderFieldContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace antlr::generated
