CC=clang++

# check for cpu feature set
OS=$(shell uname -s)
ifeq ($(OS),Darwin)
	CPUFLAGS=sysctl -n machdep.cpu.features
endif
ifeq ($(OS),Linux)
	CPUFLAGS=cat /proc/cpuinfo
endif

GREP=grep -i >/dev/null
SSEFLAGS=-msse -msse2 -mssse3 -msse4.1 -msse4.2
AVXFLAGS=-mavx
AVX2FLAGS=-mavx2
AVX512FLAGS=-mavx512f
SIMDFLAGS=$(shell $(CPUFLAGS) | $(GREP) SSE && echo $(SSEFLAGS))
SIMDFLAGS +=$(shell $(CPUFLAGS) | $(GREP) AVX && echo $(AVXFLAGS))
SIMDFLAGS +=$(shell $(CPUFLAGS) | $(GREP) AVX2 && echo $(AVX2FLAGS))
SIMDFLAGS +=$(shell $(CPUFLAGS) | $(GREP) AVX512 && echo $(AVX512FLAGS))

CFLAGS= -Wall -Wextra -Werror -g3 -O3 -std=c++11 $(SIMDFLAGS)
LDFLAGS= $(CFLAGS)

EMCC=emcc
EMFLAGS= -Wall -Wextra -Werror -g3 -O3 -std=c++11 -msse2 -msimd128
EMLDFLAGS= $(EMFLAGS) \
-s WASM=1 -s MODULARIZE=1 -s EXPORT_ES6=0 -s ENVIRONMENT='web,worker,shell' \
-s INVOKE_RUN=0 -s EXIT_RUNTIME=0 -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
-s ALLOW_MEMORY_GROWTH=1 --pre-js wasm/prejs.js

WAT2WASM=wat2wasm --enable-simd
WASM2WAT=wasm2wat --enable-simd


SRC=src
BIN=bin
OBJS=
EMOBJS=

NATIVE_TARGET=native/stack_machine
WASM_TARGET=wasm/stack_machine.wasm
WAT_TARGET=wasm/stack_machine.wat

all: native wasm wat
native: $(NATIVE_TARGET)
wasm: $(WASM_TARGET)
wat: $(WAT_TARGET)

# auto gen rules for objects
define target_depen
# exclude wasm_bindings from native
ifneq ($1,wasm_bindings)
$(BIN)/$1.o: $2
	$(CC) $(CFLAGS) -c $(word 1,$2) -o $(BIN)/$1.o
OBJS+=$(BIN)/$1.o
endif

# exclude main from wasm
ifneq ($1,main)
$(BIN)/$1.bc: $2
	$(EMCC) $(EMFLAGS) -c $(word 1,$2) -o $(BIN)/$1.bc
EMOBJS+=$(BIN)/$1.bc
endif
endef

$(eval $(call target_depen,main,$(SRC)/main.cc $(SRC)/parser.h $(SRC)/lexer.h $(SRC)/opcodes.h))
$(eval $(call target_depen,wasm_bindings,$(SRC)/wasm_bindings.cc $(SRC)/parser.h $(SRC)/lexer.h $(SRC)/memory.h))
$(eval $(call target_depen,lexer,$(SRC)/lexer.cc $(SRC)/lexer.h))
$(eval $(call target_depen,parser,$(SRC)/parser.cc $(SRC)/parser.h $(SRC)/lexer.h $(SRC)/stringhelper.h))
$(eval $(call target_depen,opcodes,$(SRC)/opcodes.cc $(SRC)/opcodes.h $(SRC)/stringhelper.h))
$(eval $(call target_depen,directive,$(SRC)/directive.cc $(SRC)/directive.h $(SRC)/directive_handlers.h $(SRC)/stringhelper.h))
$(eval $(call target_depen,memory,$(SRC)/memory.cc $(SRC)/memory.h $(SRC)/stringhelper.h))
$(eval $(call target_depen,directive_handlers,$(SRC)/directive_handlers.cc $(SRC)/memory.h $(SRC)/stringhelper.h $(SRC)/directive.h))


# link object files
$(NATIVE_TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
$(WASM_TARGET): $(EMOBJS) wasm/prejs.js
	$(EMCC) $(EMLDFLAGS) --bind -s EXPORT_NAME="createModule" $(EMOBJS) -o $(patsubst %.wasm,%.js,$@)
$(WAT_TARGET): $(WASM_TARGET)
	$(WASM2WAT) $^ > $@


.PHONY: mkdirs
mkdirs:
	@if [ ! -d src ] ; then \
		mkdir src; \
	fi
	@if [ ! -d bin ] ; then \
		mkdir bin; \
	fi

.PHONY: clean
clean:
	rm -f $(OBJECT_FOLDER)/*.o
	rm -f $(OBJECT_FOLDER)/*.bc
	rm -f $(SHELL_TARGET)
	rm -f $(WASM_TARGET)
	rm -f $(WASM_TARGET:.wasm=.js)
	rm -f $(WASM_TARGET:.wasm=.wat)
