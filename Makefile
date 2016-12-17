CXX = g++
CXXFLAGS = --std=c++11
RM = @rm -f

all: dsemu

dsemu: main.o commands.o options.o
	$(CXX) $(CXXFLAGS) main.o options.o commands.o -o dsemu

clean:
	$(RM) *.o dsemu
