#!/bin/bash
set -e

DIR="$( cd "$(dirname $0)" && pwd )"
test "$1" = "--in-container" || exec docker run --rm --interactive --tty \
	--volume "$DIR:/mnt" llvm-build-env /mnt/step1.sh --in-container

BUILD_DIR="$DIR/build"
mkdir -p "${BUILD_DIR}"

if test -d "${BUILD_DIR}/jsoncpp"; then
	(cd "${BUILD_DIR}/jsoncpp" && git pull)
else
	git clone https://github.com/open-source-parsers/jsoncpp "${BUILD_DIR}/jsoncpp"
fi

svn co "http://llvm.org/svn/llvm-project/llvm/trunk" "${BUILD_DIR}/llvm"
svn co "http://llvm.org/svn/llvm-project/cfe/trunk" "${BUILD_DIR}/llvm/tools/clang"
svn co "http://llvm.org/svn/llvm-project/compiler-rt/trunk" "${BUILD_DIR}/llvm/projects/compiler-rt"

