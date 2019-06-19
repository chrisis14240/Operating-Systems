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
  int error;
  char *returned_value;

  struct data_type data_thread_I, data_thread_II;
  pthread_t ID_thread_I, ID_thread_II, ID_thread_III, ID_thread_IV;

  data_thread_I.data = 1;
  data_thread_II.data = 2;
  data_thread_I.p = 10;
  data_thread_II.p = 5;

  // semaphore = sem_open("semaphore_name", O_CREAT, 0700, MAX_PROCESSES);
  sem_init(&semaphore,0,1);

  error = pthread_create(&ID_thread_I, NULL, (void *)process, (void *)(&data_thread_I));
  if (error != 0) {
    perror("Thread can not be created");
    exit(-1);
  }

  error = pthread_create(&ID_thread_II, NULL, (void *)process, (void *)(&data_thread_II));
  if (error != 0) {
    perror("Thread can not be created");
    exit(-1);
  }

  pthread_join(ID_thread_I, (void **)&returned_value);
  pthread_join(ID_thread_II, (void **)&returned_value);

  sem_close(&semaphore);
  sem_unlink("semaphore_name");

  printf("\n");
  return 0;
}
