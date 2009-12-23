# vim: noexpandtab
sinclude Makefile.local

LOGIC_SDK_VERSION ?= 2.0.4
LOGIC_SDK_HOME    ?= ../LogicSDK-$(LOGIC_SDK_VERSION)
LOGIC_LIB         ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/OSX/lib
LOGIC_INCLUDE     ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/OSX/include

CFLAGS   += -I$(LOGIC_INCLUDE)

LDFLAGS += -L$(LOGIC_LIB)
LDFLAGS += -framework CoreFoundation
LDFLAGS += -framework IOKit
LDFLAGS += -lLogicInterface
LDFLAGS += -lboost_thread-xgcc40-mt-s-1_39
LDFLAGS += -lboost_unit_test_framework-mt
LD = g++
CC = g++

BINS = $(addprefix work/bin/,logic_read logic_to_event logic_trigger pattern_generator)

TEST_SRC = $(wildcard test_*.cpp)
TEST_OBJS = $(patsubst %.cpp,%.o,$(TEST_SRC))
TEST_RUNS = $(patsubst %.cpp,work/test/%-run,$(TEST_SRC))
TESTS = $(patsubst %.cpp,work/test/%,$(TEST_SRC))

all: $(TEST_RUNS) $(BINS)

work/test/%: %.o common.o

work/bin/logic_to_event: logic_to_event.o
work/bin/logic_read: logic_read.o
work/bin/logic_trigger: logic_trigger.o
work/bin/pattern_generator: pattern_generator.o

work/test/test_EventTime: test_EventTime.o

$(BINS): .deps common.o main.o
$(BINS) $(TESTS): .deps common.o

$(TEST_RUNS): work/test/%-run: work/test/%
	$<
	touch $@

work/bin/%:%.o
	@mkdir -p work/bin
	@echo LD $@
	@$(LD) $(LDFLAGS) $(filter %.o, $^) -o $@

work/test/%:%.o
	@mkdir -p work/test
	@echo LD $@
	@$(LD) $(LDFLAGS) $(filter %.o, $^) -o $@

%.o:%.cpp
	@echo CXX $@
	@$(CXX) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(wildcard *.o) *~
	rm -rf work

sinclude .deps
.deps: $(wildcard *.h)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM $(wildcard *.cpp) > .deps
