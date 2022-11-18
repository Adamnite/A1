# A1: Develop smart contracts for the Adamnite blockchain

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

Learn more about the A1 project:

- [Types](Docs/Types.md)
- [Declarations](Docs/Declarations.md)
- [AOC](AOC/README.md)

## Development

### Prerequisites

- WASI SDK 15.0
- LLVM 14
- Clang 14

Since A1 is compiled down to WASM, we need a WASM sysroot as well as a WASM runtime library. We recommend using [WASI-SDK](https://github.com/WebAssembly/wasi-sdk), but any other toolchain should work as well.

Run following commands in order to get both sysroot and the runtime library:

```sh
$ # WASI sysroot
$ curl -LO https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-15/wasi-sysroot-15.0.tar.gz
$ tar xzvf wasi-sysroot-15.0.tar.gz
$ export WASI_SYSROOT=$(pwd)/wasi-sysroot
$
$ # WASI runtime
$ curl -LO https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-15/libclang_rt.builtins-wasm32-wasi-15.0.tar.gz
$ tar xzvf libclang_rt.builtins-wasm32-wasi-15.0.tar.gz
$ export WASI_RUNTIME=$(pwd)/lib/wasi/libclang_rt.builtins-wasm32.a
```

### MacOS specific

If you are using MacOS, please run the following commands to download the remaining prerequisites:

```sh
$ brew install wget llvm@14 cmake ninja
$ export PATH="$(brew --prefix llvm@14)/bin:${PATH}"
```

## Ubuntu specific

If you are using Ubuntu or a Linux-based distribution, please run the following commands to install the remaining prerequisities:

```sh
$ # Install cmake
$ sudo snap install cmake --classic
$ # Install ninja
$ sudo wget -qO /usr/local/bin/ninja.gz https://github.com/ninja-build/ninja/releases/latest/download/ninja-linux.zip
$ sudo gunzip /usr/local/bin/ninja.gz
$ sudo chmod a+x /usr/local/bin/ninja
$ # Install LLVM and it's dependencies
$ sudo apt update
$ sudo apt install build-essential llvm-14 zlib1g-dev clang
$ sudo apt-get -y install clang-14 libclang-14-dev libffi-dev libedit-dev libpfm4-dev libtinfo-dev
$ # Hacky workaround to trick LLVM
$ touch /usr/lib/llvm-14/lib/libMLIRSupportIndentedOstream.a
```

### Building

Make sure you are in the A1 directory. Then, execute the following commands in order to build the AOC - A1 compiler:

```sh
$ mkdir -p build && cd build
$ cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_SYSROOT_PATH=$WASI_SYSROOT -DWASM_RUNTIME_LIBRARY_PATH=$WASI_RUNTIME ..
$ ninja
```

## Smart contracts

We recommend using [Visual Studio Code](https://code.visualstudio.com/) as your IDE for ease of use and future A1 formatting extension integration.

You can find basic smart contract examples in the [Examples](https://github.com/Adamnite/A1/tree/main/Examples) directory. Your best bets for testing are probably the addition.ao contract and the hello_world.ao contract.

```py
contract HelloWorld:
    def get() -> str:
        return "Hello World!"

let main = HelloWorld()
print(main.get())
```

A1 source files have `.ao` extension.

In order to compile and run above smart contract, you will first need to install WebAssembly runtime (we recommend using [wasmtime](https://github.com/bytecodealliance/wasmtime) but any other runtime will work too) and then run the following commands:

```sh
$ aoc hello_world.ao -o test.wasm
$ wasmtime test.wasm
```

The output should be:

```
Hello, world!
```

## Join us

A1 is committed to a welcoming environment where everyone can contribute.

- Join the design discussion at our [GitHub forum](https://github.com/Adamnite/A1/discussions)
- See our [contributing guidelines](CONTRIBUTING.md) for information about the A1 development community
- We discuss A1 and other features of the Adamnite Blockchain on [Discord](https://discord.gg/AxbRrXvS)
