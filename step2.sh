#!/bin/bash
set -e
. scripts/in_container.sh

cd "$BUILD_DIR/jsoncpp"
python amalgamate.py

mkdir -p "$BUILD_DIR/llvm-build"
cd "$BUILD_DIR/llvm-build"
cmake -G 'Unix Makefiles' \
  -D 'LLVM_TARGETS_TO_BUILD=' \
  -D 'LLVM_BUILD_TOOLS=OFF' \
  "$BUILD_DIR/llvm"
make
