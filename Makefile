obj-m:= task1try.o

all:
	gcc -Wall -o main task1try.c -pthread

clean:
	rm main 
