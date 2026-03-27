CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined

all: compare

compare: compare.o fileutil.o
	$(CC) $(CFLAGS) -o compare compare.o fileutil.o -lm

compare.o: compare.c fileutil.h
	$(CC) $(CFLAGS) -c compare.c

fileutil.o: fileutil.c fileutil.h
	$(CC) $(CFLAGS) -c fileutil.c

clean:
	rm -f *.o compare