CC=gcc
CFLAGS=-pthread

all: main

#main:
#	cd ./server
#	make
#	cd ../client
#	make

clean:
	rm ./server/main.o
	rm ./client/main.o
