# LLVM i286 Code Generator - Project Plan

## Project Goal

Build a code generator that takes LLVM IR and produces 80286 16-bit protected-mode
assembly (NASM syntax), which is then assembled with NASM and linked with OpenWatcom
`wlink` into OS/2 1.x NE executables, testable via `2ine_debugger`.

## Key Decisions

- **LLVM IR source**: clang generates IR; we focus on the backend code generator
- **ANTLR4 C++ runtime**: pre-built binaries (not built from source)
- **Output**: NASM text assembly -> nasm -> wlink -> NE format (OS/2 1.x uses NE, not LX)
- **Build system**: CMake, C++ implementation
- **Testing**: clang -> IR -> codegen -> NASM -> wlink -> lx_loader (2ine)
- **Target CPU**: Intel 80286, 16-bit protected mode
- **Initial OS target**: OS/2 1.x
- **Future OS target**: NetBSD (requires 32/64-bit emulation via 16-bit instructions)
- **Language support**: C17 initially, C++ later

## Existing Resources

| Resource | Location | Purpose |
|----------|----------|---------|
| ANTLR4 tool (Java) | `antlr4/tool/` | Generate C++ parser from .g4 grammars |
| ANTLR4 C++ runtime | `antlr4/runtime/Cpp/` | Pre-built runtime library |
| LLVM IR grammar | `grammars-v4/llvm-ir/LLVMIR.g4` | ANTLR4 grammar for LLVM IR |
| C grammar | `grammars-v4/c/` | ANTLR4 grammar for C17 |
| C++ grammar | `grammars-v4/cpp/` | ANTLR4 grammar for C++ |
| 2ine debugger | `2ine_debugger/` | OS/2 executable runner + debugger |
| NASM | System package (2.16.01) | Assembler for generated output |
| wlink | `~/ow/open-watcom-v2/rel/binl/wlink` | Linker for NE executables |
| clang | System package (18.1.3) | C -> LLVM IR generation |

---

## Phase 1: Project Skeleton & Build System

### 1.1 Directory Layout

```
llvm_i286/
├── CMakeLists.txt                 # Top-level build
├── src/
│   ├── ir/                        # LLVM IR parsing & AST
│   │   ├── IrParser.h/cpp         # Wraps ANTLR4 parser
│   │   ├── IrAst.h/cpp            # Internal IR AST nodes
│   │   └── IrVisitor.h/cpp        # ANTLR4 visitor -> our AST
│   ├── codegen/                   # Core code generation
│   │   ├── CodeGen.h/cpp          # Main pipeline orchestrator
│   │   ├── InstructionSelect.h/cpp # LLVM IR -> 286 instructions
│   │   ├── RegisterAlloc.h/cpp    # Register allocator
│   │   ├── Emitter.h/cpp          # NASM text emission
│   │   └── Addressing.h/cpp       # Segment:offset management
│   ├── target/                    # 286 target definitions
│   │   ├── x86_286.h/cpp         # Instruction set, opcodes, encodings
│   │   └── CallingConv.h/cpp     # OS/2 1.x calling convention
│   ├── runtime/                   # Emulation helpers (32/64-bit)
│   ├── formats/                   # NE format module-definition writer
│   └── frontend/                  # C17 frontend (later)
│       ├── CLexerAdaptor.h/cpp
│       └── CToIr.h/cpp
├── antlr/                         # ANTLR4 generated sources
│   ├── LLVMIRLexer.h/cpp
│   ├── LLVMIRParser.h/cpp
│   ├── LLVMIRBaseVisitor.h/cpp
│   └── ...
├── tests/
│   ├── unit/                      # Unit tests per component
│   ├── integration/               # End-to-end (IR -> NASM) tests
│   ├── os2/                       # OS/2 test programs (C source)
│   └── run_tests.sh               # Test runner script
├── 2ine_debugger/                 # (existing) test harness
├── antlr4/                        # (existing) ANTLR4 source
└── grammars-v4/                   # (existing) grammars
```

### 1.2 CMake Build Setup

- Locate pre-built ANTLR4 C++ runtime headers + libraries
- Set up custom commands to invoke ANTLR4 Java tool on `LLVMIR.g4`
- Build the codegen as a library + a `llvm-i286` executable
- Add test targets that run the full pipeline

---

## Phase 2: LLVM IR Parsing

### 2.1 ANTLR4 Setup

- Build the ANTLR4 Java tool from `antlr4/tool/` (needed once to generate C++ parser)
- Generate C++ lexer/parser/visitor from `grammars-v4/llvm-ir/LLVMIR.g4`
- Place generated files in `antlr/` directory
- Link against pre-built ANTLR4 C++ runtime

### 2.2 IR AST Construction

Implement an ANTLR4 `BaseVisitor` that walks the parse tree and builds an internal AST:

**Module**: source filename, target triple, data layout, globals, functions

**Function**: name, return type, params, basic blocks, attributes

**Basic Block**: label, instructions, terminator

**Instruction**: opcode, operands, result type, metadata

**Type system**: int, float, pointer, vector, array, struct, named types

**Instruction categories to handle**:

