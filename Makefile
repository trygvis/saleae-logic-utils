sinclude Makefile.local

LOGIC_SDK_VERSION ?= 2.0.4
LOGIC_SDK_HOME    ?= ../LogicSDK-$(LOGIC_SDK_VERSION)
LOGIC_LIB         ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/OSX/lib
LOGIC_INCLUDE     ?= $(LOGIC_SDK_HOME)/LogicConsole/3rdParty/OSX/include

CFLAGS   = -I$(LOGIC_INCLUDE)
CPPFLAGS = -I$(LOGIC_INCLUDE)

LDFLAGS  = -L$(LOGIC_LIB)
LDFLAGS += -framework CoreFoundation
LDFLAGS += -framework IOKit
LDFLAGS += -lLogicInterface
LDFLAGS += -lboost_thread-xgcc40-mt-s-1_39
LD = g++
CC = g++

BINS = $(addprefix bin/,logic_read logic_to_event logic_trigger pattern_generator)

all: $(BINS)

bin/logic_to_event: logic_to_event.o common.o
bin/logic_read: logic_read.o common.o
bin/logic_trigger: logic_trigger.o common.o
bin/pattern_generator: pattern_generator.o common.o
$(BINS): .deps

bin/%:%.o
	@mkdir -p bin
	@echo LD $@
	@$(LD) $(LDFLAGS) $(filter-out .deps, $^) -o $@

%.o:%.cpp
	@echo CXX $@
	@$(CXX) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f $(wildcard *.o) *~ $(BINS)

sinclude .deps
.deps: $(wildcard *.cpp) $(wildcard *.h)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM $(wildcard *.cpp) > .deps
