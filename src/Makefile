CC=gcc
LD=ld

default: all

all: deps utils ifaces target

deps:
	$(CC) -c factory.c
	$(CC) -c eventq.c

ifaces:
	$(CC) -c netlink.c

utils:
	$(CC) -c debug.c
	$(CC) -c utils.c

target:
	$(CC) -c main.c
	$(LD) -r factory.o debug.o utils.o -o mylib.o
	$(LD) -r netlink.o mylib.o eventq.o -o deps.o
	$(CC) main.o deps.o -o main -lpthread -g

clean:
	rm -rf *.o
	rm -rf main
