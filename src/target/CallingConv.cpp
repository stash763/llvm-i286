// Calling Convention Implementation

#include "target/CallingConv.h"

namespace llvm_i286 {
namespace target {

const CallingConvInfo& CallingConv::getOS2_16bit() {
    static const CallingConvInfo info = {
        .argumentRegisters = {}, // Stack-based, no register arguments
        .returnReg = Reg16::AX,
        .returnRegHi = Reg16::DX, // For 32-bit returns: DX:AX
        .callerCleanup = true,
        .stackAlignment = 2,
        .calleeSavedRegs = {Reg16::BX, Reg16::SI, Reg16::DI, Reg16::BP},
        .usesFramePointer = true
    };
    return info;
}

std::vector<std::string> CallingConv::generatePrologue(const CallingConvInfo& info,
                                                       int numLocals) {
    std::vector<std::string> prologue;
    
    // Standard prologue
    prologue.push_back("push bp");
    prologue.push_back("mov bp, sp");
    
    // Save callee-saved registers
    for (const auto& reg : info.calleeSavedRegs) {
        if (reg != Reg16::BP) { // BP already saved above
            prologue.push_back("push " + x86_286::regToString(reg));
        }
    }
    
    // Allocate space for locals
    if (numLocals > 0) {
        int localSize = numLocals * 2; // 2 bytes per local (16-bit)
        prologue.push_back("sub sp, " + std::to_string(localSize));
    }
    
    return prologue;
}

std::vector<std::string> CallingConv::generateEpilogue(const CallingConvInfo& info,
                                                       int numLocals) {
    std::vector<std::string> epilogue;
    
    // Deallocate locals
    if (numLocals > 0) {
        int localSize = numLocals * 2;
        epilogue.push_back("add sp, " + std::to_string(localSize));
    }
    
    // Restore callee-saved registers (reverse order)
    for (auto it = info.calleeSavedRegs.rbegin(); it != info.calleeSavedRegs.rend(); ++it) {
        if (*it != Reg16::BP) {
            epilogue.push_back("pop " + x86_286::regToString(*it));
        }
    }
    
    // Standard epilogue
    epilogue.push_back("pop bp");
    epilogue.push_back("ret");
    
    return epilogue;
}

int CallingConv::getArgumentStackOffset(int argIndex) {
    // Arguments are at [bp+4], [bp+6], [bp+8], etc.
    // [bp] = saved BP, [bp+2] = return address
    return 4 + (argIndex * 2);
}

std::vector<std::string> CallingConv::generateCall(const CallingConvInfo& info,
                                                   const std::string& funcName,
                                                   const std::vector<std::string>& args) {
    std::vector<std::string> callSeq;
    
    // Push arguments right-to-left
    for (auto it = args.rbegin(); it != args.rend(); ++it) {
        callSeq.push_back("push " + *it);
    }
    
    // Call function
    callSeq.push_back("call " + funcName);
    
    // Caller cleanup: pop arguments
    if (info.callerCleanup && !args.empty()) {
        int totalSize = args.size() * 2;
        callSeq.push_back("add sp, " + std::to_string(totalSize));
    }
    
    return callSeq;
}

std::vector<std::string> CallingConv::generateReturn(const CallingConvInfo& info,
                                                     const std::string& retVal) {
    std::vector<std::string> retSeq;
    
    // Move return value to AX (if not already there)
    if (!retVal.empty() && retVal != "ax") {
        retSeq.push_back("mov ax, " + retVal);
    }
    
    return retSeq;
}

} // namespace target
} // namespace llvm_i286
