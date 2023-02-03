## Documentation

- [Types](Docs/Types.md)
- [Declarations](Docs/Declarations.md)


# I. Introduction

A1 is Adamnite's smart contract programming language. It aims to bring the **simplicity and expressiveness** of Pythonic syntax to the blockchain world. It is designed to make it easy for **both experienced and less experienced developers** to write, test and deploy smart contracts on Adamnite's ecosystem.

With A1, you can write smart contracts using familiar Python syntax and semantics, while also taking advantage of the **security and immutability** provided by the blockchain.

This documentation will guide you through the process of writing, testing and deploying A1 smart contracts,
as well as provide examples and best practices for common use cases.

### Key Features

A1 provides an easy-to-use script that is inherently readable, supports the use of data feeds and external APIs, and is modular, allowing for the use of packages and standards that can drive the creation of complex contracts. This standard library in particular will serve to make the development of contracts easier, and will also protect developers from making rudimentary errors that can compromise their contract.

You can read more about A1's technical specifications in the Programming Language and Execution section of Adamnite's [Technical Paper](https://adamnite.org/Adamnite_Technical_Paper.pdf).

# II. Getting Started

- Installation and setup all A1 source files utilize the `.ao` extension.
## A1 Smart-Contract Language Documentation
<br>
<i> Version 0.1.0 </i>

- [Types](Docs/Types.md)
- [Declarations](Docs/Declarations.md)


# I. Introduction
<br>
A1 is Adamnite's smart contract programming language. A1 aims to bring the simplicity and expressiveness of Pythonic syntax to the blockchain world. 
It is designed to make it easy for experienced as well as new developers alike to write, test and deploy smart contracts on Adamnite's ecosystem. 
<br> <br>
With A1, you can write smart contracts using familiar Python syntax and semantics, while also taking advantage of the security and immutability 
provided by the blockchain. This documentation will guide you through the process of writing, testing and deploying A1 smart contracts, 
as well as provide examples and best practices for common use cases.
<br><br>

- Key Features
<br>
A1 provides an easy-to-use script that is inherently readable,
supports the use of data feeds and external APIs, and is modular,
allowing for the use of packages and standards that can drive the
creation of complex contracts. This standard library in particular
will serve to make the development of contracts easier, and will
also protect developers from making rudimentary errors that can
compromise their contract.
<br>
You can read more about A1's technical specifications in the Programming Language and Execution section of Adamnite's 

[Technical Paper](https://adamnite.org/Adamnite_Technical_Paper.pdf).
<br><br>

# II. Getting Started
- <strong> Installation and Setup <strong>
<br>
To start developing with A1, download and run A1's official packages. You can find A1's packages here with detailed instructions
on how to install and setup. 
<br><br>
Once packages are installed, load up your IDE of choice and start developing smart contracts in A1. Visual Studio Code is the recommended IDE for A1 development as Adamnite offers an official VSC extension for A1, you can find out more as well as download this extension
<br>
VSCode [Extension](/VSCodeExtension/README.md)
<br><br>
- <strong> Create and Deploy a Basic Smart Contract </strong>
<br>

All A1 source files utilize the `.ao` extension.
<br> <br>
- <strong> Compiling A1 code </strong>
<br>
```cd``` into the CLI folder and compile the A1 code to a file by running the debug command ```./cli debug --from-file examples/sum.ao 
9703bdb17a160ed80486a83aa3c413c1 ===> i64 (i64, i64)``` replacing the examples/sum.ao with whichever .ao or A1 code file you would like to compile.
<br>
This should then return the hash of the code, parameters, and types as well. Using the returned hash run the execute command in the CLI like so:
```cli % ./cli execute --from-file examples/sum.a1 --call-args 0x123,1 --gas 1000000 --function 9703bdb17a160ed80486a83aa3c413c1```
<br>
Lastly, to build - ```cd``` into the ```core/VM/cli folder``` and run ```go build```
