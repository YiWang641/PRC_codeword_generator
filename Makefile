# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -fopenmp
LDFLAGS  := -fopenmp

# Object files
PRC_OBJS := PRCWatermarker.o
EXP_OBJS := experiment.o
T1_OBJS  := test1_timed.o
T2_OBJS  := test2_evaluate.o
T3_OBJS := test3_timing.o

# Default target
all: experiment test1 test2 test3

# Link demo
experiment: $(PRC_OBJS) $(EXP_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Link Test 1
test1: $(PRC_OBJS) $(T1_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Link Test 2
test2: $(PRC_OBJS) $(T2_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Link Test 3: timing compare
test3: $(PRC_OBJS) $(T3_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Clean up
.PHONY: clean
clean:
	rm -f *.o experiment test1 test2 test1_codewords.txt
