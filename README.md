# A1 Language: <br/> Develop smart contracts for the Adamnite blockchain

[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](https://github.com/m-peko/bitflags/blob/master/LICENSE)
[![Build](https://github.com/Adamnite/A1/actions/workflows/build.yml/badge.svg)](https://github.com/Adamnite/A1/actions/workflows/build.yml)
[![Codecov](https://codecov.io/gh/Adamnite/A1/branch/main/graph/badge.svg?token=YBUVS7JAEQ)](https://codecov.io/gh/Adamnite/A1)

A1 programming language is a high-level programming language for developing smart contracts for the Adamnite blockchain. With its light syntax, it primarily strives for simplicity and ease of use.

<p align="center">
    <a href="#getting-started">Getting started</a> |
    <a href="#getting-started">Development</a> |
    <a href="#join-us">Join us</a>
</p>

## Getting started

Learn more about the A1 project:

- [Types](Docs/Types.md)
- [Declarations](Docs/Declarations.md)

## Development

### Building

In order to build the A1 compiler, execute the following commands:

```sh
$ # create build directory
$ mkdir build
$ cd build

$ # generate configuration files
$ cmake -GNinja ..

$ # compile
$ ninja
```

### Testing

In order to run unit tests, execute the following commands:

```sh
$ # create build directory
$ mkdir build
$ cd build

$ # generate configuration files
$ cmake -GNinja -DENABLE_TESTING=ON ..

$ # compile
$ ninja

$ # run tests
$ ctest
```

## Join us

A1 is committed to a welcoming environment where everyone can contribute.

- See our [contributing guidelines](CONTRIBUTING.md) for information about the A1 development community
- We discuss A1 and Adamnite Blockchain on [Discord](https://discord.gg/AxbRrXvS)