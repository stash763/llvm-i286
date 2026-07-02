
// Generated from /home/stash/code/llvm_i286/grammars-v4/llvm-ir/LLVMIR.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "LLVMIRParser.h"


namespace antlr::generated {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by LLVMIRParser.
 */
class  LLVMIRVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by LLVMIRParser.
   */
    virtual std::any visitCompilationUnit(LLVMIRParser::CompilationUnitContext *context) = 0;

    virtual std::any visitTargetDef(LLVMIRParser::TargetDefContext *context) = 0;

    virtual std::any visitSourceFilename(LLVMIRParser::SourceFilenameContext *context) = 0;

    virtual std::any visitTargetDataLayout(LLVMIRParser::TargetDataLayoutContext *context) = 0;

    virtual std::any visitTargetTriple(LLVMIRParser::TargetTripleContext *context) = 0;

    virtual std::any visitTopLevelEntity(LLVMIRParser::TopLevelEntityContext *context) = 0;

    virtual std::any visitModuleAsm(LLVMIRParser::ModuleAsmContext *context) = 0;

    virtual std::any visitTypeDef(LLVMIRParser::TypeDefContext *context) = 0;

    virtual std::any visitComdatDef(LLVMIRParser::ComdatDefContext *context) = 0;

    virtual std::any visitGlobalDecl(LLVMIRParser::GlobalDeclContext *context) = 0;

    virtual std::any visitGlobalDef(LLVMIRParser::GlobalDefContext *context) = 0;

    virtual std::any visitIndirectSymbolDef(LLVMIRParser::IndirectSymbolDefContext *context) = 0;

    virtual std::any visitFuncDecl(LLVMIRParser::FuncDeclContext *context) = 0;

    virtual std::any visitFuncDef(LLVMIRParser::FuncDefContext *context) = 0;

    virtual std::any visitAttrGroupDef(LLVMIRParser::AttrGroupDefContext *context) = 0;

    virtual std::any visitNamedMetadataDef(LLVMIRParser::NamedMetadataDefContext *context) = 0;

    virtual std::any visitMetadataDef(LLVMIRParser::MetadataDefContext *context) = 0;

    virtual std::any visitUseListOrder(LLVMIRParser::UseListOrderContext *context) = 0;

    virtual std::any visitUseListOrderBB(LLVMIRParser::UseListOrderBBContext *context) = 0;

    virtual std::any visitFuncHeader(LLVMIRParser::FuncHeaderContext *context) = 0;

    virtual std::any visitIndirectSymbol(LLVMIRParser::IndirectSymbolContext *context) = 0;

    virtual std::any visitCallingConv(LLVMIRParser::CallingConvContext *context) = 0;

    virtual std::any visitCallingConvInt(LLVMIRParser::CallingConvIntContext *context) = 0;

    virtual std::any visitFuncHdrField(LLVMIRParser::FuncHdrFieldContext *context) = 0;

    virtual std::any visitGc(LLVMIRParser::GcContext *context) = 0;

    virtual std::any visitPrefix(LLVMIRParser::PrefixContext *context) = 0;

    virtual std::any visitPrologue(LLVMIRParser::PrologueContext *context) = 0;

    virtual std::any visitPersonality(LLVMIRParser::PersonalityContext *context) = 0;

    virtual std::any visitReturnAttribute(LLVMIRParser::ReturnAttributeContext *context) = 0;

    virtual std::any visitFuncBody(LLVMIRParser::FuncBodyContext *context) = 0;

    virtual std::any visitBasicBlock(LLVMIRParser::BasicBlockContext *context) = 0;

    virtual std::any visitInstruction(LLVMIRParser::InstructionContext *context) = 0;

    virtual std::any visitTerminator(LLVMIRParser::TerminatorContext *context) = 0;

    virtual std::any visitLocalDefTerm(LLVMIRParser::LocalDefTermContext *context) = 0;

    virtual std::any visitValueTerminator(LLVMIRParser::ValueTerminatorContext *context) = 0;

    virtual std::any visitRetTerm(LLVMIRParser::RetTermContext *context) = 0;

    virtual std::any visitBrTerm(LLVMIRParser::BrTermContext *context) = 0;

    virtual std::any visitCondBrTerm(LLVMIRParser::CondBrTermContext *context) = 0;

    virtual std::any visitSwitchTerm(LLVMIRParser::SwitchTermContext *context) = 0;

    virtual std::any visitIndirectBrTerm(LLVMIRParser::IndirectBrTermContext *context) = 0;

    virtual std::any visitResumeTerm(LLVMIRParser::ResumeTermContext *context) = 0;

    virtual std::any visitCatchRetTerm(LLVMIRParser::CatchRetTermContext *context) = 0;

    virtual std::any visitCleanupRetTerm(LLVMIRParser::CleanupRetTermContext *context) = 0;

    virtual std::any visitUnreachableTerm(LLVMIRParser::UnreachableTermContext *context) = 0;

    virtual std::any visitInvokeTerm(LLVMIRParser::InvokeTermContext *context) = 0;

    virtual std::any visitCallBrTerm(LLVMIRParser::CallBrTermContext *context) = 0;

    virtual std::any visitCatchSwitchTerm(LLVMIRParser::CatchSwitchTermContext *context) = 0;

