<p align="center">
    <img width="15%" align="center" src="./Docs/Images/Logo.png">
</p>

<h1 align="center">Develop smart contracts for the Adamnite blockchain</h1>

<p align="center">
    <a href="#getting-started">Getting started</a> |
    <a href="#development">Development</a> |
    <a href="#smart-contracts">Smart contracts</a> |
    <a href="#join-us">Join us</a>
</p>

<br/>

<img align="right" width="50%" src="./Docs/Images/HelloWorld.png">

[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](https://github.com/m-peko/bitflags/blob/master/LICENSE)
[![Build](https://github.com/Adamnite/A1/actions/workflows/build.yml/badge.svg)](https://github.com/Adamnite/A1/actions/workflows/build.yml)
[![Codecov](https://codecov.io/gh/Adamnite/A1/branch/main/graph/badge.svg?token=YBUVS7JAEQ)](https://codecov.io/gh/Adamnite/A1)

A1 is a high-level programming language for developing smart contracts for the Adamnite blockchain. With its light syntax, it primarily strives for simplicity and ease of use while retaining the scalability and security required to write immutable multi-party smart contracts.

## Getting started

Learn more about the A1 programming language:

- [Types](Docs/Types.md)
- [Declarations](Docs/Declarations.md)

## Development

### Prerequisites

- WASI SDK 15.0
- LLVM 14
- Clang 14

A1 programming language is compiled down to WASM. Thus, the WASM-based toolchain is needed. Using [WASI-SDK](https://github.com/WebAssembly/wasi-sdk) is recommended but any other toolchain (e.g. Emscripten) should work as well.

Run following commands in order to get the WASI SDK for MacOS:

```sh
$ export WASI_VERSION=15
$ export WASI_VERSION_FULL=${WASI_VERSION}.0
$ wget https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-${WASI_VERSION}/wasi-sdk-${WASI_VERSION_FULL}-macos.tar.gz
$ tar xvf wasi-sdk-${WASI_VERSION_FULL}-macos.tar.gz
```

Learn more about the project:

- [CoreUtils](CoreUtils/README.md)
- [Toolchain](Toolchain/README.md)

## Smart contracts

Basic smart contract examples can be found in the [Examples](https://github.com/Adamnite/A1/tree/main/Examples) directory. Following code snippet represents simple Hello World example:

```py
contract HelloWorld:
    def get() -> str:
        return "Hello World!"

let main = HelloWorld()
print(main.get())
```

A1 source files have `.ao` extension.

In order to compile and run the above smart contract, you will first need to install WebAssembly runtime (we recommend using [wasmtime](https://github.com/bytecodealliance/wasmtime) but any other runtime will work too) and then run the following commands:

```sh
$ aoc hello_world.ao -o test.wasm
$ wasmtime test.wasm
```

The output should be:

```
Hello, world!
```

Using [Visual Studio Code](https://code.visualstudio.com/) IDE is recommended for the development of the A1 smart contracts. Learn more about the VSCode A1 extension [here](VSCodeExtension/README.md).

## Join us

A1 is committed to a welcoming environment where everyone can contribute.

- Join the design discussion at our [GitHub forum](https://github.com/Adamnite/A1/discussions)
- See our [contributing guidelines](CONTRIBUTING.md) for information about the A1 development community
- We discuss A1 and other features of the Adamnite Blockchain on [Discord](https://discord.gg/AxbRrXvS)
