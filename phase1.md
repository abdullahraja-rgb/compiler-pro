The **command-line compiler program is written in C++** (`.cpp`).

For example:

```text
src/main.cpp
```

It will eventually work like this:

```bash
./mycc examples/return_42.c
```

- `mycc` → your compiler, written in **C++**
- `return_42.c` → the small **C program** your compiler processes
- Output → assembly or a native executable

So: compiler itself = **C++**; input language initially = **C**.
