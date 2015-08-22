CXXFLAGS=-std=c++1y -Igoogletest/include -Wall -O0 -g
LDFLAGS=-Lgoogletest -lgtest


all: insertion_sort

test: all
	nosetests *.py
	./insertion_sort

clean:
	@rm -f insertion_sort
	@rm -f *.pyc
	@rm -rf *.dSYM
