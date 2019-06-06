#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <strings.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define PORT 3535
#define BACKLOG 32

struct dogType {
    int age;
    int height;
    int clinicID;
    float weight;
    char gender;
    char name[32];
    char type[32];
    char breed[16];
    bool isDeleted;
    int previousRegister;
};

// server varibles
size_t size;
int err, fd; // fd_I
socklen_t clientSize;
struct sockaddr_in server, client_I;

// dogProgram variables
FILE *file;
struct dogType *data;
int maxClinicID, nRegisters, nDeletedRegisters;
int  firstOcurrence[100009], lastOcurrence[100009];


long hashIdx(char *str) {
    long hash = 0;
    int c;

    while (c = *str++)
        hash = (tolower(c) + (hash << 6) + (hash << 16) - hash) % 100009;

    return hash;
}

int initData(void *ptr) {
	struct dogType *aux, *aux_I;
  int nRegister = 0;
	aux = (struct dogType*)ptr;
  aux_I = (struct dogType*)ptr;
  FILE *auxFile = fopen("auxDataDogs.dat", "rb+");
  FILE *realFile = fopen("dataDogs.dat", "a+");
  fclose(realFile);
  realFile = fopen("dataDogs.dat", "rb+");


  while (fread(aux, sizeof(struct dogType), 1, auxFile)) {
    if (!aux->isDeleted) {
      long hash = hashIdx(aux->name);
      if (lastOcurrence[hash] != -1) {
        aux->previousRegister = lastOcurrence[hash];
        fwrite(aux, sizeof(struct dogType), 1, realFile);
        lastOcurrence[hash] = nRegisters;
      } else {
        firstOcurrence[hash] = nRegisters;
        lastOcurrence[hash] = nRegisters;
        fwrite(aux, sizeof(struct dogType), 1, realFile);
      }

      maxClinicID = aux->clinicID;
      nRegister++;
      nRegisters++;
    }
  }

  fclose(auxFile);
  fclose(realFile);
  printf("El servidor esta listo para recibir peticiones de clientes\n");
	return 0;
}


int insertRegister(void *ptr, struct dogType *aux,  int fd_I) {
  struct dogType insertData, *auxData;
  auxData = (struct dogType *)ptr;

  insertData = *aux;
  insertData.clinicID = maxClinicID + 1;

  long hash = hashIdx(insertData.name);

  file = fopen("dataDogs.dat", "rb+");
  if (lastOcurrence[hash] != -1) {
    fseek(file, (lastOcurrence[hash])*sizeof(struct dogType), SEEK_SET);
    fread(auxData, sizeof(struct dogType), 1, file);
    fseek(file, (lastOcurrence[hash])*sizeof(struct dogType), SEEK_SET);
    auxData->previousRegister = nRegisters;
    fwrite(auxData, sizeof(struct dogType), 1, file);
  } else {
    lastOcurrence[hash] = nRegisters;
  }
  fclose(file);

  maxClinicID++;
  nRegisters++;

  file = fopen("dataDogs.dat", "a+");

  int err = fwrite(&insertData, sizeof(struct dogType), 1, file);
  if (err == 0) {
      perror("error in write");
      exit(-1);
  }

  fclose(file);
  return 0;
}

int readRegister(void *ptr,  int fd_I) {
  struct dogType *aux;
  aux = ptr;
  file = fopen("dataDogs.dat", "a+");

  int idx;
  // printf("Actualmente hay %d registros, por favor ingrese el numero de registro que desea ver.\n", nRegisters);
  send(fd_I, &nRegisters, sizeof(int), 0);
  recv(fd_I, &idx, sizeof(int), 0);
  // printf("idx: %d\n", idx);

  int counter = 0;

  while (counter != idx) {
      err = fread(aux, sizeof(struct dogType), 1, file);

      if (err == -1) {
        perror("error while reading");
        exit(-1);
      }

      if (!aux->isDeleted)
        counter++;
  }

  err = send(fd_I, aux, sizeof(struct dogType), 0);

  if (err == -1) {
    perror("error while sending");
    exit(-1);
  }

  fclose(file);

	return 0;
}

int searchByName(void *ptr,  int fd_I) {
  struct dogType *aux;
  aux = ptr;
  file = fopen("dataDogs.dat", "a+");

  char name[32];
  for (int i = 0; i < 32; i++) {
    name[i] = 0;
  }

  recv(fd_I, name, 32, 0);

  long hash = hashIdx(name);


  bool flag = false;

  if (lastOcurrence[hash] == -1) {
    printf("No hay ningun registro con el nombre recibido\n");
    send(fd_I, "None", 4, 0);
    fclose(file);
    return 0;
  }

  send(fd_I, "Some", 4, 0);
  fseek(file, (lastOcurrence[hash])*sizeof(struct dogType), SEEK_SET);

  do {
      bool flag = false;
      int err = fread(aux, sizeof(struct dogType), 1, file);

    	if (err == 0) {
    		perror("error while reading");
    		exit(-1);
    	}

      for (int i = 0; i < 32; i++) {
        if (tolower(name[i]) == 0 || tolower(aux->name[i]) == 0) {
          break;
        }

        if (tolower(name[i]) != tolower(aux->name[i])) {
          flag = true;
          break;
        }
      }

      if (flag) {
        break;
      }

      if (!aux->isDeleted) {
          send(fd_I, aux, sizeof(struct dogType), 0);
          if (err == -1) {
            perror("error while sending");
            exit(-1);
          }
      }

      if (aux->previousRegister == -1)
        break;

      fseek(file, (aux->previousRegister)*sizeof(struct dogType), SEEK_SET);
  } while(aux->previousRegister != -1);

  aux->previousRegister = -2;
  send(fd_I, aux, sizeof(struct dogType), 0);

  fclose(file);
  return 0;
}

