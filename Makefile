CXXFLAGS=-std=c++1y -Igoogletest/include -Wall -O0 -g
LDFLAGS=-Lgoogletest -lgtest


EXECUTABLES = \
	insertion_sort \
	merge_sort


all: $(EXECUTABLES)

test: all
	nosetests *.py
	$(foreach file,$(EXECUTABLES),./$(file)${\n})

clean:
	@$(foreach file,$(EXECUTABLES),rm -f $(file)${\n})
	@rm -f *.pyc
	@rm -rf *.dSYM


define \n


endef
