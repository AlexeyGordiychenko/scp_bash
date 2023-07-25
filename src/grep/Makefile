CC=gcc
CFLAGS=-Wall -Werror -Wextra

all: grep

grep: grep.c grep.h
	$(CC) $(CFLAGS) grep.c grep.h -o grep

clean:
	rm grep || true

rebuild: clean all
