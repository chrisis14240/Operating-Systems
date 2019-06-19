CC = gcc
CFLAGS = -pthread

run: mutexmake
	./mutex
	
mutexmake: mutex.c 
	$(CC) $(CFLAGS) -o mutex mutex.c 

