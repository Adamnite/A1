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

High-level programming language for developing smart contracts for the Adamnite blockchain.

With its light syntax, A1 primarily strives for simplicity and ease of use while retaining the scalability and security required to write immutable multi-party smart contracts.

## Getting started

Learn more about the A1 programming language:

- [Types](Docs/Types.md)
- [Declarations](Docs/Declarations.md)

## Development

Learn more about the project development:

- [CoreUtils](CoreUtils/README.md)
- [Toolchain](Toolchain/README.md)

## Smart contracts

Basic smart contract examples can be found in the [Examples](https://github.com/Adamnite/A1/tree/main/Examples) directory. Following code snippet represents simple Hello World example:

```py
contract HelloWorld:
    def get() -> str:
        return "Hello World!"
```

A1 source files have `.ao` extension.

Using [Visual Studio Code](https://code.visualstudio.com/) IDE is recommended for the development of the A1 smart contracts. Learn more about the VSCode A1 extension [here](VSCodeExtension/README.md).

## Join us

A1 is committed to a welcoming environment where everyone can contribute.

- Join the design discussion at our [GitHub forum](https://github.com/Adamnite/A1/discussions)
- See our [contributing guidelines](CONTRIBUTING.md) for information about the A1 development community
- We discuss A1 and other features of the Adamnite Blockchain on [Discord](https://discord.gg/AxbRrXvS)
