// Call Operations - Instruction Selection
// Handler for call instruction lowering

#include "codegen/InstructionSelectInternal.h"
#include "codegen/NasmSafe.h"

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>

namespace llvm_i286 {
namespace codegen {

// Compute the size in bytes of an LLVM IR type
static int64_t typeSizeBytes(const ir::Type* type,
                             const std::map<std::string, std::unique_ptr<ir::Type>>* typeDefs) {
    if (!type) return 0;
    switch (type->kind) {
        case ir::TypeKind::Integer:
            return type->bitWidth / 8;
        case ir::TypeKind::Pointer:
            return 4; // 32-bit pointer
        case ir::TypeKind::Array:
            return (int64_t)type->numElements * typeSizeBytes(type->elementType.get(), typeDefs);
        case ir::TypeKind::Struct: {
            int64_t size = 0;
            for (const auto& elem : type->structElements) {
                size += typeSizeBytes(elem.get(), typeDefs);
            }
            return size;
        }
        case ir::TypeKind::Named: {
            if (typeDefs) {
                auto it = typeDefs->find(type->name);
                if (it != typeDefs->end()) {
                    return typeSizeBytes(it->second.get(), typeDefs);
                }
            }
            return 0;
        }
        default:
            return 0;
    }
}

// Parse a type name from a GEP string starting at position pos
// Returns the type name and updates pos to point after the type
static std::string parseTypeName(const std::string& str, size_t& pos) {
    size_t start = pos;
    // Skip leading whitespace
    while (start < str.size() && str[start] == ' ') start++;
    
    if (start >= str.size()) return "";
    
    if (str[start] == '%') {
        // Named type: %struct.NAME or %TYPE
        size_t end = start;
        while (end < str.size() && str[end] != ',' && str[end] != ')' && str[end] != ' ') end++;
        pos = end;
        return str.substr(start, end - start);
    }
    
    // Array type: [N x TYPE]
    if (str[start] == '[') {
        int depth = 1;
        size_t end = start + 1;
        while (end < str.size() && depth > 0) {
            if (str[end] == '[') depth++;
            else if (str[end] == ']') depth--;
            end++;
        }
        pos = end;
        return str.substr(start, end - start);
    }
    
    // Simple type: i8, i16, i32, i64, ptr, etc.
    size_t end = start;
    while (end < str.size() && str[end] != ',' && str[end] != ')' && str[end] != ' ') end++;
    pos = end;
    return str.substr(start, end - start);
}

// Compute byte offset for a GEP constant expression
// GEP format: getelementptr [inbounds] (TYPE, ptr @name, i32 N, i32 M, ...)
int64_t computeGEPByteOffset(const std::string& gepStr,
                                    const std::map<std::string, std::unique_ptr<ir::Type>>* typeDefs) {
    // Find the opening paren after getelementptr
    auto parenPos = gepStr.find('(');
    if (parenPos == std::string::npos) return 0;

    size_t pos = parenPos + 1;
    // Parse source type
    std::string sourceType = parseTypeName(gepStr, pos);

    // Skip comma and spaces
    while (pos < gepStr.size() && (gepStr[pos] == ',' || gepStr[pos] == ' ')) pos++;

    // Skip "ptr@name" or "ptr @name" part - find the next comma after @
    auto atPos = gepStr.find('@', pos);
    if (atPos == std::string::npos) return 0;
    auto nextComma = gepStr.find(',', atPos);
    if (nextComma == std::string::npos) return 0;
    pos = nextComma + 1;

    // Collect all indices
    std::vector<int64_t> indices;
    while (pos < gepStr.size()) {
        // Skip whitespace and commas
        while (pos < gepStr.size() && (gepStr[pos] == ',' || gepStr[pos] == ' ')) pos++;
        if (pos >= gepStr.size() || gepStr[pos] == ')') break;

        // Parse type prefix: try i64 first, then i32 (handles space-stripped "i320" as i32+0)
        if (gepStr.compare(pos, 3, "i64") == 0) {
            pos += 3;
        } else if (gepStr.compare(pos, 3, "i32") == 0) {
            pos += 3;
        } else {
            break; // Unknown type prefix
        }

        // Skip optional space
        while (pos < gepStr.size() && gepStr[pos] == ' ') pos++;

        // Parse number
        std::string numStr;
        if (pos < gepStr.size() && gepStr[pos] == '-') {
            numStr += '-';
            pos++;
        }
        while (pos < gepStr.size() && isdigit(gepStr[pos])) {
            numStr += gepStr[pos];
            pos++;
        }
        if (!numStr.empty() && numStr != "-") {
            indices.push_back(std::stoll(numStr));
        }
    }

    if (indices.empty()) return 0;

    // Compute byte offset
    int64_t offset = 0;
    
    // First index: multiply by sizeof(sourceType)
    int64_t sourceSize = 0;
    if (sourceType[0] == '%') {
        // Named struct type
        if (typeDefs) {
            auto it = typeDefs->find(sourceType);
            if (it != typeDefs->end()) {
                sourceSize = typeSizeBytes(it->second.get(), typeDefs);
            }
        }
    } else if (sourceType[0] == '[') {
        // Array type: [N x TYPE]
        // Parse N
        auto xPos = sourceType.find("x");
        if (xPos != std::string::npos) {
            std::string numStr;
            size_t i = 1; // skip '['
            while (i < sourceType.size() && (sourceType[i] == ' ' || isdigit(sourceType[i]))) {
                if (isdigit(sourceType[i])) numStr += sourceType[i];
                i++;
            }
            if (!numStr.empty()) {
                int64_t n = std::stoll(numStr);
                // Parse element type
                std::string elemType;
                size_t j = xPos + 1;
                while (j < sourceType.size() && sourceType[j] == ' ') j++;
                while (j < sourceType.size() && sourceType[j] != ']' && sourceType[j] != ' ') j++;
                elemType = sourceType.substr(xPos + 1, j - xPos - 1);
                // Trim
                while (!elemType.empty() && elemType.back() == ' ') elemType.pop_back();
                while (!elemType.empty() && elemType.front() == ' ') elemType.erase(0, 1);
                
                int64_t elemSize = 0;
                if (elemType == "i8") elemSize = 1;
                else if (elemType == "i16") elemSize = 2;
                else if (elemType == "i32") elemSize = 4;
                else if (elemType == "i64") elemSize = 8;
                else if (elemType == "ptr") elemSize = 4;
                sourceSize = n * elemSize;
            }
        }
    } else {
        // Simple type
        if (sourceType == "i8") sourceSize = 1;
        else if (sourceType == "i16") sourceSize = 2;
        else if (sourceType == "i32") sourceSize = 4;
        else if (sourceType == "i64") sourceSize = 8;
        else if (sourceType == "ptr") sourceSize = 4;
    }
    
    offset += indices[0] * sourceSize;
    
    // Subsequent indices: walk through the type
    const ir::Type* currentType = nullptr;
    std::unique_ptr<ir::Type> resolvedType;
    
    // Resolve the source type
    if (sourceType[0] == '%' && typeDefs) {
        auto it = typeDefs->find(sourceType);
        if (it != typeDefs->end()) {
            currentType = it->second.get();
        }
    } else if (sourceType[0] == '[') {
        // Array source type: [NxTYPE]
        // After first index, we're inside the array; subsequent indices index into elements
        auto xPos = sourceType.find("x");
        if (xPos != std::string::npos) {
            // Parse element type after 'x'
            std::string elemType;
            size_t j = xPos + 1;
            while (j < sourceType.size() && sourceType[j] == ' ') j++;
            while (j < sourceType.size() && sourceType[j] != ']' && sourceType[j] != ' ') j++;
            elemType = sourceType.substr(xPos + 1, j - xPos - 1);
            while (!elemType.empty() && elemType.back() == ' ') elemType.pop_back();
            while (!elemType.empty() && elemType.front() == ' ') elemType.erase(0, 1);

            auto arrType = std::make_unique<ir::Type>();
            arrType->kind = ir::TypeKind::Array;
            if (elemType == "ptr") {
                arrType->elementType = ir::Type::makeInteger(32);
            } else if (elemType == "i8") {
                arrType->elementType = ir::Type::makeInteger(8);
            } else if (elemType == "i16") {
                arrType->elementType = ir::Type::makeInteger(16);
            } else if (elemType == "i32") {
                arrType->elementType = ir::Type::makeInteger(32);
            } else if (elemType == "i64") {
                arrType->elementType = ir::Type::makeInteger(64);
            } else {
                arrType->elementType = ir::Type::makeVoid();
            }
            resolvedType = std::move(arrType);
            currentType = resolvedType.get();
        }
    }
    
    for (size_t i = 1; i < indices.size() && currentType; i++) {
        if (currentType->kind == ir::TypeKind::Struct) {
            // Struct field index
            int64_t fieldOffset = 0;
            for (int j = 0; j < indices[i] && j < (int)currentType->structElements.size(); j++) {
                fieldOffset += typeSizeBytes(currentType->structElements[j].get(), typeDefs);
            }
            offset += fieldOffset;
            // Move to the field type
            if (indices[i] >= 0 && indices[i] < (int64_t)currentType->structElements.size()) {
                const ir::Type* fieldType = currentType->structElements[indices[i]].get();
                // If it's a named type, resolve it
                if (fieldType->kind == ir::TypeKind::Named && typeDefs) {
                    auto it = typeDefs->find(fieldType->name);
                    if (it != typeDefs->end()) {
                        currentType = it->second.get();
                    } else {
                        currentType = nullptr;
                    }
                } else {
                    currentType = fieldType;
                }
            } else {
                currentType = nullptr;
            }
        } else if (currentType->kind == ir::TypeKind::Array) {
            // Array element index
            offset += indices[i] * typeSizeBytes(currentType->elementType.get(), typeDefs);
            currentType = currentType->elementType.get();
        } else {
            // Simple type or other - can't index further
            break;
        }
    }
    
    // Handle simple types (non-struct, non-array source) where there's only one index
    // The first index already computed the offset correctly for simple types
    
    return offset;
}

std::vector<LoweredInstruction> lowerCall(SelectorState& state,
    const ir::Instruction& irInst, const std::string& resultReg) {
    // Lower call: result = call callee(arg1, arg2, ...)
    // Calling convention: arguments pushed right-to-left, return value in AX
    // Caller cleans up stack after call

    std::vector<LoweredInstruction> loweredVec;
    LoweredInstruction lowered;

    bool skipCallAndCleanup = false;
    
    std::string callee;
    if (!irInst.calleeName.empty()) {
        callee = irInst.calleeName;
    } else if (!irInst.operands.empty()) {
        callee = irInst.operands[0].name;
    }

    // Handle inline asm calls: asm "", "=r,0,..."(@function)
    // This is a no-op template that loads a function address into a register.
    // Emit: lea ax, [function]; mov dx, cs  (for code pointers)
    // Then store result to vreg's spill slot.
    if (irInst.isInlineAsm) {
        std::string funcName = safeNasmName(callee);
        // Convert leading dot to underscore (@.str -> _str)
        if (!funcName.empty() && funcName[0] == '.') {
            funcName = "_" + funcName.substr(1);
        }
        
        LoweredInstruction loadAddr;
        
        Instruction286 leaAx;
        leaAx.mnemonic = "lea";
        leaAx.operands.push_back("ax");
        leaAx.operands.push_back("[" + funcName + "]");
        loadAddr.instructions.push_back(leaAx);
        
        Instruction286 movDxCs;
        movDxCs.mnemonic = "mov";
        movDxCs.operands.push_back("dx");
        movDxCs.operands.push_back("cs");
        loadAddr.instructions.push_back(movDxCs);
        
        // Store result to spill slot if there's a result vreg
        if (!irInst.resultName.empty()) {
            std::string slot = state.frame.allocResultSlot(irInst.resultName, 4, true);
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + slot + "]");
            storeLow.operands.push_back("ax");
            loadAddr.instructions.push_back(storeLow);
            
            std::string highOffset = state.frame.getHighBpOffset(slot);
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            storeHigh.operands.push_back("[" + highOffset + "]");
            storeHigh.operands.push_back("dx");
            loadAddr.instructions.push_back(storeHigh);
            
            state.frame.setPhysReg(irInst.resultName, slot);
        }
        
        loweredVec.push_back(loadAddr);
        return loweredVec;
    }

