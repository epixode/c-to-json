
BUILD_DIR = /mnt/build
LLVM_SRC = $(BUILD_DIR)/llvm
LLVM_BUILD = $(BUILD_DIR)/llvm-build

PROGRAM = $(BUILD_DIR)/c-to-json

SRCS = main.cpp $(BUILD_DIR)/jsoncpp/dist/jsoncpp.cpp
SRCSPRECOMP = precompiled.h

CXX=g++
LD=$(CXX)
CXXFLAGS=-std=c++11 -fno-rtti -O0 -g
CPPFLAGS=\
	-I$(LLVM_SRC)/include -I$(LLVM_SRC)/tools/clang/include \
	-I$(LLVM_BUILD)/include -I$(LLVM_BUILD)/tools/clang/include \
	-I$(BUILD_DIR)/jsoncpp/dist
LDFLAGS=\
	-L$(LLVM_BUILD)/lib \
	-lclangIndex \
	-lclangARCMigrate \
	-lclangRewriteFrontend \
	-lclangFormat \
	-lclangTooling \
	-lclangToolingCore \
	-lclangFrontend \
	-lclangDriver \
	-lclangSerialization \
	-lclangParse \
	-lclangSema \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangRewrite \
	-lclangAnalysis \
	-lclangEdit \
	-lclangASTMatchers \
	-lclangAST \
	-lclangLex \
	-lclangBasic \
	-lLLVMAsmParser \
	-lLLVMBitReader \
	-lLLVMCore \
	-lLLVMMC \
	-lLLVMMCParser \
	-lLLVMOption \
	-lLLVMSupport \
	-lpthread \
	-lncurses \
	-ldl \
	-lz

DEPSDIR = $(BUILD_DIR)/.deps/$(PROGRAM)
OBJSDIR = $(BUILD_DIR)/.objs/$(PROGRAM)
DEPS = $(SRCS:%.cpp=$(DEPSDIR)/%.d)
OBJS = $(SRCS:%.cpp=$(OBJSDIR)/%.o)
DEPSPRECOMP = $(SRCSPRECOMP:%.h=$(DEPSDIR)/%.d)
OBJSPRECOMP = $(SRCSPRECOMP:%.h=%.h.gch)
CURRENTMAKEFILE=$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

all: $(PROGRAM)

.PHONY: clean
clean:
	-rm -rf $(DEPSPRECOMP) $(DEPSDIR) $(OBJSPRECOMP) $(OBJSDIR) $(PROGRAM)

$(PROGRAM): $(DEPSPRECOMP) $(DEPS) $(OBJSPRECOMP) $(OBJS)
	$(LD) $(OBJS) $(CXXFLAGS) $(LDFLAGS) -o $@

$(DEPSDIR)/%.d: %.cpp $(CURRENTMAKEFILE)
	@echo "Making dependencies for $<"
	@dirname $@ | xargs mkdir -p 2>/dev/null || echo "$@ already exists" >/dev/null
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $< 2>/dev/null | sed 's#.*:# $@ :#1' > $@

$(DEPSDIR)/%.d: %.h $(CURRENTMAKEFILE)
	@echo "Making dependencies for precompiled header $<"
	@dirname $@ | xargs mkdir -p 2>/dev/null || echo "$@ already exists" >/dev/null
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $< 2>/dev/null | sed 's#.*:# $@ :#1' > $@

%.h.gch:%.h $(DEPSPRECOMP) $(CURRENTMAKEFILE)
	@echo "Precompiling header $@..."
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $< || echo "error. Disabling precompiled header"
	@echo "...Done"

$(OBJSDIR)/%.o: %.cpp $(DEPSDIR)/%.d $(CURRENTMAKEFILE)
	@echo "Compiling source $@..."
	@dirname $@ | xargs mkdir -p 2>/dev/null || echo "$@ already exists" >/dev/null
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SRCSPRECOMP:%.h=-include %.h) -c $< -o $@

ifneq ($(strip $(MAKECMDGOALS)),clean)
ifneq ($(strip $(DEPSPRECOMP)),)
-include $(DEPSPRECOMP)
endif
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif
