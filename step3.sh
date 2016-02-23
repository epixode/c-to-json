#!/bin/sh

DIR="$( cd "$(dirname $0)" && pwd )"
test "$1" = "--in-container" || exec docker run --rm --interactive --tty \
  --volume "$DIR:/mnt" llvm-build-env /mnt/step3.sh --in-container

cd /mnt/src
make
strip /mnt/build/c-to-json
