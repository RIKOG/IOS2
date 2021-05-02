LC_ALL=cs_CZ.utf8

CFLAGS =-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread
COMPILER = gcc

make: proj2
proj2: proj2.c
	$(COMPILER) $(CFLAGS) proj2.c -o proj

run: proj2
	./proj2

.PHONY : clean clean_o
clean:
	rm proj2 *.o