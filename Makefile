CC=c++
CXXFLAGS=-std=c++1y -Wall -O0 -g -Ivendor/Catch/include
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


.PHONY: all
all: test_main.o $(EXECUTABLES) xorshift

ecdh.o: ecdh.cpp ecdh.h

xorshift: CXXFLAGS += -Ivendor/TestU01-1.2.3/include -Wno-writable-strings
xorshift: LDFLAGS += -Lvendor/TestU01-1.2.3/testu01/.libs -ltestu01
xorshift: xorshift.o test_main.o vendor/TestU01-1.2.3
	$(CC) $(LDFLAGS) $< -o $@
test-xorshift:
	./xorshift "[bigcrush]"

vendor/TestU01-1.2.3: vendor/TestU01.zip
	unzip $< -d vendor
	cd $@ && CFLAGS="-Wno-return-type" ./configure && make


.PHONY: clean
clean:
	rm -rf *.pyc *.o *.dSYM $(EXECUTABLES) vendor/TestU01-1.2.3


# generated rules

define GENERIC_TEST

.PHONY: test
test:: $(1)
	./$$< ||:


endef

$(foreach exec,$(EXECUTABLES),$(eval $(call GENERIC_TEST,$(exec))))