- Binary arithmetic: add, sub, mul, udiv, sdiv, urem, srem, fadd, fsub, fmul, fdiv, frem
- Bitwise: and, or, xor, shl, lshr, ashr
- Memory: alloca, load, store, getelementptr, cmpxchg, atomicrmw
- Control flow: br, condbr, switch, ret, unreachable, indirectbr
- Calls: call, invoke, tail call
- Conversions: trunc, zext, sext, fptrunc, fpext, fptoui, fptosi, uitofp, sitofp, ptrtoint, inttoptr, bitcast, addrspacecast
- Comparison: icmp, fcmp
- Aggregate: extractvalue, insertvalue, extractelement, insertelement, shufflevector
- Other: phi, select, freeze, va_arg, landingpad, fence

---

## Phase 3: 80286 Target Backend

### 3.1 286 Instruction Set Definition

Define the complete 80286 instruction set in C++ data structures. For each instruction:
mnemonic, opcode bytes, ModR/M encoding rules, operand types.

**Supported instructions**:
MOV, ADD, SUB, AND, OR, XOR, CMP, TEST, PUSH, POP, CALL, RET, JMP, Jcc,
LEA, INC, DEC, NEG, NOT, SHL, SHR, SAR, MUL, IMUL, DIV, IDIV, CBW, CWD,
NOP, INT, XCHG, XLAT, LAHF, SAHF, PUSHF, POPF, CLC, STC, CMC, CLD, STD

**286 limitations to remember**:
- No SIB bytes (only ModR/M for memory addressing)
- No 32-bit operands
- No MOVZX/MOVSX (those are 386+)
- No BT/BTS/BTR/BTC (those are 386+)
- No SETcc (those are 386+)
- No CMOVcc (those are Pentium Pro+)
- Limited addressing modes: [bx+si], [bx+di], [bp+si], [bp+di], [si], [di], [bp], [bx], plus 8/16/32-bit displacements

### 3.2 NASM Text Emitter

Emit assembly in NASM syntax, matching the style in `2ine_debugger/tests/asm/test.asm`:

- `BITS 16` directive
- Segment declarations (`segment _TEXT CLASS=CODE`, `segment _DATA CLASS=DATA`)
- Symbol definitions and references
- Instruction mnemonics with proper NASM operand syntax
- Module definition file (`.def`) generation for `wlink`
- Handle NASM-specific syntax: `far`/`near` qualifiers, `word`/`byte` size specifiers, segment overrides

### 3.3 Register Allocator

The 80286 has only 8 general-purpose 16-bit registers:
AX, BX, CX, DX, SI, DI, BP, SP

**Linear scan register allocator**:

- **AX**: implicit for multiply/divide/IO - avoid for general allocation when possible
- **BX, SI, DI**: preferred for long-lived values (callee-saved in OS/2 convention)
- **CX**: loop counter - use for loop-related values
- **BP**: frame pointer - reserved
- **SP**: stack pointer - reserved

**Spilling**: generate PUSH/POP sequences or stack slot access via `[bp+offset]`

### 3.4 Instruction Selection

Lower each LLVM IR instruction to 286 instruction sequences:

| LLVM IR | 286 Sequence |
|---------|-------------|
| `add i16` | `add ax, bx` |
| `add i32` | `add ax, bx` + `adc dx, cx` (multi-word) |
| `mul i16` | `imul bx` (result in DX:AX) |
| `udiv i16` | `div bx` (AX/DX <- DX:AX / BX) |
| `load ptr` | `mov ax, [bx]` + segment override if needed |
| `store ptr` | `mov [bx], ax` + segment override |
| `getelementptr` | `lea` or manual offset calculation |
| `br label` | `jmp .label` |
| `br i1 cond, l1, l2` | Compare + `jcc .l1` / `jmp .l2` |
| `call` | `push` args + `call func` |
| `ret` | Move return value to AX + `ret` |
| `icmp eq` | `cmp` + `sete`-equivalent (branch sequence) |
| `phi` | Move to common register at each predecessor's branch |
| `select` | Compare + conditional move (no CMOV, use branch) |

### 3.5 OS/2 1.x Calling Convention

- Parameters passed on stack, right-to-left
- Return value in AX (16-bit) or DX:AX (32-bit)
- Caller cleans the stack
- Callee-saved: BX, SI, DI, BP
- Caller-saved: AX, CX, DX
- Function prologue: `push bp; mov bp, sp; push si; push di; push bx`
- Function epilogue: `pop bx; pop di; pop si; pop bp; ret`
- Stack alignment: word-aligned (2 bytes)

---

## Phase 4: Addressing & Segmentation

### 4.1 Segment:Offset Management

- Model segments as logical groups: `_TEXT` (code), `_DATA` (initialized data), `STACK` (stack), `_BSS` (uninitialized)
- Track which segment each global/variable lives in
- Emit segment override prefixes (`cs:`, `ds:`, `es:`, `ss:`) when accessing cross-segment data
- For the initial OS/2 target: use a flat data model where `_TEXT`, `_DATA`, and `STACK` are grouped into `DGROUP`

### 4.2 Stack Frame Layout

