# A1

[![Build](https://github.com/Adamnite/A1/actions/workflows/build.yml/badge.svg)](https://github.com/Adamnite/A1/actions/workflows/build.yml)
[![Codecov](https://codecov.io/gh/Adamnite/A1/branch/main/graph/badge.svg?token=YBUVS7JAEQ)](https://codecov.io/gh/Adamnite/A1)

A1 is a dynamically typed, high-level programming language for developing smart contracts on the Adamnite Blockchain. A1 borrows concepts from Python, C, and best practices for PL research to at once create a programming language that is modular, simple, and efficient.

## Building

In order to compile the library, execute the following commands:

```sh
$ # create build directory
$ mkdir build
$ cd build

$ # generate configuration files
$ cmake -GNinja ..

$ # compile
$ ninja
```

## Testing

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

## Contributing

Please check our contribution guidelines [here](CONTRIBUTING.md).

Thank you for your help!
