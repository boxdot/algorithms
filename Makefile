CC=c++
CXXFLAGS=-std=c++1y -Wall -O0 -g
LDFLAGS=test_main.o

EXECUTABLES = \
	sorting \
	next_permutation \
	johnson_trotter \
	3sum \
	ecdh \
	cnf \
	intersection \
	range_search \
	xorshift


all: test_main.o $(EXECUTABLES)

ecdh.o: ecdh.cpp ecdh.h

xorshift: CXXFLAGS += -I/usr/local/include -Wno-writable-strings
xorshift: LDFLAGS += -Lvendor/TestU01/testu01 -ltestu01


test: all
	$(foreach file,$(EXECUTABLES),./$(file)${\n})

clean:
	rm -rf *.pyc *.o *.dSYM $(EXECUTABLES)


define \n


endef