    virtual std::any visitLabel(LLVMIRParser::LabelContext *context) = 0;

    virtual std::any visitCase(LLVMIRParser::CaseContext *context) = 0;

    virtual std::any visitUnwindTarget(LLVMIRParser::UnwindTargetContext *context) = 0;

    virtual std::any visitHandlers(LLVMIRParser::HandlersContext *context) = 0;

    virtual std::any visitMetadataNode(LLVMIRParser::MetadataNodeContext *context) = 0;

    virtual std::any visitDiExpression(LLVMIRParser::DiExpressionContext *context) = 0;

    virtual std::any visitDiExpressionField(LLVMIRParser::DiExpressionFieldContext *context) = 0;

    virtual std::any visitGlobalField(LLVMIRParser::GlobalFieldContext *context) = 0;

    virtual std::any visitSection(LLVMIRParser::SectionContext *context) = 0;

    virtual std::any visitComdat(LLVMIRParser::ComdatContext *context) = 0;

    virtual std::any visitPartition(LLVMIRParser::PartitionContext *context) = 0;

    virtual std::any visitConstant(LLVMIRParser::ConstantContext *context) = 0;

    virtual std::any visitBoolConst(LLVMIRParser::BoolConstContext *context) = 0;

    virtual std::any visitIntConst(LLVMIRParser::IntConstContext *context) = 0;

    virtual std::any visitFloatConst(LLVMIRParser::FloatConstContext *context) = 0;

    virtual std::any visitNullConst(LLVMIRParser::NullConstContext *context) = 0;

    virtual std::any visitNoneConst(LLVMIRParser::NoneConstContext *context) = 0;

    virtual std::any visitStructConst(LLVMIRParser::StructConstContext *context) = 0;

    virtual std::any visitArrayConst(LLVMIRParser::ArrayConstContext *context) = 0;

    virtual std::any visitVectorConst(LLVMIRParser::VectorConstContext *context) = 0;

    virtual std::any visitZeroInitializerConst(LLVMIRParser::ZeroInitializerConstContext *context) = 0;

    virtual std::any visitUndefConst(LLVMIRParser::UndefConstContext *context) = 0;

    virtual std::any visitPoisonConst(LLVMIRParser::PoisonConstContext *context) = 0;

    virtual std::any visitBlockAddressConst(LLVMIRParser::BlockAddressConstContext *context) = 0;

    virtual std::any visitDsoLocalEquivalentConst(LLVMIRParser::DsoLocalEquivalentConstContext *context) = 0;

    virtual std::any visitNoCFIConst(LLVMIRParser::NoCFIConstContext *context) = 0;

    virtual std::any visitConstantExpr(LLVMIRParser::ConstantExprContext *context) = 0;

    virtual std::any visitTypeConst(LLVMIRParser::TypeConstContext *context) = 0;

    virtual std::any visitMetadataAttachment(LLVMIRParser::MetadataAttachmentContext *context) = 0;

    virtual std::any visitMdNode(LLVMIRParser::MdNodeContext *context) = 0;

    virtual std::any visitMdTuple(LLVMIRParser::MdTupleContext *context) = 0;

    virtual std::any visitMetadata(LLVMIRParser::MetadataContext *context) = 0;

    virtual std::any visitDiArgList(LLVMIRParser::DiArgListContext *context) = 0;

    virtual std::any visitTypeValue(LLVMIRParser::TypeValueContext *context) = 0;

    virtual std::any visitValue(LLVMIRParser::ValueContext *context) = 0;

    virtual std::any visitInlineAsm(LLVMIRParser::InlineAsmContext *context) = 0;

    virtual std::any visitMdString(LLVMIRParser::MdStringContext *context) = 0;

    virtual std::any visitMdFieldOrInt(LLVMIRParser::MdFieldOrIntContext *context) = 0;

    virtual std::any visitDiSPFlag(LLVMIRParser::DiSPFlagContext *context) = 0;

    virtual std::any visitFuncAttribute(LLVMIRParser::FuncAttributeContext *context) = 0;

    virtual std::any visitType(LLVMIRParser::TypeContext *context) = 0;

    virtual std::any visitParams(LLVMIRParser::ParamsContext *context) = 0;

    virtual std::any visitParam(LLVMIRParser::ParamContext *context) = 0;

    virtual std::any visitParamAttribute(LLVMIRParser::ParamAttributeContext *context) = 0;

    virtual std::any visitAttrString(LLVMIRParser::AttrStringContext *context) = 0;

    virtual std::any visitAttrPair(LLVMIRParser::AttrPairContext *context) = 0;

    virtual std::any visitAlign(LLVMIRParser::AlignContext *context) = 0;

    virtual std::any visitAlignPair(LLVMIRParser::AlignPairContext *context) = 0;

    virtual std::any visitAlignStack(LLVMIRParser::AlignStackContext *context) = 0;

    virtual std::any visitAlignStackPair(LLVMIRParser::AlignStackPairContext *context) = 0;

    virtual std::any visitAllocKind(LLVMIRParser::AllocKindContext *context) = 0;

    virtual std::any visitAllocSize(LLVMIRParser::AllocSizeContext *context) = 0;

    virtual std::any visitUnwindTable(LLVMIRParser::UnwindTableContext *context) = 0;

    virtual std::any visitVectorScaleRange(LLVMIRParser::VectorScaleRangeContext *context) = 0;

