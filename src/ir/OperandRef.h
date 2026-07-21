// OperandRef - Typed Operand Representation
// Replaces string-based operand classification with a typed representation
// that carries kind, value, and type information.

#ifndef LLVM_I286_IR_OPERAND_REF_H
#define LLVM_I286_IR_OPERAND_REF_H

#include <string>
#include <cstdint>

namespace llvm_i286 {
namespace ir {

// Lightweight type descriptor for operands
struct TypeRef {
    int bitWidth = 0;       // 0 = unknown, 1, 8, 16, 32, 64
    bool isPointer = false;
    bool isFloat = false;
    bool isStruct = false;
    bool isArray = false;
    std::string typeName;   // For named types (struct names)

    bool is32bit() const { return bitWidth == 32; }
    bool is16bit() const { return bitWidth == 16; }
    bool is8bit() const { return bitWidth == 8; }
    bool is64bit() const { return bitWidth == 64; }
    bool is1bit() const { return bitWidth == 1; }
};

// Typed operand representation - replaces string-based classification
class OperandRef {
public:
    enum class Kind {
        Constant,      // Integer/FP constant
        Vreg,          // %name - SSA virtual register
        Global,        // @name - global variable/function
        GEPExpr,       // getelementptr(...) constant expression
        PtrToIntExpr,  // ptrtoint(...) constant expression
        Null,          // null pointer constant
        Undef,         // undefined value
        BoolConst,     // true/false
        Label,         // basic block label (for br/phi/switch)
        InlineAsm,     // inline asm expression
        Unknown        // fallback for unclassified operands
    };

    Kind kind = Kind::Unknown;
    std::string name;       // Raw name (vreg name without %, global name without @, or expression text)
    std::string rawName;    // Original raw text from IR (with @ or % prefix)
    int64_t constValue = 0; // For Constant (integer value)
    bool boolValue = false; // For BoolConst (true=true, false=false)
    TypeRef type;

    // For GEP/PtrToInt expressions (computed once during parsing):
    std::string baseGlobal; // The @name inside the expression
    int64_t gepOffset = 0;  // Computed byte offset

    // Classification helpers (replaces string prefix checks)
    bool isConstant() const { return kind == Kind::Constant; }
    bool isVreg() const { return kind == Kind::Vreg; }
    bool isGlobal() const { return kind == Kind::Global || kind == Kind::GEPExpr || kind == Kind::PtrToIntExpr; }
    bool isGEPExpr() const { return kind == Kind::GEPExpr; }
    bool isPtrToIntExpr() const { return kind == Kind::PtrToIntExpr; }
    bool isNull() const { return kind == Kind::Null; }
    bool isUndef() const { return kind == Kind::Undef; }
    bool isBool() const { return kind == Kind::BoolConst; }
    bool isLabel() const { return kind == Kind::Label; }
    bool is32bit() const { return type.is32bit(); }
    bool is16bit() const { return type.is16bit(); }
    bool is8bit() const { return type.is8bit(); }
    bool isPointer() const { return type.isPointer; }

    // Convert global name to NASM format (single source of truth)
    // @.str -> _str, @main -> main, @.str.3 -> _str.3
    std::string nasmName() const;
};

// Classify an operand from its raw IR text (with @ or % prefix)
// Single source of truth for operand classification.
OperandRef classifyOperand(const std::string& rawText);

} // namespace ir
} // namespace llvm_i286

#endif // LLVM_I286_IR_OPERAND_REF_H