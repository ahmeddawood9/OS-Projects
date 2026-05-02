CC = gcc
CFLAGS = -Wall -Werror -pthread -O

all: mapreduce.o wordcount

mapreduce.o: mapreduce.c mapreduce.h
	$(CC) $(CFLAGS) -c mapreduce.c

wordcount: wordcount.c mapreduce.o
	$(CC) $(CFLAGS) -o wordcount wordcount.c mapreduce.o

clean:
	rm -f *.o wordcount
