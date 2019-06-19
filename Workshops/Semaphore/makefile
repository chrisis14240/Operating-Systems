CC = gcc
CFLAGS = -pthread

all: semaphoremake nthreadsemaphoremake
	
semaphoremake: semaphore.c
	$(CC) $(CFLAGS) -o semaphore semaphore.c

nthreadsemaphoremake: nThreadsSemaphore.c
	$(CC) $(CFLAGS) -o nSemaphore nThreadsSemaphore.c
