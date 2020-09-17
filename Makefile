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
-s WASM=1 -s MODULARIZE=1 -s=EXPORT_ES6=0 -s ENVIRONMENT='web,worker,shell' \
-s INVOKE_RUN=0 -s EXIT_RUNTIME=0 -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
-s ALLOW_MEMORY_GROWTH=1 --pre-js wasm/prejs.js

WAT2WASM=wat2wasm
WASM2WAT=wasm2wat

EXTENSION=cc
SOURCES=main.cc lexer.cc parser.cc opcodes.cc directive.cc memory.cc
EMSOURCES=lexer.cc parser.cc opcodes.cc directive.cc memory.cc
SOURCE_FOLDER=src
OBJECTS = $(patsubst %.$(EXTENSION),$(OBJECT_FOLDER)/%.o,$(SOURCES))
OBJECT_FOLDER=bin
EMOBJECTS = $(patsubst %.$(EXTENSION),$(OBJECT_FOLDER)/%.bc,$(EMSOURCES))

NATIVE_TARGET=native/stack_machine
WASM_TARGET=wasm/stack_machine.wasm

all: native wasm
native: $(NATIVE_TARGET)
wasm: $(WASM_TARGET)



# compile for native
$(NATIVE_TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJECT_FOLDER)/main.o: $(SOURCE_FOLDER)/main.cc $(SOURCE_FOLDER)/parser.h $(SOURCE_FOLDER)/lexer.h $(SOURCE_FOLDER)/opcodes.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/lexer.o: $(SOURCE_FOLDER)/lexer.cc $(SOURCE_FOLDER)/lexer.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/parser.o: $(SOURCE_FOLDER)/parser.cc $(SOURCE_FOLDER)/parser.h $(SOURCE_FOLDER)/lexer.h $(SOURCE_FOLDER)/stringhelper.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/opcodes.o: $(SOURCE_FOLDER)/opcodes.cc $(SOURCE_FOLDER)/opcodes.h $(SOURCE_FOLDER)/stringhelper.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/directive.o: $(SOURCE_FOLDER)/directive.cc $(SOURCE_FOLDER)/directive.h $(SOURCE_FOLDER)/directive_handlers.h $(SOURCE_FOLDER)/stringhelper.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/memory.o: $(SOURCE_FOLDER)/memory.cc $(SOURCE_FOLDER)/memory.h $(SOURCE_FOLDER)/stringhelper.h
	$(CC) $(CFLAGS) -c $< -o $@





$(WASM_TARGET): $(EMOBJECTS) wasm/prejs.js
	$(EMCC) $(EMLDFLAGS) --bind -s EXPORT_NAME="createModule" $(EMOBJECTS) -o $(patsubst %.wasm,%.js,$@)
	$(WASM2WAT) $@ > $(patsubst %.wasm,%.wat,$@)


$(OBJECT_FOLDER)/lexer.bc: $(SOURCE_FOLDER)/lexer.cc $(SOURCE_FOLDER)/lexer.h
	$(EMCC) $(EMFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/parser.bc: $(SOURCE_FOLDER)/parser.cc $(SOURCE_FOLDER)/parser.h $(SOURCE_FOLDER)/lexer.h
	$(EMCC) $(EMFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/opcodes.bc: $(SOURCE_FOLDER)/opcodes.cc $(SOURCE_FOLDER)/opcodes.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/directive.bc: $(SOURCE_FOLDER)/directive.cc $(SOURCE_FOLDER)/directive.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECT_FOLDER)/memory.bc: $(SOURCE_FOLDER)/memory.cc $(SOURCE_FOLDER)/memory.h
	$(CC) $(CFLAGS) -c $< -o $@

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
