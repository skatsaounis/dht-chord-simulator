CXX = g++
CXXARGS = --std=c++11
RM = rm

all:
	$(CXX) $(CXXARGS) -c main.cpp -o main.o
	$(CXX) $(CXXARGS) -c options.cpp -o options.o
	$(CXX) $(CXXARGS) main.o options.o -o dsemu

clean:
	$(RM) *.o
	$(RM) dsemu
