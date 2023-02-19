FROM ubuntu:22.04

ENV DEPENDENCIES="build-essential gcc cmake ninja-build python3 git wget"                                        \
    TMP_GITHUB_PAT=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 \
    LLVM_PATH=/tmp/llvm                                                                                          \
    WASI_VERSION=15                                                                                              \
    WASI_VERSION_FULL=15.0                                                                                       \
    WASI_SDK_PATH=/tmp/wasi-sdk-15.0

RUN apt-get update \
    && apt-get install -y ${DEPENDENCIES}

WORKDIR /tmp

# Install LLVM
RUN git clone -b v14.0.6-advm-patch https://github.com/Adamnite/llvm-project \
    && mkdir /tmp/llvm-project/llvm/build                                    \
    && cd /tmp/llvm-project/llvm/build                                       \
    && cmake                                                                 \
        -DLLVM_ENABLE_PROJECTS="lld"                                         \
        -DLLVM_ENABLE_TERMINFO=OFF                                           \
        -DLLVM_ENABLE_ZSTD=OFF                                               \
        -DLLVM_ENABLE_LIBEDIT=OFF                                            \
        -DCMAKE_BUILD_TYPE=Release                                           \
        -GNinja ..                                                           \
    && ninja                                                                 \
    && cmake -DCMAKE_INSTALL_PREFIX=${LLVM_PATH} -P cmake_install.cmake      \
    && rm -rf /tmp/llvm-project

# Install WASI SDK
RUN wget -P /tmp https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-${WASI_VERSION}/wasi-sdk-${WASI_VERSION_FULL}-linux.tar.gz \
    && tar xvf wasi-sdk-${WASI_VERSION_FULL}-linux.tar.gz                                                                                      \
    && rm -rf wasi-sdk-${WASI_VERSION_FULL}-linux.tar.gz

# Build A1 compiler
RUN git clone https://Adamnite:${TMP_GITHUB_PAT}@github.com/Adamnite/A1.git                                  \
    && mkdir -p /tmp/A1/Toolchain/build                                                                      \
    && cd /tmp/A1/Toolchain/build                                                                            \
    && cmake                                                                                                 \
        -DLLVM_DIR=${LLVM_PATH}/lib/cmake/llvm                                                               \
        -DCMAKE_BUILD_TYPE=Release                                                                           \
        -DENABLE_TESTS=OFF                                                                                   \
        -DWASM_SYSROOT_PATH=${WASI_SDK_PATH}/share/wasi-sysroot                                              \
        -DWASM_RUNTIME_LIBRARY_PATH=${WASI_SDK_PATH}/lib/clang/14.0.3/lib/wasi/libclang_rt.builtins-wasm32.a \
        -GNinja ..                                                                                           \
    && ninja                                                                                                 \
    && cp /tmp/A1/Toolchain/build/bin/aoc /usr/local/bin/                                                    \
    && rm -rf /tmp/A1                                                                                        \
    && aoc --version

FROM ubuntu:22.04

COPY --from=0 /tmp/llvm/bin/wasm-ld /usr/local/bin/
COPY --from=0 /usr/local/bin/aoc /usr/local/bin/
