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


#define MAX_PROCESSES 2
#define NUM_THREADS 8

sem_t semaphore;

struct data_type {
  int data;
  int p;
}

* process(void *data) {
  struct data_type *process_data;
  process_data = (struct data_type*) data;
  int a, i, p;

  sem_wait(&semaphore);
  a = process_data->data;
  p = process_data -> p;

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


  fflush(stdout);
  sem_post(&semaphore);
}

int main() {
  int i, error;
  char *returned_value;

  struct data_type thread_data[NUM_THREADS];
  pthread_t ID_thread[NUM_THREADS];


  for (i = 0; i < NUM_THREADS; i++) {
    thread_data[i].data = i;
    thread_data[i].p = i+1;
  }

  // semaphore = sem_open("semaphore_name", O_CREAT, 0700, MAX_PROCESSES);
  sem_init(&semaphore,0,1);

  for (i = 0; i < NUM_THREADS; i++) {
    error = pthread_create(&ID_thread[i], NULL, (void *)process, (void *)(&thread_data[i]));
    if (error != 0) {
      perror("Thread can not be created");
      exit(-1);
    }
  }

  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(ID_thread[i], (void **)&returned_value);
  }

  sem_close(&semaphore);
  sem_unlink("semaphore_name");

  printf("\n");
  return 0;
}
