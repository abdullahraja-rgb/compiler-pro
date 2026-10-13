# C Compiler in C++

A small C compiler written in C++, inspired by Nora Sandler’s
_Writing a C Compiler_.

The project follows the book’s compiler architecture and development stages,
while branching out through my own C++ implementations, design decisions,
debugging tools, benchmarks and experiments.

It currently supports integer expressions, arithmetic, comparisons, unary
operators and short-circuiting logical operators. Source code passes through
a lexer, parser, abstract syntax tree, TACKY intermediate representation and
x86-64 assembly generator.

The project is still in development, with variables, control flow, loops and
functions planned for later stages.
