CXXFLAGS=-std=c++1y -Wall -O0 -g


EXECUTABLES = \
	insertion_sort \
	merge_sort \
	next_permutation


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
