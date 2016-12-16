CXX = g++
RM = rm

all:
	$(CXX) -c main.cpp -o main.o
	$(CXX) -c options.cpp -o options.o
	$(CXX) main.o options.o -o dsemu

clean:
	$(RM) *.o
	$(RM) dsemu
