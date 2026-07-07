# Musl libc Port — Progress & Next Steps

## Context

After successfully porting `memcpy`, `memmove`, `memset` from musl as C replacements for i386 assembly, the next phase targets:

1. **Syscall layer** — unblocks all I/O, exit, time, and OS integration
2. **String/ctype functions** — low-risk codegen testing and coverage
3. **Codegen alias support** — required for musl's `weak_alias` pattern

**Ultimate goal:** Full end-to-end pipeline: C → clang → LLVM IR → llvm-i286 → NASM → nasm → wlink → OS/2 .exe → 2ine

---

## Completed Work

### Phase 0: Build 2ine ✅
- Built via podman in `2ine_debugger/build/`
- Artifacts: `lx_loader`, `td2ine`, `lib2ine.so`, `libdoscalls.so`, and other OS/2 API shared libraries
- Fixed `endLxProcess` undefined reference by adding `__attribute__((used))`
- Installed `libsdl2-2.0-0:i386` dependency

### Phase 1: Build Runtime Library ✅
- `runtime/runtime.lib` built via `build_runtime.sh`
- Includes: `mul32.o`, `div32.o`, `itoa.o`, `printnum.o`, `os2_syscall.o`

### Phase 2: Codegen Alias Support ✅
**Files modified:**
- `src/ir/IrAst.h` — added `std::map<std::string, std::string> aliases` to Module struct
- `src/ir/IrVisitor.h` — added `visitIndirectSymbolDef` declaration
- `src/ir/IrVisitor.cpp` — added `visitIndirectSymbolDef` handler that parses alias→target mappings
- `src/codegen/InstructionSelect.h` — added `setAliasMap` method declaration
- `src/codegen/InstructionSelect.cpp` — added `setAliasMap` implementation
- `src/codegen/CallOps.cpp` — added alias resolution in `lowerCall` (checks `state.aliasMap`)
- `src/codegen/CodeGen.cpp` — added `selector.setAliasMap(module.aliases)` call

**Result:** musl's `weak_alias(old, new)` now resolves correctly at code generation time.

### Phase 3: Syscall Layer Implementation ✅
**File created:** `runtime/os2_syscall.asm`

**Implemented syscalls:**
| Syscall | Number | OS/2 API | Status |
|---------|--------|----------|--------|
| `exit` / `exit_group` | 6 | `DosExit(action, result)` | ✅ |
| `write` | 2 | `DosWrite(hf, buf, count, &bytesWritten)` | ✅ |
| `read` | 1 | `DosRead(hf, buf, count, &bytesRead)` | ✅ |
| `close` | 4 | `DosClose(hf)` | ✅ |
| `getpid` | 7 | stub (returns 0) | ✅ |
| All others | various | return `-ENOSYS` (-38) | ✅ |

**Calling convention:** 32-bit parameters on stack (right-to-left), return in DX:AX.

---

## Current Test Status

**Existing test suite:** 13/13 pass (2 skips for known `printf` limitations)

```
Test 1: hello... PASS (output: 0)
Test 2: test_add... PASS (output: 30)
Test 3: test_add_local... PASS (output: 42)
Test 4: test_hello... PASS (output: 42)
Test 5: test_mul32... PASS (output: 20000)
Test 6: test_mul32_simple... PASS (output: 32)
Test 7: test_mul... PASS (output: 20000)
Test 8: test_mul_local... PASS (output: 20000)
Test 9: test_mul_print... PASS (output: 32)
Test 10: test_printf... SKIP (known limitation)
Test 11: test_printf_simple... SKIP (known limitation)
Test 12: test_printnum... PASS (output: 42)
Test 13: test_return... PASS (output: 42)
```

---

## Remaining Work

### Phase 4: Port String/Ctype Functions

**Approach:** Compile each function individually through the pipeline (standalone tests, no full libc linkage).

**Include paths** (baked into `clang_i286` wrapper):
```
-I musl/include -I musl/arch/i286 -I musl/arch/generic
```

### 4a. Trivially simple functions (pure C, ≤25 lines, no deps)

