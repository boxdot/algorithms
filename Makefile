CC=$(CXX)
CXXFLAGS=-std=c++14 -Wall -Wextra -O0 -g -Ivendor/Catch/include

EXECUTABLES := \
	sorting \
	next_permutation \
	johnson_trotter \
	3sum \
	ecdh \
	cnf \
	intersection \
	range_search \
	xorshift

TESTU01_PATH := vendor/TestU01-1.2.3
TESTU01_INCPATH := vendor/TestU01-1.2.3/dist/include
TESTU01_LIBPATH := vendor/TestU01-1.2.3/dist/lib


all: test_main.o $(EXECUTABLES)

ecdh.o: ecdh.cpp ecdh.h

xorshift: CXXFLAGS += -I$(TESTU01_INCPATH) -Wno-writable-strings
xorshift: LDFLAGS += -L$(TESTU01_LIBPATH) -ltestu01
xorshift.o: xorshift.cpp $(TESTU01_PATH)
	$(CXX) $(CXXFLAGS)  -c -o $@ $<

test-xorshift:
	./xorshift "[bigcrush]"

$(TESTU01_PATH): vendor/TestU01.zip
	unzip $< -d vendor
	cd $@ && CFLAGS="-Wno-return-type" ./configure --prefix=$$(pwd)/dist &&  make -j && make install


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
