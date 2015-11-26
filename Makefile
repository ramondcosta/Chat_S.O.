CC=gcc
CFLAGS=-pthread

all: main

main:
	make server/Makefile
	make client/Makefile

clean:
	cd server
	rm *.o
	cd ../client
	rm *.o
