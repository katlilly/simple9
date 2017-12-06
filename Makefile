
CC = gcc
CFLAGS = -ansi -pedantic -W -Wall -g

compress: compress.c
	$(CC) $(CFLAGS) -c compress.c


fake-data-compress: compress.c fake-data-compress.c
	$(CC) $(CFLAGS) -o fake-data-compress fake-data-compress.c \
	compress.c


wsj-compress: compress.c wsj-compress.c
	$(CC) $(CFLAGS) -o wsj-compress wsj-compress.c compress.c


do-wsj-compress: wsj-compress postings.bin
	./wsj-compress postings.bin


clean:
	rm wsj-compress compress.o