    // Handle LLVM intrinsics: lower llvm.memcpy to memcpy call
    bool isLLVMMemcpy = (callee.find("llvm.memcpy") != std::string::npos);
    if (isLLVMMemcpy) {
        callee = "memcpy";
    }
    
    // Handle LLVM intrinsics: lower llvm.memset to memset call
    bool isLLVMMemset = (callee.find("llvm.memset") != std::string::npos);
    if (isLLVMMemset) {
        callee = "memset";
    }
    
    // Skip inline assembly calls (compiler barriers, etc.)
    if (callee.find("asm") == 0 || callee.find("asm sideeffect") == 0) {
        // Inline asm is a no-op for our purposes - just return empty lowered
        return loweredVec;
    }
    
    // Skip null function pointer calls (undefined behavior)
    if (callee == "null" || callee == "0") {
        return loweredVec;
    }
    
    // Detect llvm.va_start for inline generation
    bool isLLVAVaStart = (callee == "llvm.va_start");
    
    // Lower llvm.va_end to llvm_va_end (no-op, but keep for now)
    bool isLLVAVaEnd = (callee == "llvm.va_end");
    if (isLLVAVaEnd) {
        callee = "llvm_va_end";
    }
    
    // Resolve alias: if callee is an alias, use the target function
    auto aliasIt = state.aliasMap.find(callee);
    if (aliasIt != state.aliasMap.end()) {
        callee = aliasIt->second;
    }

