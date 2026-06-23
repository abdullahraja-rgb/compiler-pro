# Roadmap: Building a C / C++ Compiler

A phase-by-phase plan. Each phase has a **milestone** (the thing that works at the
end), a **build-along** resource you code against, and **theory references** you read
to understand _why_. Build-along books are followed linearly; theory books are
desk references you dip into per topic.

---

## The resources (and how to use each)

| Short name            | Full title                                                                                     | Role                                                                                                                  |
| --------------------- | ---------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------- |
| **WACC**              | _Writing a C Compiler_ — Nora Sandler (No Starch, 2024)                                        | **Primary build-along.** Builds a real C compiler emitting x64 assembly, one feature per chapter. This is your spine. |
| **CI**                | _Crafting Interpreters_ — Robert Nystrom (free at craftinginterpreters.com)                    | Best on-ramp. Builds two complete language implementations. Read Part I before/alongside WACC for the front end.      |
| **EaC**               | _Engineering a Compiler_, 3rd ed. — Cooper & Torczon (2022)                                    | Main theory reference. Strong on IR, optimization, and the back end.                                                  |
| **DB**                | _Compilers: Principles, Techniques & Tools_, 2nd ed. — Aho, Lam, Sethi, Ullman ("Dragon Book") | Theory lookup, especially parsing and grammars.                                                                       |
| **chibicc**           | Rui Ueyama's small C compiler (GitHub)                                                         | Reference implementation. Read it commit-by-commit when stuck.                                                        |
| **LLVM Kaleidoscope** | Official tutorial at llvm.org + LLVM Language Reference                                        | Only if you take the LLVM back-end route (see Phase 7+).                                                              |

> One book to read cover-to-cover: none. **WACC is followed in order**; everything
> else is consulted per topic. If you read one theory book front-to-back for pleasure,
> make it EaC.

---

## Phase 0 — Orientation (1–2 days)

**Goal:** Hold the whole pipeline in your head before writing code.

- **CI Ch. 1–3** — Introduction, _A Map of the Territory_, _The Lox Language_.
  Ch. 2 is the single best diagram of the full compiler pipeline you'll find.
- **EaC Ch. 1** — Overview of Compilation.
- **WACC Introduction** — sets up the project structure and the test suite you'll
  use the whole way (Sandler ships a test harness; clone it now).

**Decide now:** what language you'll _write the compiler in_ (C++, Rust, OCaml, and
Python are all common — WACC's pseudocode is language-agnostic), and what you'll
_target_ (x64 assembly is what WACC teaches; ARM64 if you're on Apple Silicon — the
concepts transfer but register names/ABI differ).

---

## Phase 1 — Lexer + first real executable (the "return 42" milestone)

**Milestone:** `int main(void) { return 42; }` compiles to a native binary that
exits with code 42. This wires up lexing → parsing → codegen → assemble+link
end-to-end. Everything after is incremental.

- **WACC Ch. 1** — A Minimal Compiler. _Do this fully before anything else._
- **CI Ch. 4** — Scanning (lexer fundamentals).
- **CI Ch. 5–6** — Representing Code, Parsing Expressions (AST + recursive descent).
- **Theory:** EaC Ch. 2 (Scanners) / DB Ch. 3 (Lexical Analysis); EaC Ch. 3
  (Parsers) / DB Ch. 4 (Syntax Analysis).

> You don't need parser-generator theory (LR tables etc.) to succeed — recursive
> descent, taught in CI Ch. 6, is what real compilers like Clang and GCC use. Read
> the LR material in DB Ch. 4 for understanding, not because you'll implement it.

---

## Phase 2 — Expressions & operators

**Milestone:** `1 + 2 * 3 - (4 / 2)` evaluates correctly — i.e. precedence and
associativity are right, and unary operators work.

- **WACC Ch. 2** — Unary Operators.
- **WACC Ch. 3** — Binary Operators (here you meet **precedence climbing / Pratt
  parsing**, the key technique).
- **WACC Ch. 4** — Logical and Relational Operators (`&&`, `||`, `==`, short-circuit).
- **CI Ch. 17** — Compiling Expressions (Nystrom's Pratt-parser walkthrough is the
  clearest explanation in print).
- **Theory:** EaC Ch. 7 (Code Shape) for how expressions become instructions.

This is also where WACC introduces **TACKY**, its three-address intermediate
representation. Cross-read **EaC Ch. 4 (Intermediate Representations)** and
**DB Ch. 6 (Intermediate-Code Generation)** to see why an IR sits between AST and
assembly.

---

## Phase 3 — Variables & semantic analysis

**Milestone:** local variables, assignment, and the compiler _rejects_ bad programs
(undeclared variables, duplicate declarations) with real error messages.

- **WACC Ch. 5** — Local Variables (stack slots, the symbol table).
- **CI Ch. 8** — Statements and State.
- **CI Ch. 11** — Resolving and Binding (scope resolution — important and easy to
  get subtly wrong).
- **Theory:** EaC Ch. 5 (Syntax-Driven Translation) — this is the new chapter in the
  3rd edition on _semantic elaboration_, exactly this phase. DB Ch. 5
  (Syntax-Directed Translation) covers the same ground.

---

## Phase 4 — Control flow

**Milestone:** `if`/`else`, `while`, `for`, and conditional expressions all work;
you can write loops that actually loop.

- **WACC Ch. 6** — If Statements and Conditional Expressions.
- **WACC Ch. 7** — Compound Statements (blocks and nested scopes).
- **WACC Ch. 8** — Loops (including `break`/`continue` label management).
- **CI Ch. 9** — Control Flow.
- **CI Ch. 23** — Jumping Back and Forth (bytecode-level branch/jump emission).
- **Theory:** EaC Ch. 7 (Code Shape) covers control-structure codegen.

