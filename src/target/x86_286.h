// x86 286 Instruction Set Definitions
// Complete instruction set for 80286

#ifndef LLVM_I286_TARGET_X86_286_H
#define LLVM_I286_TARGET_X86_286_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace llvm_i286 {
namespace target {

// 286 Register definitions
enum class Reg8 {
    AL, CL, DL, BL, AH, CH, DH, BH
};

enum class Reg16 {
    AX, CX, DX, BX, SP, BP, SI, DI
};

enum class SegReg {
    ES, CS, SS, DS
};

// Addressing modes for 286 (no SIB bytes)
enum class AddrMode {
    Register,           // reg
    RegisterIndirect,   // [reg]
    Based,              // [bp+disp] or [bx+disp]
    Indexed,            // [si+disp] or [di+disp]
    BasedIndexed,       // [bx+si], [bx+di], [bp+si], [bp+di]
    BasedIndexedDisp,   // [bx+si+disp], etc.
    Immediate,          // imm8 or imm16
    Direct,             // [disp16]
};

// Valid base+index combinations for 286
struct AddrModeSpec {
    std::string base;    // "bx" or "bp" or empty
    std::string index;   // "si" or "di" or empty
    bool hasDisplacement;
    int displacementSize; // 0, 8, or 16 bits
};

// Instruction encoding information
struct InstructionEncoding {
    std::string mnemonic;
    std::vector<uint8_t> opcodeBytes;
    bool hasModRM;
    bool hasDisplacement;
    int displacementSize; // 0, 1, or 2 bytes
    bool hasImmediate;
    int immediateSize; // 0, 1, or 2 bytes
    AddrMode addrMode;
};

// 286 Instruction Set
class x86_286 {
public:
    // Get all valid registers
    static const std::vector<Reg16>& getRegisters16();
    static const std::vector<Reg8>& getRegisters8();
    static const std::vector<SegReg>& getSegmentRegisters();
    
    // Register encoding for ModR/M byte
    static uint8_t regToEncoding(Reg16 reg);
    static uint8_t regToEncoding(Reg8 reg);
    
    // Convert register to NASM string
    static std::string regToString(Reg16 reg);
    static std::string regToString(Reg8 reg);
    static std::string segToString(SegReg seg);
    
    // Get caller-saved registers (volatile across calls)
    static const std::vector<Reg16>& getCallerSavedRegisters();
    
    // Get callee-saved registers (preserved across calls)
    static const std::vector<Reg16>& getCalleeSavedRegisters();
    
    // Get special-purpose registers
    static Reg16 getStackPointer();
    static Reg16 getFramePointer();
    static Reg16 getAccumulator();
    static Reg16 getCountRegister();
    
    // Check if a register is valid for a given addressing mode
    static bool isValidBaseRegister(Reg16 reg);
    static bool isValidIndexRegister(Reg16 reg);
    
    // Generate ModR/M byte
    static uint8_t makeModRM(uint8_t mod, uint8_t reg, uint8_t rm);
    
    // Instruction availability (some instructions are 386+)
    static bool isInstructionAvailable(const std::string& mnemonic);
};

} // namespace target
} // namespace llvm_i286

#endif // LLVM_I286_TARGET_X86_286_H