    // Push arguments (right-to-left order)
    // Arguments are in irInst.callArgs (if available) or irInst.operands[1..n]
    std::vector<std::string> args;
    if (!irInst.callArgs.empty()) {
        args = irInst.callArgs;
    } else {
        // Fallback: use operands starting from index 1
        for (size_t i = 1; i < irInst.operands.size(); i++) {
            args.push_back(irInst.operands[i].name);
        }
    }

    // For llvm.memcpy intrinsic: drop the last argument (is_volatile, i1 type)
    // llvm.memcpy has 4 args: (dst, src, size, is_volatile) but memcpy has 3: (dst, src, size)
    if (isLLVMMemcpy && args.size() > 3) {
        args.pop_back();
    }
    
    // For llvm.memset intrinsic: drop the last argument (is_volatile, i1 type)
    // llvm.memset has 4 args: (dst, val, size, is_volatile) but memset has 3: (dst, val, size)
    if (isLLVMMemset && args.size() > 3) {
        args.pop_back();
    }
    
    // For llvm.va_start: add synthetic second argument (address of last named param)
    // Our C implementation expects: llvm_va_start(void *ap, void *last_arg)
    // The last_arg should point to the first parameter of the calling function
      // In the calling function's stack frame: [bp+6] = first param (far call)
    if (isLLVAVaStart) {
        // Add synthetic argument: address of last named parameter (bp+6)
        args.push_back("bp_plus_6_sentinel");
        // We need a local copy of callArgKinds to modify (since irInst is const)
        // The synthetic arg is a far pointer (32-bit)
        // We'll handle this in the stack cleanup by checking for synthetic args
        
        // Mark the va_list alloca as SS-derived for pointer loads
        // After va_start, the va_list contains SS-derived pointers to varargs
        if (!args.empty()) {
            std::string apArg = args[0]; // First arg is the ap alloca
            // The ap alloca's contents are SS-derived pointers
            state.ssDerivedPtrVregs.insert(apArg);
        }
    }
    
    // llvm.va_end is a no-op - clear args to skip call and cleanup
    if (isLLVAVaEnd) {
        args.clear();
    }

