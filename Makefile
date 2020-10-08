CC=gcc
CFLAGS=-std=c99 -Wall -Wextra

sheet: sheet.o

clean:
	rm -f sheet sheet.o