    virtual std::any visitByRefAttr(LLVMIRParser::ByRefAttrContext *context) = 0;

    virtual std::any visitByval(LLVMIRParser::ByvalContext *context) = 0;

    virtual std::any visitDereferenceable(LLVMIRParser::DereferenceableContext *context) = 0;

    virtual std::any visitElementType(LLVMIRParser::ElementTypeContext *context) = 0;

    virtual std::any visitInAlloca(LLVMIRParser::InAllocaContext *context) = 0;

    virtual std::any visitParamAttr(LLVMIRParser::ParamAttrContext *context) = 0;

    virtual std::any visitPreallocated(LLVMIRParser::PreallocatedContext *context) = 0;

    virtual std::any visitStructRetAttr(LLVMIRParser::StructRetAttrContext *context) = 0;

    virtual std::any visitFirstClassType(LLVMIRParser::FirstClassTypeContext *context) = 0;

    virtual std::any visitConcreteType(LLVMIRParser::ConcreteTypeContext *context) = 0;

    virtual std::any visitIntType(LLVMIRParser::IntTypeContext *context) = 0;

    virtual std::any visitFloatType(LLVMIRParser::FloatTypeContext *context) = 0;

    virtual std::any visitPointerType(LLVMIRParser::PointerTypeContext *context) = 0;

    virtual std::any visitVectorType(LLVMIRParser::VectorTypeContext *context) = 0;

    virtual std::any visitLabelType(LLVMIRParser::LabelTypeContext *context) = 0;

    virtual std::any visitArrayType(LLVMIRParser::ArrayTypeContext *context) = 0;

    virtual std::any visitStructType(LLVMIRParser::StructTypeContext *context) = 0;

    virtual std::any visitNamedType(LLVMIRParser::NamedTypeContext *context) = 0;

    virtual std::any visitMmxType(LLVMIRParser::MmxTypeContext *context) = 0;

    virtual std::any visitTokenType(LLVMIRParser::TokenTypeContext *context) = 0;

    virtual std::any visitOpaquePointerType(LLVMIRParser::OpaquePointerTypeContext *context) = 0;

    virtual std::any visitAddrSpace(LLVMIRParser::AddrSpaceContext *context) = 0;

    virtual std::any visitThreadLocal(LLVMIRParser::ThreadLocalContext *context) = 0;

    virtual std::any visitMetadataType(LLVMIRParser::MetadataTypeContext *context) = 0;

    virtual std::any visitBitCastExpr(LLVMIRParser::BitCastExprContext *context) = 0;

    virtual std::any visitGetElementPtrExpr(LLVMIRParser::GetElementPtrExprContext *context) = 0;

    virtual std::any visitGepIndex(LLVMIRParser::GepIndexContext *context) = 0;

    virtual std::any visitAddrSpaceCastExpr(LLVMIRParser::AddrSpaceCastExprContext *context) = 0;

    virtual std::any visitIntToPtrExpr(LLVMIRParser::IntToPtrExprContext *context) = 0;

    virtual std::any visitICmpExpr(LLVMIRParser::ICmpExprContext *context) = 0;

    virtual std::any visitFCmpExpr(LLVMIRParser::FCmpExprContext *context) = 0;

    virtual std::any visitSelectExpr(LLVMIRParser::SelectExprContext *context) = 0;

    virtual std::any visitTruncExpr(LLVMIRParser::TruncExprContext *context) = 0;

    virtual std::any visitZExtExpr(LLVMIRParser::ZExtExprContext *context) = 0;

    virtual std::any visitSExtExpr(LLVMIRParser::SExtExprContext *context) = 0;

    virtual std::any visitFpTruncExpr(LLVMIRParser::FpTruncExprContext *context) = 0;

    virtual std::any visitFpExtExpr(LLVMIRParser::FpExtExprContext *context) = 0;

    virtual std::any visitFpToUiExpr(LLVMIRParser::FpToUiExprContext *context) = 0;

    virtual std::any visitFpToSiExpr(LLVMIRParser::FpToSiExprContext *context) = 0;

    virtual std::any visitUiToFpExpr(LLVMIRParser::UiToFpExprContext *context) = 0;

    virtual std::any visitSiToFpExpr(LLVMIRParser::SiToFpExprContext *context) = 0;

    virtual std::any visitPtrToIntExpr(LLVMIRParser::PtrToIntExprContext *context) = 0;

    virtual std::any visitExtractElementExpr(LLVMIRParser::ExtractElementExprContext *context) = 0;

    virtual std::any visitInsertElementExpr(LLVMIRParser::InsertElementExprContext *context) = 0;

    virtual std::any visitShuffleVectorExpr(LLVMIRParser::ShuffleVectorExprContext *context) = 0;

    virtual std::any visitShlExpr(LLVMIRParser::ShlExprContext *context) = 0;

    virtual std::any visitLShrExpr(LLVMIRParser::LShrExprContext *context) = 0;

    virtual std::any visitAShrExpr(LLVMIRParser::AShrExprContext *context) = 0;

    virtual std::any visitAndExpr(LLVMIRParser::AndExprContext *context) = 0;

    virtual std::any visitOrExpr(LLVMIRParser::OrExprContext *context) = 0;