    // Push arguments right-to-left
    for (int i = static_cast<int>(args.size()) - 1; i >= 0; i--) {
        std::string argName = args[i];
        
        // Detect floating-point constants that we can't materialize
        // Skip them by pushing 0 instead (FP operations not fully supported)
        bool argIsFPConst = argName.find("0xK") != std::string::npos ||
                            (argName.find('e') != std::string::npos && 
                             (argName.find('+') != std::string::npos || argName.find('-') != std::string::npos));
        if (argIsFPConst) {
            // Push 0 instead of the FP constant
            Instruction286 pushZero;
            pushZero.mnemonic = "push";
            pushZero.operands.push_back("0");
            lowered.instructions.push_back(pushZero);
            continue;
        }
        
        // Handle synthetic bp+6 sentinel for llvm_va_start
        if (argName == "bp_plus_6_sentinel") {
            // Push the far pointer address of the last named parameter (bp+6)
            // Far calls: bp+2 = return IP, bp+4 = return CS, bp+6 = first param offset
            // High word = SS (segment selector for stack), low word = offset
            Instruction286 leaArg;
            leaArg.mnemonic = "lea";
            leaArg.operands.push_back("ax");
            leaArg.operands.push_back("[bp+6]");
            lowered.instructions.push_back(leaArg);
            
            Instruction286 pushHigh;
            pushHigh.mnemonic = "push";
            pushHigh.operands.push_back("SS");
            lowered.instructions.push_back(pushHigh);
            
            Instruction286 pushLow;
            pushLow.mnemonic = "push";
            pushLow.operands.push_back("ax");
            lowered.instructions.push_back(pushLow);
            continue;
        }

        // Determine if this is a vreg reference (starts with '%') or a constant/global
        bool argIsVregRef = (!argName.empty() && argName[0] == '%');
        // Use argName directly for vreg lookup (maps use full name with % prefix)
        std::string vregLookupName = argName;

        // Check if argument is a global variable (starts with '.' or '@')
        bool isGlobal = (!argName.empty() && (argName[0] == '.' || argName[0] == '@'));

        // Check if argument is a getelementptr constant expression
        bool isGEPExpr = (!argName.empty() && argName.substr(0, 13) == "getelementptr");
        std::string gepGlobalName;
        int64_t gepOffset = 0;
        if (isGEPExpr) {
            // Parse: getelementptr(TYPE, ptr @name, i32 N, i32 M, ...)
            auto atPos = argName.find('@');
            if (atPos != std::string::npos) {
                auto commaPos = argName.find(',', atPos);
                if (commaPos != std::string::npos) {
                    gepGlobalName = argName.substr(atPos + 1, commaPos - atPos - 1);
                }
            }
            // Compute byte offset using struct type information
            gepOffset = computeGEPByteOffset(argName, state.typeDefinitions);
            isGlobal = true;
        }

        // Check if argument is a ptrtoint constant expression: ptrtoint(ptr@global to i32)
        // May also contain nested getelementptr: ptrtoint(ptr getelementptr(...@global...) to i32)
        bool isPtrToIntExpr = (!argName.empty() && argName.substr(0, 8) == "ptrtoint");
        std::string ptrToIntGlobalName;
        int64_t ptrToIntGepOffset = 0;
        bool ptrToIntHasGep = false;
        if (isPtrToIntExpr) {
            // Check for nested getelementptr inside ptrtoint
            auto gepPos = argName.find("getelementptr");
            if (gepPos != std::string::npos) {
                // ptrtoint wrapping GEP: extract global name and compute offset
                auto atPos = argName.find('@', gepPos);
                if (atPos != std::string::npos) {
                    auto commaPos = argName.find(',', atPos);
                    if (commaPos != std::string::npos) {
                        ptrToIntGlobalName = argName.substr(atPos + 1, commaPos - atPos - 1);
                    }
                    // Extract the GEP substring by finding matching parens
                    // GEP starts at gepPos, find its opening '(' and matching ')'
                    size_t parenStart = argName.find('(', gepPos);
                    if (parenStart != std::string::npos) {
                        size_t parenCount = 0;
                        size_t end = parenStart;
                        for (size_t i = parenStart; i < argName.size(); i++) {
                            if (argName[i] == '(') parenCount++;
                            else if (argName[i] == ')') {
                                parenCount--;
                                if (parenCount == 0) {
                                    end = i + 1;
                                    break;
                                }
                            }
                        }
                        std::string gepSubstr = argName.substr(gepPos, end - gepPos);
                        ptrToIntGepOffset = computeGEPByteOffset(gepSubstr, state.typeDefinitions);
                    }
                    ptrToIntHasGep = true;
                }
            } else {
                // Simple ptrtoint: ptrtoint(ptr@name to i32)
                auto atPos = argName.find('@');
                if (atPos != std::string::npos) {
                    auto toPos = argName.find("to", atPos + 1);
                    if (toPos != std::string::npos) {
                        ptrToIntGlobalName = argName.substr(atPos + 1, toPos - atPos - 1);
                    } else {
                        auto endPos = argName.find(')', atPos + 1);
                        ptrToIntGlobalName = argName.substr(atPos + 1, endPos != std::string::npos ? endPos - atPos - 1 : std::string::npos);
                    }
                }
            }
            isGlobal = true;
        }

        // Convert global variable name to NASM format
        // Must match CodeGen.cpp definition: .str → _str, @main → main
        std::string nasmName = argName;
        if (isGEPExpr && !gepGlobalName.empty()) {
            nasmName = gepGlobalName;
        } else if (isGlobal && isPtrToIntExpr && !ptrToIntGlobalName.empty()) {
            nasmName = ptrToIntGlobalName;
        }
        // Strip @ prefix (function names like @main → main)
        if (!nasmName.empty() && nasmName[0] == '@') {
            nasmName = nasmName.substr(1);
        }
        // Convert . prefix to _ (hidden globals like .str → _str)
        if (!nasmName.empty() && nasmName[0] == '.') {
            nasmName = "_" + nasmName.substr(1);
        }
        // Mangle NASM reserved words
        if (!nasmName.empty()) {
            nasmName = safeNasmName(nasmName);
        }

        // Check if argument is a vreg/parameter (either register-allocated or stack-allocated)
        // Only look up vreg maps if the argument has a % prefix (vreg reference)
        bool argIsVreg = false;
        if (argIsVregRef) {
            argIsVreg = (state.frame.hasSlot(argName)) ||
                         (state.frame.hasSlot(argName));
        }

        // Check if argument is a constant (not a vreg)
        bool isConst = false;
        if (!argIsVreg && !isGlobal) {
            if (argName == "null") {
                isConst = true;
            } else {
                try {
                    std::stoi(argName);
                    isConst = true;
                } catch (...) {}
            }
        }

        // Check if this is a 32-bit argument
        // For LLVM intrinsics renamed to C functions (memset/memcpy), all arguments
        // are 32-bit per C ABI (int and size_t are 32-bit; i8 is promoted to int)
        bool is32 = false;
        if (isLLVMMemset || isLLVMMemcpy) {
            is32 = true;
        } else {
            auto declIt = state.funcParamBitWidths.find(callee);
            if (declIt != state.funcParamBitWidths.end()) {
                // Use the function's parameter types
                if (i >= 0 && i < (int)declIt->second.size()) {
                    is32 = (declIt->second[i] == 32);
                } else if (i >= (int)declIt->second.size()) {
                    // Argument beyond the fixed parameter count (variadic function)
                    // Fall back to result type or assume 32-bit
                    is32 = irInst.resultType ? (irInst.resultType->bitWidth == 32) : true;
                }
            } else if (irInst.resultType) {
                // Fallback: assume all arguments are the same type as result
                is32 = irInst.resultType->bitWidth == 32;
            }
        }

        if (is32) {
            // 32-bit argument: push high word first, then low word (little-endian stack layout)
            // Check if this is a pointer argument (push ADDRESS, not DATA)
            bool isPtrArg = (i >= 0 && i < (int)irInst.callArgKinds.size()) ? irInst.callArgKinds[i] : false;
            
            if (isGlobal) {
                if (isPtrArg) {
                    // Pointer argument - push the far pointer ADDRESS (seg:offset)
                    if (isGEPExpr) {
                        // GEP: compute address via lea, then add offset, then push
                        // (OMF fixup doesn't preserve constant displacement in lea)
                        Instruction286 leaAddr;
                        leaAddr.mnemonic = "lea";
                        leaAddr.operands.push_back("ax");
                        leaAddr.operands.push_back("[" + nasmName + "]");
                        lowered.instructions.push_back(leaAddr);
                        if (gepOffset != 0) {
                            Instruction286 addOff;
                            addOff.mnemonic = "add";
                            addOff.operands.push_back("ax");
                            addOff.operands.push_back(std::to_string(gepOffset));
                            lowered.instructions.push_back(addOff);
                        }

                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back("ds");
                        lowered.instructions.push_back(pushHigh);

                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back("ax");
                        lowered.instructions.push_back(pushLow);
                    } else {
                    // Non-GEP global pointer argument: push far pointer address
                    // seg fixups resolve to 0xFFFF in OMF, so use lea + push cs/ds
                    // Check if this is a function (in CS) or data (in DS)
                    std::string nameWithoutPrefix = argName;
                    if (!nameWithoutPrefix.empty() && nameWithoutPrefix[0] == '@') {
                        nameWithoutPrefix = nameWithoutPrefix.substr(1);
                    }
                    bool isFunction = (state.funcParamBitWidths.find(nameWithoutPrefix) != state.funcParamBitWidths.end());

                    Instruction286 leaAddr;
                    leaAddr.mnemonic = "lea";
                    leaAddr.operands.push_back("ax");
                    leaAddr.operands.push_back("[" + nasmName + "]");
                    lowered.instructions.push_back(leaAddr);

                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back(isFunction ? "cs" : "ds");
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back("ax");
                    lowered.instructions.push_back(pushLow);
                    }
               } else if (isPtrToIntExpr || isGEPExpr) {
                    // ptrtoint or GEP constant expression - push address of global
                    // High word = 0, low word = address (offset)
                    // (OMF fixup doesn't preserve constant displacement in lea)
                    Instruction286 leaLow;
                    leaLow.mnemonic = "lea";
                    leaLow.operands.push_back("ax");
                    leaLow.operands.push_back("[" + nasmName + "]");
                    lowered.instructions.push_back(leaLow);
                    int64_t effectiveOffset = 0;
                    if (isGEPExpr) {
                        effectiveOffset = gepOffset;
                    } else if (ptrToIntHasGep) {
                        effectiveOffset = ptrToIntGepOffset;
                    }
                    if (effectiveOffset != 0) {
                        Instruction286 addOff;
                        addOff.mnemonic = "add";
                        addOff.operands.push_back("ax");
                        addOff.operands.push_back(std::to_string(effectiveOffset));
                        lowered.instructions.push_back(addOff);
                    }

                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back("0");
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back("ax");
                    lowered.instructions.push_back(pushLow);
                } else {
                    // Global variable - push both words of VALUE stored at address
                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back("word [" + nasmName + "+2]");
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back("word [" + nasmName + "]");
                    lowered.instructions.push_back(pushLow);
                }
            } else if (isConst) {
                // 32-bit constant (including null as 0): split into low and high words
                int32_t val = 0;
                if (args[i] != "null") {
                    try { val = std::stoi(args[i]); } catch (...) { val = 0; }
                }
                int16_t lowWord = (int16_t)((uint32_t)val & 0xFFFF);
                int16_t highWord = (int16_t)((uint32_t)val >> 16);

                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back("ax");
                movConst.operands.push_back(std::to_string((int)lowWord));
                lowered.instructions.push_back(movConst);

                Instruction286 pushHigh;
                pushHigh.mnemonic = "push";
                pushHigh.operands.push_back(std::to_string((int)highWord));
                lowered.instructions.push_back(pushHigh);

                Instruction286 pushLow;
                pushLow.mnemonic = "push";
                pushLow.operands.push_back("ax");
                lowered.instructions.push_back(pushLow);
            } else if (isPtrArg && !isGlobal) {
                // Pointer vreg argument - push the far pointer ADDRESS (seg:offset)
                // Get the physical register for the pointer value
                std::string argReg = state.frame.getPhysReg(vregLookupName);
                
                // Determine if this pointer is SS-derived
                // Note: params are NOT SS-derived because the pointer VALUE could point to DS or SS
                bool argIsSS = state.frame.isAlloca(vregLookupName) ||
                               state.ssDerivedPtrVregs.count(vregLookupName) > 0;
                std::string selector = argIsSS ? "ss" : "0";
                
                if (argReg.find("bp") != std::string::npos) {
                    // Check if this is an alloca: the stack offset IS the pointer value
                    bool argIsAlloca = state.frame.isAlloca(vregLookupName);
                    if (argIsAlloca) {
                        // Alloca result: the stack offset IS the pointer value
                        // Push selector (SS for SS-derived, 0 for DS-derived) and bp+offset (offset)
                        int offset = 0;
                        std::string offsetStr = argReg.substr(2); // Remove "bp"
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
                        Instruction286 leaAx;
                        leaAx.mnemonic = "lea";
                        leaAx.operands.push_back("ax");
                        if (offset < 0) {
                            leaAx.operands.push_back("[" + std::string("bp") + std::to_string(offset) + "]");
                        } else if (offset > 0) {
                            leaAx.operands.push_back("[" + std::string("bp+") + std::to_string(offset) + "]");
                        } else {
                            leaAx.operands.push_back("[" + std::string("bp") + "]");
                        }
                        lowered.instructions.push_back(leaAx);

                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back(selector);
                        lowered.instructions.push_back(pushHigh);

                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back("ax");
                        lowered.instructions.push_back(pushLow);
                    } else {
                        // Pointer value stored on stack - push both words
                        // Push selector (high word) first, then offset (low word)
                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back("word [" + argReg + "+2]");
                        lowered.instructions.push_back(pushHigh);
                        
                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back("word [" + argReg + "]");
                        lowered.instructions.push_back(pushLow);
                    }
                } else {
                    // Pointer value in a register - push selector (high), then register (low)
                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back(selector);
                    lowered.instructions.push_back(pushHigh);
                    
                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back(argReg);
                    lowered.instructions.push_back(pushLow);
                }
            } else {
                // Check if this argument is an alloca result (stack offset IS the address)
                // Use argName (with % prefix) for consistent lookup with other codegen
                bool argIsAlloca = state.frame.isAlloca(argName);
                // Determine if this pointer is SS-derived
                // Note: params are NOT SS-derived because the pointer VALUE could point to DS or SS
                bool argIsSS = state.frame.isAlloca(argName) ||
                               state.ssDerivedPtrVregs.count(argName) > 0;
                std::string selector = argIsSS ? "ss" : "0";
                  if (argIsAlloca) {
                    // Alloca result: the stack offset IS the pointer value
                    // We need to compute bp + offset at runtime and push that as the address
                    // Push low word = bp + offset (computed at runtime via lea)
                    // Push high word = selector (SS for SS-derived, 0 for DS-derived)
                    std::string argReg = state.frame.getPhysReg(argName);
                    int offset = 0;
                    if (argReg.find("bp") != std::string::npos) {
                        std::string offsetStr = argReg.substr(2); // Remove "bp"
                        if (!offsetStr.empty()) {
                            try { offset = std::stoi(offsetStr); } catch (...) {}
                        }
                    }
                    // Compute bp + offset: lea ax, [bp+offset]
                    Instruction286 leaAx;
                    leaAx.mnemonic = "lea";
                    leaAx.operands.push_back("ax");
                    if (offset < 0) {
                        leaAx.operands.push_back("[" + std::string("bp") + std::to_string(offset) + "]");
                    } else if (offset > 0) {
                        leaAx.operands.push_back("[" + std::string("bp+") + std::to_string(offset) + "]");
                    } else {
                        leaAx.operands.push_back("[" + std::string("bp") + "]");
                    }
                    lowered.instructions.push_back(leaAx);

                    Instruction286 pushHigh;
                    pushHigh.mnemonic = "push";
                    pushHigh.operands.push_back(selector);
                    lowered.instructions.push_back(pushHigh);

                    Instruction286 pushLow;
                    pushLow.mnemonic = "push";
                    pushLow.operands.push_back("ax");
                    lowered.instructions.push_back(pushLow);
                } else {
                    std::string argReg = state.frame.getPhysReg(vregLookupName);
                    if (argReg.find("bp") != std::string::npos) {
                        // Memory operand (value stored at stack location) - push both words
                        // Push high word first, then low word
                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back("word [" + argReg + "+2]");
                        lowered.instructions.push_back(pushHigh);

                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back("word [" + argReg + "]");
                        lowered.instructions.push_back(pushLow);
                    } else {
                        // Register operand - push DX (high), then register (low)
                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back("dx");
                        lowered.instructions.push_back(pushHigh);

                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back(argReg);
                        lowered.instructions.push_back(pushLow);
                    }
                }
            }
        } else {
            // 16-bit argument (original code)
            if (isGlobal) {
                // Global variable - push its offset/label directly
                Instruction286 pushInst;
                pushInst.mnemonic = "push";
                pushInst.operands.push_back(nasmName);
                lowered.instructions.push_back(pushInst);
            } else if (isConst) {
                // Load constant into AX first, then push
                Instruction286 movConst;
                movConst.mnemonic = "mov";
                movConst.operands.push_back("ax");
                movConst.operands.push_back(args[i]);
                lowered.instructions.push_back(movConst);

                Instruction286 pushInst;
                pushInst.mnemonic = "push";
                pushInst.operands.push_back("ax");
                lowered.instructions.push_back(pushInst);
            } else {
                // Check if this is a pointer argument (push ADDRESS, not DATA)
                bool isPtrArg16 = (i >= 0 && i < (int)irInst.callArgKinds.size()) ? irInst.callArgKinds[i] : false;
                
                if (isPtrArg16) {
                    // Pointer argument - push the far pointer ADDRESS (seg:offset)
                    std::string argReg = state.frame.getPhysReg(vregLookupName);
                    if (argReg.find("bp") != std::string::npos) {
                        // Check if this is an alloca: the stack offset IS the pointer value
                        bool argIsAlloca = state.frame.isAlloca(vregLookupName);
                        if (argIsAlloca) {
                            // Alloca result: the stack offset IS the pointer value
                            int offset = 0;
                            std::string offsetStr = argReg.substr(2); // Remove "bp"
                            if (!offsetStr.empty()) {
                                try { offset = std::stoi(offsetStr); } catch (...) {}
                            }
                            Instruction286 leaAx;
                            leaAx.mnemonic = "lea";
                            leaAx.operands.push_back("ax");
                            if (offset < 0) {
                                leaAx.operands.push_back("[" + std::string("bp") + std::to_string(offset) + "]");
                            } else if (offset > 0) {
                                leaAx.operands.push_back("[" + std::string("bp+") + std::to_string(offset) + "]");
                            } else {
                                leaAx.operands.push_back("[" + std::string("bp") + "]");
                            }
                            lowered.instructions.push_back(leaAx);

                            Instruction286 pushHigh;
                            pushHigh.mnemonic = "push";
                            pushHigh.operands.push_back("ss");
                            lowered.instructions.push_back(pushHigh);

                            Instruction286 pushLow;
                            pushLow.mnemonic = "push";
                            pushLow.operands.push_back("ax");
                            lowered.instructions.push_back(pushLow);
                        } else {
                            // Pointer value stored on stack - push both words
                            // Push selector (high word) first, then offset (low word)
                            Instruction286 pushHigh;
                            pushHigh.mnemonic = "push";
                            pushHigh.operands.push_back("word [" + argReg + "+2]");
                            lowered.instructions.push_back(pushHigh);
                            
                            Instruction286 pushLow;
                            pushLow.mnemonic = "push";
                            pushLow.operands.push_back("word [" + argReg + "]");
                            lowered.instructions.push_back(pushLow);
                        }
                    } else {
                        // Pointer value in a register - push DX (high), then register (low)
                        Instruction286 pushHigh;
                        pushHigh.mnemonic = "push";
                        pushHigh.operands.push_back("dx");
                        lowered.instructions.push_back(pushHigh);
                        
                        Instruction286 pushLow;
                        pushLow.mnemonic = "push";
                        pushLow.operands.push_back(argReg);
                        lowered.instructions.push_back(pushLow);
                    }
                } else {
                    std::string argReg = state.frame.getPhysReg(vregLookupName);
                    if (argReg.find("bp") != std::string::npos) {
                        // Memory operand - load into AX first
                        Instruction286 loadInst;
                        loadInst.mnemonic = "mov";
                        loadInst.operands.push_back("ax");
                        loadInst.operands.push_back("[" + argReg + "]");
                        lowered.instructions.push_back(loadInst);

                        Instruction286 pushInst;
                        pushInst.mnemonic = "push";
                        pushInst.operands.push_back("ax");
                        lowered.instructions.push_back(pushInst);
                    } else {
                        // Register operand - push directly
                        Instruction286 pushInst;
                        pushInst.mnemonic = "push";
                        pushInst.operands.push_back(argReg);
                        lowered.instructions.push_back(pushInst);
                    }
                }
            }
        }
    }

