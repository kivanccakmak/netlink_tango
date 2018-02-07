CC=gcc
LD=ld

default: all

all: deps target

deps:
	$(CC) -c handler.c
	$(CC) -c myioctls.c
	$(CC) -c main.c

target:
	$(LD) -r handler.o myioctls.o -o mylib.o
	$(CC) main.o mylib.o -o main 

clean:
	rm -rf *.o
	rm -rf main
