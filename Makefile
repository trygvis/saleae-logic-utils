# vim: noexpandtab

UNAME_S=$(shell uname -s)

ifeq ($(UNAME_S), Darwin)
	PLATFORM=DARWIN
else
	ifeq ($(UNAME_S), Linux)
		PLATFORM=LINUX
	endif
endif

ifeq ($(PLATFORM),)
	$(error Unknown platform: $(UNAME_S))
endif

sinclude Makefile.local

LOGIC_SDK_VERSION ?= 2.0.4
LOGIC_SDK_HOME    ?= ../LogicSDK-$(LOGIC_SDK_VERSION)
ifeq ($(PLATFORM), DARWIN)
LOGIC_LIB         ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/OSX/lib
LOGIC_INCLUDE     ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/OSX/include
else
LOGIC_LIB         ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/Linux/lib
LOGIC_INCLUDE     ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/Linux/include
endif

CFLAGS   += -I$(LOGIC_INCLUDE)
CFLAGS   += -Ilib

LDFLAGS += -L$(LOGIC_LIB)
ifeq ($(PLATFORM), DARWIN)
LDFLAGS += -framework CoreFoundation
LDFLAGS += -framework IOKit
LDFLAGS += -lboost_thread-xgcc40-mt-s-1_39
else
LDFLAGS += -m32
LDFLAGS += -lboost_thread-gcc43-mt-s-1_39
endif
LDFLAGS += -lLogicInterface
LDFLAGS += -lboost_unit_test_framework-mt
LD = g++
CC = g++

LIB_HEADERS = $(wildcard lib/*.h)
LIB_SRC = $(wildcard lib/*.cpp)
LIB_OBJS = $(patsubst lib/%.cpp,work/%.o,$(LIB_SRC))

BIN_SRC = $(wildcard src/*.cpp)
BIN_OBJS = $(patsubst src/%.cpp,work/%.o,$(BIN_SRC))
BINS = $(patsubst src/%.cpp,work/bin/%,$(BIN_SRC))

TESTS_SRC = $(wildcard tests/*.cpp)
TESTS_OBJS = $(patsubst tests/%.cpp,work/%.o,$(TESTS_SRC))
TESTS = $(patsubst tests/%.cpp,work/tests/%,$(TESTS_SRC))
TESTS_RUNS = $(patsubst tests/%.cpp,work/tests/%-run,$(TESTS_SRC))

all: $(TESTS) $(TESTS_RUNS) $(BINS)

$(LIB_SRC): .deps

$(LIB_SRC): $(LIB_HEADERS)
$(BINS): $(LIB_OBJS) $(LIB_HEADERS)
$(TESTS): $(filter-out work/main.o,$(LIB_OBJS)) $(LIB_HEADERS)

.SECONDARY: $(TEST_OBJS)

work/tests/%-run: work/tests/%
	@$<
	@touch $@

work/bin/%: work/%.o
	@mkdir -p work/bin
	@echo LD $@
	@$(LD) $(LDFLAGS) $(filter %.o, $^) -o $@

work/tests/%: work/%.o
	@mkdir -p work/tests
	@echo LD $@
	@$(LD) $(LDFLAGS) $(filter %.o, $^) -o $@

work/%.o: lib/%.cpp
	@if [ ! -d work ]; then mkdir work; fi
	@echo CXX $@
	@$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

work/%.o: src/%.cpp
	@if [ ! -d work ]; then mkdir work; fi
	@echo CXX $@
	@$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

work/%.o: tests/%.cpp
	@if [ ! -d work ]; then mkdir work; fi
	@echo CXX $@
	@$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	@echo Removing work/
	@rm -rf work $(wildcard *~) .deps

sinclude .deps
.deps: $(wildcard lib/*.h)
	@echo Creating .deps
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MM $(wildcard */*.cpp) > .deps.tmp
	@mv .deps.tmp .deps
