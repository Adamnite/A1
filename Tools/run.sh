#!/bin/bash

/tmp/aoc $1 > /tmp/test.ll
clang /tmp/test.ll -o /tmp/test

printf "\nOutput:\n\n"

# Run the executable
/tmp/test