# Mora

A hand-written compiler for a modern systems programming language, built in C++23.

> ⚠️ **Early stage** — lexer implemented, parser and interpreter in development.

## Language Features (planned)

Mora is a statically-typed language inspired by Rust and Elixir:

| Feature | Syntax | Status |
|---------|--------|--------|
| Immutable bindings | `let x = 5` | Planned |
| Functions | `fn add(a: Int, b: Int) -> Int { a + b }` | Planned |
| Pattern matching | `match value { ... }` | Planned |
| Structs & Enums | `struct Point { x, y }` | Planned |
| Traits & Impls | `trait Show { fn show() }` — `impl Show for Point { ... }` | Planned |
| Pipe operator | `data |> transform |> output` | Planned |
| Raw strings | `r#"raw text"#` | Lexer done |
| Format strings | `f"Hello, {name}!"` | Lexer done |
| Macros | `macro!()` | Planned |

## Architecture

```
Source → Scanner → Tokens → Parser → AST → Interpreter
  ✅        ✅        ✅       🚧      🚧        🚧
```

### Lexer (implemented)
- Hand-written scanner, no generator tools
- Full Unicode support (`char32_t`)
- Raw strings, format strings, block comments
- Error recovery with accumulated diagnostics

### Parser & Interpreter (planned)
- Recursive descent parser
- Tree-walking interpreter
- Arena allocator for AST nodes

## Tech Stack

- **C++23** — `std::string_view`, designated initializers, modern CMake
- **CMake 3.24+** — build system
- **Catch2 v3** — testing framework
- Zero external dependencies for the compiler core

## Build

```bash
cmake -B build -S .
cmake --build build

# Run the lexer
./build/mora example.mora

# Run tests
cd build && ctest
```

## Why "Mora"?

From the Latin *mora* ("delay, pause") — the compiler pauses to understand your code before executing it. Also a nod to the metrical unit in poetry: structure before meaning.

## License

MIT
