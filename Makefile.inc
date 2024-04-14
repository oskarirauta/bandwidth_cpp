BANDWIDTH_DIR?=./bandwidth
INCLUDES += -I$(BANDWIDTH_DIR)/include

BANDWIDTH_OBJS:= \
	objs/interface_t.o \
	objs/bandwidth_t.o \
	objs/bps_t.o

objs/bps_t.o: $(BANDWIDTH_DIR)/src/bps_t.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/interface_t.o: $(BANDWIDTH_DIR)/src/interface_t.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/bandwidth_t.o: $(BANDWIDTH_DIR)/src/bandwidth_t.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;
