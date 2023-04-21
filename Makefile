CFLAGS = -std=c99 -Wall -g -Os

all: shell

alloc.o: alloc.c alloc.h  error.h
error.o: error.c error.h  alloc.h
parse.o: parse.c parse.h  alloc.h error.h
shell.o: shell.c  parse.h error.h

shell: shell.o alloc.o error.o parse.o
	$(CC) -o $@ $^ -lreadline

cd.o: cd.c

cd: cd.o
	$(CC) -o $@ $^

CXXFLAGS = -std=c++14 -Wall -g -Os -I ./bandit

test.o: test.cc
parsetest.o: parsetest.cc  parse.h

test: test.o parsetest.o parse.o error.o alloc.o
	g++ -o $@ $^

clean:
	@rm -f alloc.o error.o parse.o shell.o shell cd.o cd test.o parsetest.o test
