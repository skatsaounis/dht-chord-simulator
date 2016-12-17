CXX = g++
CXXARGS = --std=c++11
RM = rm

all: commands.o
	$(CXX) $(CXXARGS) -c main.cpp -o main.o
	$(CXX) $(CXXARGS) -c options.cpp -o options.o
	$(CXX) $(CXXARGS) main.o options.o commands.o -o dsemu

commands.o: commands.cpp
	$(CXX) $(CXXARGS) -c commands.cpp -o commands.o

clean:
	$(RM) *.o
	$(RM) dsemu
