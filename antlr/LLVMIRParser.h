
// Generated from /home/stash/code/llvm-i286/antlr/LLVMIR.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"


namespace antlr::generated {


class  LLVMIRParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, T__39 = 40, T__40 = 41, T__41 = 42, T__42 = 43, T__43 = 44, 
    T__44 = 45, T__45 = 46, T__46 = 47, T__47 = 48, T__48 = 49, T__49 = 50, 
    T__50 = 51, T__51 = 52, T__52 = 53, T__53 = 54, T__54 = 55, T__55 = 56, 
    T__56 = 57, T__57 = 58, T__58 = 59, T__59 = 60, T__60 = 61, T__61 = 62, 
    T__62 = 63, T__63 = 64, T__64 = 65, T__65 = 66, T__66 = 67, T__67 = 68, 
    T__68 = 69, T__69 = 70, T__70 = 71, T__71 = 72, T__72 = 73, T__73 = 74, 
    T__74 = 75, T__75 = 76, T__76 = 77, T__77 = 78, T__78 = 79, T__79 = 80, 
    T__80 = 81, T__81 = 82, T__82 = 83, T__83 = 84, T__84 = 85, T__85 = 86, 
    T__86 = 87, T__87 = 88, T__88 = 89, T__89 = 90, T__90 = 91, T__91 = 92, 
    T__92 = 93, T__93 = 94, T__94 = 95, T__95 = 96, T__96 = 97, T__97 = 98, 
    T__98 = 99, T__99 = 100, T__100 = 101, T__101 = 102, T__102 = 103, T__103 = 104, 
    T__104 = 105, T__105 = 106, T__106 = 107, T__107 = 108, T__108 = 109, 
    T__109 = 110, T__110 = 111, T__111 = 112, T__112 = 113, T__113 = 114, 
    T__114 = 115, T__115 = 116, T__116 = 117, T__117 = 118, T__118 = 119, 
    T__119 = 120, T__120 = 121, T__121 = 122, T__122 = 123, T__123 = 124, 
    T__124 = 125, T__125 = 126, T__126 = 127, T__127 = 128, T__128 = 129, 
    T__129 = 130, T__130 = 131, T__131 = 132, T__132 = 133, T__133 = 134, 
    T__134 = 135, T__135 = 136, T__136 = 137, T__137 = 138, T__138 = 139, 
    T__139 = 140, T__140 = 141, T__141 = 142, T__142 = 143, T__143 = 144, 
    T__144 = 145, T__145 = 146, T__146 = 147, T__147 = 148, T__148 = 149, 
    T__149 = 150, T__150 = 151, T__151 = 152, T__152 = 153, T__153 = 154, 
    T__154 = 155, T__155 = 156, T__156 = 157, T__157 = 158, T__158 = 159, 
    T__159 = 160, T__160 = 161, T__161 = 162, T__162 = 163, T__163 = 164, 
    T__164 = 165, T__165 = 166, T__166 = 167, T__167 = 168, T__168 = 169, 
    T__169 = 170, T__170 = 171, T__171 = 172, T__172 = 173, T__173 = 174, 
    T__174 = 175, T__175 = 176, T__176 = 177, T__177 = 178, T__178 = 179, 
    T__179 = 180, T__180 = 181, T__181 = 182, T__182 = 183, T__183 = 184, 
    T__184 = 185, T__185 = 186, T__186 = 187, T__187 = 188, T__188 = 189, 
    T__189 = 190, T__190 = 191, T__191 = 192, T__192 = 193, T__193 = 194, 
    T__194 = 195, T__195 = 196, T__196 = 197, T__197 = 198, T__198 = 199, 
    T__199 = 200, T__200 = 201, T__201 = 202, T__202 = 203, T__203 = 204, 
    T__204 = 205, T__205 = 206, T__206 = 207, T__207 = 208, T__208 = 209, 
    T__209 = 210, T__210 = 211, T__211 = 212, T__212 = 213, T__213 = 214, 
    T__214 = 215, T__215 = 216, T__216 = 217, T__217 = 218, T__218 = 219, 
    T__219 = 220, T__220 = 221, T__221 = 222, T__222 = 223, T__223 = 224, 
    T__224 = 225, T__225 = 226, T__226 = 227, T__227 = 228, T__228 = 229, 
    T__229 = 230, T__230 = 231, T__231 = 232, T__232 = 233, T__233 = 234, 
    T__234 = 235, T__235 = 236, T__236 = 237, T__237 = 238, T__238 = 239, 
    T__239 = 240, T__240 = 241, T__241 = 242, T__242 = 243, T__243 = 244, 
    T__244 = 245, T__245 = 246, T__246 = 247, T__247 = 248, T__248 = 249, 
    T__249 = 250, T__250 = 251, T__251 = 252, T__252 = 253, T__253 = 254, 
    T__254 = 255, T__255 = 256, T__256 = 257, T__257 = 258, T__258 = 259, 
    T__259 = 260, T__260 = 261, T__261 = 262, T__262 = 263, T__263 = 264, 
    T__264 = 265, T__265 = 266, T__266 = 267, T__267 = 268, T__268 = 269, 
    T__269 = 270, T__270 = 271, T__271 = 272, T__272 = 273, T__273 = 274, 
    T__274 = 275, T__275 = 276, T__276 = 277, T__277 = 278, T__278 = 279, 
    T__279 = 280, T__280 = 281, T__281 = 282, T__282 = 283, T__283 = 284, 
    T__284 = 285, T__285 = 286, T__286 = 287, T__287 = 288, T__288 = 289, 
    T__289 = 290, T__290 = 291, T__291 = 292, T__292 = 293, T__293 = 294, 
    T__294 = 295, T__295 = 296, T__296 = 297, T__297 = 298, T__298 = 299, 
    T__299 = 300, T__300 = 301, T__301 = 302, T__302 = 303, T__303 = 304, 
    T__304 = 305, T__305 = 306, T__306 = 307, T__307 = 308, T__308 = 309, 
    T__309 = 310, T__310 = 311, T__311 = 312, T__312 = 313, T__313 = 314, 
    T__314 = 315, T__315 = 316, T__316 = 317, T__317 = 318, T__318 = 319, 
    T__319 = 320, T__320 = 321, T__321 = 322, T__322 = 323, T__323 = 324, 
    T__324 = 325, T__325 = 326, T__326 = 327, T__327 = 328, T__328 = 329, 
    T__329 = 330, T__330 = 331, T__331 = 332, T__332 = 333, T__333 = 334, 
    T__334 = 335, T__335 = 336, T__336 = 337, T__337 = 338, T__338 = 339, 
    T__339 = 340, T__340 = 341, T__341 = 342, T__342 = 343, T__343 = 344, 
    T__344 = 345, T__345 = 346, T__346 = 347, T__347 = 348, T__348 = 349, 
    T__349 = 350, T__350 = 351, T__351 = 352, T__352 = 353, T__353 = 354, 
    T__354 = 355, T__355 = 356, T__356 = 357, T__357 = 358, T__358 = 359, 
    T__359 = 360, T__360 = 361, T__361 = 362, T__362 = 363, T__363 = 364, 
    T__364 = 365, T__365 = 366, T__366 = 367, T__367 = 368, T__368 = 369, 
    T__369 = 370, T__370 = 371, T__371 = 372, T__372 = 373, T__373 = 374, 
    T__374 = 375, T__375 = 376, T__376 = 377, T__377 = 378, T__378 = 379, 
    T__379 = 380, T__380 = 381, T__381 = 382, T__382 = 383, T__383 = 384, 
    T__384 = 385, T__385 = 386, T__386 = 387, T__387 = 388, T__388 = 389, 
    T__389 = 390, T__390 = 391, T__391 = 392, T__392 = 393, T__393 = 394, 
    T__394 = 395, T__395 = 396, T__396 = 397, T__397 = 398, T__398 = 399, 
    T__399 = 400, T__400 = 401, T__401 = 402, T__402 = 403, T__403 = 404, 
    T__404 = 405, T__405 = 406, T__406 = 407, T__407 = 408, T__408 = 409, 
    T__409 = 410, T__410 = 411, T__411 = 412, T__412 = 413, T__413 = 414, 
    T__414 = 415, T__415 = 416, T__416 = 417, T__417 = 418, T__418 = 419, 
    T__419 = 420, T__420 = 421, T__421 = 422, T__422 = 423, T__423 = 424, 
    T__424 = 425, T__425 = 426, T__426 = 427, T__427 = 428, T__428 = 429, 
    T__429 = 430, T__430 = 431, T__431 = 432, T__432 = 433, T__433 = 434, 
    T__434 = 435, T__435 = 436, T__436 = 437, T__437 = 438, T__438 = 439, 
    T__439 = 440, T__440 = 441, T__441 = 442, T__442 = 443, T__443 = 444, 
    T__444 = 445, T__445 = 446, T__446 = 447, T__447 = 448, T__448 = 449, 
    T__449 = 450, T__450 = 451, T__451 = 452, T__452 = 453, T__453 = 454, 
    T__454 = 455, T__455 = 456, T__456 = 457, T__457 = 458, T__458 = 459, 
    T__459 = 460, T__460 = 461, T__461 = 462, T__462 = 463, T__463 = 464, 
    T__464 = 465, T__465 = 466, T__466 = 467, T__467 = 468, T__468 = 469, 
    T__469 = 470, T__470 = 471, T__471 = 472, T__472 = 473, T__473 = 474, 
    T__474 = 475, T__475 = 476, T__476 = 477, T__477 = 478, T__478 = 479, 
    T__479 = 480, T__480 = 481, T__481 = 482, T__482 = 483, T__483 = 484, 
    T__484 = 485, T__485 = 486, T__486 = 487, Comment = 488, WhiteSpace = 489, 
    IntLit = 490, FloatLit = 491, StringLit = 492, GlobalIdent = 493, LocalIdent = 494, 
    LabelIdent = 495, AttrGroupId = 496, ComdatName = 497, MetadataName = 498, 
    MetadataId = 499, IntType = 500, DwarfTag = 501, DwarfAttEncoding = 502, 
    DiFlag = 503, DispFlag = 504, DwarfLang = 505, DwarfCc = 506, ChecksumKind = 507, 
    DwarfVirtuality = 508, DwarfMacinfo = 509, DwarfOp = 510
  };

  enum {
    RuleCompilationUnit = 0, RuleTargetDef = 1, RuleSourceFilename = 2, 
    RuleTargetDataLayout = 3, RuleTargetTriple = 4, RuleTopLevelEntity = 5, 
    RuleModuleAsm = 6, RuleTypeDef = 7, RuleComdatDef = 8, RuleGlobalDecl = 9, 
    RuleGlobalDef = 10, RuleIndirectSymbolDef = 11, RuleFuncDecl = 12, RuleFuncDef = 13, 
    RuleAttrGroupDef = 14, RuleNamedMetadataDef = 15, RuleMetadataDef = 16, 
    RuleUseListOrder = 17, RuleUseListOrderBB = 18, RuleFuncHeader = 19, 
    RuleIndirectSymbol = 20, RuleCallingConv = 21, RuleCallingConvInt = 22, 
    RuleFuncHdrField = 23, RuleGc = 24, RulePrefix = 25, RulePrologue = 26, 
    RulePersonality = 27, RuleReturnAttribute = 28, RuleFuncBody = 29, RuleBasicBlock = 30, 
    RuleInstruction = 31, RuleTerminator = 32, RuleLocalDefTerm = 33, RuleValueTerminator = 34, 
    RuleRetTerm = 35, RuleBrTerm = 36, RuleCondBrTerm = 37, RuleSwitchTerm = 38, 
    RuleIndirectBrTerm = 39, RuleResumeTerm = 40, RuleCatchRetTerm = 41, 
    RuleCleanupRetTerm = 42, RuleUnreachableTerm = 43, RuleInvokeTerm = 44, 
    RuleCallBrTerm = 45, RuleCatchSwitchTerm = 46, RuleLabel = 47, RuleCase = 48, 
    RuleUnwindTarget = 49, RuleHandlers = 50, RuleMetadataNode = 51, RuleDiExpression = 52, 
    RuleDiExpressionField = 53, RuleGlobalField = 54, RuleSection = 55, 
    RuleComdat = 56, RulePartition = 57, RuleConstant = 58, RuleBoolConst = 59, 
    RuleIntConst = 60, RuleFloatConst = 61, RuleNullConst = 62, RuleNoneConst = 63, 
    RuleStructConst = 64, RuleArrayConst = 65, RuleVectorConst = 66, RuleZeroInitializerConst = 67, 
    RuleUndefConst = 68, RulePoisonConst = 69, RuleBlockAddressConst = 70, 
    RuleDsoLocalEquivalentConst = 71, RuleNoCFIConst = 72, RuleConstantExpr = 73, 
    RuleTypeConst = 74, RuleMetadataAttachment = 75, RuleMdNode = 76, RuleMdTuple = 77, 
    RuleMetadata = 78, RuleDiArgList = 79, RuleTypeValue = 80, RuleValue = 81, 
    RuleInlineAsm = 82, RuleMdString = 83, RuleMdFieldOrInt = 84, RuleDiSPFlag = 85, 
    RuleFuncAttribute = 86, RuleType = 87, RuleParams = 88, RuleParam = 89, 
    RuleParamAttribute = 90, RuleAttrString = 91, RuleAttrPair = 92, RuleAlign = 93, 
    RuleAlignPair = 94, RuleAlignStack = 95, RuleAlignStackPair = 96, RuleAllocKind = 97, 
    RuleAllocSize = 98, RuleUnwindTable = 99, RuleVectorScaleRange = 100, 
    RuleByRefAttr = 101, RuleByval = 102, RuleDereferenceable = 103, RuleElementType = 104, 
    RuleInAlloca = 105, RuleParamAttr = 106, RulePreallocated = 107, RuleStructRetAttr = 108, 
    RuleFirstClassType = 109, RuleConcreteType = 110, RuleIntType = 111, 
    RuleFloatType = 112, RulePointerType = 113, RuleVectorType = 114, RuleLabelType = 115, 
    RuleArrayType = 116, RuleStructType = 117, RuleNamedType = 118, RuleMmxType = 119, 
    RuleTokenType = 120, RuleOpaquePointerType = 121, RuleAddrSpace = 122, 
    RuleThreadLocal = 123, RuleMetadataType = 124, RuleBitCastExpr = 125, 
    RuleGetElementPtrExpr = 126, RuleGepIndex = 127, RuleAddrSpaceCastExpr = 128, 
    RuleIntToPtrExpr = 129, RuleICmpExpr = 130, RuleFCmpExpr = 131, RuleSelectExpr = 132, 
    RuleTruncExpr = 133, RuleZExtExpr = 134, RuleSExtExpr = 135, RuleFpTruncExpr = 136, 
    RuleFpExtExpr = 137, RuleFpToUiExpr = 138, RuleFpToSiExpr = 139, RuleUiToFpExpr = 140, 
    RuleSiToFpExpr = 141, RulePtrToIntExpr = 142, RuleExtractElementExpr = 143, 
    RuleInsertElementExpr = 144, RuleShuffleVectorExpr = 145, RuleShlExpr = 146, 
    RuleLShrExpr = 147, RuleAShrExpr = 148, RuleAndExpr = 149, RuleOrExpr = 150, 
    RuleXorExpr = 151, RuleAddExpr = 152, RuleSubExpr = 153, RuleMulExpr = 154, 
    RuleFNegExpr = 155, RuleLocalDefInst = 156, RuleValueInstruction = 157, 
    RuleStoreInst = 158, RuleSyncScope = 159, RuleFenceInst = 160, RuleFNegInst = 161, 
    RuleAddInst = 162, RuleFAddInst = 163, RuleSubInst = 164, RuleFSubInst = 165, 
    RuleMulInst = 166, RuleFMulInst = 167, RuleUDivInst = 168, RuleSDivInst = 169, 
    RuleFDivInst = 170, RuleURemInst = 171, RuleSRemInst = 172, RuleFRemInst = 173, 
    RuleShlInst = 174, RuleLShrInst = 175, RuleAShrInst = 176, RuleAndInst = 177, 
    RuleOrInst = 178, RuleXorInst = 179, RuleExtractElementInst = 180, RuleInsertElementInst = 181, 
    RuleShuffleVectorInst = 182, RuleExtractValueInst = 183, RuleInsertValueInst = 184, 
    RuleAllocaInst = 185, RuleLoadInst = 186, RuleCmpXchgInst = 187, RuleAtomicRMWInst = 188, 
    RuleGetElementPtrInst = 189, RuleTruncInst = 190, RuleZExtInst = 191, 
    RuleSExtInst = 192, RuleFpTruncInst = 193, RuleFpExtInst = 194, RuleFpToUiInst = 195, 
    RuleFpToSiInst = 196, RuleUiToFpInst = 197, RuleSiToFpInst = 198, RulePtrToIntInst = 199, 
    RuleIntToPtrInst = 200, RuleBitCastInst = 201, RuleAddrSpaceCastInst = 202, 
    RuleICmpInst = 203, RuleFCmpInst = 204, RulePhiInst = 205, RuleSelectInst = 206, 
    RuleFreezeInst = 207, RuleCallInst = 208, RuleVaargInst = 209, RuleLandingPadInst = 210, 
    RuleCatchPadInst = 211, RuleCleanupPadInst = 212, RuleInc = 213, RuleOperandBundle = 214, 
    RuleClause = 215, RuleArgs = 216, RuleArg = 217, RuleExceptionArg = 218, 
    RuleExceptionPad = 219, RuleExternalLinkage = 220, RuleInternalLinkage = 221, 
    RuleLinkage = 222, RulePreemption = 223, RuleVisibility = 224, RuleDllStorageClass = 225, 
    RuleTlsModel = 226, RuleUnnamedAddr = 227, RuleExternallyInitialized = 228, 
    RuleImmutable = 229, RuleFuncAttr = 230, RuleDistinct = 231, RuleInBounds = 232, 
    RuleReturnAttr = 233, RuleOverflowFlag = 234, RuleIPred = 235, RuleFPred = 236, 
    RuleAtomicOrdering = 237, RuleCallingConvEnum = 238, RuleFastMathFlag = 239, 
    RuleAtomicOp = 240, RuleFloatKind = 241, RuleSpecializedMDNode = 242, 
    RuleDiBasicType = 243, RuleDiCommonBlock = 244, RuleDiCompileUnit = 245, 
    RuleDiCompositeType = 246, RuleDiCompositeTypeField = 247, RuleDiDerivedType = 248, 
    RuleDiDerivedTypeField = 249, RuleDiEnumerator = 250, RuleDiEnumeratorField = 251, 
    RuleDiFile = 252, RuleDiFileField = 253, RuleDiGlobalVariable = 254, 
    RuleDiGlobalVariableField = 255, RuleDiGlobalVariableExpression = 256, 
    RuleDiGlobalVariableExpressionField = 257, RuleDiImportedEntity = 258, 
    RuleDiImportedEntityField = 259, RuleDiLabel = 260, RuleDiLabelField = 261, 
    RuleDiLexicalBlock = 262, RuleDiLexicalBlockField = 263, RuleDiLexicalBlockFile = 264, 
    RuleDiLexicalBlockFileField = 265, RuleDiLocalVariable = 266, RuleDiLocalVariableField = 267, 
    RuleDiLocation = 268, RuleDiLocationField = 269, RuleDiMacro = 270, 
    RuleDiMacroField = 271, RuleDiMacroFile = 272, RuleDiMacroFileField = 273, 
    RuleDiModule = 274, RuleDiModuleField = 275, RuleDiNamespace = 276, 
    RuleDiNamespaceField = 277, RuleDiObjCProperty = 278, RuleDiObjCPropertyField = 279, 
    RuleDiStringType = 280, RuleDiStringTypeField = 281, RuleDiSubprogram = 282, 
    RuleDiSubprogramField = 283, RuleDiSubrange = 284, RuleDiSubrangeField = 285, 
    RuleDiSubroutineType = 286, RuleDiTemplateTypeParameter = 287, RuleDiTemplateValueParameter = 288, 
    RuleGenericDiNode = 289, RuleDiTemplateTypeParameterField = 290, RuleDiCompileUnitField = 291, 
    RuleDiCommonBlockField = 292, RuleDiBasicTypeField = 293, RuleGenericDINodeField = 294, 
    RuleTagField = 295, RuleHeaderField = 296, RuleOperandsField = 297, 
    RuleDiTemplateValueParameterField = 298, RuleNameField = 299, RuleTypeField = 300, 
    RuleDefaultedField = 301, RuleValueField = 302, RuleMdField = 303, RuleDiSubroutineTypeField = 304, 
    RuleFlagsField = 305, RuleDiFlags = 306, RuleCcField = 307, RuleAlignField = 308, 
    RuleAllocatedField = 309, RuleAnnotationsField = 310, RuleArgField = 311, 
    RuleAssociatedField = 312, RuleAttributesField = 313, RuleBaseTypeField = 314, 
    RuleChecksumField = 315, RuleChecksumkindField = 316, RuleColumnField = 317, 
    RuleConfigMacrosField = 318, RuleContainingTypeField = 319, RuleCountField = 320, 
    RuleDebugInfoForProfilingField = 321, RuleDeclarationField = 322, RuleDirectoryField = 323, 
    RuleDiscriminatorField = 324, RuleDataLocationField = 325, RuleDiscriminatorIntField = 326, 
    RuleDwarfAddressSpaceField = 327, RuleDwoIdField = 328, RuleElementsField = 329, 
    RuleEmissionKindField = 330, RuleEncodingField = 331, RuleEntityField = 332, 
    RuleEnumsField = 333, RuleExportSymbolsField = 334, RuleExprField = 335, 
    RuleExtraDataField = 336, RuleFileField = 337, RuleFilenameField = 338, 
    RuleFlagsStringField = 339, RuleGetterField = 340, RuleGlobalsField = 341, 
    RuleIdentifierField = 342, RuleImportsField = 343, RuleIncludePathField = 344, 
    RuleInlinedAtField = 345, RuleIsDeclField = 346, RuleIsDefinitionField = 347, 
    RuleIsImplicitCodeField = 348, RuleIsLocalField = 349, RuleIsOptimizedField = 350, 
    RuleIsUnsignedField = 351, RuleApiNotesField = 352, RuleLanguageField = 353, 
    RuleLineField = 354, RuleLinkageNameField = 355, RuleLowerBoundField = 356, 
    RuleMacrosField = 357, RuleNameTableKindField = 358, RuleNodesField = 359, 
    RuleOffsetField = 360, RuleProducerField = 361, RuleRangesBaseAddressField = 362, 
    RuleRankField = 363, RuleRetainedNodesField = 364, RuleRetainedTypesField = 365, 
    RuleRuntimeLangField = 366, RuleRuntimeVersionField = 367, RuleScopeField = 368, 
    RuleScopeLineField = 369, RuleSdkField = 370, RuleSetterField = 371, 
    RuleSizeField = 372, RuleSourceField = 373, RuleSpFlagsField = 374, 
    RuleSplitDebugFilenameField = 375, RuleSplitDebugInliningField = 376, 
    RuleStrideField = 377, RuleStringLengthField = 378, RuleStringLengthExpressionField = 379, 
    RuleStringLocationExpressionField = 380, RuleSysrootField = 381, RuleTargetFuncNameField = 382, 
    RuleTemplateParamsField = 383, RuleThisAdjustmentField = 384, RuleThrownTypesField = 385, 
    RuleTypeMacinfoField = 386, RuleTypesField = 387, RuleUnitField = 388, 
    RuleUpperBoundField = 389, RuleValueIntField = 390, RuleValueStringField = 391, 
    RuleVarField = 392, RuleVirtualIndexField = 393, RuleVirtualityField = 394, 
    RuleVtableHolderField = 395
  };

  explicit LLVMIRParser(antlr4::TokenStream *input);

  LLVMIRParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~LLVMIRParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class CompilationUnitContext;
  class TargetDefContext;
  class SourceFilenameContext;
  class TargetDataLayoutContext;
  class TargetTripleContext;
  class TopLevelEntityContext;
  class ModuleAsmContext;
  class TypeDefContext;
  class ComdatDefContext;
  class GlobalDeclContext;
  class GlobalDefContext;
  class IndirectSymbolDefContext;
  class FuncDeclContext;
  class FuncDefContext;
  class AttrGroupDefContext;
  class NamedMetadataDefContext;
  class MetadataDefContext;
  class UseListOrderContext;
  class UseListOrderBBContext;
  class FuncHeaderContext;
  class IndirectSymbolContext;
  class CallingConvContext;
  class CallingConvIntContext;
  class FuncHdrFieldContext;
  class GcContext;
  class PrefixContext;
  class PrologueContext;
  class PersonalityContext;
  class ReturnAttributeContext;
  class FuncBodyContext;
  class BasicBlockContext;
  class InstructionContext;
  class TerminatorContext;
  class LocalDefTermContext;
  class ValueTerminatorContext;
  class RetTermContext;
  class BrTermContext;
  class CondBrTermContext;
  class SwitchTermContext;
  class IndirectBrTermContext;
  class ResumeTermContext;
  class CatchRetTermContext;
  class CleanupRetTermContext;
  class UnreachableTermContext;
  class InvokeTermContext;
  class CallBrTermContext;
  class CatchSwitchTermContext;
  class LabelContext;
  class CaseContext;
  class UnwindTargetContext;
  class HandlersContext;
  class MetadataNodeContext;
  class DiExpressionContext;
  class DiExpressionFieldContext;
  class GlobalFieldContext;
  class SectionContext;
  class ComdatContext;
  class PartitionContext;
  class ConstantContext;
  class BoolConstContext;
  class IntConstContext;
  class FloatConstContext;
  class NullConstContext;
  class NoneConstContext;
  class StructConstContext;
  class ArrayConstContext;
  class VectorConstContext;
  class ZeroInitializerConstContext;
  class UndefConstContext;
  class PoisonConstContext;
  class BlockAddressConstContext;
  class DsoLocalEquivalentConstContext;
  class NoCFIConstContext;
  class ConstantExprContext;
  class TypeConstContext;
  class MetadataAttachmentContext;
  class MdNodeContext;
  class MdTupleContext;
  class MetadataContext;
  class DiArgListContext;
  class TypeValueContext;
  class ValueContext;
  class InlineAsmContext;
  class MdStringContext;
  class MdFieldOrIntContext;
  class DiSPFlagContext;
  class FuncAttributeContext;
  class TypeContext;
  class ParamsContext;
  class ParamContext;
  class ParamAttributeContext;
  class AttrStringContext;
  class AttrPairContext;
  class AlignContext;
  class AlignPairContext;
  class AlignStackContext;
  class AlignStackPairContext;
  class AllocKindContext;
  class AllocSizeContext;
  class UnwindTableContext;
  class VectorScaleRangeContext;
  class ByRefAttrContext;
  class ByvalContext;
  class DereferenceableContext;
  class ElementTypeContext;
  class InAllocaContext;
  class ParamAttrContext;
  class PreallocatedContext;
  class StructRetAttrContext;
  class FirstClassTypeContext;
  class ConcreteTypeContext;
  class IntTypeContext;
  class FloatTypeContext;
  class PointerTypeContext;
  class VectorTypeContext;
  class LabelTypeContext;
  class ArrayTypeContext;
  class StructTypeContext;
  class NamedTypeContext;
  class MmxTypeContext;
  class TokenTypeContext;
  class OpaquePointerTypeContext;
  class AddrSpaceContext;
  class ThreadLocalContext;
  class MetadataTypeContext;
  class BitCastExprContext;
  class GetElementPtrExprContext;
  class GepIndexContext;
  class AddrSpaceCastExprContext;
  class IntToPtrExprContext;
  class ICmpExprContext;
  class FCmpExprContext;
  class SelectExprContext;
  class TruncExprContext;
  class ZExtExprContext;
  class SExtExprContext;
  class FpTruncExprContext;
  class FpExtExprContext;
  class FpToUiExprContext;
  class FpToSiExprContext;
  class UiToFpExprContext;
  class SiToFpExprContext;
  class PtrToIntExprContext;
  class ExtractElementExprContext;
  class InsertElementExprContext;
  class ShuffleVectorExprContext;
  class ShlExprContext;
  class LShrExprContext;
  class AShrExprContext;
  class AndExprContext;
  class OrExprContext;
  class XorExprContext;
  class AddExprContext;
  class SubExprContext;
  class MulExprContext;
  class FNegExprContext;
  class LocalDefInstContext;
  class ValueInstructionContext;
  class StoreInstContext;
  class SyncScopeContext;
  class FenceInstContext;
  class FNegInstContext;
  class AddInstContext;
  class FAddInstContext;
  class SubInstContext;
  class FSubInstContext;
  class MulInstContext;
  class FMulInstContext;
  class UDivInstContext;
  class SDivInstContext;
  class FDivInstContext;
  class URemInstContext;
  class SRemInstContext;
  class FRemInstContext;
  class ShlInstContext;
  class LShrInstContext;
  class AShrInstContext;
  class AndInstContext;
  class OrInstContext;
  class XorInstContext;
  class ExtractElementInstContext;
  class InsertElementInstContext;
  class ShuffleVectorInstContext;
  class ExtractValueInstContext;
  class InsertValueInstContext;
  class AllocaInstContext;
  class LoadInstContext;
  class CmpXchgInstContext;
  class AtomicRMWInstContext;
  class GetElementPtrInstContext;
  class TruncInstContext;
  class ZExtInstContext;
  class SExtInstContext;
  class FpTruncInstContext;
  class FpExtInstContext;
  class FpToUiInstContext;
  class FpToSiInstContext;
  class UiToFpInstContext;
  class SiToFpInstContext;
  class PtrToIntInstContext;
  class IntToPtrInstContext;
  class BitCastInstContext;
  class AddrSpaceCastInstContext;
  class ICmpInstContext;
  class FCmpInstContext;
  class PhiInstContext;
  class SelectInstContext;
  class FreezeInstContext;
  class CallInstContext;
  class VaargInstContext;
  class LandingPadInstContext;
  class CatchPadInstContext;
  class CleanupPadInstContext;
  class IncContext;
  class OperandBundleContext;
  class ClauseContext;
  class ArgsContext;
  class ArgContext;
  class ExceptionArgContext;
  class ExceptionPadContext;
  class ExternalLinkageContext;
  class InternalLinkageContext;
  class LinkageContext;
  class PreemptionContext;
  class VisibilityContext;
  class DllStorageClassContext;
  class TlsModelContext;
  class UnnamedAddrContext;
  class ExternallyInitializedContext;
  class ImmutableContext;
  class FuncAttrContext;
  class DistinctContext;
  class InBoundsContext;
  class ReturnAttrContext;
  class OverflowFlagContext;
  class IPredContext;
  class FPredContext;
  class AtomicOrderingContext;
  class CallingConvEnumContext;
  class FastMathFlagContext;
  class AtomicOpContext;
  class FloatKindContext;
  class SpecializedMDNodeContext;
  class DiBasicTypeContext;
  class DiCommonBlockContext;
  class DiCompileUnitContext;
  class DiCompositeTypeContext;
  class DiCompositeTypeFieldContext;
  class DiDerivedTypeContext;
  class DiDerivedTypeFieldContext;
  class DiEnumeratorContext;
  class DiEnumeratorFieldContext;
  class DiFileContext;
  class DiFileFieldContext;
  class DiGlobalVariableContext;
  class DiGlobalVariableFieldContext;
  class DiGlobalVariableExpressionContext;
  class DiGlobalVariableExpressionFieldContext;
  class DiImportedEntityContext;
  class DiImportedEntityFieldContext;
  class DiLabelContext;
  class DiLabelFieldContext;
  class DiLexicalBlockContext;
  class DiLexicalBlockFieldContext;
  class DiLexicalBlockFileContext;
  class DiLexicalBlockFileFieldContext;
  class DiLocalVariableContext;
  class DiLocalVariableFieldContext;
  class DiLocationContext;
  class DiLocationFieldContext;
  class DiMacroContext;
  class DiMacroFieldContext;
  class DiMacroFileContext;
  class DiMacroFileFieldContext;
  class DiModuleContext;
  class DiModuleFieldContext;
  class DiNamespaceContext;
  class DiNamespaceFieldContext;
  class DiObjCPropertyContext;
  class DiObjCPropertyFieldContext;
  class DiStringTypeContext;
  class DiStringTypeFieldContext;
  class DiSubprogramContext;
  class DiSubprogramFieldContext;
  class DiSubrangeContext;
  class DiSubrangeFieldContext;
  class DiSubroutineTypeContext;
  class DiTemplateTypeParameterContext;
  class DiTemplateValueParameterContext;
  class GenericDiNodeContext;
  class DiTemplateTypeParameterFieldContext;
  class DiCompileUnitFieldContext;
  class DiCommonBlockFieldContext;
  class DiBasicTypeFieldContext;
  class GenericDINodeFieldContext;
  class TagFieldContext;
  class HeaderFieldContext;
  class OperandsFieldContext;
  class DiTemplateValueParameterFieldContext;
  class NameFieldContext;
  class TypeFieldContext;
  class DefaultedFieldContext;
  class ValueFieldContext;
  class MdFieldContext;
  class DiSubroutineTypeFieldContext;
  class FlagsFieldContext;
  class DiFlagsContext;
  class CcFieldContext;
  class AlignFieldContext;
  class AllocatedFieldContext;
  class AnnotationsFieldContext;
  class ArgFieldContext;
  class AssociatedFieldContext;
  class AttributesFieldContext;
  class BaseTypeFieldContext;
  class ChecksumFieldContext;
  class ChecksumkindFieldContext;
  class ColumnFieldContext;
  class ConfigMacrosFieldContext;
  class ContainingTypeFieldContext;
  class CountFieldContext;
  class DebugInfoForProfilingFieldContext;
  class DeclarationFieldContext;
  class DirectoryFieldContext;
  class DiscriminatorFieldContext;
  class DataLocationFieldContext;
  class DiscriminatorIntFieldContext;
  class DwarfAddressSpaceFieldContext;
  class DwoIdFieldContext;
  class ElementsFieldContext;
  class EmissionKindFieldContext;
  class EncodingFieldContext;
  class EntityFieldContext;
  class EnumsFieldContext;
  class ExportSymbolsFieldContext;
  class ExprFieldContext;
  class ExtraDataFieldContext;
  class FileFieldContext;
  class FilenameFieldContext;
  class FlagsStringFieldContext;
  class GetterFieldContext;
  class GlobalsFieldContext;
  class IdentifierFieldContext;
  class ImportsFieldContext;
  class IncludePathFieldContext;
  class InlinedAtFieldContext;
  class IsDeclFieldContext;
  class IsDefinitionFieldContext;
  class IsImplicitCodeFieldContext;
  class IsLocalFieldContext;
  class IsOptimizedFieldContext;
  class IsUnsignedFieldContext;
  class ApiNotesFieldContext;
  class LanguageFieldContext;
  class LineFieldContext;
  class LinkageNameFieldContext;
  class LowerBoundFieldContext;
  class MacrosFieldContext;
  class NameTableKindFieldContext;
  class NodesFieldContext;
  class OffsetFieldContext;
  class ProducerFieldContext;
  class RangesBaseAddressFieldContext;
  class RankFieldContext;
  class RetainedNodesFieldContext;
  class RetainedTypesFieldContext;
  class RuntimeLangFieldContext;
  class RuntimeVersionFieldContext;
  class ScopeFieldContext;
  class ScopeLineFieldContext;
  class SdkFieldContext;
  class SetterFieldContext;
  class SizeFieldContext;
  class SourceFieldContext;
  class SpFlagsFieldContext;
  class SplitDebugFilenameFieldContext;
  class SplitDebugInliningFieldContext;
  class StrideFieldContext;
  class StringLengthFieldContext;
  class StringLengthExpressionFieldContext;
  class StringLocationExpressionFieldContext;
  class SysrootFieldContext;
  class TargetFuncNameFieldContext;
  class TemplateParamsFieldContext;
  class ThisAdjustmentFieldContext;
  class ThrownTypesFieldContext;
  class TypeMacinfoFieldContext;
  class TypesFieldContext;
  class UnitFieldContext;
  class UpperBoundFieldContext;
  class ValueIntFieldContext;
  class ValueStringFieldContext;
  class VarFieldContext;
  class VirtualIndexFieldContext;
  class VirtualityFieldContext;
  class VtableHolderFieldContext; 

  class  CompilationUnitContext : public antlr4::ParserRuleContext {
  public:
    CompilationUnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<TopLevelEntityContext *> topLevelEntity();
    TopLevelEntityContext* topLevelEntity(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CompilationUnitContext* compilationUnit();

  class  TargetDefContext : public antlr4::ParserRuleContext {
  public:
    TargetDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetDataLayoutContext *targetDataLayout();
    TargetTripleContext *targetTriple();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TargetDefContext* targetDef();

  class  SourceFilenameContext : public antlr4::ParserRuleContext {
  public:
    SourceFilenameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SourceFilenameContext* sourceFilename();

  class  TargetDataLayoutContext : public antlr4::ParserRuleContext {
  public:
    TargetDataLayoutContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TargetDataLayoutContext* targetDataLayout();

  class  TargetTripleContext : public antlr4::ParserRuleContext {
  public:
    TargetTripleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TargetTripleContext* targetTriple();

  class  TopLevelEntityContext : public antlr4::ParserRuleContext {
  public:
    TopLevelEntityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SourceFilenameContext *sourceFilename();
    TargetDefContext *targetDef();
    ModuleAsmContext *moduleAsm();
    TypeDefContext *typeDef();
    ComdatDefContext *comdatDef();
    GlobalDeclContext *globalDecl();
    GlobalDefContext *globalDef();
    IndirectSymbolDefContext *indirectSymbolDef();
    FuncDeclContext *funcDecl();
    FuncDefContext *funcDef();
    AttrGroupDefContext *attrGroupDef();
    NamedMetadataDefContext *namedMetadataDef();
    MetadataDefContext *metadataDef();
    UseListOrderContext *useListOrder();
    UseListOrderBBContext *useListOrderBB();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TopLevelEntityContext* topLevelEntity();

  class  ModuleAsmContext : public antlr4::ParserRuleContext {
  public:
    ModuleAsmContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ModuleAsmContext* moduleAsm();

  class  TypeDefContext : public antlr4::ParserRuleContext {
  public:
    TypeDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LocalIdent();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeDefContext* typeDef();

  class  ComdatDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *selectionKind = nullptr;
    ComdatDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ComdatName();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ComdatDefContext* comdatDef();

  class  GlobalDeclContext : public antlr4::ParserRuleContext {
  public:
    GlobalDeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GlobalIdent();
    ExternalLinkageContext *externalLinkage();
    ImmutableContext *immutable();
    TypeContext *type();
    PreemptionContext *preemption();
    VisibilityContext *visibility();
    DllStorageClassContext *dllStorageClass();
    ThreadLocalContext *threadLocal();
    UnnamedAddrContext *unnamedAddr();
    AddrSpaceContext *addrSpace();
    ExternallyInitializedContext *externallyInitialized();
    std::vector<GlobalFieldContext *> globalField();
    GlobalFieldContext* globalField(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);
    std::vector<FuncAttributeContext *> funcAttribute();
    FuncAttributeContext* funcAttribute(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GlobalDeclContext* globalDecl();

  class  GlobalDefContext : public antlr4::ParserRuleContext {
  public:
    GlobalDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GlobalIdent();
    ImmutableContext *immutable();
    TypeContext *type();
    ConstantContext *constant();
    InternalLinkageContext *internalLinkage();
    PreemptionContext *preemption();
    VisibilityContext *visibility();
    DllStorageClassContext *dllStorageClass();
    ThreadLocalContext *threadLocal();
    UnnamedAddrContext *unnamedAddr();
    AddrSpaceContext *addrSpace();
    ExternallyInitializedContext *externallyInitialized();
    std::vector<GlobalFieldContext *> globalField();
    GlobalFieldContext* globalField(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);
    std::vector<FuncAttributeContext *> funcAttribute();
    FuncAttributeContext* funcAttribute(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GlobalDefContext* globalDef();

  class  IndirectSymbolDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *indirectSymbolKind = nullptr;
    IndirectSymbolDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GlobalIdent();
    TypeContext *type();
    IndirectSymbolContext *indirectSymbol();
    LinkageContext *linkage();
    PreemptionContext *preemption();
    VisibilityContext *visibility();
    DllStorageClassContext *dllStorageClass();
    ThreadLocalContext *threadLocal();
    UnnamedAddrContext *unnamedAddr();
    std::vector<PartitionContext *> partition();
    PartitionContext* partition(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IndirectSymbolDefContext* indirectSymbolDef();

  class  FuncDeclContext : public antlr4::ParserRuleContext {
  public:
    FuncDeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FuncHeaderContext *funcHeader();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncDeclContext* funcDecl();

  class  FuncDefContext : public antlr4::ParserRuleContext {
  public:
    FuncDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FuncHeaderContext *funcHeader();
    FuncBodyContext *funcBody();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncDefContext* funcDef();

  class  AttrGroupDefContext : public antlr4::ParserRuleContext {
  public:
    AttrGroupDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AttrGroupId();
    std::vector<FuncAttributeContext *> funcAttribute();
    FuncAttributeContext* funcAttribute(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttrGroupDefContext* attrGroupDef();

  class  NamedMetadataDefContext : public antlr4::ParserRuleContext {
  public:
    NamedMetadataDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MetadataName();
    std::vector<MetadataNodeContext *> metadataNode();
    MetadataNodeContext* metadataNode(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NamedMetadataDefContext* namedMetadataDef();

  class  MetadataDefContext : public antlr4::ParserRuleContext {
  public:
    MetadataDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MetadataId();
    MdTupleContext *mdTuple();
    SpecializedMDNodeContext *specializedMDNode();
    DistinctContext *distinct();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MetadataDefContext* metadataDef();

  class  UseListOrderContext : public antlr4::ParserRuleContext {
  public:
    UseListOrderContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    std::vector<antlr4::tree::TerminalNode *> IntLit();
    antlr4::tree::TerminalNode* IntLit(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UseListOrderContext* useListOrder();

  class  UseListOrderBBContext : public antlr4::ParserRuleContext {
  public:
    UseListOrderBBContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GlobalIdent();
    antlr4::tree::TerminalNode *LocalIdent();
    std::vector<antlr4::tree::TerminalNode *> IntLit();
    antlr4::tree::TerminalNode* IntLit(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UseListOrderBBContext* useListOrderBB();

  class  FuncHeaderContext : public antlr4::ParserRuleContext {
  public:
    FuncHeaderContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    antlr4::tree::TerminalNode *GlobalIdent();
    ParamsContext *params();
    LinkageContext *linkage();
    PreemptionContext *preemption();
    VisibilityContext *visibility();
    DllStorageClassContext *dllStorageClass();
    CallingConvContext *callingConv();
    std::vector<ReturnAttributeContext *> returnAttribute();
    ReturnAttributeContext* returnAttribute(size_t i);
    UnnamedAddrContext *unnamedAddr();
    AddrSpaceContext *addrSpace();
    std::vector<FuncHdrFieldContext *> funcHdrField();
    FuncHdrFieldContext* funcHdrField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncHeaderContext* funcHeader();

  class  IndirectSymbolContext : public antlr4::ParserRuleContext {
  public:
    IndirectSymbolContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    BitCastExprContext *bitCastExpr();
    GetElementPtrExprContext *getElementPtrExpr();
    AddrSpaceCastExprContext *addrSpaceCastExpr();
    IntToPtrExprContext *intToPtrExpr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IndirectSymbolContext* indirectSymbol();

  class  CallingConvContext : public antlr4::ParserRuleContext {
  public:
    CallingConvContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CallingConvEnumContext *callingConvEnum();
    CallingConvIntContext *callingConvInt();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CallingConvContext* callingConv();

  class  CallingConvIntContext : public antlr4::ParserRuleContext {
  public:
    CallingConvIntContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CallingConvIntContext* callingConvInt();

  class  FuncHdrFieldContext : public antlr4::ParserRuleContext {
  public:
    FuncHdrFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FuncAttributeContext *funcAttribute();
    SectionContext *section();
    PartitionContext *partition();
    ComdatContext *comdat();
    AlignContext *align();
    GcContext *gc();
    PrefixContext *prefix();
    PrologueContext *prologue();
    PersonalityContext *personality();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncHdrFieldContext* funcHdrField();

  class  GcContext : public antlr4::ParserRuleContext {
  public:
    GcContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GcContext* gc();

  class  PrefixContext : public antlr4::ParserRuleContext {
  public:
    PrefixContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PrefixContext* prefix();

  class  PrologueContext : public antlr4::ParserRuleContext {
  public:
    PrologueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PrologueContext* prologue();

  class  PersonalityContext : public antlr4::ParserRuleContext {
  public:
    PersonalityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PersonalityContext* personality();

  class  ReturnAttributeContext : public antlr4::ParserRuleContext {
  public:
    ReturnAttributeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ReturnAttrContext *returnAttr();
    DereferenceableContext *dereferenceable();
    AlignContext *align();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ReturnAttributeContext* returnAttribute();

  class  FuncBodyContext : public antlr4::ParserRuleContext {
  public:
    FuncBodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<BasicBlockContext *> basicBlock();
    BasicBlockContext* basicBlock(size_t i);
    std::vector<UseListOrderContext *> useListOrder();
    UseListOrderContext* useListOrder(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncBodyContext* funcBody();

  class  BasicBlockContext : public antlr4::ParserRuleContext {
  public:
    BasicBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TerminatorContext *terminator();
    antlr4::tree::TerminalNode *LabelIdent();
    std::vector<InstructionContext *> instruction();
    InstructionContext* instruction(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BasicBlockContext* basicBlock();

  class  InstructionContext : public antlr4::ParserRuleContext {
  public:
    InstructionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LocalDefInstContext *localDefInst();
    ValueInstructionContext *valueInstruction();
    StoreInstContext *storeInst();
    FenceInstContext *fenceInst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InstructionContext* instruction();

  class  TerminatorContext : public antlr4::ParserRuleContext {
  public:
    TerminatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LocalDefTermContext *localDefTerm();
    ValueTerminatorContext *valueTerminator();
    RetTermContext *retTerm();
    BrTermContext *brTerm();
    CondBrTermContext *condBrTerm();
    SwitchTermContext *switchTerm();
    IndirectBrTermContext *indirectBrTerm();
    ResumeTermContext *resumeTerm();
    CatchRetTermContext *catchRetTerm();
    CleanupRetTermContext *cleanupRetTerm();
    UnreachableTermContext *unreachableTerm();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TerminatorContext* terminator();

  class  LocalDefTermContext : public antlr4::ParserRuleContext {
  public:
    LocalDefTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LocalIdent();
    ValueTerminatorContext *valueTerminator();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LocalDefTermContext* localDefTerm();

  class  ValueTerminatorContext : public antlr4::ParserRuleContext {
  public:
    ValueTerminatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    InvokeTermContext *invokeTerm();
    CallBrTermContext *callBrTerm();
    CatchSwitchTermContext *catchSwitchTerm();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueTerminatorContext* valueTerminator();

  class  RetTermContext : public antlr4::ParserRuleContext {
  public:
    RetTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);
    ConcreteTypeContext *concreteType();
    ValueContext *value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RetTermContext* retTerm();

  class  BrTermContext : public antlr4::ParserRuleContext {
  public:
    BrTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LabelContext *label();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BrTermContext* brTerm();

  class  CondBrTermContext : public antlr4::ParserRuleContext {
  public:
    CondBrTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntType();
    ValueContext *value();
    std::vector<LabelContext *> label();
    LabelContext* label(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CondBrTermContext* condBrTerm();

  class  SwitchTermContext : public antlr4::ParserRuleContext {
  public:
    SwitchTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    LabelContext *label();
    std::vector<CaseContext *> case_();
    CaseContext* case_(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SwitchTermContext* switchTerm();

  class  IndirectBrTermContext : public antlr4::ParserRuleContext {
  public:
    IndirectBrTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    std::vector<LabelContext *> label();
    LabelContext* label(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IndirectBrTermContext* indirectBrTerm();

  class  ResumeTermContext : public antlr4::ParserRuleContext {
  public:
    ResumeTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ResumeTermContext* resumeTerm();

  class  CatchRetTermContext : public antlr4::ParserRuleContext {
  public:
    CatchRetTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValueContext *value();
    LabelContext *label();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CatchRetTermContext* catchRetTerm();

  class  CleanupRetTermContext : public antlr4::ParserRuleContext {
  public:
    CleanupRetTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValueContext *value();
    UnwindTargetContext *unwindTarget();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CleanupRetTermContext* cleanupRetTerm();

  class  UnreachableTermContext : public antlr4::ParserRuleContext {
  public:
    UnreachableTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UnreachableTermContext* unreachableTerm();

  class  InvokeTermContext : public antlr4::ParserRuleContext {
  public:
    InvokeTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    ValueContext *value();
    ArgsContext *args();
    std::vector<LabelContext *> label();
    LabelContext* label(size_t i);
    CallingConvContext *callingConv();
    std::vector<ReturnAttributeContext *> returnAttribute();
    ReturnAttributeContext* returnAttribute(size_t i);
    AddrSpaceContext *addrSpace();
    std::vector<FuncAttributeContext *> funcAttribute();
    FuncAttributeContext* funcAttribute(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);
    std::vector<OperandBundleContext *> operandBundle();
    OperandBundleContext* operandBundle(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InvokeTermContext* invokeTerm();

  class  CallBrTermContext : public antlr4::ParserRuleContext {
  public:
    CallBrTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    ValueContext *value();
    ArgsContext *args();
    std::vector<LabelContext *> label();
    LabelContext* label(size_t i);
    CallingConvContext *callingConv();
    std::vector<ReturnAttributeContext *> returnAttribute();
    ReturnAttributeContext* returnAttribute(size_t i);
    AddrSpaceContext *addrSpace();
    std::vector<FuncAttributeContext *> funcAttribute();
    FuncAttributeContext* funcAttribute(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);
    std::vector<OperandBundleContext *> operandBundle();
    OperandBundleContext* operandBundle(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CallBrTermContext* callBrTerm();

  class  CatchSwitchTermContext : public antlr4::ParserRuleContext {
  public:
    CatchSwitchTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExceptionPadContext *exceptionPad();
    HandlersContext *handlers();
    UnwindTargetContext *unwindTarget();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CatchSwitchTermContext* catchSwitchTerm();

  class  LabelContext : public antlr4::ParserRuleContext {
  public:
    LabelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LocalIdent();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LabelContext* label();

  class  CaseContext : public antlr4::ParserRuleContext {
  public:
    CaseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    LabelContext *label();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CaseContext* case_();

  class  UnwindTargetContext : public antlr4::ParserRuleContext {
  public:
    UnwindTargetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LabelContext *label();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UnwindTargetContext* unwindTarget();

  class  HandlersContext : public antlr4::ParserRuleContext {
  public:
    HandlersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<LabelContext *> label();
    LabelContext* label(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  HandlersContext* handlers();

  class  MetadataNodeContext : public antlr4::ParserRuleContext {
  public:
    MetadataNodeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MetadataId();
    DiExpressionContext *diExpression();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MetadataNodeContext* metadataNode();

  class  DiExpressionContext : public antlr4::ParserRuleContext {
  public:
    DiExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiExpressionFieldContext *> diExpressionField();
    DiExpressionFieldContext* diExpressionField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiExpressionContext* diExpression();

  class  DiExpressionFieldContext : public antlr4::ParserRuleContext {
  public:
    DiExpressionFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();
    antlr4::tree::TerminalNode *DwarfAttEncoding();
    antlr4::tree::TerminalNode *DwarfOp();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiExpressionFieldContext* diExpressionField();

  class  GlobalFieldContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *sanitizerKind = nullptr;
    GlobalFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SectionContext *section();
    PartitionContext *partition();
    ComdatContext *comdat();
    AlignContext *align();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GlobalFieldContext* globalField();

  class  SectionContext : public antlr4::ParserRuleContext {
  public:
    SectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SectionContext* section();

  class  ComdatContext : public antlr4::ParserRuleContext {
  public:
    ComdatContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ComdatName();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ComdatContext* comdat();

  class  PartitionContext : public antlr4::ParserRuleContext {
  public:
    PartitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PartitionContext* partition();

  class  ConstantContext : public antlr4::ParserRuleContext {
  public:
    ConstantContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();
    IntConstContext *intConst();
    FloatConstContext *floatConst();
    NullConstContext *nullConst();
    NoneConstContext *noneConst();
    StructConstContext *structConst();
    ArrayConstContext *arrayConst();
    VectorConstContext *vectorConst();
    ZeroInitializerConstContext *zeroInitializerConst();
    antlr4::tree::TerminalNode *GlobalIdent();
    UndefConstContext *undefConst();
    PoisonConstContext *poisonConst();
    BlockAddressConstContext *blockAddressConst();
    DsoLocalEquivalentConstContext *dsoLocalEquivalentConst();
    NoCFIConstContext *noCFIConst();
    ConstantExprContext *constantExpr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstantContext* constant();

  class  BoolConstContext : public antlr4::ParserRuleContext {
  public:
    BoolConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BoolConstContext* boolConst();

  class  IntConstContext : public antlr4::ParserRuleContext {
  public:
    IntConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IntConstContext* intConst();

  class  FloatConstContext : public antlr4::ParserRuleContext {
  public:
    FloatConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FloatLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FloatConstContext* floatConst();

  class  NullConstContext : public antlr4::ParserRuleContext {
  public:
    NullConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NullConstContext* nullConst();

  class  NoneConstContext : public antlr4::ParserRuleContext {
  public:
    NoneConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NoneConstContext* noneConst();

  class  StructConstContext : public antlr4::ParserRuleContext {
  public:
    StructConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StructConstContext* structConst();

  class  ArrayConstContext : public antlr4::ParserRuleContext {
  public:
    ArrayConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArrayConstContext* arrayConst();

  class  VectorConstContext : public antlr4::ParserRuleContext {
  public:
    VectorConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VectorConstContext* vectorConst();

  class  ZeroInitializerConstContext : public antlr4::ParserRuleContext {
  public:
    ZeroInitializerConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ZeroInitializerConstContext* zeroInitializerConst();

  class  UndefConstContext : public antlr4::ParserRuleContext {
  public:
    UndefConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UndefConstContext* undefConst();

  class  PoisonConstContext : public antlr4::ParserRuleContext {
  public:
    PoisonConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PoisonConstContext* poisonConst();

  class  BlockAddressConstContext : public antlr4::ParserRuleContext {
  public:
    BlockAddressConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GlobalIdent();
    antlr4::tree::TerminalNode *LocalIdent();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockAddressConstContext* blockAddressConst();

  class  DsoLocalEquivalentConstContext : public antlr4::ParserRuleContext {
  public:
    DsoLocalEquivalentConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GlobalIdent();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DsoLocalEquivalentConstContext* dsoLocalEquivalentConst();

  class  NoCFIConstContext : public antlr4::ParserRuleContext {
  public:
    NoCFIConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GlobalIdent();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NoCFIConstContext* noCFIConst();

  class  ConstantExprContext : public antlr4::ParserRuleContext {
  public:
    ConstantExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FNegExprContext *fNegExpr();
    AddExprContext *addExpr();
    SubExprContext *subExpr();
    MulExprContext *mulExpr();
    ShlExprContext *shlExpr();
    LShrExprContext *lShrExpr();
    AShrExprContext *aShrExpr();
    AndExprContext *andExpr();
    OrExprContext *orExpr();
    XorExprContext *xorExpr();
    ExtractElementExprContext *extractElementExpr();
    InsertElementExprContext *insertElementExpr();
    ShuffleVectorExprContext *shuffleVectorExpr();
    GetElementPtrExprContext *getElementPtrExpr();
    TruncExprContext *truncExpr();
    ZExtExprContext *zExtExpr();
    SExtExprContext *sExtExpr();
    FpTruncExprContext *fpTruncExpr();
    FpExtExprContext *fpExtExpr();
    FpToUiExprContext *fpToUiExpr();
    FpToSiExprContext *fpToSiExpr();
    UiToFpExprContext *uiToFpExpr();
    SiToFpExprContext *siToFpExpr();
    PtrToIntExprContext *ptrToIntExpr();
    IntToPtrExprContext *intToPtrExpr();
    BitCastExprContext *bitCastExpr();
    AddrSpaceCastExprContext *addrSpaceCastExpr();
    ICmpExprContext *iCmpExpr();
    FCmpExprContext *fCmpExpr();
    SelectExprContext *selectExpr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstantExprContext* constantExpr();

  class  TypeConstContext : public antlr4::ParserRuleContext {
  public:
    TypeConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FirstClassTypeContext *firstClassType();
    ConstantContext *constant();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeConstContext* typeConst();

  class  MetadataAttachmentContext : public antlr4::ParserRuleContext {
  public:
    MetadataAttachmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MetadataName();
    MdNodeContext *mdNode();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MetadataAttachmentContext* metadataAttachment();

  class  MdNodeContext : public antlr4::ParserRuleContext {
  public:
    MdNodeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdTupleContext *mdTuple();
    antlr4::tree::TerminalNode *MetadataId();
    SpecializedMDNodeContext *specializedMDNode();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MdNodeContext* mdNode();

  class  MdTupleContext : public antlr4::ParserRuleContext {
  public:
    MdTupleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<MdFieldContext *> mdField();
    MdFieldContext* mdField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MdTupleContext* mdTuple();

  class  MetadataContext : public antlr4::ParserRuleContext {
  public:
    MetadataContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    MdStringContext *mdString();
    MdTupleContext *mdTuple();
    antlr4::tree::TerminalNode *MetadataId();
    DiArgListContext *diArgList();
    SpecializedMDNodeContext *specializedMDNode();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MetadataContext* metadata();

  class  DiArgListContext : public antlr4::ParserRuleContext {
  public:
    DiArgListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiArgListContext* diArgList();

  class  TypeValueContext : public antlr4::ParserRuleContext {
  public:
    TypeValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FirstClassTypeContext *firstClassType();
    ValueContext *value();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeValueContext* typeValue();

  class  ValueContext : public antlr4::ParserRuleContext {
  public:
    ValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ConstantContext *constant();
    antlr4::tree::TerminalNode *LocalIdent();
    InlineAsmContext *inlineAsm();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueContext* value();

  class  InlineAsmContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *sideEffect = nullptr;
    antlr4::Token *alignStackTok = nullptr;
    antlr4::Token *intelDialect = nullptr;
    antlr4::Token *unwind = nullptr;
    InlineAsmContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> StringLit();
    antlr4::tree::TerminalNode* StringLit(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InlineAsmContext* inlineAsm();

  class  MdStringContext : public antlr4::ParserRuleContext {
  public:
    MdStringContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MdStringContext* mdString();

  class  MdFieldOrIntContext : public antlr4::ParserRuleContext {
  public:
    MdFieldOrIntContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MdFieldOrIntContext* mdFieldOrInt();

  class  DiSPFlagContext : public antlr4::ParserRuleContext {
  public:
    DiSPFlagContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();
    antlr4::tree::TerminalNode *DispFlag();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiSPFlagContext* diSPFlag();

  class  FuncAttributeContext : public antlr4::ParserRuleContext {
  public:
    FuncAttributeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AttrStringContext *attrString();
    AttrPairContext *attrPair();
    antlr4::tree::TerminalNode *AttrGroupId();
    AlignPairContext *alignPair();
    AlignStackContext *alignStack();
    AlignStackPairContext *alignStackPair();
    AllocKindContext *allocKind();
    AllocSizeContext *allocSize();
    FuncAttrContext *funcAttr();
    PreallocatedContext *preallocated();
    UnwindTableContext *unwindTable();
    VectorScaleRangeContext *vectorScaleRange();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncAttributeContext* funcAttribute();

  class  TypeContext : public antlr4::ParserRuleContext {
  public:
    TypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IntTypeContext *intType();
    FloatTypeContext *floatType();
    OpaquePointerTypeContext *opaquePointerType();
    VectorTypeContext *vectorType();
    LabelTypeContext *labelType();
    ArrayTypeContext *arrayType();
    StructTypeContext *structType();
    NamedTypeContext *namedType();
    MmxTypeContext *mmxType();
    TokenTypeContext *tokenType();
    MetadataTypeContext *metadataType();
    TypeContext *type();
    ParamsContext *params();
    AddrSpaceContext *addrSpace();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeContext* type();
  TypeContext* type(int precedence);
  class  ParamsContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *ellipsis = nullptr;
    ParamsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ParamContext *> param();
    ParamContext* param(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParamsContext* params();

  class  ParamContext : public antlr4::ParserRuleContext {
  public:
    ParamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    std::vector<ParamAttributeContext *> paramAttribute();
    ParamAttributeContext* paramAttribute(size_t i);
    antlr4::tree::TerminalNode *LocalIdent();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParamContext* param();

  class  ParamAttributeContext : public antlr4::ParserRuleContext {
  public:
    ParamAttributeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AttrStringContext *attrString();
    AttrPairContext *attrPair();
    AlignContext *align();
    AlignStackContext *alignStack();
    ByRefAttrContext *byRefAttr();
    ByvalContext *byval();
    DereferenceableContext *dereferenceable();
    ElementTypeContext *elementType();
    InAllocaContext *inAlloca();
    ParamAttrContext *paramAttr();
    PreallocatedContext *preallocated();
    StructRetAttrContext *structRetAttr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParamAttributeContext* paramAttribute();

  class  AttrStringContext : public antlr4::ParserRuleContext {
  public:
    AttrStringContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttrStringContext* attrString();

  class  AttrPairContext : public antlr4::ParserRuleContext {
  public:
    AttrPairContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> StringLit();
    antlr4::tree::TerminalNode* StringLit(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttrPairContext* attrPair();

  class  AlignContext : public antlr4::ParserRuleContext {
  public:
    AlignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AlignContext* align();

  class  AlignPairContext : public antlr4::ParserRuleContext {
  public:
    AlignPairContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AlignPairContext* alignPair();

  class  AlignStackContext : public antlr4::ParserRuleContext {
  public:
    AlignStackContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AlignStackContext* alignStack();

  class  AlignStackPairContext : public antlr4::ParserRuleContext {
  public:
    AlignStackPairContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AlignStackPairContext* alignStackPair();

  class  AllocKindContext : public antlr4::ParserRuleContext {
  public:
    AllocKindContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AllocKindContext* allocKind();

  class  AllocSizeContext : public antlr4::ParserRuleContext {
  public:
    AllocSizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IntLit();
    antlr4::tree::TerminalNode* IntLit(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AllocSizeContext* allocSize();

  class  UnwindTableContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *unwindTableKind = nullptr;
    UnwindTableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UnwindTableContext* unwindTable();

  class  VectorScaleRangeContext : public antlr4::ParserRuleContext {
  public:
    VectorScaleRangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IntLit();
    antlr4::tree::TerminalNode* IntLit(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VectorScaleRangeContext* vectorScaleRange();

  class  ByRefAttrContext : public antlr4::ParserRuleContext {
  public:
    ByRefAttrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ByRefAttrContext* byRefAttr();

  class  ByvalContext : public antlr4::ParserRuleContext {
  public:
    ByvalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ByvalContext* byval();

  class  DereferenceableContext : public antlr4::ParserRuleContext {
  public:
    DereferenceableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DereferenceableContext* dereferenceable();

  class  ElementTypeContext : public antlr4::ParserRuleContext {
  public:
    ElementTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElementTypeContext* elementType();

  class  InAllocaContext : public antlr4::ParserRuleContext {
  public:
    InAllocaContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InAllocaContext* inAlloca();

  class  ParamAttrContext : public antlr4::ParserRuleContext {
  public:
    ParamAttrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParamAttrContext* paramAttr();

  class  PreallocatedContext : public antlr4::ParserRuleContext {
  public:
    PreallocatedContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PreallocatedContext* preallocated();

  class  StructRetAttrContext : public antlr4::ParserRuleContext {
  public:
    StructRetAttrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StructRetAttrContext* structRetAttr();

  class  FirstClassTypeContext : public antlr4::ParserRuleContext {
  public:
    FirstClassTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ConcreteTypeContext *concreteType();
    MetadataTypeContext *metadataType();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FirstClassTypeContext* firstClassType();

  class  ConcreteTypeContext : public antlr4::ParserRuleContext {
  public:
    ConcreteTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IntTypeContext *intType();
    FloatTypeContext *floatType();
    PointerTypeContext *pointerType();
    VectorTypeContext *vectorType();
    LabelTypeContext *labelType();
    ArrayTypeContext *arrayType();
    StructTypeContext *structType();
    NamedTypeContext *namedType();
    MmxTypeContext *mmxType();
    TokenTypeContext *tokenType();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConcreteTypeContext* concreteType();

  class  IntTypeContext : public antlr4::ParserRuleContext {
  public:
    IntTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntType();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IntTypeContext* intType();

  class  FloatTypeContext : public antlr4::ParserRuleContext {
  public:
    FloatTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FloatKindContext *floatKind();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FloatTypeContext* floatType();

  class  PointerTypeContext : public antlr4::ParserRuleContext {
  public:
    PointerTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    AddrSpaceContext *addrSpace();
    OpaquePointerTypeContext *opaquePointerType();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PointerTypeContext* pointerType();

  class  VectorTypeContext : public antlr4::ParserRuleContext {
  public:
    VectorTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VectorTypeContext* vectorType();

  class  LabelTypeContext : public antlr4::ParserRuleContext {
  public:
    LabelTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LabelTypeContext* labelType();

  class  ArrayTypeContext : public antlr4::ParserRuleContext {
  public:
    ArrayTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArrayTypeContext* arrayType();

  class  StructTypeContext : public antlr4::ParserRuleContext {
  public:
    StructTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeContext *> type();
    TypeContext* type(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StructTypeContext* structType();

  class  NamedTypeContext : public antlr4::ParserRuleContext {
  public:
    NamedTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LocalIdent();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NamedTypeContext* namedType();

  class  MmxTypeContext : public antlr4::ParserRuleContext {
  public:
    MmxTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MmxTypeContext* mmxType();

  class  TokenTypeContext : public antlr4::ParserRuleContext {
  public:
    TokenTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TokenTypeContext* tokenType();

  class  OpaquePointerTypeContext : public antlr4::ParserRuleContext {
  public:
    OpaquePointerTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AddrSpaceContext *addrSpace();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OpaquePointerTypeContext* opaquePointerType();

  class  AddrSpaceContext : public antlr4::ParserRuleContext {
  public:
    AddrSpaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AddrSpaceContext* addrSpace();

  class  ThreadLocalContext : public antlr4::ParserRuleContext {
  public:
    ThreadLocalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TlsModelContext *tlsModel();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ThreadLocalContext* threadLocal();

  class  MetadataTypeContext : public antlr4::ParserRuleContext {
  public:
    MetadataTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MetadataTypeContext* metadataType();

  class  BitCastExprContext : public antlr4::ParserRuleContext {
  public:
    BitCastExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BitCastExprContext* bitCastExpr();

  class  GetElementPtrExprContext : public antlr4::ParserRuleContext {
  public:
    GetElementPtrExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    TypeConstContext *typeConst();
    InBoundsContext *inBounds();
    std::vector<GepIndexContext *> gepIndex();
    GepIndexContext* gepIndex(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GetElementPtrExprContext* getElementPtrExpr();

  class  GepIndexContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *inRange = nullptr;
    GepIndexContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GepIndexContext* gepIndex();

  class  AddrSpaceCastExprContext : public antlr4::ParserRuleContext {
  public:
    AddrSpaceCastExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AddrSpaceCastExprContext* addrSpaceCastExpr();

  class  IntToPtrExprContext : public antlr4::ParserRuleContext {
  public:
    IntToPtrExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IntToPtrExprContext* intToPtrExpr();

  class  ICmpExprContext : public antlr4::ParserRuleContext {
  public:
    ICmpExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IPredContext *iPred();
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ICmpExprContext* iCmpExpr();

  class  FCmpExprContext : public antlr4::ParserRuleContext {
  public:
    FCmpExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FPredContext *fPred();
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FCmpExprContext* fCmpExpr();

  class  SelectExprContext : public antlr4::ParserRuleContext {
  public:
    SelectExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SelectExprContext* selectExpr();

  class  TruncExprContext : public antlr4::ParserRuleContext {
  public:
    TruncExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TruncExprContext* truncExpr();

  class  ZExtExprContext : public antlr4::ParserRuleContext {
  public:
    ZExtExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ZExtExprContext* zExtExpr();

  class  SExtExprContext : public antlr4::ParserRuleContext {
  public:
    SExtExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SExtExprContext* sExtExpr();

  class  FpTruncExprContext : public antlr4::ParserRuleContext {
  public:
    FpTruncExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FpTruncExprContext* fpTruncExpr();

  class  FpExtExprContext : public antlr4::ParserRuleContext {
  public:
    FpExtExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FpExtExprContext* fpExtExpr();

  class  FpToUiExprContext : public antlr4::ParserRuleContext {
  public:
    FpToUiExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FpToUiExprContext* fpToUiExpr();

  class  FpToSiExprContext : public antlr4::ParserRuleContext {
  public:
    FpToSiExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FpToSiExprContext* fpToSiExpr();

  class  UiToFpExprContext : public antlr4::ParserRuleContext {
  public:
    UiToFpExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UiToFpExprContext* uiToFpExpr();

  class  SiToFpExprContext : public antlr4::ParserRuleContext {
  public:
    SiToFpExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SiToFpExprContext* siToFpExpr();

  class  PtrToIntExprContext : public antlr4::ParserRuleContext {
  public:
    PtrToIntExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();
    TypeContext *type();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PtrToIntExprContext* ptrToIntExpr();

  class  ExtractElementExprContext : public antlr4::ParserRuleContext {
  public:
    ExtractElementExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExtractElementExprContext* extractElementExpr();

  class  InsertElementExprContext : public antlr4::ParserRuleContext {
  public:
    InsertElementExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InsertElementExprContext* insertElementExpr();

  class  ShuffleVectorExprContext : public antlr4::ParserRuleContext {
  public:
    ShuffleVectorExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ShuffleVectorExprContext* shuffleVectorExpr();

  class  ShlExprContext : public antlr4::ParserRuleContext {
  public:
    ShlExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);
    std::vector<OverflowFlagContext *> overflowFlag();
    OverflowFlagContext* overflowFlag(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ShlExprContext* shlExpr();

  class  LShrExprContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *exact = nullptr;
    LShrExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LShrExprContext* lShrExpr();

  class  AShrExprContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *exact = nullptr;
    AShrExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AShrExprContext* aShrExpr();

  class  AndExprContext : public antlr4::ParserRuleContext {
  public:
    AndExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AndExprContext* andExpr();

  class  OrExprContext : public antlr4::ParserRuleContext {
  public:
    OrExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OrExprContext* orExpr();

  class  XorExprContext : public antlr4::ParserRuleContext {
  public:
    XorExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  XorExprContext* xorExpr();

  class  AddExprContext : public antlr4::ParserRuleContext {
  public:
    AddExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);
    std::vector<OverflowFlagContext *> overflowFlag();
    OverflowFlagContext* overflowFlag(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AddExprContext* addExpr();

  class  SubExprContext : public antlr4::ParserRuleContext {
  public:
    SubExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);
    std::vector<OverflowFlagContext *> overflowFlag();
    OverflowFlagContext* overflowFlag(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SubExprContext* subExpr();

  class  MulExprContext : public antlr4::ParserRuleContext {
  public:
    MulExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeConstContext *> typeConst();
    TypeConstContext* typeConst(size_t i);
    std::vector<OverflowFlagContext *> overflowFlag();
    OverflowFlagContext* overflowFlag(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MulExprContext* mulExpr();

  class  FNegExprContext : public antlr4::ParserRuleContext {
  public:
    FNegExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeConstContext *typeConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FNegExprContext* fNegExpr();

  class  LocalDefInstContext : public antlr4::ParserRuleContext {
  public:
    LocalDefInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LocalIdent();
    ValueInstructionContext *valueInstruction();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LocalDefInstContext* localDefInst();

  class  ValueInstructionContext : public antlr4::ParserRuleContext {
  public:
    ValueInstructionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FNegInstContext *fNegInst();
    AddInstContext *addInst();
    FAddInstContext *fAddInst();
    SubInstContext *subInst();
    FSubInstContext *fSubInst();
    MulInstContext *mulInst();
    FMulInstContext *fMulInst();
    UDivInstContext *uDivInst();
    SDivInstContext *sDivInst();
    FDivInstContext *fDivInst();
    URemInstContext *uRemInst();
    SRemInstContext *sRemInst();
    FRemInstContext *fRemInst();
    ShlInstContext *shlInst();
    LShrInstContext *lShrInst();
    AShrInstContext *aShrInst();
    AndInstContext *andInst();
    OrInstContext *orInst();
    XorInstContext *xorInst();
    ExtractElementInstContext *extractElementInst();
    InsertElementInstContext *insertElementInst();
    ShuffleVectorInstContext *shuffleVectorInst();
    ExtractValueInstContext *extractValueInst();
    InsertValueInstContext *insertValueInst();
    AllocaInstContext *allocaInst();
    LoadInstContext *loadInst();
    CmpXchgInstContext *cmpXchgInst();
    AtomicRMWInstContext *atomicRMWInst();
    GetElementPtrInstContext *getElementPtrInst();
    TruncInstContext *truncInst();
    ZExtInstContext *zExtInst();
    SExtInstContext *sExtInst();
    FpTruncInstContext *fpTruncInst();
    FpExtInstContext *fpExtInst();
    FpToUiInstContext *fpToUiInst();
    FpToSiInstContext *fpToSiInst();
    UiToFpInstContext *uiToFpInst();
    SiToFpInstContext *siToFpInst();
    PtrToIntInstContext *ptrToIntInst();
    IntToPtrInstContext *intToPtrInst();
    BitCastInstContext *bitCastInst();
    AddrSpaceCastInstContext *addrSpaceCastInst();
    ICmpInstContext *iCmpInst();
    FCmpInstContext *fCmpInst();
    PhiInstContext *phiInst();
    SelectInstContext *selectInst();
    FreezeInstContext *freezeInst();
    CallInstContext *callInst();
    VaargInstContext *vaargInst();
    LandingPadInstContext *landingPadInst();
    CatchPadInstContext *catchPadInst();
    CleanupPadInstContext *cleanupPadInst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueInstructionContext* valueInstruction();

  class  StoreInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *volatile_ = nullptr;
    antlr4::Token *atomic = nullptr;
    StoreInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    AlignContext *align();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);
    AtomicOrderingContext *atomicOrdering();
    SyncScopeContext *syncScope();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StoreInstContext* storeInst();

  class  SyncScopeContext : public antlr4::ParserRuleContext {
  public:
    SyncScopeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SyncScopeContext* syncScope();

  class  FenceInstContext : public antlr4::ParserRuleContext {
  public:
    FenceInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AtomicOrderingContext *atomicOrdering();
    SyncScopeContext *syncScope();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FenceInstContext* fenceInst();

  class  FNegInstContext : public antlr4::ParserRuleContext {
  public:
    FNegInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FNegInstContext* fNegInst();

  class  AddInstContext : public antlr4::ParserRuleContext {
  public:
    AddInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<OverflowFlagContext *> overflowFlag();
    OverflowFlagContext* overflowFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AddInstContext* addInst();

  class  FAddInstContext : public antlr4::ParserRuleContext {
  public:
    FAddInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FAddInstContext* fAddInst();

  class  SubInstContext : public antlr4::ParserRuleContext {
  public:
    SubInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<OverflowFlagContext *> overflowFlag();
    OverflowFlagContext* overflowFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SubInstContext* subInst();

  class  FSubInstContext : public antlr4::ParserRuleContext {
  public:
    FSubInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FSubInstContext* fSubInst();

  class  MulInstContext : public antlr4::ParserRuleContext {
  public:
    MulInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<OverflowFlagContext *> overflowFlag();
    OverflowFlagContext* overflowFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MulInstContext* mulInst();

  class  FMulInstContext : public antlr4::ParserRuleContext {
  public:
    FMulInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FMulInstContext* fMulInst();

  class  UDivInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *exact = nullptr;
    UDivInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UDivInstContext* uDivInst();

  class  SDivInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *exact = nullptr;
    SDivInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SDivInstContext* sDivInst();

  class  FDivInstContext : public antlr4::ParserRuleContext {
  public:
    FDivInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FDivInstContext* fDivInst();

  class  URemInstContext : public antlr4::ParserRuleContext {
  public:
    URemInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  URemInstContext* uRemInst();

  class  SRemInstContext : public antlr4::ParserRuleContext {
  public:
    SRemInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SRemInstContext* sRemInst();

  class  FRemInstContext : public antlr4::ParserRuleContext {
  public:
    FRemInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FRemInstContext* fRemInst();

  class  ShlInstContext : public antlr4::ParserRuleContext {
  public:
    ShlInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<OverflowFlagContext *> overflowFlag();
    OverflowFlagContext* overflowFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ShlInstContext* shlInst();

  class  LShrInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *exact = nullptr;
    LShrInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LShrInstContext* lShrInst();

  class  AShrInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *exact = nullptr;
    AShrInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AShrInstContext* aShrInst();

  class  AndInstContext : public antlr4::ParserRuleContext {
  public:
    AndInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AndInstContext* andInst();

  class  OrInstContext : public antlr4::ParserRuleContext {
  public:
    OrInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OrInstContext* orInst();

  class  XorInstContext : public antlr4::ParserRuleContext {
  public:
    XorInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  XorInstContext* xorInst();

  class  ExtractElementInstContext : public antlr4::ParserRuleContext {
  public:
    ExtractElementInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExtractElementInstContext* extractElementInst();

  class  InsertElementInstContext : public antlr4::ParserRuleContext {
  public:
    InsertElementInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InsertElementInstContext* insertElementInst();

  class  ShuffleVectorInstContext : public antlr4::ParserRuleContext {
  public:
    ShuffleVectorInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ShuffleVectorInstContext* shuffleVectorInst();

  class  ExtractValueInstContext : public antlr4::ParserRuleContext {
  public:
    ExtractValueInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    std::vector<antlr4::tree::TerminalNode *> IntLit();
    antlr4::tree::TerminalNode* IntLit(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExtractValueInstContext* extractValueInst();

  class  InsertValueInstContext : public antlr4::ParserRuleContext {
  public:
    InsertValueInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    std::vector<antlr4::tree::TerminalNode *> IntLit();
    antlr4::tree::TerminalNode* IntLit(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InsertValueInstContext* insertValueInst();

  class  AllocaInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *inAllocaTok = nullptr;
    antlr4::Token *swiftError = nullptr;
    AllocaInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    TypeValueContext *typeValue();
    AlignContext *align();
    AddrSpaceContext *addrSpace();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AllocaInstContext* allocaInst();

  class  LoadInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *volatile_ = nullptr;
    antlr4::Token *atomic = nullptr;
    LoadInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    TypeValueContext *typeValue();
    AlignContext *align();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);
    AtomicOrderingContext *atomicOrdering();
    SyncScopeContext *syncScope();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LoadInstContext* loadInst();

  class  CmpXchgInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *weak = nullptr;
    antlr4::Token *volatile_ = nullptr;
    CmpXchgInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    std::vector<AtomicOrderingContext *> atomicOrdering();
    AtomicOrderingContext* atomicOrdering(size_t i);
    SyncScopeContext *syncScope();
    AlignContext *align();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CmpXchgInstContext* cmpXchgInst();

  class  AtomicRMWInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *volatile_ = nullptr;
    AtomicRMWInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AtomicOpContext *atomicOp();
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    AtomicOrderingContext *atomicOrdering();
    SyncScopeContext *syncScope();
    AlignContext *align();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AtomicRMWInstContext* atomicRMWInst();

  class  GetElementPtrInstContext : public antlr4::ParserRuleContext {
  public:
    GetElementPtrInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    InBoundsContext *inBounds();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GetElementPtrInstContext* getElementPtrInst();

  class  TruncInstContext : public antlr4::ParserRuleContext {
  public:
    TruncInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TruncInstContext* truncInst();

  class  ZExtInstContext : public antlr4::ParserRuleContext {
  public:
    ZExtInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ZExtInstContext* zExtInst();

  class  SExtInstContext : public antlr4::ParserRuleContext {
  public:
    SExtInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SExtInstContext* sExtInst();

  class  FpTruncInstContext : public antlr4::ParserRuleContext {
  public:
    FpTruncInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FpTruncInstContext* fpTruncInst();

  class  FpExtInstContext : public antlr4::ParserRuleContext {
  public:
    FpExtInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FpExtInstContext* fpExtInst();

  class  FpToUiInstContext : public antlr4::ParserRuleContext {
  public:
    FpToUiInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FpToUiInstContext* fpToUiInst();

  class  FpToSiInstContext : public antlr4::ParserRuleContext {
  public:
    FpToSiInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FpToSiInstContext* fpToSiInst();

  class  UiToFpInstContext : public antlr4::ParserRuleContext {
  public:
    UiToFpInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UiToFpInstContext* uiToFpInst();

  class  SiToFpInstContext : public antlr4::ParserRuleContext {
  public:
    SiToFpInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SiToFpInstContext* siToFpInst();

  class  PtrToIntInstContext : public antlr4::ParserRuleContext {
  public:
    PtrToIntInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PtrToIntInstContext* ptrToIntInst();

  class  IntToPtrInstContext : public antlr4::ParserRuleContext {
  public:
    IntToPtrInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IntToPtrInstContext* intToPtrInst();

  class  BitCastInstContext : public antlr4::ParserRuleContext {
  public:
    BitCastInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BitCastInstContext* bitCastInst();

  class  AddrSpaceCastInstContext : public antlr4::ParserRuleContext {
  public:
    AddrSpaceCastInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AddrSpaceCastInstContext* addrSpaceCastInst();

  class  ICmpInstContext : public antlr4::ParserRuleContext {
  public:
    ICmpInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IPredContext *iPred();
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ICmpInstContext* iCmpInst();

  class  FCmpInstContext : public antlr4::ParserRuleContext {
  public:
    FCmpInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FPredContext *fPred();
    TypeValueContext *typeValue();
    ValueContext *value();
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FCmpInstContext* fCmpInst();

  class  PhiInstContext : public antlr4::ParserRuleContext {
  public:
    PhiInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    std::vector<IncContext *> inc();
    IncContext* inc(size_t i);
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PhiInstContext* phiInst();

  class  SelectInstContext : public antlr4::ParserRuleContext {
  public:
    SelectInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SelectInstContext* selectInst();

  class  FreezeInstContext : public antlr4::ParserRuleContext {
  public:
    FreezeInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FreezeInstContext* freezeInst();

  class  CallInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *tail = nullptr;
    CallInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    ValueContext *value();
    ArgsContext *args();
    std::vector<FastMathFlagContext *> fastMathFlag();
    FastMathFlagContext* fastMathFlag(size_t i);
    CallingConvContext *callingConv();
    std::vector<ReturnAttributeContext *> returnAttribute();
    ReturnAttributeContext* returnAttribute(size_t i);
    AddrSpaceContext *addrSpace();
    std::vector<FuncAttributeContext *> funcAttribute();
    FuncAttributeContext* funcAttribute(size_t i);
    std::vector<OperandBundleContext *> operandBundle();
    OperandBundleContext* operandBundle(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CallInstContext* callInst();

  class  VaargInstContext : public antlr4::ParserRuleContext {
  public:
    VaargInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();
    TypeContext *type();
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VaargInstContext* vaargInst();

  class  LandingPadInstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *cleanUp = nullptr;
    LandingPadInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    std::vector<ClauseContext *> clause();
    ClauseContext* clause(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LandingPadInstContext* landingPadInst();

  class  CatchPadInstContext : public antlr4::ParserRuleContext {
  public:
    CatchPadInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LocalIdent();
    std::vector<ExceptionArgContext *> exceptionArg();
    ExceptionArgContext* exceptionArg(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CatchPadInstContext* catchPadInst();

  class  CleanupPadInstContext : public antlr4::ParserRuleContext {
  public:
    CleanupPadInstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExceptionPadContext *exceptionPad();
    std::vector<ExceptionArgContext *> exceptionArg();
    ExceptionArgContext* exceptionArg(size_t i);
    std::vector<MetadataAttachmentContext *> metadataAttachment();
    MetadataAttachmentContext* metadataAttachment(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CleanupPadInstContext* cleanupPadInst();

  class  IncContext : public antlr4::ParserRuleContext {
  public:
    IncContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValueContext *value();
    antlr4::tree::TerminalNode *LocalIdent();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IncContext* inc();

  class  OperandBundleContext : public antlr4::ParserRuleContext {
  public:
    OperandBundleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();
    std::vector<TypeValueContext *> typeValue();
    TypeValueContext* typeValue(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OperandBundleContext* operandBundle();

  class  ClauseContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *clauseType = nullptr;
    ClauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeValueContext *typeValue();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClauseContext* clause();

  class  ArgsContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *ellipsis = nullptr;
    ArgsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ArgContext *> arg();
    ArgContext* arg(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArgsContext* args();

  class  ArgContext : public antlr4::ParserRuleContext {
  public:
    ArgContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ConcreteTypeContext *concreteType();
    ValueContext *value();
    std::vector<ParamAttributeContext *> paramAttribute();
    ParamAttributeContext* paramAttribute(size_t i);
    MetadataTypeContext *metadataType();
    MetadataContext *metadata();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArgContext* arg();

  class  ExceptionArgContext : public antlr4::ParserRuleContext {
  public:
    ExceptionArgContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ConcreteTypeContext *concreteType();
    ValueContext *value();
    MetadataTypeContext *metadataType();
    MetadataContext *metadata();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExceptionArgContext* exceptionArg();

  class  ExceptionPadContext : public antlr4::ParserRuleContext {
  public:
    ExceptionPadContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NoneConstContext *noneConst();
    antlr4::tree::TerminalNode *LocalIdent();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExceptionPadContext* exceptionPad();

  class  ExternalLinkageContext : public antlr4::ParserRuleContext {
  public:
    ExternalLinkageContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExternalLinkageContext* externalLinkage();

  class  InternalLinkageContext : public antlr4::ParserRuleContext {
  public:
    InternalLinkageContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InternalLinkageContext* internalLinkage();

  class  LinkageContext : public antlr4::ParserRuleContext {
  public:
    LinkageContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    InternalLinkageContext *internalLinkage();
    ExternalLinkageContext *externalLinkage();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LinkageContext* linkage();

  class  PreemptionContext : public antlr4::ParserRuleContext {
  public:
    PreemptionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PreemptionContext* preemption();

  class  VisibilityContext : public antlr4::ParserRuleContext {
  public:
    VisibilityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VisibilityContext* visibility();

  class  DllStorageClassContext : public antlr4::ParserRuleContext {
  public:
    DllStorageClassContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DllStorageClassContext* dllStorageClass();

  class  TlsModelContext : public antlr4::ParserRuleContext {
  public:
    TlsModelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TlsModelContext* tlsModel();

  class  UnnamedAddrContext : public antlr4::ParserRuleContext {
  public:
    UnnamedAddrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UnnamedAddrContext* unnamedAddr();

  class  ExternallyInitializedContext : public antlr4::ParserRuleContext {
  public:
    ExternallyInitializedContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExternallyInitializedContext* externallyInitialized();

  class  ImmutableContext : public antlr4::ParserRuleContext {
  public:
    ImmutableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImmutableContext* immutable();

  class  FuncAttrContext : public antlr4::ParserRuleContext {
  public:
    FuncAttrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncAttrContext* funcAttr();

  class  DistinctContext : public antlr4::ParserRuleContext {
  public:
    DistinctContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DistinctContext* distinct();

  class  InBoundsContext : public antlr4::ParserRuleContext {
  public:
    InBoundsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InBoundsContext* inBounds();

  class  ReturnAttrContext : public antlr4::ParserRuleContext {
  public:
    ReturnAttrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ReturnAttrContext* returnAttr();

  class  OverflowFlagContext : public antlr4::ParserRuleContext {
  public:
    OverflowFlagContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OverflowFlagContext* overflowFlag();

  class  IPredContext : public antlr4::ParserRuleContext {
  public:
    IPredContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IPredContext* iPred();

  class  FPredContext : public antlr4::ParserRuleContext {
  public:
    FPredContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FPredContext* fPred();

  class  AtomicOrderingContext : public antlr4::ParserRuleContext {
  public:
    AtomicOrderingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AtomicOrderingContext* atomicOrdering();

  class  CallingConvEnumContext : public antlr4::ParserRuleContext {
  public:
    CallingConvEnumContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CallingConvEnumContext* callingConvEnum();

  class  FastMathFlagContext : public antlr4::ParserRuleContext {
  public:
    FastMathFlagContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FastMathFlagContext* fastMathFlag();

  class  AtomicOpContext : public antlr4::ParserRuleContext {
  public:
    AtomicOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AtomicOpContext* atomicOp();

  class  FloatKindContext : public antlr4::ParserRuleContext {
  public:
    FloatKindContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FloatKindContext* floatKind();

  class  SpecializedMDNodeContext : public antlr4::ParserRuleContext {
  public:
    SpecializedMDNodeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    DiBasicTypeContext *diBasicType();
    DiCommonBlockContext *diCommonBlock();
    DiCompileUnitContext *diCompileUnit();
    DiCompositeTypeContext *diCompositeType();
    DiDerivedTypeContext *diDerivedType();
    DiEnumeratorContext *diEnumerator();
    DiExpressionContext *diExpression();
    DiFileContext *diFile();
    DiGlobalVariableContext *diGlobalVariable();
    DiGlobalVariableExpressionContext *diGlobalVariableExpression();
    DiImportedEntityContext *diImportedEntity();
    DiLabelContext *diLabel();
    DiLexicalBlockContext *diLexicalBlock();
    DiLexicalBlockFileContext *diLexicalBlockFile();
    DiLocalVariableContext *diLocalVariable();
    DiLocationContext *diLocation();
    DiMacroContext *diMacro();
    DiMacroFileContext *diMacroFile();
    DiModuleContext *diModule();
    DiNamespaceContext *diNamespace();
    DiObjCPropertyContext *diObjCProperty();
    DiStringTypeContext *diStringType();
    DiSubprogramContext *diSubprogram();
    DiSubrangeContext *diSubrange();
    DiSubroutineTypeContext *diSubroutineType();
    DiTemplateTypeParameterContext *diTemplateTypeParameter();
    DiTemplateValueParameterContext *diTemplateValueParameter();
    GenericDiNodeContext *genericDiNode();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SpecializedMDNodeContext* specializedMDNode();

  class  DiBasicTypeContext : public antlr4::ParserRuleContext {
  public:
    DiBasicTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiBasicTypeFieldContext *> diBasicTypeField();
    DiBasicTypeFieldContext* diBasicTypeField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiBasicTypeContext* diBasicType();

  class  DiCommonBlockContext : public antlr4::ParserRuleContext {
  public:
    DiCommonBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiCommonBlockFieldContext *> diCommonBlockField();
    DiCommonBlockFieldContext* diCommonBlockField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiCommonBlockContext* diCommonBlock();

  class  DiCompileUnitContext : public antlr4::ParserRuleContext {
  public:
    DiCompileUnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiCompileUnitFieldContext *> diCompileUnitField();
    DiCompileUnitFieldContext* diCompileUnitField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiCompileUnitContext* diCompileUnit();

  class  DiCompositeTypeContext : public antlr4::ParserRuleContext {
  public:
    DiCompositeTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiCompositeTypeFieldContext *> diCompositeTypeField();
    DiCompositeTypeFieldContext* diCompositeTypeField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiCompositeTypeContext* diCompositeType();

  class  DiCompositeTypeFieldContext : public antlr4::ParserRuleContext {
  public:
    DiCompositeTypeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TagFieldContext *tagField();
    NameFieldContext *nameField();
    ScopeFieldContext *scopeField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    BaseTypeFieldContext *baseTypeField();
    SizeFieldContext *sizeField();
    AlignFieldContext *alignField();
    OffsetFieldContext *offsetField();
    FlagsFieldContext *flagsField();
    ElementsFieldContext *elementsField();
    RuntimeLangFieldContext *runtimeLangField();
    VtableHolderFieldContext *vtableHolderField();
    TemplateParamsFieldContext *templateParamsField();
    IdentifierFieldContext *identifierField();
    DiscriminatorFieldContext *discriminatorField();
    DataLocationFieldContext *dataLocationField();
    AssociatedFieldContext *associatedField();
    AllocatedFieldContext *allocatedField();
    RankFieldContext *rankField();
    AnnotationsFieldContext *annotationsField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiCompositeTypeFieldContext* diCompositeTypeField();

  class  DiDerivedTypeContext : public antlr4::ParserRuleContext {
  public:
    DiDerivedTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiDerivedTypeFieldContext *> diDerivedTypeField();
    DiDerivedTypeFieldContext* diDerivedTypeField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiDerivedTypeContext* diDerivedType();

  class  DiDerivedTypeFieldContext : public antlr4::ParserRuleContext {
  public:
    DiDerivedTypeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TagFieldContext *tagField();
    NameFieldContext *nameField();
    ScopeFieldContext *scopeField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    BaseTypeFieldContext *baseTypeField();
    SizeFieldContext *sizeField();
    AlignFieldContext *alignField();
    OffsetFieldContext *offsetField();
    FlagsFieldContext *flagsField();
    ExtraDataFieldContext *extraDataField();
    DwarfAddressSpaceFieldContext *dwarfAddressSpaceField();
    AnnotationsFieldContext *annotationsField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiDerivedTypeFieldContext* diDerivedTypeField();

  class  DiEnumeratorContext : public antlr4::ParserRuleContext {
  public:
    DiEnumeratorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiEnumeratorFieldContext *> diEnumeratorField();
    DiEnumeratorFieldContext* diEnumeratorField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiEnumeratorContext* diEnumerator();

  class  DiEnumeratorFieldContext : public antlr4::ParserRuleContext {
  public:
    DiEnumeratorFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameFieldContext *nameField();
    ValueIntFieldContext *valueIntField();
    IsUnsignedFieldContext *isUnsignedField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiEnumeratorFieldContext* diEnumeratorField();

  class  DiFileContext : public antlr4::ParserRuleContext {
  public:
    DiFileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiFileFieldContext *> diFileField();
    DiFileFieldContext* diFileField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiFileContext* diFile();

  class  DiFileFieldContext : public antlr4::ParserRuleContext {
  public:
    DiFileFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FilenameFieldContext *filenameField();
    DirectoryFieldContext *directoryField();
    ChecksumkindFieldContext *checksumkindField();
    ChecksumFieldContext *checksumField();
    SourceFieldContext *sourceField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiFileFieldContext* diFileField();

  class  DiGlobalVariableContext : public antlr4::ParserRuleContext {
  public:
    DiGlobalVariableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiGlobalVariableFieldContext *> diGlobalVariableField();
    DiGlobalVariableFieldContext* diGlobalVariableField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiGlobalVariableContext* diGlobalVariable();

  class  DiGlobalVariableFieldContext : public antlr4::ParserRuleContext {
  public:
    DiGlobalVariableFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameFieldContext *nameField();
    ScopeFieldContext *scopeField();
    LinkageNameFieldContext *linkageNameField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    TypeFieldContext *typeField();
    IsLocalFieldContext *isLocalField();
    IsDefinitionFieldContext *isDefinitionField();
    TemplateParamsFieldContext *templateParamsField();
    DeclarationFieldContext *declarationField();
    AlignFieldContext *alignField();
    AnnotationsFieldContext *annotationsField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiGlobalVariableFieldContext* diGlobalVariableField();

  class  DiGlobalVariableExpressionContext : public antlr4::ParserRuleContext {
  public:
    DiGlobalVariableExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiGlobalVariableExpressionFieldContext *> diGlobalVariableExpressionField();
    DiGlobalVariableExpressionFieldContext* diGlobalVariableExpressionField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiGlobalVariableExpressionContext* diGlobalVariableExpression();

  class  DiGlobalVariableExpressionFieldContext : public antlr4::ParserRuleContext {
  public:
    DiGlobalVariableExpressionFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VarFieldContext *varField();
    ExprFieldContext *exprField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiGlobalVariableExpressionFieldContext* diGlobalVariableExpressionField();

  class  DiImportedEntityContext : public antlr4::ParserRuleContext {
  public:
    DiImportedEntityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiImportedEntityFieldContext *> diImportedEntityField();
    DiImportedEntityFieldContext* diImportedEntityField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiImportedEntityContext* diImportedEntity();

  class  DiImportedEntityFieldContext : public antlr4::ParserRuleContext {
  public:
    DiImportedEntityFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TagFieldContext *tagField();
    ScopeFieldContext *scopeField();
    EntityFieldContext *entityField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    NameFieldContext *nameField();
    ElementsFieldContext *elementsField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiImportedEntityFieldContext* diImportedEntityField();

  class  DiLabelContext : public antlr4::ParserRuleContext {
  public:
    DiLabelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiLabelFieldContext *> diLabelField();
    DiLabelFieldContext* diLabelField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLabelContext* diLabel();

  class  DiLabelFieldContext : public antlr4::ParserRuleContext {
  public:
    DiLabelFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ScopeFieldContext *scopeField();
    NameFieldContext *nameField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLabelFieldContext* diLabelField();

  class  DiLexicalBlockContext : public antlr4::ParserRuleContext {
  public:
    DiLexicalBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiLexicalBlockFieldContext *> diLexicalBlockField();
    DiLexicalBlockFieldContext* diLexicalBlockField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLexicalBlockContext* diLexicalBlock();

  class  DiLexicalBlockFieldContext : public antlr4::ParserRuleContext {
  public:
    DiLexicalBlockFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ScopeFieldContext *scopeField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    ColumnFieldContext *columnField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLexicalBlockFieldContext* diLexicalBlockField();

  class  DiLexicalBlockFileContext : public antlr4::ParserRuleContext {
  public:
    DiLexicalBlockFileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiLexicalBlockFileFieldContext *> diLexicalBlockFileField();
    DiLexicalBlockFileFieldContext* diLexicalBlockFileField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLexicalBlockFileContext* diLexicalBlockFile();

  class  DiLexicalBlockFileFieldContext : public antlr4::ParserRuleContext {
  public:
    DiLexicalBlockFileFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ScopeFieldContext *scopeField();
    FileFieldContext *fileField();
    DiscriminatorIntFieldContext *discriminatorIntField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLexicalBlockFileFieldContext* diLexicalBlockFileField();

  class  DiLocalVariableContext : public antlr4::ParserRuleContext {
  public:
    DiLocalVariableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiLocalVariableFieldContext *> diLocalVariableField();
    DiLocalVariableFieldContext* diLocalVariableField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLocalVariableContext* diLocalVariable();

  class  DiLocalVariableFieldContext : public antlr4::ParserRuleContext {
  public:
    DiLocalVariableFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ScopeFieldContext *scopeField();
    NameFieldContext *nameField();
    ArgFieldContext *argField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    TypeFieldContext *typeField();
    FlagsFieldContext *flagsField();
    AlignFieldContext *alignField();
    AnnotationsFieldContext *annotationsField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLocalVariableFieldContext* diLocalVariableField();

  class  DiLocationContext : public antlr4::ParserRuleContext {
  public:
    DiLocationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiLocationFieldContext *> diLocationField();
    DiLocationFieldContext* diLocationField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLocationContext* diLocation();

  class  DiLocationFieldContext : public antlr4::ParserRuleContext {
  public:
    DiLocationFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LineFieldContext *lineField();
    ColumnFieldContext *columnField();
    ScopeFieldContext *scopeField();
    InlinedAtFieldContext *inlinedAtField();
    IsImplicitCodeFieldContext *isImplicitCodeField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiLocationFieldContext* diLocationField();

  class  DiMacroContext : public antlr4::ParserRuleContext {
  public:
    DiMacroContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiMacroFieldContext *> diMacroField();
    DiMacroFieldContext* diMacroField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiMacroContext* diMacro();

  class  DiMacroFieldContext : public antlr4::ParserRuleContext {
  public:
    DiMacroFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeMacinfoFieldContext *typeMacinfoField();
    LineFieldContext *lineField();
    NameFieldContext *nameField();
    ValueStringFieldContext *valueStringField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiMacroFieldContext* diMacroField();

  class  DiMacroFileContext : public antlr4::ParserRuleContext {
  public:
    DiMacroFileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiMacroFileFieldContext *> diMacroFileField();
    DiMacroFileFieldContext* diMacroFileField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiMacroFileContext* diMacroFile();

  class  DiMacroFileFieldContext : public antlr4::ParserRuleContext {
  public:
    DiMacroFileFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeMacinfoFieldContext *typeMacinfoField();
    LineFieldContext *lineField();
    FileFieldContext *fileField();
    NodesFieldContext *nodesField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiMacroFileFieldContext* diMacroFileField();

  class  DiModuleContext : public antlr4::ParserRuleContext {
  public:
    DiModuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiModuleFieldContext *> diModuleField();
    DiModuleFieldContext* diModuleField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiModuleContext* diModule();

  class  DiModuleFieldContext : public antlr4::ParserRuleContext {
  public:
    DiModuleFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ScopeFieldContext *scopeField();
    NameFieldContext *nameField();
    ConfigMacrosFieldContext *configMacrosField();
    IncludePathFieldContext *includePathField();
    ApiNotesFieldContext *apiNotesField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    IsDeclFieldContext *isDeclField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiModuleFieldContext* diModuleField();

  class  DiNamespaceContext : public antlr4::ParserRuleContext {
  public:
    DiNamespaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiNamespaceFieldContext *> diNamespaceField();
    DiNamespaceFieldContext* diNamespaceField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiNamespaceContext* diNamespace();

  class  DiNamespaceFieldContext : public antlr4::ParserRuleContext {
  public:
    DiNamespaceFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ScopeFieldContext *scopeField();
    NameFieldContext *nameField();
    ExportSymbolsFieldContext *exportSymbolsField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiNamespaceFieldContext* diNamespaceField();

  class  DiObjCPropertyContext : public antlr4::ParserRuleContext {
  public:
    DiObjCPropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiObjCPropertyFieldContext *> diObjCPropertyField();
    DiObjCPropertyFieldContext* diObjCPropertyField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiObjCPropertyContext* diObjCProperty();

  class  DiObjCPropertyFieldContext : public antlr4::ParserRuleContext {
  public:
    DiObjCPropertyFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameFieldContext *nameField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    SetterFieldContext *setterField();
    GetterFieldContext *getterField();
    AttributesFieldContext *attributesField();
    TypeFieldContext *typeField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiObjCPropertyFieldContext* diObjCPropertyField();

  class  DiStringTypeContext : public antlr4::ParserRuleContext {
  public:
    DiStringTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiStringTypeFieldContext *> diStringTypeField();
    DiStringTypeFieldContext* diStringTypeField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiStringTypeContext* diStringType();

  class  DiStringTypeFieldContext : public antlr4::ParserRuleContext {
  public:
    DiStringTypeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TagFieldContext *tagField();
    NameFieldContext *nameField();
    StringLengthFieldContext *stringLengthField();
    StringLengthExpressionFieldContext *stringLengthExpressionField();
    StringLocationExpressionFieldContext *stringLocationExpressionField();
    SizeFieldContext *sizeField();
    AlignFieldContext *alignField();
    EncodingFieldContext *encodingField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiStringTypeFieldContext* diStringTypeField();

  class  DiSubprogramContext : public antlr4::ParserRuleContext {
  public:
    DiSubprogramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiSubprogramFieldContext *> diSubprogramField();
    DiSubprogramFieldContext* diSubprogramField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiSubprogramContext* diSubprogram();

  class  DiSubprogramFieldContext : public antlr4::ParserRuleContext {
  public:
    DiSubprogramFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ScopeFieldContext *scopeField();
    NameFieldContext *nameField();
    LinkageNameFieldContext *linkageNameField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();
    TypeFieldContext *typeField();
    IsLocalFieldContext *isLocalField();
    IsDefinitionFieldContext *isDefinitionField();
    ScopeLineFieldContext *scopeLineField();
    ContainingTypeFieldContext *containingTypeField();
    VirtualityFieldContext *virtualityField();
    VirtualIndexFieldContext *virtualIndexField();
    ThisAdjustmentFieldContext *thisAdjustmentField();
    FlagsFieldContext *flagsField();
    SpFlagsFieldContext *spFlagsField();
    IsOptimizedFieldContext *isOptimizedField();
    UnitFieldContext *unitField();
    TemplateParamsFieldContext *templateParamsField();
    DeclarationFieldContext *declarationField();
    RetainedNodesFieldContext *retainedNodesField();
    ThrownTypesFieldContext *thrownTypesField();
    AnnotationsFieldContext *annotationsField();
    TargetFuncNameFieldContext *targetFuncNameField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiSubprogramFieldContext* diSubprogramField();

  class  DiSubrangeContext : public antlr4::ParserRuleContext {
  public:
    DiSubrangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiSubrangeFieldContext *> diSubrangeField();
    DiSubrangeFieldContext* diSubrangeField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiSubrangeContext* diSubrange();

  class  DiSubrangeFieldContext : public antlr4::ParserRuleContext {
  public:
    DiSubrangeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CountFieldContext *countField();
    LowerBoundFieldContext *lowerBoundField();
    UpperBoundFieldContext *upperBoundField();
    StrideFieldContext *strideField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiSubrangeFieldContext* diSubrangeField();

  class  DiSubroutineTypeContext : public antlr4::ParserRuleContext {
  public:
    DiSubroutineTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiSubroutineTypeFieldContext *> diSubroutineTypeField();
    DiSubroutineTypeFieldContext* diSubroutineTypeField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiSubroutineTypeContext* diSubroutineType();

  class  DiTemplateTypeParameterContext : public antlr4::ParserRuleContext {
  public:
    DiTemplateTypeParameterContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiTemplateTypeParameterFieldContext *> diTemplateTypeParameterField();
    DiTemplateTypeParameterFieldContext* diTemplateTypeParameterField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiTemplateTypeParameterContext* diTemplateTypeParameter();

  class  DiTemplateValueParameterContext : public antlr4::ParserRuleContext {
  public:
    DiTemplateValueParameterContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiTemplateValueParameterFieldContext *> diTemplateValueParameterField();
    DiTemplateValueParameterFieldContext* diTemplateValueParameterField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiTemplateValueParameterContext* diTemplateValueParameter();

  class  GenericDiNodeContext : public antlr4::ParserRuleContext {
  public:
    GenericDiNodeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<GenericDINodeFieldContext *> genericDINodeField();
    GenericDINodeFieldContext* genericDINodeField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GenericDiNodeContext* genericDiNode();

  class  DiTemplateTypeParameterFieldContext : public antlr4::ParserRuleContext {
  public:
    DiTemplateTypeParameterFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameFieldContext *nameField();
    TypeFieldContext *typeField();
    DefaultedFieldContext *defaultedField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiTemplateTypeParameterFieldContext* diTemplateTypeParameterField();

  class  DiCompileUnitFieldContext : public antlr4::ParserRuleContext {
  public:
    DiCompileUnitFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LanguageFieldContext *languageField();
    FileFieldContext *fileField();
    ProducerFieldContext *producerField();
    IsOptimizedFieldContext *isOptimizedField();
    FlagsStringFieldContext *flagsStringField();
    RuntimeVersionFieldContext *runtimeVersionField();
    SplitDebugFilenameFieldContext *splitDebugFilenameField();
    EmissionKindFieldContext *emissionKindField();
    EnumsFieldContext *enumsField();
    RetainedTypesFieldContext *retainedTypesField();
    GlobalsFieldContext *globalsField();
    ImportsFieldContext *importsField();
    MacrosFieldContext *macrosField();
    DwoIdFieldContext *dwoIdField();
    SplitDebugInliningFieldContext *splitDebugInliningField();
    DebugInfoForProfilingFieldContext *debugInfoForProfilingField();
    NameTableKindFieldContext *nameTableKindField();
    RangesBaseAddressFieldContext *rangesBaseAddressField();
    SysrootFieldContext *sysrootField();
    SdkFieldContext *sdkField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiCompileUnitFieldContext* diCompileUnitField();

  class  DiCommonBlockFieldContext : public antlr4::ParserRuleContext {
  public:
    DiCommonBlockFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ScopeFieldContext *scopeField();
    DeclarationFieldContext *declarationField();
    NameFieldContext *nameField();
    FileFieldContext *fileField();
    LineFieldContext *lineField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiCommonBlockFieldContext* diCommonBlockField();

  class  DiBasicTypeFieldContext : public antlr4::ParserRuleContext {
  public:
    DiBasicTypeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TagFieldContext *tagField();
    NameFieldContext *nameField();
    SizeFieldContext *sizeField();
    AlignFieldContext *alignField();
    EncodingFieldContext *encodingField();
    FlagsFieldContext *flagsField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiBasicTypeFieldContext* diBasicTypeField();

  class  GenericDINodeFieldContext : public antlr4::ParserRuleContext {
  public:
    GenericDINodeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TagFieldContext *tagField();
    HeaderFieldContext *headerField();
    OperandsFieldContext *operandsField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GenericDINodeFieldContext* genericDINodeField();

  class  TagFieldContext : public antlr4::ParserRuleContext {
  public:
    TagFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DwarfTag();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TagFieldContext* tagField();

  class  HeaderFieldContext : public antlr4::ParserRuleContext {
  public:
    HeaderFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  HeaderFieldContext* headerField();

  class  OperandsFieldContext : public antlr4::ParserRuleContext {
  public:
    OperandsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<MdFieldContext *> mdField();
    MdFieldContext* mdField(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OperandsFieldContext* operandsField();

  class  DiTemplateValueParameterFieldContext : public antlr4::ParserRuleContext {
  public:
    DiTemplateValueParameterFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TagFieldContext *tagField();
    NameFieldContext *nameField();
    TypeFieldContext *typeField();
    DefaultedFieldContext *defaultedField();
    ValueFieldContext *valueField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiTemplateValueParameterFieldContext* diTemplateValueParameterField();

  class  NameFieldContext : public antlr4::ParserRuleContext {
  public:
    NameFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NameFieldContext* nameField();

  class  TypeFieldContext : public antlr4::ParserRuleContext {
  public:
    TypeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeFieldContext* typeField();

  class  DefaultedFieldContext : public antlr4::ParserRuleContext {
  public:
    DefaultedFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DefaultedFieldContext* defaultedField();

  class  ValueFieldContext : public antlr4::ParserRuleContext {
  public:
    ValueFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueFieldContext* valueField();

  class  MdFieldContext : public antlr4::ParserRuleContext {
  public:
    MdFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NullConstContext *nullConst();
    MetadataContext *metadata();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MdFieldContext* mdField();

  class  DiSubroutineTypeFieldContext : public antlr4::ParserRuleContext {
  public:
    DiSubroutineTypeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FlagsFieldContext *flagsField();
    CcFieldContext *ccField();
    TypesFieldContext *typesField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiSubroutineTypeFieldContext* diSubroutineTypeField();

  class  FlagsFieldContext : public antlr4::ParserRuleContext {
  public:
    FlagsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    DiFlagsContext *diFlags();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FlagsFieldContext* flagsField();

  class  DiFlagsContext : public antlr4::ParserRuleContext {
  public:
    DiFlagsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> DiFlag();
    antlr4::tree::TerminalNode* DiFlag(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiFlagsContext* diFlags();

  class  CcFieldContext : public antlr4::ParserRuleContext {
  public:
    CcFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DwarfCc();
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CcFieldContext* ccField();

  class  AlignFieldContext : public antlr4::ParserRuleContext {
  public:
    AlignFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AlignFieldContext* alignField();

  class  AllocatedFieldContext : public antlr4::ParserRuleContext {
  public:
    AllocatedFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AllocatedFieldContext* allocatedField();

  class  AnnotationsFieldContext : public antlr4::ParserRuleContext {
  public:
    AnnotationsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AnnotationsFieldContext* annotationsField();

  class  ArgFieldContext : public antlr4::ParserRuleContext {
  public:
    ArgFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArgFieldContext* argField();

  class  AssociatedFieldContext : public antlr4::ParserRuleContext {
  public:
    AssociatedFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AssociatedFieldContext* associatedField();

  class  AttributesFieldContext : public antlr4::ParserRuleContext {
  public:
    AttributesFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttributesFieldContext* attributesField();

  class  BaseTypeFieldContext : public antlr4::ParserRuleContext {
  public:
    BaseTypeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BaseTypeFieldContext* baseTypeField();

  class  ChecksumFieldContext : public antlr4::ParserRuleContext {
  public:
    ChecksumFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ChecksumFieldContext* checksumField();

  class  ChecksumkindFieldContext : public antlr4::ParserRuleContext {
  public:
    ChecksumkindFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ChecksumKind();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ChecksumkindFieldContext* checksumkindField();

  class  ColumnFieldContext : public antlr4::ParserRuleContext {
  public:
    ColumnFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ColumnFieldContext* columnField();

  class  ConfigMacrosFieldContext : public antlr4::ParserRuleContext {
  public:
    ConfigMacrosFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConfigMacrosFieldContext* configMacrosField();

  class  ContainingTypeFieldContext : public antlr4::ParserRuleContext {
  public:
    ContainingTypeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ContainingTypeFieldContext* containingTypeField();

  class  CountFieldContext : public antlr4::ParserRuleContext {
  public:
    CountFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldOrIntContext *mdFieldOrInt();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CountFieldContext* countField();

  class  DebugInfoForProfilingFieldContext : public antlr4::ParserRuleContext {
  public:
    DebugInfoForProfilingFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DebugInfoForProfilingFieldContext* debugInfoForProfilingField();

  class  DeclarationFieldContext : public antlr4::ParserRuleContext {
  public:
    DeclarationFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DeclarationFieldContext* declarationField();

  class  DirectoryFieldContext : public antlr4::ParserRuleContext {
  public:
    DirectoryFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DirectoryFieldContext* directoryField();

  class  DiscriminatorFieldContext : public antlr4::ParserRuleContext {
  public:
    DiscriminatorFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiscriminatorFieldContext* discriminatorField();

  class  DataLocationFieldContext : public antlr4::ParserRuleContext {
  public:
    DataLocationFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DataLocationFieldContext* dataLocationField();

  class  DiscriminatorIntFieldContext : public antlr4::ParserRuleContext {
  public:
    DiscriminatorIntFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DiscriminatorIntFieldContext* discriminatorIntField();

  class  DwarfAddressSpaceFieldContext : public antlr4::ParserRuleContext {
  public:
    DwarfAddressSpaceFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DwarfAddressSpaceFieldContext* dwarfAddressSpaceField();

  class  DwoIdFieldContext : public antlr4::ParserRuleContext {
  public:
    DwoIdFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DwoIdFieldContext* dwoIdField();

  class  ElementsFieldContext : public antlr4::ParserRuleContext {
  public:
    ElementsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElementsFieldContext* elementsField();

  class  EmissionKindFieldContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *emissionKind = nullptr;
    EmissionKindFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EmissionKindFieldContext* emissionKindField();

  class  EncodingFieldContext : public antlr4::ParserRuleContext {
  public:
    EncodingFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();
    antlr4::tree::TerminalNode *DwarfAttEncoding();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EncodingFieldContext* encodingField();

  class  EntityFieldContext : public antlr4::ParserRuleContext {
  public:
    EntityFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EntityFieldContext* entityField();

  class  EnumsFieldContext : public antlr4::ParserRuleContext {
  public:
    EnumsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumsFieldContext* enumsField();

  class  ExportSymbolsFieldContext : public antlr4::ParserRuleContext {
  public:
    ExportSymbolsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExportSymbolsFieldContext* exportSymbolsField();

  class  ExprFieldContext : public antlr4::ParserRuleContext {
  public:
    ExprFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprFieldContext* exprField();

  class  ExtraDataFieldContext : public antlr4::ParserRuleContext {
  public:
    ExtraDataFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExtraDataFieldContext* extraDataField();

  class  FileFieldContext : public antlr4::ParserRuleContext {
  public:
    FileFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FileFieldContext* fileField();

  class  FilenameFieldContext : public antlr4::ParserRuleContext {
  public:
    FilenameFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FilenameFieldContext* filenameField();

  class  FlagsStringFieldContext : public antlr4::ParserRuleContext {
  public:
    FlagsStringFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FlagsStringFieldContext* flagsStringField();

  class  GetterFieldContext : public antlr4::ParserRuleContext {
  public:
    GetterFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GetterFieldContext* getterField();

  class  GlobalsFieldContext : public antlr4::ParserRuleContext {
  public:
    GlobalsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GlobalsFieldContext* globalsField();

  class  IdentifierFieldContext : public antlr4::ParserRuleContext {
  public:
    IdentifierFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdentifierFieldContext* identifierField();

  class  ImportsFieldContext : public antlr4::ParserRuleContext {
  public:
    ImportsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportsFieldContext* importsField();

  class  IncludePathFieldContext : public antlr4::ParserRuleContext {
  public:
    IncludePathFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IncludePathFieldContext* includePathField();

  class  InlinedAtFieldContext : public antlr4::ParserRuleContext {
  public:
    InlinedAtFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InlinedAtFieldContext* inlinedAtField();

  class  IsDeclFieldContext : public antlr4::ParserRuleContext {
  public:
    IsDeclFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IsDeclFieldContext* isDeclField();

  class  IsDefinitionFieldContext : public antlr4::ParserRuleContext {
  public:
    IsDefinitionFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IsDefinitionFieldContext* isDefinitionField();

  class  IsImplicitCodeFieldContext : public antlr4::ParserRuleContext {
  public:
    IsImplicitCodeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IsImplicitCodeFieldContext* isImplicitCodeField();

  class  IsLocalFieldContext : public antlr4::ParserRuleContext {
  public:
    IsLocalFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IsLocalFieldContext* isLocalField();

  class  IsOptimizedFieldContext : public antlr4::ParserRuleContext {
  public:
    IsOptimizedFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IsOptimizedFieldContext* isOptimizedField();

  class  IsUnsignedFieldContext : public antlr4::ParserRuleContext {
  public:
    IsUnsignedFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IsUnsignedFieldContext* isUnsignedField();

  class  ApiNotesFieldContext : public antlr4::ParserRuleContext {
  public:
    ApiNotesFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ApiNotesFieldContext* apiNotesField();

  class  LanguageFieldContext : public antlr4::ParserRuleContext {
  public:
    LanguageFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DwarfLang();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LanguageFieldContext* languageField();

  class  LineFieldContext : public antlr4::ParserRuleContext {
  public:
    LineFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LineFieldContext* lineField();

  class  LinkageNameFieldContext : public antlr4::ParserRuleContext {
  public:
    LinkageNameFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LinkageNameFieldContext* linkageNameField();

  class  LowerBoundFieldContext : public antlr4::ParserRuleContext {
  public:
    LowerBoundFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldOrIntContext *mdFieldOrInt();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LowerBoundFieldContext* lowerBoundField();

  class  MacrosFieldContext : public antlr4::ParserRuleContext {
  public:
    MacrosFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MacrosFieldContext* macrosField();

  class  NameTableKindFieldContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *nameTableKind = nullptr;
    NameTableKindFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NameTableKindFieldContext* nameTableKindField();

  class  NodesFieldContext : public antlr4::ParserRuleContext {
  public:
    NodesFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NodesFieldContext* nodesField();

  class  OffsetFieldContext : public antlr4::ParserRuleContext {
  public:
    OffsetFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OffsetFieldContext* offsetField();

  class  ProducerFieldContext : public antlr4::ParserRuleContext {
  public:
    ProducerFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProducerFieldContext* producerField();

  class  RangesBaseAddressFieldContext : public antlr4::ParserRuleContext {
  public:
    RangesBaseAddressFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RangesBaseAddressFieldContext* rangesBaseAddressField();

  class  RankFieldContext : public antlr4::ParserRuleContext {
  public:
    RankFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldOrIntContext *mdFieldOrInt();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RankFieldContext* rankField();

  class  RetainedNodesFieldContext : public antlr4::ParserRuleContext {
  public:
    RetainedNodesFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RetainedNodesFieldContext* retainedNodesField();

  class  RetainedTypesFieldContext : public antlr4::ParserRuleContext {
  public:
    RetainedTypesFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RetainedTypesFieldContext* retainedTypesField();

  class  RuntimeLangFieldContext : public antlr4::ParserRuleContext {
  public:
    RuntimeLangFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DwarfLang();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RuntimeLangFieldContext* runtimeLangField();

  class  RuntimeVersionFieldContext : public antlr4::ParserRuleContext {
  public:
    RuntimeVersionFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RuntimeVersionFieldContext* runtimeVersionField();

  class  ScopeFieldContext : public antlr4::ParserRuleContext {
  public:
    ScopeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ScopeFieldContext* scopeField();

  class  ScopeLineFieldContext : public antlr4::ParserRuleContext {
  public:
    ScopeLineFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ScopeLineFieldContext* scopeLineField();

  class  SdkFieldContext : public antlr4::ParserRuleContext {
  public:
    SdkFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SdkFieldContext* sdkField();

  class  SetterFieldContext : public antlr4::ParserRuleContext {
  public:
    SetterFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SetterFieldContext* setterField();

  class  SizeFieldContext : public antlr4::ParserRuleContext {
  public:
    SizeFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SizeFieldContext* sizeField();

  class  SourceFieldContext : public antlr4::ParserRuleContext {
  public:
    SourceFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SourceFieldContext* sourceField();

  class  SpFlagsFieldContext : public antlr4::ParserRuleContext {
  public:
    SpFlagsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DiSPFlagContext *> diSPFlag();
    DiSPFlagContext* diSPFlag(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SpFlagsFieldContext* spFlagsField();

  class  SplitDebugFilenameFieldContext : public antlr4::ParserRuleContext {
  public:
    SplitDebugFilenameFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SplitDebugFilenameFieldContext* splitDebugFilenameField();

  class  SplitDebugInliningFieldContext : public antlr4::ParserRuleContext {
  public:
    SplitDebugInliningFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BoolConstContext *boolConst();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SplitDebugInliningFieldContext* splitDebugInliningField();

  class  StrideFieldContext : public antlr4::ParserRuleContext {
  public:
    StrideFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldOrIntContext *mdFieldOrInt();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StrideFieldContext* strideField();

  class  StringLengthFieldContext : public antlr4::ParserRuleContext {
  public:
    StringLengthFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StringLengthFieldContext* stringLengthField();

  class  StringLengthExpressionFieldContext : public antlr4::ParserRuleContext {
  public:
    StringLengthExpressionFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StringLengthExpressionFieldContext* stringLengthExpressionField();

  class  StringLocationExpressionFieldContext : public antlr4::ParserRuleContext {
  public:
    StringLocationExpressionFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StringLocationExpressionFieldContext* stringLocationExpressionField();

  class  SysrootFieldContext : public antlr4::ParserRuleContext {
  public:
    SysrootFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SysrootFieldContext* sysrootField();

  class  TargetFuncNameFieldContext : public antlr4::ParserRuleContext {
  public:
    TargetFuncNameFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TargetFuncNameFieldContext* targetFuncNameField();

  class  TemplateParamsFieldContext : public antlr4::ParserRuleContext {
  public:
    TemplateParamsFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TemplateParamsFieldContext* templateParamsField();

  class  ThisAdjustmentFieldContext : public antlr4::ParserRuleContext {
  public:
    ThisAdjustmentFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ThisAdjustmentFieldContext* thisAdjustmentField();

  class  ThrownTypesFieldContext : public antlr4::ParserRuleContext {
  public:
    ThrownTypesFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ThrownTypesFieldContext* thrownTypesField();

  class  TypeMacinfoFieldContext : public antlr4::ParserRuleContext {
  public:
    TypeMacinfoFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DwarfMacinfo();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeMacinfoFieldContext* typeMacinfoField();

  class  TypesFieldContext : public antlr4::ParserRuleContext {
  public:
    TypesFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypesFieldContext* typesField();

  class  UnitFieldContext : public antlr4::ParserRuleContext {
  public:
    UnitFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UnitFieldContext* unitField();

  class  UpperBoundFieldContext : public antlr4::ParserRuleContext {
  public:
    UpperBoundFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldOrIntContext *mdFieldOrInt();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UpperBoundFieldContext* upperBoundField();

  class  ValueIntFieldContext : public antlr4::ParserRuleContext {
  public:
    ValueIntFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueIntFieldContext* valueIntField();

  class  ValueStringFieldContext : public antlr4::ParserRuleContext {
  public:
    ValueStringFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *StringLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueStringFieldContext* valueStringField();

  class  VarFieldContext : public antlr4::ParserRuleContext {
  public:
    VarFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VarFieldContext* varField();

  class  VirtualIndexFieldContext : public antlr4::ParserRuleContext {
  public:
    VirtualIndexFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IntLit();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VirtualIndexFieldContext* virtualIndexField();

  class  VirtualityFieldContext : public antlr4::ParserRuleContext {
  public:
    VirtualityFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DwarfVirtuality();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VirtualityFieldContext* virtualityField();

  class  VtableHolderFieldContext : public antlr4::ParserRuleContext {
  public:
    VtableHolderFieldContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MdFieldContext *mdField();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VtableHolderFieldContext* vtableHolderField();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool typeSempred(TypeContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

}  // namespace antlr::generated
