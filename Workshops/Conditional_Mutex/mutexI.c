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
pthread_cond_t empty, full;

int buffer[MAX_BUFFER], data_num;

* consumer() {
  struct data_type * process_data;
  int a, i, j, p, position = 0, data;

  for (i = 0; i < MAX_DATA; i++) {
    pthread_mutex_lock(&mutex);
    while(data_num == 0)
      pthread_cond_wait(&empty, &mutex);

    data = buffer[position];
    if (position == 15)
      position = 0;
    else
      position++;

    data_num--;

    if (data_num == MAX_BUFFER -1)
        pthread_cond_signal(&full);

    pthread_mutex_unlock(&mutex);

    printf("\nThe following data has been consumed: %d", data);
    fflush(stdout);
    sleep(1);

  }

    pthread_exit(0);
}


* producer() {
  struct data_type *process_data;
  int a, i, j, p, position = 0, data;

  for (i = 0; i < MAX_DATA; i++) {
    pthread_mutex_lock(&mutex);
    while(data_num == MAX_BUFFER)
      pthread_cond_wait(&full, &mutex);

    buffer[position] = i;
    data = i;

    if (position == 15)
      position = 0;
    else
      position++;

    data_num++;

    if (data_num == 1)
      pthread_cond_signal(&empty);

    pthread_mutex_unlock(&mutex);
    printf("\nThe following data has been produced: %d", data);
    fflush(stdout);
  }

    pthread_exit(0);
}

int main() {
  int error;
  char *retorned_value;

  pthread_t ID_thread_I,ID_thread_II, ID_thread_III, ID_thread_IV;

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&full, NULL);
  pthread_cond_init(&empty, NULL);

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
  pthread_cond_destroy(&full);
  pthread_cond_destroy(&empty);

  return 0;
}