    // For llvm.va_start: generate inline code instead of calling external function
    if (isLLVAVaStart) {
        // We pushed 2 args (ap and last_arg), each 32-bit (4 bytes each = 8 bytes total)
        // Stack layout after pushes: [ap_offset][ap_selector][last_arg_offset][last_arg_selector]
        // After call far pushes return CS:IP: [return_IP][return_CS][ap_offset][ap_selector][last_arg_offset][last_arg_selector]
        // We can access these from [bp+2], [bp+4], [bp+6], [bp+8], [bp+10], [bp+12] respectively
        // Actually, since we haven't called anything, we can pop them directly
        
        // Pop last_arg (the arg we just pushed last, so it's on top of stack)
        // last_arg was pushed as: push SS (selector), push ax (offset where ax=bp+6)
        // So stack top is: [last_arg_offset][last_arg_selector=SS]
        
        // Pop last_arg_selector into DX
        Instruction286 popLastArgSel;
        popLastArgSel.mnemonic = "pop";
        popLastArgSel.operands.push_back("dx");
        lowered.instructions.push_back(popLastArgSel);
        
        // Pop last_arg_offset into AX
        Instruction286 popLastArgOff;
        popLastArgOff.mnemonic = "pop";
        popLastArgOff.operands.push_back("ax");
        lowered.instructions.push_back(popLastArgOff);
        
        // Pop ap_selector into CX (we'll use this for storing to *ap)
        Instruction286 popApSel;
        popApSel.mnemonic = "pop";
        popApSel.operands.push_back("cx");
        lowered.instructions.push_back(popApSel);
        
        // Pop ap_offset into BX
        Instruction286 popApOff;
        popApOff.mnemonic = "pop";
        popApOff.operands.push_back("bx");
        lowered.instructions.push_back(popApOff);
        
        // Compute first_arg = last_arg + 4
        Instruction286 addFirstArg;
        addFirstArg.mnemonic = "add";
        addFirstArg.operands.push_back("ax");
        addFirstArg.operands.push_back("4");
        lowered.instructions.push_back(addFirstArg);
        // Note: carry flag is set if overflow, but we ignore it for now
        // In practice, last_arg is a stack offset, so adding 4 won't overflow
        
        // Store first_arg to *ap
        // *ap = first_arg offset (low word)
        Instruction286 storeApLow;
        storeApLow.mnemonic = "mov";
        storeApLow.operands.push_back("[ss:bx]");
        storeApLow.operands.push_back("ax");
        lowered.instructions.push_back(storeApLow);
        
        // *(ap+2) = SS (selector for stack)
        Instruction286 storeApHigh;
        storeApHigh.mnemonic = "mov";
        storeApHigh.operands.push_back("[ss:bx+2]");
        storeApHigh.operands.push_back("ss");
        lowered.instructions.push_back(storeApHigh);
        
        // Set flag to skip the normal call and stack cleanup (we already popped the args)
        skipCallAndCleanup = true;
    }
    
