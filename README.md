# A1: <br/> Develop smart contracts for the Adamnite blockchain

[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](https://github.com/m-peko/bitflags/blob/master/LICENSE)
[![Build](https://github.com/Adamnite/A1/actions/workflows/build.yml/badge.svg)](https://github.com/Adamnite/A1/actions/workflows/build.yml)
[![Codecov](https://codecov.io/gh/Adamnite/A1/branch/main/graph/badge.svg?token=YBUVS7JAEQ)](https://codecov.io/gh/Adamnite/A1)

A1 programming language is a high-level programming language for developing smart contracts for the Adamnite blockchain. With its light syntax, it primarily strives for simplicity and ease of use while retaining the scalability and security required to write immutable multi-party smart contracts.

<p align="center">
    <a href="#getting-started">Getting started</a> |
    <a href="#development">Development</a> |
    <a href="#join-us">Join us</a>
</p>

## Getting started

Learn more about the A1 project:

- [Types](Docs/Types.md)
- [Declarations](Docs/Declarations.md)
- [AOC](AOC/README.md)

## Development

### Building

First, there are few prerequisites to be installed:

```sh
$ brew install wget llvm@14 cmake ninja
$ export PATH="$(brew --prefix llvm@14)/bin:${PATH}"
```

Then, execute the following commands in order to build the AOC - A1 compiler:

```sh
$ mkdir -p build && cd build
$ cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
$ ninja
```

Once the build is successful, you are ready to start playing around!

There are many examples under `Examples` directory. Here is the basic `hello_world.ao`:

```python
contract HelloWorld:
    def get() -> str:
        return "Hello, world!"

var = HelloWorld()
print(var.get())
```

Try out the compiler by running the following command:

```sh
$ ./build/bin/aoc hello_world.ao > test.ll
$ clang test.ll -o test
$ ./test
```

## Join us

A1 is committed to a welcoming environment where everyone can contribute.

- Join the design discussion at our [GitHub forum](https://github.com/Adamnite/A1/discussions)
- See our [contributing guidelines](CONTRIBUTING.md) for information about the A1 development community
- We discuss A1 and the Adamnite Blockchain on [Discord](https://discord.gg/AxbRrXvS)
