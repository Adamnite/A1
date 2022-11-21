# Toolchain

Learn more about the AOC, the A1 programming language compiler, [here](AOC/README.md).

## Development

Assuming that the WASI SDK toolchain is installed and on the path `$WASI_SDK_PATH`, one can build `CoreLib` library and the `AOC` compiler by following below instructions.

### MacOS specific

Run the following commands to install the rest of prerequisites on the MacOS:

```sh
$ brew install wget llvm@14 cmake ninja
$ export PATH="$(brew --prefix llvm@14)/bin:${PATH}"
```

## Ubuntu specific

Run the following commands to install the rest of prerequisites on the Ubuntu:

```sh
$ # Install cmake
$ sudo snap install cmake --classic
$
$ # Install ninja
$ sudo wget -qO /usr/local/bin/ninja.gz https://github.com/ninja-build/ninja/releases/latest/download/ninja-linux.zip
$ sudo gunzip /usr/local/bin/ninja.gz
$ sudo chmod a+x /usr/local/bin/ninja
$
$ # Install LLVM and it's dependencies
$ sudo apt update
$ sudo apt install build-essential llvm-14 zlib1g-dev clang
$ sudo apt-get -y install clang-14 libclang-14-dev libffi-dev libedit-dev libpfm4-dev libtinfo-dev
$
$ # Hacky workaround to trick LLVM
$ touch /usr/lib/llvm-14/lib/libMLIRSupportIndentedOstream.a
```

### Building

Run the following commands in order to build the `AOC` compiler:

```sh
$ cd Toolchain
$ mkdir -p build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=OFF \
    -DWASM_SYSROOT_PATH=$WASI_SDK_PATH/share/wasi-sysroot \
    -DWASM_RUNTIME_LIBRARY_PATH=$WASI_SDK_PATH/lib/clang/14.0.3/lib/wasi/libclang_rt.builtins-wasm32.a \
    -GNinja ..
$ ninja
```