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

test: all
	$(foreach file,$(EXECUTABLES),./$(file)${\n})

clean:
	rm -rf *.pyc *.o *.dSYM $(EXECUTABLES)


define \n


endef
