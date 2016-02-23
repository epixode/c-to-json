
#include <iostream>
#include <fstream>
#include <vector>

#include <json/json.h>

#include <llvm/Config/llvm-config.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/Support/CrashRecoveryContext.h>
#include <llvm/Support/MemoryBuffer.h>

#include <clang/Basic/Version.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Lex/Lexer.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/RecursiveASTVisitor.h>