    // llvm.va_end is a no-op - skip entirely
    if (isLLVAVaEnd) {
        skipCallAndCleanup = true;
    }
    
    // Call the function (skip for inline intrinsics like llvm.va.start)
    if (!skipCallAndCleanup) {
        // Check if callee is an indirect call through a vreg (function pointer)
        if (!callee.empty() && callee[0] == '%') {
            // Indirect call: load function pointer and call through register
            // The callee vreg should map to a stack slot or register
            std::string calleeReg = state.frame.getPhysReg(callee);
            if (calleeReg.find("bp") != std::string::npos) {
                // Value is at a stack location - load into BX and call
                Instruction286 loadPtr;
                loadPtr.mnemonic = "lea";
                loadPtr.operands.push_back("bx");
                loadPtr.operands.push_back("[" + calleeReg + "]");
                lowered.instructions.push_back(loadPtr);

                // Load far pointer from [ss:bx] into dx:ax
                // bx points to a stack location (bp-relative), so use SS prefix
                Instruction286 loadOff;
                loadOff.mnemonic = "mov";
                loadOff.operands.push_back("ax");
                loadOff.operands.push_back("[ss:bx]");
                lowered.instructions.push_back(loadOff);

                Instruction286 loadSeg;
                loadSeg.mnemonic = "mov";
                loadSeg.operands.push_back("dx");
                loadSeg.operands.push_back("[ss:bx+2]");
                lowered.instructions.push_back(loadSeg);

                // Push return CS
                Instruction286 pushCs;
                pushCs.mnemonic = "push";
                pushCs.operands.push_back("cs");
                lowered.instructions.push_back(pushCs);

                // call near to trampoline - pushes return IP (continuation addr)
                std::string trampLabel = state.currentFunc->name + "_indirect_far_" + std::to_string(state.labelCounter++);
                Instruction286 callTramp;
                callTramp.mnemonic = "call";
                callTramp.operands.push_back(trampLabel);
                lowered.instructions.push_back(callTramp);

                // Trampoline (emitted at end of function): push dx, push ax, retf
                // Stack at trampoline: [return_IP][return_CS]
                // After push dx, push ax: [target_off][target_seg][return_IP][return_CS]
                // retf: pops IP=target_off, CS=target_seg → jumps to target
                // Target retf: pops IP=return_IP, CS=return_CS → returns to continuation
                LoweredInstruction trampBlock;
                Instruction286 pushSeg;
                pushSeg.mnemonic = "push";
                pushSeg.operands.push_back("dx");
                trampBlock.instructions.push_back(pushSeg);
                Instruction286 pushOff;
                pushOff.mnemonic = "push";
                pushOff.operands.push_back("ax");
                trampBlock.instructions.push_back(pushOff);
                Instruction286 retfInst;
                retfInst.mnemonic = "retf";
                trampBlock.instructions.push_back(retfInst);
                trampBlock.label = trampLabel;
                state.pendingTrampolines.push_back(trampBlock);
            } else {
                // Value is in a register - call through that register
                // This shouldn't happen for far calls, but handle it
                Instruction286 callIndirect;
                callIndirect.mnemonic = "call";
                callIndirect.operands.push_back("far " + calleeReg);
                lowered.instructions.push_back(callIndirect);
            }
       } else {
            Instruction286 callInst;
            callInst.mnemonic = "call";
            callInst.operands.push_back("far " + safeNasmName(callee));
            lowered.instructions.push_back(callInst);
        }
    }

