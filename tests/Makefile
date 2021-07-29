CXX= g++
CXXFLAGS= -Wall -ggdb


OBJS= $(patsubst %.cc,%.o,$(wildcard *.cc)) 

picsimlab: $(OBJS)
	@echo "Linking tests"
	@$(CXX) $(CXXFLAGS) $(OBJS) -otests $(LIBS) 

%.o: %.cc
	@echo "Compiling $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

clean:
	rm -rf tests *.o
