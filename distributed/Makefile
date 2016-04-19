CXXFLAGS := -std=c++1y -O0 -g
CC=$(CXX)
all: system

system: system.o astream.o distsys.o
astream.o : astream.cpp astream.h
distsys.o: distsys.cpp distsys.h


clean:
	rm -rf system *.o *.dSYM