| Function | File | Lines | What it tests |
|---|---|---|---|
| `strlen` | `string/strlen.c` | 22 | pointer arithmetic, word-at-a-time scan |
| `strcmp` | `string/strcmp.c` | 7 | char comparison |
| `strncmp` | `string/strncmp.c` | 9 | bounded comparison |
| `memcmp` | `string/memcmp.c` | 8 | byte comparison |
| `isalpha` | `ctype/isalpha.c` | 14 | arithmetic + comparison |
| `isdigit` | `ctype/isdigit.c` | 14 | arithmetic + comparison |
| `isalnum` | `ctype/isalnum.c` | ~14 | arithmetic + comparison |
| `isspace` | `ctype/isspace.c` | ~14 | comparison |
| `toupper` | `ctype/toupper.c` | ~14 | conditional + arithmetic |
| `tolower` | `ctype/tolower.c` | ~14 | conditional + arithmetic |
| `isascii` | `ctype/isascii.c` | ~7 | trivial |
| `toascii` | `ctype/toascii.c` | ~7 | trivial |

### 4b. Moderate complexity (depends on other string functions)

| Function | File | Lines | Depends on | weak_alias? |
|---|---|---|---|---|
| `stpcpy` | `string/stpcpy.c` | 29 | none (uses word ops) | Yes |
| `strcpy` | `string/strcpy.c` | 7 | stpcpy | No |
| `strncpy` | `string/strncpy.c` | ~30 | none | No |
| `strchr` | `string/strchr.c` | ~15 | none | No |
| `strrchr` | `string/strrchr.c` | ~15 | none | No |

---

### Phase 5: Integration Tests

#### 5a. String/Ctype tests (Phase 4 dependencies)

Create standalone test programs in `tests/os2/`:

| Test | Source | Tests | Expected |
|---|---|---|---|
| `test_strlen.c` | standalone, call strlen | strlen | exit 0 if strlen("hello")==5 |
| `test_strcmp.c` | standalone, call strcmp | strcmp | exit 0 if strcmp("abc","abc")==0 |
| `test_memcmp.c` | standalone, call memcmp | memcmp | exit 0 |
| `test_isalpha.c` | standalone, call isalpha | isalpha | exit 0 |
| `test_isdigit.c` | standalone, call isdigit | isdigit | exit 0 |

#### 5b. Syscall tests (Phase 3 dependencies)

| Test | Source | Tests | Expected |
|---|---|---|---|
| `test_exit.c` | musl exit.h → exit(0) | _Exit → DosExit | exit 0 |
| `test_write.c` | musl unistd.h → write(1, "hello\n", 6) | DosWrite | prints "hello\n" |
| `test_hello_musl.c` | write(1, msg, strlen(msg)) | string + write | prints message |

#### 5c. Update test runner

- Add new tests to `EXPECTED_OUTPUT` in `tests/run_tests.sh`
- Update `EXPECTED_FAILURES` as needed

---

### Phase 6: Next Musl Components After String Functions

After string/ctype functions are working, the next musl components to port are those that unblock full program execution through libc:

#### 6a. Exit Path (unblocks clean shutdown)
| File | Purpose | Depends on |
|---|---|---|
| `src/exit/_Exit.c` | Calls `__syscall(SYS_exit_group, ec)` | syscall layer ✅ |
| `src/exit/exit.c` | Calls `__funcs_on_exit`, `__libc_exit_fini`, `__stdio_exit`, then `_Exit` | weak_alias, internal libc |
| `src/exit/atexit.c` | Register atexit handlers | weak_alias, internal libc |

#### 6b. Startup Code (unblocks `main()` invocation)
| File | Purpose | Depends on |
|---|---|---|
| `src/env/__libc_start_main.c` | Calls `main(argc, argv, envp)` then `exit()` | syscall layer, internal libc |
| `src/internal/libc.c` | Defines `__libc` global, `__progname` | none (pure data) |

#### 6c. Stdio Basics (unblocks `printf`/`fprintf`)
| File | Purpose | Depends on |
|---|---|---|
| `src/stdio/__stdio_exit.c` | Flush/close streams on exit | weak_alias |
| `src/stdio/__stdout_write.c` | Write to stdout file descriptor | `write()` syscall |
| `src/stdio/__overflow.c` | Handle buffer overflow for output streams | internal stdio |

