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
	range_search

# xorshift is not in the above list due to 4h test runtime


.PHONY: all
all: test_main.o $(EXECUTABLES) xorshift

ecdh.o: ecdh.cpp ecdh.h

xorshift: CXXFLAGS += -I/usr/local/include -Wno-writable-strings
xorshift: LDFLAGS += -Lvendor/TestU01/testu01 -ltestu01


.PHONY: clean
clean:
	rm -rf *.pyc *.o *.dSYM $(EXECUTABLES)


# generated rules

define GENERIC_TEST

.PHONY: test
test:: $(1)
	./$$< ||:


endef

$(foreach exec,$(EXECUTABLES),$(eval $(call GENERIC_TEST,$(exec))))
