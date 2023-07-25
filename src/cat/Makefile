CC=gcc
CFLAGS=-Wall -Werror -Wextra

all: cat

cat: cat.c cat.h
	$(CC) $(CFLAGS) cat.c cat.h -o cat

clean:
	rm cat || true

rebuild: clean all
