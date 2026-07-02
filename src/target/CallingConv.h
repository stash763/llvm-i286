// Calling Convention
// OS/2 1.x 16-bit protected mode calling convention

#ifndef LLVM_I286_TARGET_CALLINGCONV_H
#define LLVM_I286_TARGET_CALLINGCONV_H

#include <string>
#include <vector>

#include "target/x86_286.h"

namespace llvm_i286 {
namespace target {

struct CallingConvInfo {
    // Register usage
    std::vector<Reg16> argumentRegisters; // Registers used for passing arguments (none for stack-based)
    Reg16 returnReg;                       // Register for return value
    Reg16 returnRegHi;                     // High word for 32-bit return values (DX for DX:AX)
    
    // Stack discipline
    bool callerCleanup;                    // true = caller pops args, false = callee pops
    int stackAlignment;                    // Stack alignment in bytes
    
    // Prologue/epilogue
    std::vector<Reg16> calleeSavedRegs;    // Registers callee must save
    bool usesFramePointer;                 // Whether to use BP as frame pointer
};

class CallingConv {
public:
    // Get OS/2 1.x calling convention
    static const CallingConvInfo& getOS2_16bit();
    
    // Generate function prologue
    static std::vector<std::string> generatePrologue(const CallingConvInfo& info,
                                                    int numLocals);
    
    // Generate function epilogue
    static std::vector<std::string> generateEpilogue(const CallingConvInfo& info,
                                                    int numLocals);
    
    // Calculate stack offset for argument N (0-based)
    static int getArgumentStackOffset(int argIndex);
    
    // Generate call sequence
    static std::vector<std::string> generateCall(const CallingConvInfo& info,
                                                const std::string& funcName,
                                                const std::vector<std::string>& args);
    
    // Generate return sequence
    static std::vector<std::string> generateReturn(const CallingConvInfo& info,
                                                  const std::string& retVal);
};

} // namespace target
} // namespace llvm_i286

#endif // LLVM_I286_TARGET_CALLINGCONV_H