**At the end of this phase your language is Turing-complete.** You can write real
(if tedious) programs. This is the first natural "I built a real compiler" stopping
point if you want one.

---

## Phase 5 — Functions

**Milestone:** multiple functions with parameters, return values, and recursion
(write `fib`/`factorial` and have them work). You now respect a real calling
convention (System V AMD64 ABI on Linux/x64).

- **WACC Ch. 9** — Functions (stack frames, prologue/epilogue, argument passing).
- **WACC Ch. 10** — File-scope Variables and Storage-class Specifiers
  (`static`, `extern`, globals, linkage).
- **CI Ch. 10** — Functions; **CI Ch. 24** — Calls and Functions (bytecode VM view);
  **CI Ch. 25** — Closures (skip unless you want closures — C doesn't have them, but
  the mechanics inform C++ lambdas later).
- **Theory:** EaC Ch. 6 (Implementing Procedures) and DB Ch. 7 (Run-Time
  Environments) — the definitive treatment of stack frames, the procedure
  abstraction, and addressability.

> **This is the recommended primary target for a learning project.** A compiler that
> handles variables, full arithmetic, control flow, and functions emitting a working
> native binary is unambiguously "a real compiler." Phases 1–5 are achievable solo
> over a few months. Everything past here is the long tail.

---

## Phase 6 — Types beyond `int`

**Milestone:** pointers, arrays, strings, structs, and `malloc`/`free` work — the
program can manipulate memory directly. This is where a C-subset starts to feel like
actual C.

- **WACC Ch. 11** — Long Integers; **Ch. 12** — Unsigned Integers;
  **Ch. 13** — Floating-Point Numbers (the type system and conversions grow here).
- **WACC Ch. 14** — Pointers.
- **WACC Ch. 15** — Arrays and Pointer Arithmetic.
- **WACC Ch. 16** — Characters and Strings.
- **WACC Ch. 17** — Supporting Dynamic Memory Allocation.
- **WACC Ch. 18** — Structures (aggregate layout, alignment, member access).
- **Theory:** EaC Ch. 7 (Code Shape) for aggregate/array layout; DB Ch. 6 for type
  systems and type checking.

Completing WACC through Ch. 18 gives you a compiler for a **substantial subset of C**.
This is the realistic ceiling of the "follow one book" approach.

---

## Phase 7 — Optimization & register allocation

**Milestone:** generated code is meaningfully faster — constants folded, dead code
removed, and values kept in registers instead of memory.

- **WACC Ch. 19** — Optimizing TACKY Programs (constant folding, dead-store
  elimination, copy propagation — practical, on your own IR).
- **WACC Ch. 20** — Register Allocation (graph coloring, the capstone chapter).
- **Theory (this is where EaC earns its place):**
  - EaC Ch. 8 — Introduction to Optimization
  - EaC Ch. 9 — Data-Flow Analysis
  - EaC Ch. 10 — Scalar Optimization
  - EaC Ch. 13 — Register Allocation
  - (EaC Ch. 11 Instruction Selection, Ch. 12 Instruction Scheduling for the deep back end)
- DB Ch. 9 (Machine-Independent Optimizations) as a second angle.
- **SSA form:** if you want to optimize the way modern compilers do, study Static
  Single Assignment — EaC covers it; it's also the heart of LLVM IR.

---

## Phase 8 — The C++ cliff (where it stops being a weekend project)

A C compiler is a _huge_ achievement and a realistic solo goal. Full C++ is one of
the hardest compilation targets that exists, and there is **no single book** that
walks you through building one. Tackle these as independent research projects, each
substantial on its own:

1. **Classes & objects** — vtables, `this`, member layout, virtual dispatch.
   _Start with_ CI Ch. 12–13 (Classes, Inheritance) for the concepts, then study how
   the Itanium C++ ABI lays out objects and vtables.
2. **Name mangling** — to link against real C++ you must match an existing ABI
   (Itanium on Linux/macOS, MSVC on Windows). This is spec-reading, not textbook work.
3. **Templates** — two-phase name lookup and instantiation. This alone is harder than
   everything in Phases 1–7 combined.
4. **Overload resolution** — famously one of the most intricate algorithms in any
   language spec.
5. **Exceptions** — stack unwinding, landing pads, the unwind tables.
6. **The standard library** — effectively another enormous project.

### The pragmatic pivot: emit LLVM IR

If your goal is to compile _real-world C++_ rather than to learn back-end codegen by
hand, switch your back end to **LLVM IR** around Phase 6–7. You keep your own
lexer/parser/semantic-analysis front end and hand the IR to LLVM, which gives you
world-class optimization and every target architecture for free.

- **LLVM Kaleidoscope tutorial** (llvm.org) — builds a small front end that emits
  LLVM IR; the canonical starting point.
- **LLVM Language Reference** — your daily reference once you commit to this path.
- Trade-off: you learn _much_ less about optimization and machine-code generation
  (LLVM does it), but you reach a far more capable compiler far faster.

---

## Suggested reading order, condensed

1. **CI Ch. 1–6** → get the pipeline + a working front end in your head.
2. **WACC Ch. 1–10** → build the core compiler (through functions). _This is the
   heart of the journey._
3. **WACC Ch. 11–18** → grow it into a real C subset.
4. **WACC Ch. 19–20 + EaC Ch. 8–13** → make it fast.
5. Decide: **hand-written assembly back end** (keep going as-is) vs **LLVM IR**
   (pivot to the Kaleidoscope tutorial). This choice, more than anything, determines
   how far toward "real C++" you can practically push.

Keep **EaC** and the **Dragon Book** on the desk throughout; read **chibicc**'s
commit history whenever a WACC chapter doesn't click.