    virtual std::any visitXorExpr(LLVMIRParser::XorExprContext *context) = 0;

    virtual std::any visitAddExpr(LLVMIRParser::AddExprContext *context) = 0;

    virtual std::any visitSubExpr(LLVMIRParser::SubExprContext *context) = 0;

    virtual std::any visitMulExpr(LLVMIRParser::MulExprContext *context) = 0;

    virtual std::any visitFNegExpr(LLVMIRParser::FNegExprContext *context) = 0;

    virtual std::any visitLocalDefInst(LLVMIRParser::LocalDefInstContext *context) = 0;

    virtual std::any visitValueInstruction(LLVMIRParser::ValueInstructionContext *context) = 0;

    virtual std::any visitStoreInst(LLVMIRParser::StoreInstContext *context) = 0;

    virtual std::any visitSyncScope(LLVMIRParser::SyncScopeContext *context) = 0;

    virtual std::any visitFenceInst(LLVMIRParser::FenceInstContext *context) = 0;

    virtual std::any visitFNegInst(LLVMIRParser::FNegInstContext *context) = 0;

    virtual std::any visitAddInst(LLVMIRParser::AddInstContext *context) = 0;

    virtual std::any visitFAddInst(LLVMIRParser::FAddInstContext *context) = 0;

    virtual std::any visitSubInst(LLVMIRParser::SubInstContext *context) = 0;

    virtual std::any visitFSubInst(LLVMIRParser::FSubInstContext *context) = 0;

    virtual std::any visitMulInst(LLVMIRParser::MulInstContext *context) = 0;

    virtual std::any visitFMulInst(LLVMIRParser::FMulInstContext *context) = 0;

    virtual std::any visitUDivInst(LLVMIRParser::UDivInstContext *context) = 0;

    virtual std::any visitSDivInst(LLVMIRParser::SDivInstContext *context) = 0;

    virtual std::any visitFDivInst(LLVMIRParser::FDivInstContext *context) = 0;

    virtual std::any visitURemInst(LLVMIRParser::URemInstContext *context) = 0;

    virtual std::any visitSRemInst(LLVMIRParser::SRemInstContext *context) = 0;

    virtual std::any visitFRemInst(LLVMIRParser::FRemInstContext *context) = 0;

    virtual std::any visitShlInst(LLVMIRParser::ShlInstContext *context) = 0;

    virtual std::any visitLShrInst(LLVMIRParser::LShrInstContext *context) = 0;

    virtual std::any visitAShrInst(LLVMIRParser::AShrInstContext *context) = 0;

    virtual std::any visitAndInst(LLVMIRParser::AndInstContext *context) = 0;

    virtual std::any visitOrInst(LLVMIRParser::OrInstContext *context) = 0;

    virtual std::any visitXorInst(LLVMIRParser::XorInstContext *context) = 0;

    virtual std::any visitExtractElementInst(LLVMIRParser::ExtractElementInstContext *context) = 0;

    virtual std::any visitInsertElementInst(LLVMIRParser::InsertElementInstContext *context) = 0;

    virtual std::any visitShuffleVectorInst(LLVMIRParser::ShuffleVectorInstContext *context) = 0;

    virtual std::any visitExtractValueInst(LLVMIRParser::ExtractValueInstContext *context) = 0;

    virtual std::any visitInsertValueInst(LLVMIRParser::InsertValueInstContext *context) = 0;

    virtual std::any visitAllocaInst(LLVMIRParser::AllocaInstContext *context) = 0;

    virtual std::any visitLoadInst(LLVMIRParser::LoadInstContext *context) = 0;

    virtual std::any visitCmpXchgInst(LLVMIRParser::CmpXchgInstContext *context) = 0;

    virtual std::any visitAtomicRMWInst(LLVMIRParser::AtomicRMWInstContext *context) = 0;

    virtual std::any visitGetElementPtrInst(LLVMIRParser::GetElementPtrInstContext *context) = 0;

    virtual std::any visitTruncInst(LLVMIRParser::TruncInstContext *context) = 0;

    virtual std::any visitZExtInst(LLVMIRParser::ZExtInstContext *context) = 0;

    virtual std::any visitSExtInst(LLVMIRParser::SExtInstContext *context) = 0;

    virtual std::any visitFpTruncInst(LLVMIRParser::FpTruncInstContext *context) = 0;

    virtual std::any visitFpExtInst(LLVMIRParser::FpExtInstContext *context) = 0;

    virtual std::any visitFpToUiInst(LLVMIRParser::FpToUiInstContext *context) = 0;

    virtual std::any visitFpToSiInst(LLVMIRParser::FpToSiInstContext *context) = 0;

    virtual std::any visitUiToFpInst(LLVMIRParser::UiToFpInstContext *context) = 0;

    virtual std::any visitSiToFpInst(LLVMIRParser::SiToFpInstContext *context) = 0;

    virtual std::any visitPtrToIntInst(LLVMIRParser::PtrToIntInstContext *context) = 0;

    virtual std::any visitIntToPtrInst(LLVMIRParser::IntToPtrInstContext *context) = 0;

    virtual std::any visitBitCastInst(LLVMIRParser::BitCastInstContext *context) = 0;

    virtual std::any visitAddrSpaceCastInst(LLVMIRParser::AddrSpaceCastInstContext *context) = 0;

