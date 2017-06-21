CC=gcc
SRC=netlink_test.c
TARGET=netlink_test

default: all

all: $(SRC)
	$(CC) $(SRC) -o $(TARGET)

clean:
	rm -rf $(TARGET)

