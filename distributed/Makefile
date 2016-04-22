CXXFLAGS := -std=c++1y -O0 -g
CC = $(CXX)
BINS := snapshot
all: $(BINS)

$(BINS): astream.o distsys.o

astream.o : astream.cpp astream.h
distsys.o: distsys.cpp distsys.h


clean:
	rm -rf $(BINS) *.o *.dSYM