    virtual std::any visitICmpInst(LLVMIRParser::ICmpInstContext *context) = 0;

    virtual std::any visitFCmpInst(LLVMIRParser::FCmpInstContext *context) = 0;

    virtual std::any visitPhiInst(LLVMIRParser::PhiInstContext *context) = 0;

    virtual std::any visitSelectInst(LLVMIRParser::SelectInstContext *context) = 0;

    virtual std::any visitFreezeInst(LLVMIRParser::FreezeInstContext *context) = 0;

    virtual std::any visitCallInst(LLVMIRParser::CallInstContext *context) = 0;

    virtual std::any visitVaargInst(LLVMIRParser::VaargInstContext *context) = 0;

    virtual std::any visitLandingPadInst(LLVMIRParser::LandingPadInstContext *context) = 0;

    virtual std::any visitCatchPadInst(LLVMIRParser::CatchPadInstContext *context) = 0;

    virtual std::any visitCleanupPadInst(LLVMIRParser::CleanupPadInstContext *context) = 0;

    virtual std::any visitInc(LLVMIRParser::IncContext *context) = 0;

    virtual std::any visitOperandBundle(LLVMIRParser::OperandBundleContext *context) = 0;

    virtual std::any visitClause(LLVMIRParser::ClauseContext *context) = 0;

    virtual std::any visitArgs(LLVMIRParser::ArgsContext *context) = 0;

    virtual std::any visitArg(LLVMIRParser::ArgContext *context) = 0;

    virtual std::any visitExceptionArg(LLVMIRParser::ExceptionArgContext *context) = 0;

    virtual std::any visitExceptionPad(LLVMIRParser::ExceptionPadContext *context) = 0;

    virtual std::any visitExternalLinkage(LLVMIRParser::ExternalLinkageContext *context) = 0;

    virtual std::any visitInternalLinkage(LLVMIRParser::InternalLinkageContext *context) = 0;

    virtual std::any visitLinkage(LLVMIRParser::LinkageContext *context) = 0;

    virtual std::any visitPreemption(LLVMIRParser::PreemptionContext *context) = 0;

    virtual std::any visitVisibility(LLVMIRParser::VisibilityContext *context) = 0;

    virtual std::any visitDllStorageClass(LLVMIRParser::DllStorageClassContext *context) = 0;

    virtual std::any visitTlsModel(LLVMIRParser::TlsModelContext *context) = 0;

    virtual std::any visitUnnamedAddr(LLVMIRParser::UnnamedAddrContext *context) = 0;

    virtual std::any visitExternallyInitialized(LLVMIRParser::ExternallyInitializedContext *context) = 0;

    virtual std::any visitImmutable(LLVMIRParser::ImmutableContext *context) = 0;

    virtual std::any visitFuncAttr(LLVMIRParser::FuncAttrContext *context) = 0;

    virtual std::any visitDistinct(LLVMIRParser::DistinctContext *context) = 0;

    virtual std::any visitInBounds(LLVMIRParser::InBoundsContext *context) = 0;

    virtual std::any visitReturnAttr(LLVMIRParser::ReturnAttrContext *context) = 0;

    virtual std::any visitOverflowFlag(LLVMIRParser::OverflowFlagContext *context) = 0;

    virtual std::any visitIPred(LLVMIRParser::IPredContext *context) = 0;

    virtual std::any visitFPred(LLVMIRParser::FPredContext *context) = 0;

    virtual std::any visitAtomicOrdering(LLVMIRParser::AtomicOrderingContext *context) = 0;

    virtual std::any visitCallingConvEnum(LLVMIRParser::CallingConvEnumContext *context) = 0;

    virtual std::any visitFastMathFlag(LLVMIRParser::FastMathFlagContext *context) = 0;

    virtual std::any visitAtomicOp(LLVMIRParser::AtomicOpContext *context) = 0;

    virtual std::any visitFloatKind(LLVMIRParser::FloatKindContext *context) = 0;

    virtual std::any visitSpecializedMDNode(LLVMIRParser::SpecializedMDNodeContext *context) = 0;

    virtual std::any visitDiBasicType(LLVMIRParser::DiBasicTypeContext *context) = 0;

    virtual std::any visitDiCommonBlock(LLVMIRParser::DiCommonBlockContext *context) = 0;

    virtual std::any visitDiCompileUnit(LLVMIRParser::DiCompileUnitContext *context) = 0;

    virtual std::any visitDiCompositeType(LLVMIRParser::DiCompositeTypeContext *context) = 0;

    virtual std::any visitDiCompositeTypeField(LLVMIRParser::DiCompositeTypeFieldContext *context) = 0;

    virtual std::any visitDiDerivedType(LLVMIRParser::DiDerivedTypeContext *context) = 0;

    virtual std::any visitDiDerivedTypeField(LLVMIRParser::DiDerivedTypeFieldContext *context) = 0;

    virtual std::any visitDiEnumerator(LLVMIRParser::DiEnumeratorContext *context) = 0;

    virtual std::any visitDiEnumeratorField(LLVMIRParser::DiEnumeratorFieldContext *context) = 0;

    virtual std::any visitDiFile(LLVMIRParser::DiFileContext *context) = 0;

    virtual std::any visitDiFileField(LLVMIRParser::DiFileFieldContext *context) = 0;

