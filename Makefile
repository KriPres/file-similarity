CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined

all: compare

compare: compare.o fileutil.o wfd.o jsd.o
	$(CC) $(CFLAGS) -o compare compare.o fileutil.o wfd.o jsd.o -lm

compare.o: compare.c fileutil.h wfd.h jsd.h
	$(CC) $(CFLAGS) -c compare.c

fileutil.o: fileutil.c fileutil.h
	$(CC) $(CFLAGS) -c fileutil.c

wfd.o: wfd.c wfd.h
	$(CC) $(CFLAGS) -c wfd.c

jsd.o: jsd.c jsd.h
	$(CC) $(CFLAGS) -c jsd.c

clean:
	rm -f *.o compare
