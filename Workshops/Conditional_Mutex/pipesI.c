#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <strings.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_BUFFER 16
#define MAX_DATA 1000

pthread_mutex_t mutex;
int buffer, data_num, pipefd[2];

void* consumer() {
  int  i, err, data;


  for (i = 0; i < MAX_DATA; i++) {
    pthread_mutex_lock(&mutex);

    err = read(pipefd[0], &data, sizeof(int));
    if (err == -1) {
      perror("err while reading from the pipe");
      exit(-1);
    }

    pthread_mutex_unlock(&mutex);

    printf("\nThe following data has been consumed: %d", data);
    fflush(stdout);
    sleep(1);

  }

  close(pipefd[0]);
  pthread_exit(0);
}


void* producer() {
  int  i, err, data;


  for (i = 0; i < MAX_DATA; i++) {
    pthread_mutex_lock(&mutex);

    err = write(pipefd[1], &i, sizeof(int));
    data = i;
    if (err == 1) {
      perror("err while writing in the pipe");
      exit(-1);
    }

    pthread_mutex_unlock(&mutex);
    printf("\nThe following data has been produced: %d", data);
    fflush(stdout);
  }

    pthread_exit(0);
}

int main() {
  int error;
  char *retorned_value;

  error = pipe(pipefd);
  if (error == -1) {
    perror("err while creating the pipe");
    exit(-1);
  }

  pthread_t ID_thread_I,ID_thread_II, ID_thread_III, ID_thread_IV;

  pthread_mutex_init(&mutex, NULL);

  error = pthread_create(&ID_thread_I, NULL, (void *)producer, NULL);
  if (error != 0) {
    perror("Thread can not be created");
    exit(-1);
  }

  pthread_create(&ID_thread_II, NULL, (void *)consumer, NULL);
  if (error != 0) {
    perror("Thread can not be created");
    exit(-1);
  }

  pthread_join(ID_thread_II, (void **)&retorned_value);
  pthread_join(ID_thread_I, (void **)&retorned_value);
  pthread_mutex_destroy(&mutex);


  return 0;
}
