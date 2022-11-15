## AOC

AOC is the official compiler for the A1 programming language.

### Usage

At the moment, AOC CLI supports only basic set of commands:

```sh
$ aoc --help
aoc - A1 smart contract programming language compiler

USAGE: aoc [OPTIONS] file

OPTIONS:
        -h, --help <help> Print help
        -v, --version <version> Print version
        -o, --output <file> Write output to specific file
        --ast <ast> Write Abstract Syntax Tree (AST) to standard output
        --llvm-ir <llvm-ir> Write generated LLVM IR code to standard output

ARGS:
        file File to be compiled
```