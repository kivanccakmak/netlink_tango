CC=gcc
SRC=netlink_test.c
DEP=myioctls.c
TARGET=netlink_test

default: all

all: $(SRC) $(DEP)
	$(CC) -Wall $(SRC) $(DEP) -o $(TARGET)

clean:
	rm -rf $(TARGET)

