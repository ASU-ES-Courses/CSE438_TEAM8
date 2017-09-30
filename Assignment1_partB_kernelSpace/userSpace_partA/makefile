ifeq ($(TEST_TARGET), Galileo2)
	CC = $(TOOLDIR)/i586-poky-linux-gcc
else
	CC = gcc
endif


all:main

main : main.o
	$(CC) -Wall -o main main.o -lpthread -lm

main.o : main.c lib_sq.h
	$(CC) -c main.c -lpthread -lm

clean:
	rm main.o main
