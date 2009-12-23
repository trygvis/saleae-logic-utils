# vim: noexpandtab
sinclude Makefile.local

LOGIC_SDK_VERSION ?= 2.0.4
LOGIC_SDK_HOME    ?= ../LogicSDK-$(LOGIC_SDK_VERSION)
LOGIC_LIB         ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/OSX/lib
LOGIC_INCLUDE     ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/OSX/include

CFLAGS   += -I$(LOGIC_INCLUDE)
CFLAGS   += -Ilib

LDFLAGS += -L$(LOGIC_LIB)
LDFLAGS += -framework CoreFoundation
LDFLAGS += -framework IOKit
LDFLAGS += -lLogicInterface
LDFLAGS += -lboost_thread-xgcc40-mt-s-1_39
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

$(BINS): $(LIB_OBJS) $(LIB_HEADERS)
#$(BINS) $(TESTS): $(LIB_OBJS)
$(TESTS): $(filter-out work/main.o,$(LIB_OBJS)) $(LIB_HEADERS)
#$(TESTS): $(LIB_OBJS) $(LIB_HEADERS)
#work/tests/test_EventTime: $(filter-out work/main.o,$(LIB_OBJS))

$(LIB_SRC): $(LIB_HEADERS)
#work/bin/%: work/%.o $(LIB_OBJS) $(LIB_HEADERS)
#work/tests/%: $(filter-out work/main.o,$(LIB_OBJS)) $(LIB_HEADERS)

yo:
	@echo TESTS=$(TESTS)
	@echo XX: $(filter-out work/main.o,$(LIB_OBJS)) $(LIB_HEADERS)

work/tests/%-run: work/tests/%
	$<
	touch $@

work/bin/%: work/%.o
	@mkdir -p work/bin
	@echo LD $@
	@$(LD) $(LDFLAGS) $(filter %.o, $^) -o $@

work/tests/%: work/%.o
	@mkdir -p work/tests
	@echo LD $@
	@$(LD) $(LDFLAGS) $(filter %.o, $^) -o $@

# $(LIB_OBJS): 
work/%.o: lib/%.cpp
	@if [ ! -d work ]; then mkdir work; fi
	@echo CXX $@
	@$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# $(BIN_OBJS): 
work/%.o: src/%.cpp
	@if [ ! -d work ]; then mkdir work; fi
	@echo CXX $@
	@$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# $(TESTS_OBJS): 
work/%.o: tests/%.cpp
	@if [ ! -d work ]; then mkdir work; fi
	@echo CXX $@
	@$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	@echo Removing work/
	@rm -rf work $(wildcard *~)

sinclude .deps
.deps: $(wildcard *.h)
	@echo Creating .deps
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MM $(wildcard *.cpp) > .deps