    virtual std::any visitDiGlobalVariable(LLVMIRParser::DiGlobalVariableContext *context) = 0;

    virtual std::any visitDiGlobalVariableField(LLVMIRParser::DiGlobalVariableFieldContext *context) = 0;

    virtual std::any visitDiGlobalVariableExpression(LLVMIRParser::DiGlobalVariableExpressionContext *context) = 0;

    virtual std::any visitDiGlobalVariableExpressionField(LLVMIRParser::DiGlobalVariableExpressionFieldContext *context) = 0;

    virtual std::any visitDiImportedEntity(LLVMIRParser::DiImportedEntityContext *context) = 0;

    virtual std::any visitDiImportedEntityField(LLVMIRParser::DiImportedEntityFieldContext *context) = 0;

    virtual std::any visitDiLabel(LLVMIRParser::DiLabelContext *context) = 0;

    virtual std::any visitDiLabelField(LLVMIRParser::DiLabelFieldContext *context) = 0;

    virtual std::any visitDiLexicalBlock(LLVMIRParser::DiLexicalBlockContext *context) = 0;

    virtual std::any visitDiLexicalBlockField(LLVMIRParser::DiLexicalBlockFieldContext *context) = 0;

    virtual std::any visitDiLexicalBlockFile(LLVMIRParser::DiLexicalBlockFileContext *context) = 0;

    virtual std::any visitDiLexicalBlockFileField(LLVMIRParser::DiLexicalBlockFileFieldContext *context) = 0;

    virtual std::any visitDiLocalVariable(LLVMIRParser::DiLocalVariableContext *context) = 0;

    virtual std::any visitDiLocalVariableField(LLVMIRParser::DiLocalVariableFieldContext *context) = 0;

    virtual std::any visitDiLocation(LLVMIRParser::DiLocationContext *context) = 0;

    virtual std::any visitDiLocationField(LLVMIRParser::DiLocationFieldContext *context) = 0;

    virtual std::any visitDiMacro(LLVMIRParser::DiMacroContext *context) = 0;

    virtual std::any visitDiMacroField(LLVMIRParser::DiMacroFieldContext *context) = 0;

    virtual std::any visitDiMacroFile(LLVMIRParser::DiMacroFileContext *context) = 0;

    virtual std::any visitDiMacroFileField(LLVMIRParser::DiMacroFileFieldContext *context) = 0;

    virtual std::any visitDiModule(LLVMIRParser::DiModuleContext *context) = 0;

    virtual std::any visitDiModuleField(LLVMIRParser::DiModuleFieldContext *context) = 0;

    virtual std::any visitDiNamespace(LLVMIRParser::DiNamespaceContext *context) = 0;

    virtual std::any visitDiNamespaceField(LLVMIRParser::DiNamespaceFieldContext *context) = 0;

    virtual std::any visitDiObjCProperty(LLVMIRParser::DiObjCPropertyContext *context) = 0;

    virtual std::any visitDiObjCPropertyField(LLVMIRParser::DiObjCPropertyFieldContext *context) = 0;

    virtual std::any visitDiStringType(LLVMIRParser::DiStringTypeContext *context) = 0;

    virtual std::any visitDiStringTypeField(LLVMIRParser::DiStringTypeFieldContext *context) = 0;

    virtual std::any visitDiSubprogram(LLVMIRParser::DiSubprogramContext *context) = 0;

    virtual std::any visitDiSubprogramField(LLVMIRParser::DiSubprogramFieldContext *context) = 0;

    virtual std::any visitDiSubrange(LLVMIRParser::DiSubrangeContext *context) = 0;

    virtual std::any visitDiSubrangeField(LLVMIRParser::DiSubrangeFieldContext *context) = 0;

    virtual std::any visitDiSubroutineType(LLVMIRParser::DiSubroutineTypeContext *context) = 0;

    virtual std::any visitDiTemplateTypeParameter(LLVMIRParser::DiTemplateTypeParameterContext *context) = 0;

    virtual std::any visitDiTemplateValueParameter(LLVMIRParser::DiTemplateValueParameterContext *context) = 0;

    virtual std::any visitGenericDiNode(LLVMIRParser::GenericDiNodeContext *context) = 0;

    virtual std::any visitDiTemplateTypeParameterField(LLVMIRParser::DiTemplateTypeParameterFieldContext *context) = 0;

    virtual std::any visitDiCompileUnitField(LLVMIRParser::DiCompileUnitFieldContext *context) = 0;

    virtual std::any visitDiCommonBlockField(LLVMIRParser::DiCommonBlockFieldContext *context) = 0;

    virtual std::any visitDiBasicTypeField(LLVMIRParser::DiBasicTypeFieldContext *context) = 0;

    virtual std::any visitGenericDINodeField(LLVMIRParser::GenericDINodeFieldContext *context) = 0;

    virtual std::any visitTagField(LLVMIRParser::TagFieldContext *context) = 0;

    virtual std::any visitHeaderField(LLVMIRParser::HeaderFieldContext *context) = 0;

    virtual std::any visitOperandsField(LLVMIRParser::OperandsFieldContext *context) = 0;

    virtual std::any visitDiTemplateValueParameterField(LLVMIRParser::DiTemplateValueParameterFieldContext *context) = 0;

