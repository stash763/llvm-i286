// NASM Safe Name Mangling Implementation

#include "codegen/NasmSafe.h"

namespace llvm_i286 {
namespace codegen {

bool isNasmReservedWord(const std::string& name) {
    static const char* reserved[] = {
        "abs", "rel", "seg", "times", "db", "dw", "dd", "dq", "dt", "do",
        "dy", "dz", "resb", "resw", "resd", "resq", "rest", "reso", "resy",
        "resz", "equ", "section", "segment", "global", "extern", "common",
        "bits", "use16", "use32", "cpu", "org", "group", "struc", "endstruc",
        "istruc", "at", "iend", "align", "alignb", "rep", "nop", "ret",
        "call", "jmp", "je", "jne", "jz", "jnz", "jg", "jge", "jl", "jle",
        "ja", "jae", "jb", "jbe", "jo", "jno", "js", "jns", "jp", "jnp",
        "jpe", "jpo", "jc", "jnc", "loop", "loope", "loopne", "loopz",
        "loopnz", "enter", "leave", "push", "pop", "int", "into", "iret",
        "iretd", "iretw", "hlt", "wait", "cmc", "clc", "stc", "cli", "sti",
        "cld", "std", "cbw", "cwd", "cdq", "cwde", "cdqe", "lahf", "sahf",
        "pushf", "popf", "pushfd", "popfd", "pushfq", "popfq", "aaa", "aas",
        "aam", "aad", "daa", "das", "xlat", "xlatb", "movsb", "movsw",
        "movsd", "movsq", "cmpsb", "cmpsw", "cmpsd", "cmpsq", "stosb",
        "stosw", "stosd", "stosq", "lodsb", "lodsw", "lodsd", "lodsq",
        "scasb", "scasw", "scasd", "scasq", "insb", "insw", "insd",
        "outsb", "outsw", "outsd", "lock", nullptr
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