    // Clean up stack (caller pays)
    if (!skipCallAndCleanup && !args.empty()) {
        // Calculate stack bytes - need to account for 32-bit args
        int stackBytes = 0;
        for (size_t argIdx = 0; argIdx < args.size(); argIdx++) {
            bool isArg32 = false;
            // For LLVM intrinsics renamed to C functions, all args are 32-bit
            if (isLLVMMemset || isLLVMMemcpy) {
                isArg32 = true;
            } else if (argIdx < irInst.callArgKinds.size() && irInst.callArgKinds[argIdx]) {
                // Check callArgKinds (true = pointer arg, which is 32-bit)
                isArg32 = true;
            } else if (isLLVAVaStart && argIdx == args.size() - 1) {
                // Synthetic last_arg argument for llvm_va_start is a far pointer (32-bit)
                isArg32 = true;
            } else {
                // Use funcParamBitWidths
                auto declIt = state.funcParamBitWidths.find(callee);
                if (declIt != state.funcParamBitWidths.end()) {
                    if (argIdx < declIt->second.size()) {
                        isArg32 = (declIt->second[argIdx] == 32);
                    } else {
                        // Beyond fixed param count (variadic) - assume 32-bit
                        isArg32 = irInst.resultType ? (irInst.resultType->bitWidth == 32) : true;
                    }
                } else if (irInst.resultType) {
                    isArg32 = irInst.resultType->bitWidth == 32;
                }
            }
            stackBytes += isArg32 ? 4 : 2;
        }
        Instruction286 addSp;
        addSp.mnemonic = "add";
        addSp.operands.push_back("sp");
        addSp.operands.push_back(std::to_string(stackBytes));
        lowered.instructions.push_back(addSp);
    }