#### 6d. Unistd (I/O syscalls)
| File | Purpose | Depends on |
|---|---|---|
| `src/unistd/write.c` | Calls `__syscall(SYS_write, fd, buf, count)` | syscall layer ✅ |
| `src/unistd/read.c` | Calls `__syscall(SYS_read, fd, buf, count)` | syscall layer ✅ |
| `src/unistd/close.c` | Calls `__syscall(SYS_close, fd)` | syscall layer ✅ |

---

### Phase 7: Musl Build Integration (Future)

Once individual file compilation works, integrate into musl's build system:

- Create `config.mak` for i286 target
- Create `musl_cc.sh` wrapper (clang → llvm-i286 → nasm)
- Build musl as static library (`libc.a`)
- Link test programs against full musl

---

## Execution Order

```
Phase 0: Build 2ine (podman)                    ✅ DONE
Phase 1: Build runtime.lib                       ✅ DONE
Phase 2: Codegen alias support                   ✅ DONE
Phase 3: Syscall layer (os2_syscall.asm)         ✅ DONE
Phase 4: Port string/ctype functions             [IN PROGRESS]
Phase 5: Integration tests                       [after Phase 4]
Phase 6: Port exit/startup/stdio                 [after Phase 5 — unblocks full musl programs]
Phase 7: Musl build integration                  [future]
```

---

## Key Infrastructure

| Item | Current State | Notes |
|---|---|---|
| Entry point | Codegen recognizes `@main`, emits `..start: → call main → call far DOSEXIT` | OS/2 NE executables |
| Test programs | All use `int main()` | Works with existing infrastructure |
| `runtime.lib` | ✅ Built | Includes os2_syscall.o |
| `2ine_debugger` | ✅ Built | lx_loader, lib2ine.so, libdoscalls.so present |
| Podman | Available (4.9.3) | Required for 2ine build only |
| `libsdl2-2.0-0:i386` | ✅ Installed | Required by lx_loader |
| Calling convention | Stack-based, right-to-left, 32-bit params as two 16-bit words | OS/2 standard |
| Return values | AX (16-bit) or DX:AX (32-bit signed) | Negative = error |
| Math runtime helpers | Register-based (ax:bx, cx:dx → di:si), `retf` | Used by codegen for 32-bit mul/div |
| Alias support | ✅ Implemented | weak_alias resolves at codegen time |
| Syscall dispatch | ✅ Implemented | `__os2_syscall0`–`__os2_syscall6` in runtime/os2_syscall.asm |

---

## File Map

### Files Created

| File | Phase | Purpose |
|---|---|---|
| `runtime/os2_syscall.asm` | 3 | OS/2 syscall dispatch layer |

### Files Modified

| File | Phase | Changes |
|---|---|---|
| `src/ir/IrAst.h` | 2 | Added `aliases` map to Module struct |
| `src/ir/IrVisitor.h` | 2 | Added `visitIndirectSymbolDef` declaration |
| `src/ir/IrVisitor.cpp` | 2 | Added `visitIndirectSymbolDef` handler |
| `src/codegen/InstructionSelect.h` | 2 | Added `setAliasMap` method |
| `src/codegen/InstructionSelect.cpp` | 2 | Added `setAliasMap` implementation |
| `src/codegen/CallOps.cpp` | 2 | Added alias resolution in `lowerCall` |
| `src/codegen/CodeGen.cpp` | 2 | Added `selector.setAliasMap(module.aliases)` |
| `2ine_debugger/lx_loader.c` | 0 | Added `__attribute__((used))` to `endLxProcess` |

---

## Next Steps

1. **Phase 4:** Port string/ctype functions (strlen, strcmp, memcmp, isalpha, isdigit, etc.)
2. **Phase 5:** Create integration tests for string/ctype functions
3. **Phase 6:** Port `_Exit`, `exit`, `__libc_start_main`, `write`, `read`, `close` to unblock full musl program execution
4. **Phase 7:** Integrate into musl build system

**Critical path:** After string functions are verified, Phase 6 (exit + startup + unistd) is what unblocks running real musl-based programs.