    virtual std::any visitNameField(LLVMIRParser::NameFieldContext *context) = 0;

    virtual std::any visitTypeField(LLVMIRParser::TypeFieldContext *context) = 0;

    virtual std::any visitDefaultedField(LLVMIRParser::DefaultedFieldContext *context) = 0;

    virtual std::any visitValueField(LLVMIRParser::ValueFieldContext *context) = 0;

    virtual std::any visitMdField(LLVMIRParser::MdFieldContext *context) = 0;

    virtual std::any visitDiSubroutineTypeField(LLVMIRParser::DiSubroutineTypeFieldContext *context) = 0;

    virtual std::any visitFlagsField(LLVMIRParser::FlagsFieldContext *context) = 0;

    virtual std::any visitDiFlags(LLVMIRParser::DiFlagsContext *context) = 0;

    virtual std::any visitCcField(LLVMIRParser::CcFieldContext *context) = 0;

    virtual std::any visitAlignField(LLVMIRParser::AlignFieldContext *context) = 0;

    virtual std::any visitAllocatedField(LLVMIRParser::AllocatedFieldContext *context) = 0;

    virtual std::any visitAnnotationsField(LLVMIRParser::AnnotationsFieldContext *context) = 0;

    virtual std::any visitArgField(LLVMIRParser::ArgFieldContext *context) = 0;

    virtual std::any visitAssociatedField(LLVMIRParser::AssociatedFieldContext *context) = 0;

    virtual std::any visitAttributesField(LLVMIRParser::AttributesFieldContext *context) = 0;

    virtual std::any visitBaseTypeField(LLVMIRParser::BaseTypeFieldContext *context) = 0;

    virtual std::any visitChecksumField(LLVMIRParser::ChecksumFieldContext *context) = 0;

    virtual std::any visitChecksumkindField(LLVMIRParser::ChecksumkindFieldContext *context) = 0;

    virtual std::any visitColumnField(LLVMIRParser::ColumnFieldContext *context) = 0;

    virtual std::any visitConfigMacrosField(LLVMIRParser::ConfigMacrosFieldContext *context) = 0;

    virtual std::any visitContainingTypeField(LLVMIRParser::ContainingTypeFieldContext *context) = 0;

    virtual std::any visitCountField(LLVMIRParser::CountFieldContext *context) = 0;

    virtual std::any visitDebugInfoForProfilingField(LLVMIRParser::DebugInfoForProfilingFieldContext *context) = 0;

    virtual std::any visitDeclarationField(LLVMIRParser::DeclarationFieldContext *context) = 0;

    virtual std::any visitDirectoryField(LLVMIRParser::DirectoryFieldContext *context) = 0;

    virtual std::any visitDiscriminatorField(LLVMIRParser::DiscriminatorFieldContext *context) = 0;

    virtual std::any visitDataLocationField(LLVMIRParser::DataLocationFieldContext *context) = 0;

    virtual std::any visitDiscriminatorIntField(LLVMIRParser::DiscriminatorIntFieldContext *context) = 0;

    virtual std::any visitDwarfAddressSpaceField(LLVMIRParser::DwarfAddressSpaceFieldContext *context) = 0;

    virtual std::any visitDwoIdField(LLVMIRParser::DwoIdFieldContext *context) = 0;

    virtual std::any visitElementsField(LLVMIRParser::ElementsFieldContext *context) = 0;

    virtual std::any visitEmissionKindField(LLVMIRParser::EmissionKindFieldContext *context) = 0;

    virtual std::any visitEncodingField(LLVMIRParser::EncodingFieldContext *context) = 0;

    virtual std::any visitEntityField(LLVMIRParser::EntityFieldContext *context) = 0;

    virtual std::any visitEnumsField(LLVMIRParser::EnumsFieldContext *context) = 0;

    virtual std::any visitExportSymbolsField(LLVMIRParser::ExportSymbolsFieldContext *context) = 0;

    virtual std::any visitExprField(LLVMIRParser::ExprFieldContext *context) = 0;

    virtual std::any visitExtraDataField(LLVMIRParser::ExtraDataFieldContext *context) = 0;

    virtual std::any visitFileField(LLVMIRParser::FileFieldContext *context) = 0;

    virtual std::any visitFilenameField(LLVMIRParser::FilenameFieldContext *context) = 0;

    virtual std::any visitFlagsStringField(LLVMIRParser::FlagsStringFieldContext *context) = 0;

    virtual std::any visitGetterField(LLVMIRParser::GetterFieldContext *context) = 0;

    virtual std::any visitGlobalsField(LLVMIRParser::GlobalsFieldContext *context) = 0;

    virtual std::any visitIdentifierField(LLVMIRParser::IdentifierFieldContext *context) = 0;

    virtual std::any visitImportsField(LLVMIRParser::ImportsFieldContext *context) = 0;

    virtual std::any visitIncludePathField(LLVMIRParser::IncludePathFieldContext *context) = 0;

    virtual std::any visitInlinedAtField(LLVMIRParser::InlinedAtFieldContext *context) = 0;

    virtual std::any visitIsDeclField(LLVMIRParser::IsDeclFieldContext *context) = 0;

    virtual std::any visitIsDefinitionField(LLVMIRParser::IsDefinitionFieldContext *context) = 0;

