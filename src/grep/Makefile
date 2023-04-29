CC=gcc
CFLAGS=-Wall -Werror -Wextra -fmax-errors=1 -g

all: s21_grep

s21_grep: grep.c grep.h
	$(CC) $(CFLAGS) grep.c grep.h -o s21_grep

clean:
	rm s21_grep || true

rebuild: clean all
