#!/bin/bash

echo "Installing prerequisites: wget, llvm, cmake, ninja"
brew install wget llvm@14 cmake ninja

echo "Downloading A1 repository, exporting to /tmp/A1..."
wget -P /tmp/A1 https://github.com/Adamnite/A1/archive/refs/heads/main.zip
cp -R /Users/marin/personal/A1 /tmp/A1

echo "Building A1 project"
cd /tmp/A1 && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -GNinja .. && ninja

echo "Copying AOC executable to /usr/bin"
mv /tmp/A1/build/bin/aoc /tmp/

echo "Cleaning things up..."
brew uninstall wget cmake ninja
rm -rf /tmp/A1
