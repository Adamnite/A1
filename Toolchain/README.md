# Toolchain

Learn more about the AOC, the A1 programming language compiler, [here](AOC/README.md).

## Development

A1 programming language is compiled down to ADVM bytecode. ADVM is an Adamnite VM derived from WASM. Thus, the WASM-based toolchain is needed. Using [WASI-SDK](https://github.com/WebAssembly/wasi-sdk) is recommended but any other toolchain (e.g. Emscripten) should work as well.

### Prerequisites

- WASI SDK 15.0
- LLVM 14.0.6
- Clang 14

#### *WASI SDK 15.0*

Run following instructions in order to get the WASI SDK for MacOS:

```sh
$ export WASI_VERSION=15
$ export WASI_VERSION_FULL=${WASI_VERSION}.0
$ wget https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-${WASI_VERSION}/wasi-sdk-${WASI_VERSION_FULL}-macos.tar.gz
$ tar xvf wasi-sdk-${WASI_VERSION_FULL}-macos.tar.gz
$ export WASI_SDK_PATH=wasi-sdk-${WASI_VERSION_FULL}
```

Procedure for other platforms should be similar to this.

#### *LLVM*

Follow below instructions to install [Adamnite's LLVM fork](https://github.com/Adamnite/llvm-project):

```sh
$ cd llvm-project/llvm && mkdir -p build && cd build
$ cmake -DLLVM_USE_LINKER=lld \
        -DLLVM_ENABLE_PROJECTS="clang;lld" \
        -DLLVM_ENABLE_TERMINFO=OFF \
        -DLLVM_ENABLE_ZSTD=OFF \
        -DLLVM_ENABLE_LIBEDIT=OFF \
        -DCMAKE_BUILD_TYPE=Debug \
        -GNinja ..
$ ninja
$ cmake -DCMAKE_INSTALL_PREFIX=/tmp/llvm -P cmake_install.cmake
```

### Building

Run the following commands in order to build the `aoc` compiler:

```sh
$ cd Toolchain && mkdir -p build && cd build
$ cmake \
    -DLLVM_DIR=/tmp/llvm/lib/cmake/llvm \
    -DClang_DIR=/tmp/llvm/lib/cmake/clang \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_TESTS=OFF \
    -DWASM_SYSROOT_PATH=$WASI_SDK_PATH/share/wasi-sysroot \
    -DWASM_RUNTIME_LIBRARY_PATH=$WASI_SDK_PATH/lib/clang/14.0.3/lib/wasi/libclang_rt.builtins-wasm32.a \
    -GNinja ..
$ ninja
```

`aoc` compiler should now be available in `build/bin` directory.