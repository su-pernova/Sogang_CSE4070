cc = gcc
target = testlib
objects = bitmap.o debug.o hash.o hex_dump.o list.o main.o

$(target): $(objects) bitmap.h debug.h hash.h hex_dump.h limits.h list.h round.h main.h
	$(cc) -o $@ $(objects)

.PHONY : clean
clean:
	rm $(target) $(objects)