int deleteRegister(void *ptr, int fd_I) {
  int idx, newIdx = 0, counter = 0;
  struct dogType *aux;
  aux = ptr;

  file = fopen("dataDogs.dat", "rb+");

  send(fd_I, &nRegisters, sizeof(int), 0);
  recv(fd_I, &idx, sizeof(int), 0);

  while (counter != idx) {
    err = fread(aux, sizeof(struct dogType), 1, file);

    if (err == 0) {
      perror("error while reading");
      exit(-1);
    }

    if (!aux->isDeleted)
      counter++;

    newIdx++;
  }

  aux->isDeleted = true;

  err = send(fd_I, aux, sizeof(struct dogType), 0);

  if (err == -1) {
    perror("error in send");
    exit(-1);
  }


  fseek(file, (newIdx-1)*sizeof(struct dogType), SEEK_SET);
  err = fwrite(aux, sizeof(struct dogType), 1, file);

  if (err == 0) {
      perror("error in write");
      exit(-1);
  }

  char command[64] = "rm ./clinicHistories/clinic_history_";
  char clinicID[10];

  sprintf(clinicID, "%d", aux->clinicID);
  strcat(command, clinicID);
  strcat(command, ".txt");

  system(command);

  nRegisters--;
  nDeletedRegisters++;
  fclose(file);
  return 0;
}

int resizeFile(void *ptr, FILE *file) {
  struct dogType *data;
  data = (struct dogType*)ptr;

  int idx = 0;
  FILE *auxFile = fopen("aux.dat", "a+");
  // fseek(file, 0, SEEK_SET);

  while (fread(data, sizeof(struct dogType), 1, file)) {
      if (!data->isDeleted) {
        err = fwrite(data, sizeof(struct dogType), 1, auxFile);
      }
      idx++;
  }

  fclose(auxFile);

  char *command = "rm ./dataDogs.dat";
  system(command);

  command = "mv ./aux.dat ./dataDogs.dat";
  system(command);

  command = "cp ./dataDogs.dat ./auxDataDogs.dat";
  system(command);

	return 0;
}

void *processRequest(void *arg) {
  int fd_I = (int *)arg;
//processRequest
processRequest: ;

    void *ptr = data;
    struct dogType *auxData= ptr;
    char optionToken[2];
    err = recv(fd_I, optionToken, 2, 0);
    char option = optionToken[0];

    switch (option) {
      case 'i':
        err = recv(fd_I, auxData, sizeof(struct dogType), 0);

        if (err == -1) {
          perror("error in recieve");
          exit(-1);
        }

        insertRegister(data, auxData, fd_I);
        goto processRequest;
      break;
      case 'r':
        readRegister(data, fd_I);
        goto processRequest;
      break;
      case 's':
        searchByName(data, fd_I);
        goto processRequest;
      break;
      case 'd':
        deleteRegister(data, fd_I);
        goto processRequest;
      break;
      case 'e':
      close(fd_I);
      break;
    }
}


int main() {

  // dogProgram data is initialized before configuring the server
  data = (struct dogType*) malloc(sizeof(struct dogType));

  if (data == NULL) {
    perror("error in malloc");
    exit(-1);
  }

  memset(lastOcurrence, -1, 100009*sizeof(lastOcurrence[0]));
  memset(firstOcurrence, -1, 100009*sizeof(firstOcurrence[0]));

  initData(data);

  // server
  fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd == -1){
		perror("error in socket");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero, 8);
	size =  sizeof(struct sockaddr_in);

	err = bind(fd, (struct sockaddr*)&server, size);
	if (err == -1) {
		perror("error in bind");
		exit(-1);
	}



	err = listen(fd, BACKLOG);
	if (err == -1) {
		perror("error in listen");
		exit(-1);
	}

  pthread_t tid;
  for (int i = 0; i < 32; i++) {
    clientSize = 0;

    int fd_client = accept(fd, (struct sockaddr*)&client_I, &clientSize);

    pthread_create(&tid, NULL, processRequest, (void *)fd_client);
  }

  file = fopen("dataDogs.dat", "a+");
  resizeFile(data, file);
  fclose(file);

  free(data);

	close(fd);
  return 0;
}
