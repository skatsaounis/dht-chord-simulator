CXX = g++
CXXFLAGS = --std=c++11
RM = @rm -f

all: dsemu

dsemu: main.o commands.o options.o daemon.o
	$(CXX) $(CXXFLAGS) $+ -o $@

clean:
	$(RM) *.o dsemu
