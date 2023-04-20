## Documentation

- [Types](Types.md)
- [Declarations](Declarations.md)


# I. Introduction

A1 is Adamnite's smart contract programming language. It aims to bring the **simplicity and expressiveness** of Pythonic syntax to the blockchain world. It is designed to make it easy for **both experienced and less experienced developers** to write, test and deploy smart contracts on Adamnite's ecosystem.

With A1, you can write smart contracts using familiar Python syntax and semantics, while also taking advantage of the **security and immutability** provided by the blockchain.

This documentation will guide you through the process of writing, testing and deploying A1 smart contracts,
as well as provide examples and best practices for common use cases.

### Key Features

A1 provides an easy-to-use script that is inherently readable, supports the use of data feeds and external APIs, and is modular, allowing for the use of packages and standards that can drive the creation of complex contracts. This standard library in particular will serve to make the development of contracts easier, and will also protect developers from making rudimentary errors that can compromise their contract.

You can read more about A1's technical specifications in the Programming Language and Execution section of Adamnite's [Technical Paper](https://adamnite.org/Adamnite_Technical_Paper.pdf).

# II. Getting Started

### Installation and Setup

1. Download package from [here](https://github.com/Adamnite/A1/releases/tag/v0.1.0-beta)
    - using Docker is probably the most convenient way of installing and using A1 compiler
2. Installation
    - if you have downloaded one of the *docker* packages, follow the steps below:
        - `$ docker load < package-docker.tar.gz`
        - `$ docker images` - find loaded image
        - `$ docker run -it package-docker bash` - enter Docker container
        - `$ aoc --version` - try out A1 compiler
    - if you have downloaded one of the *non-docker* packages, extract the files and move them to the `/usr/bin` directory

### Compilation

A1 source files utilize `.ao` extension.

1. Write your first smart contract using the A1 programming language or simply use one of the examples from [here](../Examples/)
2. Compile the code by executing the following command `$ aoc -o my-contract my-first-contract.ao`
3. Follow the steps specified in [goAdamnite](https://github.com/Adamnite/goAdamnite) to learn how to easily upload your compiled contract to the blockchain