    virtual std::any visitIsImplicitCodeField(LLVMIRParser::IsImplicitCodeFieldContext *context) = 0;

    virtual std::any visitIsLocalField(LLVMIRParser::IsLocalFieldContext *context) = 0;

    virtual std::any visitIsOptimizedField(LLVMIRParser::IsOptimizedFieldContext *context) = 0;

    virtual std::any visitIsUnsignedField(LLVMIRParser::IsUnsignedFieldContext *context) = 0;

    virtual std::any visitApiNotesField(LLVMIRParser::ApiNotesFieldContext *context) = 0;

    virtual std::any visitLanguageField(LLVMIRParser::LanguageFieldContext *context) = 0;

    virtual std::any visitLineField(LLVMIRParser::LineFieldContext *context) = 0;

    virtual std::any visitLinkageNameField(LLVMIRParser::LinkageNameFieldContext *context) = 0;

    virtual std::any visitLowerBoundField(LLVMIRParser::LowerBoundFieldContext *context) = 0;

    virtual std::any visitMacrosField(LLVMIRParser::MacrosFieldContext *context) = 0;

    virtual std::any visitNameTableKindField(LLVMIRParser::NameTableKindFieldContext *context) = 0;

    virtual std::any visitNodesField(LLVMIRParser::NodesFieldContext *context) = 0;

    virtual std::any visitOffsetField(LLVMIRParser::OffsetFieldContext *context) = 0;

    virtual std::any visitProducerField(LLVMIRParser::ProducerFieldContext *context) = 0;

    virtual std::any visitRangesBaseAddressField(LLVMIRParser::RangesBaseAddressFieldContext *context) = 0;

    virtual std::any visitRankField(LLVMIRParser::RankFieldContext *context) = 0;

    virtual std::any visitRetainedNodesField(LLVMIRParser::RetainedNodesFieldContext *context) = 0;

    virtual std::any visitRetainedTypesField(LLVMIRParser::RetainedTypesFieldContext *context) = 0;

    virtual std::any visitRuntimeLangField(LLVMIRParser::RuntimeLangFieldContext *context) = 0;

    virtual std::any visitRuntimeVersionField(LLVMIRParser::RuntimeVersionFieldContext *context) = 0;

    virtual std::any visitScopeField(LLVMIRParser::ScopeFieldContext *context) = 0;

    virtual std::any visitScopeLineField(LLVMIRParser::ScopeLineFieldContext *context) = 0;

    virtual std::any visitSdkField(LLVMIRParser::SdkFieldContext *context) = 0;

    virtual std::any visitSetterField(LLVMIRParser::SetterFieldContext *context) = 0;

    virtual std::any visitSizeField(LLVMIRParser::SizeFieldContext *context) = 0;

    virtual std::any visitSourceField(LLVMIRParser::SourceFieldContext *context) = 0;

    virtual std::any visitSpFlagsField(LLVMIRParser::SpFlagsFieldContext *context) = 0;

    virtual std::any visitSplitDebugFilenameField(LLVMIRParser::SplitDebugFilenameFieldContext *context) = 0;

    virtual std::any visitSplitDebugInliningField(LLVMIRParser::SplitDebugInliningFieldContext *context) = 0;

    virtual std::any visitStrideField(LLVMIRParser::StrideFieldContext *context) = 0;

    virtual std::any visitStringLengthField(LLVMIRParser::StringLengthFieldContext *context) = 0;

    virtual std::any visitStringLengthExpressionField(LLVMIRParser::StringLengthExpressionFieldContext *context) = 0;

    virtual std::any visitStringLocationExpressionField(LLVMIRParser::StringLocationExpressionFieldContext *context) = 0;

    virtual std::any visitSysrootField(LLVMIRParser::SysrootFieldContext *context) = 0;

    virtual std::any visitTargetFuncNameField(LLVMIRParser::TargetFuncNameFieldContext *context) = 0;

    virtual std::any visitTemplateParamsField(LLVMIRParser::TemplateParamsFieldContext *context) = 0;

    virtual std::any visitThisAdjustmentField(LLVMIRParser::ThisAdjustmentFieldContext *context) = 0;

    virtual std::any visitThrownTypesField(LLVMIRParser::ThrownTypesFieldContext *context) = 0;

    virtual std::any visitTypeMacinfoField(LLVMIRParser::TypeMacinfoFieldContext *context) = 0;

    virtual std::any visitTypesField(LLVMIRParser::TypesFieldContext *context) = 0;

    virtual std::any visitUnitField(LLVMIRParser::UnitFieldContext *context) = 0;

    virtual std::any visitUpperBoundField(LLVMIRParser::UpperBoundFieldContext *context) = 0;

    virtual std::any visitValueIntField(LLVMIRParser::ValueIntFieldContext *context) = 0;

    virtual std::any visitValueStringField(LLVMIRParser::ValueStringFieldContext *context) = 0;

    virtual std::any visitVarField(LLVMIRParser::VarFieldContext *context) = 0;

    virtual std::any visitVirtualIndexField(LLVMIRParser::VirtualIndexFieldContext *context) = 0;

    virtual std::any visitVirtualityField(LLVMIRParser::VirtualityFieldContext *context) = 0;

    virtual std::any visitVtableHolderField(LLVMIRParser::VtableHolderFieldContext *context) = 0;


};

}  // namespace antlr::generated
