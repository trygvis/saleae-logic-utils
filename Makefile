LOGIC_LIB = ../3rdParty/OSX/lib
LOGIC_INCLUDE = ../3rdParty/OSX/include

CFLAGS = -I$(LOGIC_INCLUDE)
CPPFLAGS = -I$(LOGIC_INCLUDE)

LDFLAGS  = -L$(LOGIC_LIB)
LDFLAGS += -framework CoreFoundation
LDFLAGS += -framework IOKit
LDFLAGS += -lLogicInterface
LDFLAGS += -lboost_thread-xgcc40-mt-s-1_39
LD = g++
CC = g++

BINS = $(addprefix bin/,logic_read logic_to_event logic_trigger)
#BINS = logic_read logic_to_event logic_trigger

all: $(BINS)

logic_to_event.o: logic_to_event.cpp common.h
bin/logic_to_event: logic_to_event.o common.o

logic_read.o: logic_read.cpp common.h
bin/logic_read: logic_read.o common.o

logic_trigger.o: logic_trigger.cpp common.h
bin/logic_trigger: logic_trigger.o common.o

common.o: common.cpp common.h

bin/%:%.o
	@mkdir -p bin
	@echo LD $<
	@$(LD) $(LDFLAGS) $^ -o $@

%.o:%.cpp
	@echo CXX $<
	@$(CXX) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f $(wildcard *.o) *~ $(BINS)
