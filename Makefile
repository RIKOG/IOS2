LC_ALL=cs_CZ.utf8

CFLAGS =-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread
COMPILER = gcc

all: proj2
proj2: proj2.c
	$(COMPILER) $(CFLAGS) proj2.c -o proj2

run: proj2
	./proj2 5 4 100 100

.PHONY : clean clean_o
clean:
	rm proj2