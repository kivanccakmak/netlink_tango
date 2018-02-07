CC=gcc
LD=ld

default: all

all: deps utils target

deps:
	$(CC) -c factory.c
	$(CC) -c netlink.c

utils:
	$(CC) -c debug.c
	$(CC) -c utils.c

target:
	$(CC) -c main.c
	@echo "maho"

clean:
	rm -rf *.o
	rm -rf main
