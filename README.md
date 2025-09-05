# Swirl Language Server

Next Gen Language Server for Swirl written in C++. Currently only has dummy implementations for lsp methods.

## Prerequisites

-   A C++ compiler (e.g., g++, clang++)
-   CMake (3.10 or higher)
-   A JS runtime and package manager (We use [Bun](https://bun.sh) here)

## Getting Started

### 1. Install Client Dependencies

```bash
bun install
```

### 2. Build the Language Server

This project uses CMake to build the C++ server. The executable will be created at `build/swirl_lsp`.

```bash
# Configure the build
cmake -B build -S .

# Compile the server
cmake --build build
```

### 3. Run

The `main.ts` script acts as a client that launches and communicates with the compiled server.

```bash
bun run main.ts
```

### 4. Integrate with VSCode
To use this language server in VSCode, you can set up a simple extension or use the `vscode-languageclient` library to connect to the server executable and communicate via stdio.

## Currently Supported Methods

-   initialize
-   textDocument/didOpen
-   textDocument/didChange
-   textDocument/didSave
-   textDocument/completion
-   textDocument/completionItem/resolve
-   textDocument/hover
-   textDocument/publishDiagnostics
-   $/setTrace

---

This project was created using `bun init` in bun v1.1.42. [Bun](https://bun.sh) is a fast all-in-one JavaScript runtime.
