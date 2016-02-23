# c-to-json

This project produces an executable that takes a C program on its standard
input and produces its abstract syntax tree (AST) as JSON on its standard
output.

The only requirements are docker and bash.  Run the following command to build
an image used as the build environment:

    docker build --tag llvm-build-env llvm-build-env

The scripts step*.sh start a container and automate the build process:

* step1.sh checks out the sources for llvm/clang and jsoncpp,
* step2.sh builds clang,
* step3.sh builds the c-to-json tool.

After running these scripts, the executable is found in build/c-to-json.
