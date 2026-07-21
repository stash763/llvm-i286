// OperandRef Implementation

#include "ir/OperandRef.h"
#include "codegen/NasmSafe.h"

namespace llvm_i286 {
namespace ir {

std::string OperandRef::nasmName() const {
    if (kind == Kind::Global || kind == Kind::GEPExpr || kind == Kind::PtrToIntExpr) {
        // Convert @.str -> _str, @main -> main, @.str.3 -> _str.3
        std::string n = name;
        if (!n.empty() && n[0] == '.') {
            n = "_" + n.substr(1);
        }
        return n;
    }
    return name;
}

// Classify an operand from its raw IR text (with @ or % prefix)
// This is the single source of truth for operand classification,
// replacing the 180+ string prefix checks scattered across handlers.
OperandRef classifyOperand(const std::string& rawText) {
    OperandRef ref;
    ref.rawName = rawText;

    if (rawText.empty()) {
        ref.kind = OperandRef::Kind::Unknown;
        return ref;
    }

    // Check for vreg: %name
    if (rawText[0] == '%') {
        ref.kind = OperandRef::Kind::Vreg;
        ref.name = rawText.substr(1);
        return ref;
    }

    // Check for global: @name
    if (rawText[0] == '@') {
        ref.name = rawText.substr(1);
        // Check for GEP expression: getelementptr...
        if (rawText.substr(0, 13) == "@getelementptr") {
            ref.kind = OperandRef::Kind::GEPExpr;
        } else if (rawText.substr(0, 9) == "@ptrtoint") {
            ref.kind = OperandRef::Kind::PtrToIntExpr;
        } else {
            ref.kind = OperandRef::Kind::Global;
        }
        return ref;
    }

    // Check for null
    if (rawText == "null") {
        ref.kind = OperandRef::Kind::Null;
        ref.name = "null";
        return ref;
    }

    // Check for undef
    if (rawText == "undef") {
        ref.kind = OperandRef::Kind::Undef;
        ref.name = "undef";
        return ref;
    }

    // Check for true/false
    if (rawText == "true") {
        ref.kind = OperandRef::Kind::BoolConst;
        ref.name = "true";
        ref.boolValue = true;
        ref.type.bitWidth = 1;
        return ref;
    }
    if (rawText == "false") {
        ref.kind = OperandRef::Kind::BoolConst;
        ref.name = "false";
        ref.boolValue = false;
        ref.type.bitWidth = 1;
        return ref;
    }

    // Check for GEP expression (without @ prefix)
    if (rawText.substr(0, 13) == "getelementptr") {
        ref.kind = OperandRef::Kind::GEPExpr;
        ref.name = rawText;
        return ref;
    }

    // Check for ptrtoint expression (without @ prefix)
    if (rawText.substr(0, 8) == "ptrtoint") {
        ref.kind = OperandRef::Kind::PtrToIntExpr;
        ref.name = rawText;
        return ref;
    }

    // Check for inline asm
    if (rawText.substr(0, 3) == "asm") {
        ref.kind = OperandRef::Kind::InlineAsm;
        ref.name = rawText;
        return ref;
    }

    // Try to parse as integer constant
    try {
        size_t parsed;
        int64_t val = std::stoll(rawText, &parsed);
        if (parsed == rawText.size()) {
            ref.kind = OperandRef::Kind::Constant;
            ref.name = rawText;
            ref.constValue = val;
            // Infer bit width from value
            if (val >= 0 && val <= 255) ref.type.bitWidth = 8;
            else if (val >= -32768 && val <= 65535) ref.type.bitWidth = 16;
            else ref.type.bitWidth = 32;
            return ref;
        }
    } catch (...) {
        // Not a constant integer
    }

    // Check for hex constant: 0x...
    if (rawText.size() > 2 && rawText[0] == '0' && (rawText[1] == 'x' || rawText[1] == 'X')) {
        try {
            int64_t val = std::stoll(rawText, nullptr, 16);
            ref.kind = OperandRef::Kind::Constant;
            ref.name = rawText;
            ref.constValue = val;
            ref.type.bitWidth = 32;
            return ref;
        } catch (...) {
        }
    }

    // Check for negative constant
    if (rawText[0] == '-' && rawText.size() > 1) {
        try {
            size_t parsed;
            int64_t val = std::stoll(rawText, &parsed);
            if (parsed == rawText.size()) {
                ref.kind = OperandRef::Kind::Constant;
                ref.name = rawText;
                ref.constValue = val;
                ref.type.bitWidth = 32;
                return ref;
            }
        } catch (...) {
        }
    }

    // Unknown - treat as label or other
    ref.kind = OperandRef::Kind::Unknown;
    ref.name = rawText;
    return ref;
}

} // namespace ir
} // namespace llvm_i286