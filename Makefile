CC=$(CXX)
CXXFLAGS=-std=c++1y -Wall -Wextra -O0 -g \
	-Ivendor/Catch/include \
	-Wno-missing-braces

EXECUTABLES := \
	sorting \
	next_permutation \
	johnson_trotter \
	3sum \
	ecdh \
	cnf \
	intersection \
	range_search \
	xorshift \
	distributed/snapshot \
	geometric/tests/barycentric

TESTU01_PATH := vendor/TestU01-1.2.3
TESTU01_INCPATH := vendor/TestU01-1.2.3/dist/include
TESTU01_LIBPATH := vendor/TestU01-1.2.3/dist/lib


all: test_main.o $(EXECUTABLES)

ecdh.o: ecdh.cpp ecdh.h

xorshift: LDFLAGS += -L$(TESTU01_LIBPATH) -ltestu01 -rpath $(TESTU01_LIBPATH)
xorshift.o: CXXFLAGS += -I$(TESTU01_INCPATH)
xorshift.o: xorshift.cpp | $(TESTU01_PATH)

test-xorshift:
	./xorshift "[bigcrush]"

$(TESTU01_PATH): vendor/TestU01.zip
	rm -rf $(TESTU01_PATH)
	unzip -o $< -d vendor
	cd $(TESTU01_PATH) && \
		CC=cc CFLAGS="-Wno-return-type" ./configure --prefix=$$(pwd)/dist && \
		make -j && \
		make install

distributed/snapshot: CXXFLAGS += -pthread
distributed/snapshot: LDLIBS += -lpthread
distributed/snapshot: distributed/distsys.o distributed/astream.o

geometric: $(patsubst %.cpp,%,$(wildcard geometric/tests/*.cpp))
geometric/tests/barycentric.o: $(wildcard geometric/*.h)


.PHONY: all clean geometric
clean:
	rm -rf *.pyc *.o geometric/**/*.o *.dSYM $(EXECUTABLES)


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
