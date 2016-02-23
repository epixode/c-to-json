#!/bin/sh
set -e
. scripts/in_container.sh

cd src
make

strip /mnt/build/c-to-json
