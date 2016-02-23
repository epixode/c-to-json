#!/bin/bash
set -e

DIR="$( cd "$(dirname $0)" && pwd )"
test "$1" = "--in-container" || exec docker run --rm --interactive --tty \
	--volume "$DIR:/mnt" llvm-build-env /mnt/step2.sh --in-container

BUILD_DIR=/mnt/build

mkdir -p $BUILD_DIR/llvm-build
cd $BUILD_DIR/llvm-build
cmake -G 'Unix Makefiles' \
  -D 'LLVM_TARGETS_TO_BUILD=' \
  -D 'LLVM_BUILD_TOOLS=OFF' \
  $BUILD_DIR/llvm
make

cd $BUILD_DIR/jsoncpp
python amalgamate.py

