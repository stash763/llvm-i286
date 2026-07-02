// x86 286 Instruction Set Implementation

#include "target/x86_286.h"

namespace llvm_i286 {
namespace target {

const std::vector<Reg16>& x86_286::getRegisters16() {
    static const std::vector<Reg16> regs = {
        Reg16::AX, Reg16::CX, Reg16::DX, Reg16::BX,
        Reg16::SP, Reg16::BP, Reg16::SI, Reg16::DI
    };
    return regs;
}

const std::vector<Reg8>& x86_286::getRegisters8() {
    static const std::vector<Reg8> regs = {
        Reg8::AL, Reg8::CL, Reg8::DL, Reg8::BL,
        Reg8::AH, Reg8::CH, Reg8::DH, Reg8::BH
    };
    return regs;
}

const std::vector<SegReg>& x86_286::getSegmentRegisters() {
    static const std::vector<SegReg> segs = {
        SegReg::ES, SegReg::CS, SegReg::SS, SegReg::DS
    };
    return segs;
}

uint8_t x86_286::regToEncoding(Reg16 reg) {
    switch (reg) {
        case Reg16::AX: return 0;
        case Reg16::CX: return 1;
        case Reg16::DX: return 2;
        case Reg16::BX: return 3;
        case Reg16::SP: return 4;
        case Reg16::BP: return 5;
        case Reg16::SI: return 6;
        case Reg16::DI: return 7;
    }
    return 0;
}

uint8_t x86_286::regToEncoding(Reg8 reg) {
    switch (reg) {
        case Reg8::AL: return 0;
        case Reg8::CL: return 1;
        case Reg8::DL: return 2;
        case Reg8::BL: return 3;
        case Reg8::AH: return 4;
        case Reg8::CH: return 5;
        case Reg8::DH: return 6;
        case Reg8::BH: return 7;
    }
    return 0;
}

std::string x86_286::regToString(Reg16 reg) {
    switch (reg) {
        case Reg16::AX: return "ax";
        case Reg16::CX: return "cx";
        case Reg16::DX: return "dx";
        case Reg16::BX: return "bx";
        case Reg16::SP: return "sp";
        case Reg16::BP: return "bp";
        case Reg16::SI: return "si";
        case Reg16::DI: return "di";
    }
    return "ax";
}

std::string x86_286::regToString(Reg8 reg) {
    switch (reg) {
        case Reg8::AL: return "al";
        case Reg8::CL: return "cl";
        case Reg8::DL: return "dl";
        case Reg8::BL: return "bl";
        case Reg8::AH: return "ah";
        case Reg8::CH: return "ch";
        case Reg8::DH: return "dh";
        case Reg8::BH: return "bh";
    }
    return "al";
}

std::string x86_286::segToString(SegReg seg) {
    switch (seg) {
        case SegReg::ES: return "es";
        case SegReg::CS: return "cs";
        case SegReg::SS: return "ss";
        case SegReg::DS: return "ds";
    }
    return "ds";
}

const std::vector<Reg16>& x86_286::getCallerSavedRegisters() {
    static const std::vector<Reg16> regs = {Reg16::AX, Reg16::CX, Reg16::DX};
    return regs;
}

const std::vector<Reg16>& x86_286::getCalleeSavedRegisters() {
    static const std::vector<Reg16> regs = {Reg16::BX, Reg16::SI, Reg16::DI, Reg16::BP};
    return regs;
}

Reg16 x86_286::getStackPointer() { return Reg16::SP; }
Reg16 x86_286::getFramePointer() { return Reg16::BP; }
Reg16 x86_286::getAccumulator() { return Reg16::AX; }
Reg16 x86_286::getCountRegister() { return Reg16::CX; }

bool x86_286::isValidBaseRegister(Reg16 reg) {
    return reg == Reg16::BX || reg == Reg16::BP;
}

bool x86_286::isValidIndexRegister(Reg16 reg) {
    return reg == Reg16::SI || reg == Reg16::DI;
}

uint8_t x86_286::makeModRM(uint8_t mod, uint8_t reg, uint8_t rm) {
    return ((mod & 0x03) << 6) | ((reg & 0x07) << 3) | (rm & 0x07);
}

bool x86_286::isInstructionAvailable(const std::string& mnemonic) {
    // Instructions NOT available on 286 (386+ only)
    static const std::vector<std::string> unavailable = {
        "movzx", "movsx", "bt", "bts", "btr", "btc",
        "sete", "setne", "setl", "setle", "setg", "setge",
        "cmovz", "cmovnz", "cmovl", "cmovle", "cmovg", "cmovge",
        "bsf", "bsr", "shld", "shrd",
        "enter", "leave" // leave is actually available on 286, but enter is slow
    };
    
    for (const auto& inst : unavailable) {
        if (mnemonic == inst) return false;
    }
    return true;
}

} // namespace target
} // namespace llvm_i286
