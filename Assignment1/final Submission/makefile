all:main

main : main.o
	gcc -Wall -o main main.o -lpthread -lm

main.o : main.c lib_sq.h rdtsc.h
	gcc -c main.c -lpthread -lm

clean:
	rm main.o main