```
[bp+6]    <- argument N
[bp+4]    <- argument 2
[bp+2]    <- argument 1
[bp]      <- saved BP
[bp-2]    <- local variable 1
[bp-4]    <- local variable 2
...
```

- BP-based addressing for locals and parameters
- Automatic stack slot allocation in function prologue

---

## Phase 5: Output Pipeline & Testing

### 5.1 NE Format via NASM + wlink

- Our codegen produces `.asm` files (NASM syntax, `BITS 16`)
- Our codegen produces `.def` files (module definition for wlink):
  ```
  NAME <module> WINDOWCOMPAT
  PROTMODE
  STACKSIZE 4096
  ```
- NASM assembles to `.obj`: `nasm -f obj -o output.o output.asm`
- wlink produces NE executable: `wlink system os2 d all name output.exe file output.o`

### 5.2 Test Pipeline

```
test.c -> clang -S -emit-llvm -> test.ll -> llvm-i286 -> test.asm + test.def
-> nasm -f obj -> test.o -> wlink -> test.exe -> lx_loader -> verify output
```

**Test runner script**:
1. Compiles C test files with clang to LLVM IR
2. Runs our code generator on the IR
3. Assembles with NASM
4. Links with wlink
5. Runs the resulting NE executable through 2ine's `lx_loader`
6. Compares output against expected results

### 5.3 Initial Test Cases

- `hello.c`: Simple output (DosPutMessage on OS/2)
- `arithmetic.c`: Integer arithmetic operations
- `control_flow.c`: If/else, loops, switch
- `functions.c`: Function calls, recursion
- `pointers.c`: Pointer arithmetic, arrays
- `structs.c`: Struct access, member offsets
- `32bit_ops.c`: 32-bit integer operations (tests emulation layer)

---

## Phase 6: 32/64-bit Emulation (NetBSD Preparation)

### 6.1 Multi-word Arithmetic Library

All 32/64-bit operations must be emulated using 16-bit instructions:

- **32-bit add**: Two 16-bit ADD with carry (`add` + `adc`)
- **32-bit subtract**: Two 16-bit SUB with borrow (`sub` + `sbb`)
- **32-bit multiply**: Series of 16-bit MUL + shift + add
- **32-bit divide**: Restoring division algorithm using 16-bit DIV
- **64-bit operations**: Extended versions of the above
- **32-bit shifts**: Multi-step SHL/SHR with carry rotation
- **32-bit comparisons**: CMP + conditional logic on both words

### 6.2 Runtime Support

- Emit calls to runtime helper functions for complex operations
- Runtime library compiled as 286 assembly, linked into every output
- For the NetBSD target: this becomes part of the kernel's runtime support

### 6.3 Linear Address Translation

For code expecting 32-bit flat addresses:
- Compiler pass that converts 32-bit pointers to segment:offset pairs
- Runtime segment table managed by the OS or compiler-inserted code
- Automatic segment register loading before memory accesses
- Stack-based addressing translated to SS:SP-relative

---

## Phase 7: C17 Frontend

### 7.1 C Parser

- Use `grammars-v4/c/CLexer.g4` + `CParser.g4`
- Generate C++ parser via ANTLR4
- Build AST for C17 constructs:
  - Declarations (variables, functions, typedefs)
  - Types (primitives, pointers, arrays, structs, unions, enums, bitfields)
  - Expressions (arithmetic, logical, bitwise, assignment, conditional)
  - Statements (compound, if, while, do-while, for, switch, return, goto)
  - Preprocessor (limited - may require separate handling)

### 7.2 C -> LLVM IR Generation

- Walk C AST and emit LLVM IR
- Type system mapping: C types -> LLVM types
- Control flow: structured control -> basic blocks with branches
- Memory model: stack allocation via `alloca`, loads/stores
- Function call generation matching the target calling convention

---

## Phase 8: Debug Support

### 8.1 Source-Level Debugging

- Generate NASM labels that map back to source locations
- Produce a symbol table mapping 286 addresses -> source file:line
- Integrate with `td2ine` debugger for stepping through source
- Optionally generate DWARF-compatible debug info in the object file

---

## Implementation Order

| Step | Task | Depends On |
|------|------|-----------|
| 1 | Project CMake skeleton | - |
| 2 | ANTLR4 C++ runtime integration | 1 |
| 3 | LLVM IR parser generation | 2 |
| 4 | IR AST + visitor implementation | 3 |
| 5 | 286 instruction set definitions | 1 |
| 6 | NASM text emitter | 5 |
| 7 | Basic instruction selection (simple ops) | 4, 6 |
| 8 | Register allocator | 7 |
| 9 | Calling convention + prologue/epilogue | 7, 8 |
| 10 | NE format output + wlink integration | 6 |
| 11 | Full test pipeline (clang->IR->NASM->wlink->2ine) | 10 |
| 12 | Control flow instruction selection | 9 |
| 13 | Memory operations + GEP | 9 |
| 14 | Segment:offset addressing | 13 |
| 15 | 32/64-bit emulation runtime | 7 |
| 16 | C17 frontend | 4 |
| 17 | Debug integration | 11 |
| 18 | C++ frontend | 16 |
