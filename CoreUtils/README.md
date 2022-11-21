# CoreUtils

`CoreUtils` library contains standard utility functions that can be used by the A1 smart contracts.

## Features

`CoreUtils` library currently supports only basic features:

| Features | Functions   |
| -------- |-------------|
| Crypto   | - `sha512` <br/> - `ripemd160` |
| String   | - `is_utf8` |

Once more functionality is added, `CoreUtils` library will be splitted into several smaller libraries (e.g. `Crypto`, `String`, etc.).

## Development

### Production

Assuming that the WASI SDK toolchain is installed and on the path `$WASI_SDK_PATH`, one can build `CoreUtils` library by running following commands:

```sh
$ mkdir -p build
$ cd build
$ cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=OFF \
    -DWASI_SDK_PREFIX=$WASI_SDK_PATH \
    -DCMAKE_TOOLCHAIN_FILE=$WASI_SDK_PATH/share/cmake/wasi-sdk.cmake \
    ..
$ ninja
```

### Testing

Unfortunately, WASI SDK does not yet work well with the GTest library. Therefore, build the tests with the regular Clang/GCC toolchain:

```sh
$ mkdir -p build
$ cd build
$ cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON ..
$ ninja
```