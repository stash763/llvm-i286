// NASM Safe Name Mangling Implementation

#include "codegen/NasmSafe.h"

namespace llvm_i286 {
namespace codegen {

bool isNasmReservedWord(const std::string& name) {
    static const char* reserved[] = {
        "abs", "rel", "seg", "times", "db", "dw", "dd", "dt", "do",
        "dy", "dz", "resb", "resw", "resd", "resq", "rest", "reso", "resy",
        "resz", "equ", "section", "segment", "global", "extern", "common",
        "bits", "use16", "cpu", "org", "group", "struc", "endstruc",
        "istruc", "at", "iend", "align", "alignb", "rep", "nop", "ret",
        "call", "jmp", "je", "jne", "jz", "jnz", "jg", "jge", "jl", "jle",
        "ja", "jae", "jb", "jbe", "jo", "jno", "js", "jns", "jp", "jnp",
        "jpe", "jpo", "jc", "jnc", "loop", "loope", "loopne", "loopz",
        "loopnz", "enter", "leave", "push", "pop", "int", "into", "iret",
        "hlt", "wait", "cmc", "clc", "stc", "cli", "sti",
        "cld", "std", "cbw", "cwd", "lahf", "sahf",
        "pushf", "popf", "aaa", "aas",
        "aam", "aad", "daa", "das", "xlat", "xlatb", "movsb", "movsw",
        "cmpsb", "cmpsw", "stosb",
        "stosw", "lodsb", "lodsw",
        "scasb", "scasw", "insb", "insw",
        "outsb", "outsw", "lock",
        // 80286 segment registers only (no fs/gs - those are 80386+)
        "cs", "ds", "es", "ss",
        // 80286 16-bit general-purpose registers
        "ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
        // 80286 8-bit register halves
        "al", "bl", "cl", "dl", "ah", "bh", "ch", "dh",
        "null", "ptr", "byte", "word", "dword",
        "near", "far", "short",
        nullptr
    };
    for (int i = 0; reserved[i]; i++) {
        if (name == reserved[i]) return true;
    }
    return false;
}

std::string safeNasmName(const std::string& name) {
    if (isNasmReservedWord(name)) {
        return "_" + name;
    }
    return name;
}

} // namespace codegen
} // namespace llvm_i286
