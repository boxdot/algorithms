CC=c++
CXXFLAGS=-std=c++1y -Wall -O0 -g
LDFLAGS=test_main.o

EXECUTABLES = \
	insertion_sort \
	merge_sort \
	next_permutation \
	johnson_trotter \
	radix_sort \
	3sum \
	ecdh \
	cnf


all: test_main.o $(EXECUTABLES)

ecdh.o: ecdh.cpp ecdh.h

test: all
	nosetests *.py
	$(foreach file,$(EXECUTABLES),./$(file)${\n})

clean:
	rm -rf *.pyc *.o *.dSYM $(EXECUTABLES)


define \n


endef
