Ccompiler: Ccompiler.cpp

test: Ccompiler
	./test.sh

clean:
	rm -f tmp*

.PHONY: test clean