    // Result is in AX (and DX for 32-bit)
    if (!irInst.resultName.empty()) {
        // Check if this is a 32-bit result
        bool is32 = false;
        if (irInst.resultType) {
            is32 = irInst.resultType->bitWidth == 32;
        }
        
        if (is32) {
            // 32-bit result: store ax (low) and dx (high) to stack
            std::string resultStack = state.frame.getPhysReg(irInst.resultName);
            if (resultStack == "ax" || resultStack == "bx" || resultStack == "cx" || resultStack == "dx") {
                resultStack = state.frame.allocResultSlot(irInst.resultName, 4, true);
            }
            Instruction286 storeLow;
            storeLow.mnemonic = "mov";
            storeLow.operands.push_back("[" + resultStack + "]");
            storeLow.operands.push_back("ax");
            lowered.instructions.push_back(storeLow);
            
            std::string highOffset = state.frame.getHighBpOffset(resultStack);
            Instruction286 storeHigh;
            storeHigh.mnemonic = "mov";
            storeHigh.operands.push_back("[" + highOffset + "]");
            storeHigh.operands.push_back("dx");
            lowered.instructions.push_back(storeHigh);
            
            state.frame.setPhysReg(irInst.resultName, resultStack);
        } else {
            // 16-bit result: store ax to stack
            std::string resultStack = state.frame.getPhysReg(irInst.resultName);
            if (resultStack == "ax" || resultStack == "bx" || resultStack == "cx" || resultStack == "dx") {
                resultStack = state.frame.allocResultSlot(irInst.resultName, 2, false);
            }
            Instruction286 storeResult;
            storeResult.mnemonic = "mov";
            storeResult.operands.push_back("[" + resultStack + "]");
            storeResult.operands.push_back("ax");
            lowered.instructions.push_back(storeResult);
            
            state.frame.setPhysReg(irInst.resultName, resultStack);
        }
    }

    loweredVec.push_back(lowered);
    return loweredVec;
}

} // namespace codegen
} // namespace llvm_i286
