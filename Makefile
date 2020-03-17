CFLAGS= -g -static
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)


Ccompiler: $(OBJS)
		g++ -o Ccompiler $(OBJS) $(LDFLAGS)

$(OBJS): Ccompiler.h

test: Ccompiler
	./test.sh

clean:
	rm -f Ccompiler *.o *~ tmp*

.PHONY: test clean