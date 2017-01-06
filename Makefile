CXX = g++
CXXFLAGS = --std=c++11
RM = @rm -f

all: dsemu

dsemu: main.o commands.o options.o daemon-frontend.o daemon_main.o globals.o utils.o daemon-backend.o
	$(CXX) $(CXXFLAGS) $+ -o $@

clean:
	$(RM) *.o dsemu

install: dsemu
	@chmod a+x ./install.sh
	@sudo ./install.sh

uninstall: dsemu
	@chmod a+x ./uninstall.sh
	@sudo ./uninstall.sh
