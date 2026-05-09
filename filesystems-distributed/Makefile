CC = gcc
CFLAGS = -Wall -Werror -fPIC

all: server libmfs.so

server: server.c udp.c
	$(CC) $(CFLAGS) -o server server.c udp.c

libmfs.so: mfs.c udp.c
	$(CC) $(CFLAGS) -shared -o libmfs.so mfs.c udp.c

clean:
	rm -f server libmfs.so *.o
