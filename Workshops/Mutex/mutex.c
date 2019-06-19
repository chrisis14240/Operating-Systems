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

#define MAX_PROCESSES 1
#define NUM_THREADS 8

pthread_mutex_t lock;

struct data_type {
  int data;
  int p;
}

* process(void *data) {
  struct data_type *process_data;
  process_data = (struct data_type*) data;
  int a, i, p;

  a = process_data->data;
  p = process_data -> p;

  // pthread_mutex_lock(&lock);

  for (i = 0; i <= p; i++) {
    printf("%i ", a);
  }

  printf("\n");


  fflush(stdout);
  sleep(1);

  for (i = 0; i <= p; i++) {
    printf("-");
  }

  printf("\n");

  // pthread_mutex_unlock(&lock);
  fflush(stdout);
}

int main() {
  int error, i;
  char *retorned_value;
  struct data_type thread_data[NUM_THREADS];
  pthread_t ID_thread[NUM_THREADS];

  pthread_mutex_init(&lock, NULL);

  for (i = 0; i < NUM_THREADS; i++) {
    thread_data[i].data = i;
    thread_data[i].p = i+1;
  }

  for (i = 0; i < NUM_THREADS; i++) {
    error = pthread_create(&ID_thread[i], NULL, (void *)process, (void *)(&thread_data[i]));
    if (error != 0) {
      perror("Thread can not be created");
      exit(-1);
    }
  }

    for (i = 0; i < NUM_THREADS; i++) {
      pthread_join(ID_thread[i], (void **)&retorned_value);
    }

    pthread_mutex_destroy(&lock);


    printf("\n");

    return 0;
}
