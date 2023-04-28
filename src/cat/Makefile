CC=gcc
CFLAGS=-Wall -Werror -Wextra

all: s21_cat

s21_cat: cat.c cat.h
	$(CC) $(CFLAGS) cat.c cat.h -o s21_cat

clean:
	rm s21_cat || true

rebuild: clean all
