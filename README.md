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
$ cmake -GNinja -DCoreLib_ENABLE_TESTING=ON ..

$ # compile
$ ninja

$ # run tests
$ ctest
```

## Contributing

Feel free to contribute.

If you find that any of the tests fail, please create a ticket in the issue tracker indicating the following information:

- platform
- architecture
- library version
- minimal reproducible example

If there is a JIRA ticket assigned to you, please create a branch by following these naming conventions:

- for features: `feature/<description>/<JIRA-ticket-number>`
- for refactoring: `refactor/<description>/<JIRA-ticket-number>`
- for improvements: `improvement/<description>/<JIRA-ticket-number>`
- for bugfixes: `bugfix/<description>/<JIRA-ticket-number>`

If there is no JIRA ticket, leave that part of a branch name empty.

Thank you for contributing and happy hacking!
