# A1: <br/> Develop smart contracts for the Adamnite blockchain

[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](https://github.com/m-peko/bitflags/blob/master/LICENSE)
[![Build](https://github.com/Adamnite/A1/actions/workflows/build.yml/badge.svg)](https://github.com/Adamnite/A1/actions/workflows/build.yml)
[![Codecov](https://codecov.io/gh/Adamnite/A1/branch/main/graph/badge.svg?token=YBUVS7JAEQ)](https://codecov.io/gh/Adamnite/A1)
A1 is a high-level programming language for developing smart contracts for the Adamnite blockchain. With its light syntax, it primarily strives for simplicity and ease of use while retaining the scalability and security required to write immutable multi-party smart contracts.

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

First, download this repo and extract to make sure it is an indepentdent directory. Then, make sure you have Homebrew installed. You can install Homebrew by following the instructions [here](https://brew.sh/). Make sure you follow the appropriate instructions after installation to add Homebrew to your path. Once you have installed Homebrew, run the following commands to install the prerequisites:

```sh
$ brew install wget llvm@14 cmake ninja
$ export PATH="$(brew --prefix llvm@14)/bin:${PATH}"
```

Make sure you are in the A1 directory. Then, execute the following commands in order to build the AOC - A1 compiler:

```sh
$ mkdir -p build && cd build
$ cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
$ ninja
```

Once the build is successfull, you are ready to start playing around!
You can find examples of basic contracts in the [Examples](https://github.com/Adamnite/A1/tree/main/Examples) directory, such as the "Hello World" example below.

```python
contract HelloWorld:
    def get() -> str:
        return "Hello World!"
main = HelloWorld()
print(HelloWorld.get())
```

Once you have written your contract, save it with the ".ao" extention and run the following commands to compile the program and return an output. Be sure to replace "hello_world" with the name of your A1 file.

```sh
$ ./build/bin/aoc hello_world.ao -o output
$ ./output
```

## Join us

A1 is committed to a welcoming environment where everyone can contribute.

- Join the design discussion at our [GitHub forum](https://github.com/Adamnite/A1/discussions)
- See our [contributing guidelines](CONTRIBUTING.md) for information about the A1 development community
- We discuss A1 and other features of the Adamnite Blockchain on [Discord](https://discord.gg/AxbRrXvS)
