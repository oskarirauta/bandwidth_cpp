BANDWIDTH_DIR?=bandwidth
INCLUDES += -I./$(BANDWIDTH_DIR)/include

BANDWIDTH_OBJS:= \
	objs/bw_interface.o \
	objs/bw_monitor.o

objs/bw_interface.o: $(BANDWIDTH_DIR)/src/bw_interface.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/bw_monitor.o: $(BANDWIDTH_DIR)/src/bw_monitor.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;
