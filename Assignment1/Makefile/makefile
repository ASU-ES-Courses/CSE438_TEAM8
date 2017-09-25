all:thread_creation_old

thread_creation_old : thread_creation_old.o
	gcc -o thread_creation_old thread_creation_old.o -lpthread

thread_creation_old.o : thread_creation_old.c 
	gcc -c thread_creation_old.c -lpthread

clean:
	rm thread_creation_old.o thread_creation_old
