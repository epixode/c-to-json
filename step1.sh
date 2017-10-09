#!/bin/bash
set -e
. scripts/in_container.sh

mkdir -p "${BUILD_DIR}"

git_clone() {
  if test -d "${BUILD_DIR}/$1"; then
    (cd "${BUILD_DIR}/$1" && git pull)
  else
    git clone "$2" "${BUILD_DIR}/$1"
  fi
}

git_clone jsoncpp https://github.com/open-source-parsers/jsoncpp
git_clone llvm https://github.com/llvm-mirror/llvm.git
git_clone llvm/tools/clang https://github.com/llvm-mirror/clang.git
git_clone llvm/projects/compiler-rt https://github.com/llvm-mirror/compiler-rt.git

# LLVM_REVISION=298849
# svn co -r "${LLVM_REVISION}" "http://llvm.org/svn/llvm-project/llvm/trunk" "${BUILD_DIR}/llvm"
# svn co -r "${LLVM_REVISION}" "http://llvm.org/svn/llvm-project/cfe/trunk" "${BUILD_DIR}/llvm/tools/clang"
# svn co -r "${LLVM_REVISION}" "http://llvm.org/svn/llvm-project/compiler-rt/trunk" "${BUILD_DIR}/llvm/projects/compiler-rt"
