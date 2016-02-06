CC=$(CXX)
CXXFLAGS=-std=c++14 -Wall -Wextra -O0 -g -Ivendor/Catch/include

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

xorshift: CXXFLAGS += -Ivendor/TestU01-1.2.3/include -Wno-writable-strings
xorshift: LDFLAGS += -Lvendor/TestU01-1.2.3/testu01/.libs -ltestu01
xorshift.o: xorshift.cpp vendor/TestU01-1.2.3
	$(CXX) $(CXXFLAGS)  -c -o $@ $<

test-xorshift:
	./xorshift "[bigcrush]"

vendor/TestU01-1.2.3: vendor/TestU01.zip
	unzip $< -d vendor
	cd $@ && CFLAGS="-Wno-return-type" ./configure && make


.PHONY: all clean
clean:
	rm -rf *.pyc *.o *.dSYM $(EXECUTABLES)


# generic rules

%: %.cpp test_main.o

define GENERIC_TEST
.PHONY: test
test:: $(1)
	./$$<
endef

define GENERIC_EXEC
$(1): $(1).o test_main.o
endef

$(foreach exec,$(EXECUTABLES),$(eval $(call GENERIC_EXEC,$(exec))))
$(foreach exec,$(EXECUTABLES),$(eval $(call GENERIC_TEST,$(exec))))
