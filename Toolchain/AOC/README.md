## AOC

AOC is the official compiler for the A1 programming language.

### Usage

At the moment, AOC CLI supports only basic set of commands:

```sh
aoc - A1 compiler - Develop smart contracts for the Adamnite blockchain

USAGE: aoc [OPTIONS] file

Positional arguments:
        FILE File to be compiled

Optional arguments:
        -h, --help Print help
        -v, --version Print version
        -o, --output FILE Write output to specific file
        --ast Write Abstract Syntax Tree (AST) to standard output
        --llvm-ir Write generated LLVM IR code to standard output
```