CXX = g++
CXXFLAGS = --std=c++11 -I../include
RM = @rm -f

.PHONY: clean

dsemu: main.o commands.o options.o daemon-frontend.o daemon-main.o globals.o utils.o daemon-backend.o
	$(CXX) $(CXXFLAGS) $+ -o $@

clean:
	$(RM) *.o
