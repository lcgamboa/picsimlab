CXX= g++
CXXFLAGS= -Wall -ggdb


OBJS= $(patsubst %.cc,%.o,$(filter-out speedtest.cc,$(wildcard *.cc)))

OBJS2= tests.o speedtest.o

all: $(OBJS) $(OBJS2)
	@echo "Linking tests"
	@$(CXX) $(CXXFLAGS) $(OBJS) -otests $(LIBS)
	@$(CXX) $(CXXFLAGS) $(OBJS2) -ospeedtest $(LIBS)

%.o: %.cc
	@echo "Compiling $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

clean:
	rm -rf tests speedtest *.o
