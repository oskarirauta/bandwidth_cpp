all: world

CXX?=g++
CXXFLAGS?=--std=c++17 -Wall -fPIC

OBJS:= \
	objs/main.o

BANDWIDTH_DIR:=.
include Makefile.inc
include cmdparser/Makefile.inc
include common/Makefile.inc

world: example

$(shell mkdir -p objs)

objs/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

example: $(BANDWIDTH_OBJS) $(CMDPARSER_OBJS) $(COMMON_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

.PHONY: clean
clean:
	@rm -rf objs example
