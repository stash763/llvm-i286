// LLVM IR AST Definitions
// Internal representation of LLVM IR for code generation

#ifndef LLVM_I286_IR_AST_H
#define LLVM_I286_IR_AST_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace llvm_i286 {
namespace ir {

// Forward declarations
struct Type;
struct Value;
struct Instruction;
struct BasicBlock;
struct Function;
struct Module;

// Type system
enum class TypeKind {
    Void,
    Integer,
    Float,
    Pointer,
    Vector,
    Array,
    Struct,
    Named,
    Label,
    Metadata,
    Token,
    FunctionType
};

struct Type {
    TypeKind kind;
    int bitWidth = 0; // For integer types
    std::string name; // For named types
    std::unique_ptr<Type> elementType; // For pointer, array, vector
    int numElements = 0; // For array, vector
    std::vector<std::unique_ptr<Type>> structElements; // For struct
    
    static std::unique_ptr<Type> makeVoid() {
        auto t = std::make_unique<Type>();
        t->kind = TypeKind::Void;
        return t;
    }
    
    static std::unique_ptr<Type> makeInteger(int bits) {
        auto t = std::make_unique<Type>();
        t->kind = TypeKind::Integer;
        t->bitWidth = bits;
        return t;
    }
    
    static std::unique_ptr<Type> makePointer(std::unique_ptr<Type> elemType) {
        auto t = std::make_unique<Type>();
        t->kind = TypeKind::Pointer;
        t->elementType = std::move(elemType);
        return t;
    }
    
    bool isInteger() const { return kind == TypeKind::Integer; }
    bool isPointer() const { return kind == TypeKind::Pointer; }
    bool isVoid() const { return kind == TypeKind::Void; }
    int getBitWidth() const { return bitWidth; }
};

// Value representations
enum class ValueKind {
    ConstantInt,
    ConstantFloat,
    ConstantNull,
    ConstantTrue,
    ConstantFalse,
    ConstantZeroInitializer,
    ConstantUndef,
    GlobalReference,
    LocalReference,
    MetadataRef,
    InstructionResult
};

struct Value {
    ValueKind kind;
    std::string name;
    std::unique_ptr<Type> type;
    int64_t intValue = 0;
    double floatValue = 0.0;
    std::string text; // For storing raw text (e.g., string constants)
    
    static std::unique_ptr<Value> makeInt(int64_t val, int bits) {
        auto v = std::make_unique<Value>();
        v->kind = ValueKind::ConstantInt;
        v->intValue = val;
        v->type = Type::makeInteger(bits);
        return v;
    }
    
    static std::unique_ptr<Value> makeLocalRef(const std::string& name) {
        auto v = std::make_unique<Value>();
        v->kind = ValueKind::LocalReference;
        v->name = name;
        return v;
    }
    
    static std::unique_ptr<Value> makeGlobalRef(const std::string& name) {
        auto v = std::make_unique<Value>();
        v->kind = ValueKind::GlobalReference;
        v->name = name;
        return v;
    }
};

// Instruction opcodes
enum class Opcode {
    // Binary operations
    Add, FAdd, Sub, FSub, Mul, FMul,
    UDiv, SDiv, FDiv, URem, SRem, FRem,
    
    // Bitwise operations
    Shl, LShr, AShr, And, Or, Xor,
    
    // Memory operations
    Alloca, Load, Store, GetElementPtr,
    
    // Control flow
    Br, CondBr, Switch, Ret, Unreachable,
    
    // Calls
    Call, Invoke,
    
    // Conversions
    Trunc, ZExt, SExt,
    FPTrunc, FPExt, FPToUI, FPToSI, UIToFP, SIToFP,
    PtrToInt, IntToPtr, BitCast, AddrSpaceCast,
    
    // Comparison
    ICmp, FCmp,
    
    // Aggregate
    ExtractValue, InsertValue,
    ExtractElement, InsertElement, ShuffleVector,
    
    // Other
    Phi, Select, Freeze,
    VAArg, LandingPad,
    
    // Terminators
    RetTerm, BrTerm, CondBrTerm, SwitchTerm, UnreachableTerm
};

struct Operand {
    std::unique_ptr<Value> value;
    std::unique_ptr<Type> type;
    std::string name; // For basic block labels in br/phi
};

struct Instruction {
    Opcode opcode;
    std::string resultName; // %result for instructions that produce values
    std::unique_ptr<Type> resultType;
    std::vector<Operand> operands;
    std::vector<std::pair<std::string, std::string>> metadata; // Metadata attachments
    
    // For specific instructions
    bool isVolatile = false;
    std::string alignment;
    std::string callingConv;
    
    // For GEP
    bool inBounds = false;
    
    // For comparison
    std::string predicate; // eq, ne, sge, sgt, etc.
    
    // For phi
    struct PhiInc {
        std::string value;
        std::string label;
    };
    std::vector<PhiInc> phiIncrements;
    
    // For switch
    struct SwitchCase {
        std::string value;
        std::string label;
    };
    std::vector<SwitchCase> switchCases;
    
    // For call
    std::string calleeName;
    std::vector<std::string> callArgs;
};

struct BasicBlock {
    std::string label;
    std::vector<std::unique_ptr<Instruction>> instructions;
    std::unique_ptr<Instruction> terminator;
};

// Function parameter
struct Parameter {
    std::unique_ptr<Type> type;
    std::string name;
    std::vector<std::string> attributes;
};

struct Function {
    std::string name;
    std::unique_ptr<Type> returnType;
    std::vector<std::unique_ptr<Parameter>> params;
    std::vector<std::unique_ptr<BasicBlock>> basicBlocks;
    bool isDeclaration = false; // true if 'declare', false if 'define'
    std::vector<std::string> attributes;
    std::string linkage; // external, internal, etc.
    std::string visibility; // default, hidden, protected
    std::string callingConvention;
};

// Global variable
struct GlobalVariable {
    std::string name;
    std::unique_ptr<Type> type;
    std::string linkage;
    bool isConstant = false;
    std::unique_ptr<Value> initializer;
};

struct Module {
    std::string sourceFilename;
    std::string targetTriple;
    std::string dataLayout;
    std::vector<std::unique_ptr<Function>> functions;
    std::vector<std::unique_ptr<GlobalVariable>> globals;
    std::map<std::string, std::unique_ptr<Type>> typeDefinitions;
    std::map<std::string, std::string> aliases; // alias name -> target name
};

} // namespace ir
} // namespace llvm_i286

#endif // LLVM_I286_IR_AST_H
