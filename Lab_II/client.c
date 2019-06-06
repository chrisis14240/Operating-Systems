#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 3535

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

// socket client variables
size_t size;
char buffer[32];
int err, fd, nRegisters;
struct sockaddr_in server;

struct dogType *data;

int insertRegister(void *ptr) {
  struct dogType data, *auxData;
  auxData = (struct dogType*)ptr;

  printf("Nombre: ");
  scanf(" %s", data.name);

  printf("Tipo: ");
  scanf(" %s", data.type);

  printf("Raza: ");
  scanf(" %s", data.breed);

  printf("Sexo: ");
  scanf(" %c", &data.gender);

  printf("Edad: ");
  scanf(" %d", &data.age);

  printf("Estatura: ");
  scanf(" %d", &data.height);

  printf("Peso: ");
  scanf(" %f", &data.weight);


  data.isDeleted = false;
  data.previousRegister = -1;

  err = send(fd, "in", 2, 0);
  err = send(fd, &data, sizeof(struct dogType), 0);

  if (err == -1) {
    perror("error in send");
    exit(-1);
  }

  printf("%s\n", data.name);
  printf("%s\n", data.type);
  printf("%s", data.breed);
  printf("%d\n", data.age);
  printf("%d\n", data.height);
  printf("%lf\n", data.weight);

  return 0;
}

int readRegister(void *ptr) {
  struct dogType aux;

  int idx;
  send(fd, "re", 2, 0);
  recv(fd, &nRegisters, sizeof(int), 0);
  printf("Actualmente hay %d registros, por favor ingrese el numero de registro que desea ver.\n", nRegisters);

  scanf("%d", &idx);

  while (idx < 1 || idx > nRegisters) {
      printf("Numero de registro no valido, el numero debe estar entre 1 y  %d (inclusivamente) \n", nRegisters);
      scanf("%d", &idx);
  }

  // printf("%d\n", idx);
  err = send(fd, &idx, sizeof(int), 0);
  if (err == -1) {
    perror("error while sending");
    exit(-1);
  }

  err =  recv(fd, &aux, sizeof(struct dogType), 0);
  if (err == -1) {
    perror("error while reciving");
    exit(-1);
  }

  printf("-------------------\n" );
  printf(" name: %s \n", aux.name);
  printf(" type: %s \n", aux.type);
  printf(" breed: %s", aux.breed);
  printf(" age: %d \n", aux.age);
  printf(" height: %d \n", aux.height);
  printf(" weight: %f \n", aux.weight);
  printf(" gender: %c \n", aux.gender);

  char command[64] = "nano ./clinicHistories/clinic_history_";
  char clinicID[10];

  sprintf(clinicID, "%d", aux.clinicID);
  strcat(command, clinicID);
  strcat(command, ".txt");

  char option[2] = "";
  printf("¿Desea ver la historia clinica de %s? Si/No \n", aux.name);
  scanf(" %s", option);

  option[0] = tolower(option[0]);
  option[1] = tolower(option[1]);

  if (strcmp(option, "si") == 0) {
    system(command);
  }

	return 0;
}

int searchByName(void *ptr) {
  struct dogType aux;

  char ok[4];
  char name[32];

  for (int i = 0; i < 32; i++) {
    name[i] = 0;
  }

  send(fd, "se", 2, 0);
  printf("Por favor ingrese el nombre del registro que desea buscar:\n");
  scanf(" %s", name);
  send(fd, name, 32, 0);

  recv(fd, ok, 4, 0);

  if (ok[0] == 'N') {
    printf("No hay ningun registro con el nombre recibido\n");
    printf("%s\n", ok);
    return 0;
  } else {
    recv(fd, &aux, sizeof(struct dogType), 0);
    while (aux.previousRegister != -2) {
      printf("-------------------\n" );
      printf(" name: %s \n", aux.name);
      printf(" type: %s \n", aux.type);
      printf(" breed: %s", aux.breed);
      printf(" age: %d \n", aux.age);
      printf(" height: %d \n", aux.height);
      printf(" weight: %f \n", aux.weight);
      printf(" gender: %c \n", aux.gender);

      recv(fd, &aux, sizeof(struct dogType), 0);
    }
  }

  return 0;
}

int deleteRegister(void *ptr) {
  struct dogType aux;

  int idx;
  send(fd, "de", 2, 0);
  recv(fd, &nRegisters, sizeof(int), 0);
  printf("Actualmente hay %d registros, por favor ingrese el numero de registro que desea borrar.\n", nRegisters);
  scanf("%d", &idx);

  while (idx < 1 || idx > nRegisters) {
      printf("Numero de registro no valido, el numero debe estar entre 1 y  %d (inclusivamente) \n", nRegisters);
      scanf("%d", &idx);
  }

  err = send(fd, &idx, sizeof(int), 0);
  if (err == -1) {
    perror("error while sending");
    exit(-1);
  }

  printf("El siguiente registro has sido removido: \n");
  recv(fd, &aux, sizeof(struct dogType), 0);

  printf("-----------------------\n" );
  printf(" name: %s \n", aux.name);
  printf(" type: %s \n", aux.type);
  printf(" breed: %s", aux.breed);
  printf(" age: %d \n", aux.age);
  printf(" height: %d \n", aux.height);
  printf(" weight: %f \n", aux.weight);
  printf(" gender: %c \n", aux.gender);

  return 0;
}

int menu() {
  //menú
  menu:
      // system("clear");
      printf("----------------------------------- MENU ----------------------------------------");
      printf("\nPara realizar una accion, digite el numero de una las opciones del menú: \n");
      printf("1.Ingresar registro \n");
      printf("2.Ver registro \n");
      printf("3.Borrar registro \n");
      printf("4.Buscar registro por nombre \n");
      printf("5.Salir \n");

      int option, continueOption;
      char goback[2]="", restart[2]="";
      scanf("%d", &option);
      switch (option) {
      case 1:
      backToMenu:
          printf("Ha escogido la opcion 'Ingresar registro' \n");
          printf("Si desea volver al menú ingrese '-1', si desea continuar ingrese '0':\n");
          scanf(" %d", &continueOption);
          if (continueOption == -1) {
              system("clear");
              goto menu;
          } else if(continueOption!=0){
              system("clear");
              printf("Opcion no valida \n");
              goto backToMenu;
          }
          addAnotherPet:
          printf("Ingrese los datos de la mascota: \n");
          insertRegister(data);
          invalidInput_AddPet:
              printf("¿Desea ingresar otro registro? (Si/No) \n");
              scanf(" %s", restart);

              // recieved input turned into lowercase letters to better handle
              restart[0] = tolower(restart[0]);
              restart[1] = tolower(restart[1]);

              if (strcmp(restart, "si") == 0) {
                  system("clear");
                  goto addAnotherPet;
              }
              else if (strcmp(restart, "no") == 0) {
                  backToMenu_fromAddRegister:
                      printf("¿Desea volver al menú principal?  (Si/No) \n");
                      scanf(" %s", goback);

                      goback[0] = tolower(goback[0]);
                      goback[1] = tolower(goback[1]);

                      if (strcmp(goback, "si") == 0) {
                          system("clear");
                          goto menu;
                      }
                      else if (strcmp(goback, "no") == 0) {
                          break;
                      }
                      else {
                          system("clear");
                          printf("Opcion no valida, intente de nuevo \n");
                          goto backToMenu_fromAddRegister;
                      }
              }
              else {
                  system("clear");
                  printf("Opcion no valida \n");
                  goto invalidInput_AddPet;
              }

          break;
      case 2:
      backToMenu_fromSeeRegister:
          printf("Ha escogido la opcion 'Ver registro' \n");
          printf("Si desea volver al menú ingrese '-1', si desea continuar ingrese '0':\n");
          scanf(" %d", &continueOption);

          if (continueOption == -1) {
              system("clear");
              goto menu;
          } else if(continueOption!=0){
              system("clear");
              printf("Opcion no valida \n");
              goto backToMenu_fromSeeRegister;
          }

          seeAnotherRegister:
          readRegister(data);

          invalidInput_SeeRegister:
          printf("¿Desea Ver otro registro? (Si/No) \n");
          scanf(" %s", restart);

          restart[0] = tolower(restart[0]);
          restart[1] = tolower(restart[1]);

          if (strcmp(restart, "si") == 0) {
              system("clear");
              goto seeAnotherRegister;
          }
          else if (strcmp(restart, "no") == 0) {
              backToMenu_fromSeeRegister_I:
              printf("¿Desea volver al menú principal? (Si/No) \n");
              scanf(" %s", goback);

              goback[0] = tolower(goback[0]);
              goback[1] = tolower(goback[1]);

              if (strcmp(goback, "si") == 0) {
                  system("clear");
                  goto menu;
              }
              else if (strcmp(goback, "no") == 0) {
                  break;
              }
              else {
                  system("clear");
                  printf("Opcion no valida \n");
                  goto backToMenu_fromSeeRegister_I;
              }
          }
          else {
              system("clear");
              printf("Opcion no valida \n");
              goto invalidInput_SeeRegister;
          }
          break;
      case 3:
      backToMenu_fromDeleteRegister:
          printf("Ha escogido la opcion 'Borrar registro' \n");
          printf("Si desea volver al menú ingrese '-1', si desea continuar ingrese '0':\n");

          scanf(" %d", &continueOption);
          if (continueOption == -1) {
              system("clear");
              goto menu;
          } else if(continueOption!=0){
              system("clear");
              printf("Opcion no valida \n");
              goto backToMenu_fromDeleteRegister;
          }

          deleteAnotherRegister:
          deleteRegister(data);

          invalidInput_DeleteRegister:
          printf("¿Desea borrar otro registro?  (Si/No)\n");
          scanf(" %s", restart);

          restart[0] = tolower(restart[0]);
          restart[1] = tolower(restart[1]);

          if (strcmp(restart, "si") == 0) {
              system("clear");
              goto deleteAnotherRegister;
          }
          else if (strcmp(restart, "no") == 0) {

              backToMenu_fromDeleteRegister_I:
              printf("¿Desea volver al menú principal? (Si/No) \n");
              scanf(" %s", goback);

              goback[0] = tolower(goback[0]);
              goback[1] = tolower(goback[1]);


              if (strcmp(goback, "si") == 0) {
                  system("clear");
                  goto menu;
              }
              else if (strcmp(goback, "no") == 0) {
                  goto closeProgram;
              }
              else {
                  system("clear");
                  printf("Opcion no valida \n");
              goto backToMenu_fromDeleteRegister_I;
              }
          }
          else {
              system("clear");
              printf("Opcion no valida \n");
              goto invalidInput_DeleteRegister;
          }

          break;
      case 4:
      backToMenu_fromSearchRegister:
          printf("Ha escogido la opcion 'Buscar registro' \n");
          printf("Si desea volver al menú ingrese '-1', si desea continuar ingrese '0':\n");

          scanf(" %d", &continueOption);
          if (continueOption == -1) {
              system("clear");
              goto menu;
          } else if(continueOption!=0 ){
              system("clear");
              printf("opcion no valida \n");
              goto backToMenu_fromSearchRegister;
          }

          searchAnotherRegister:
          searchByName(data);

          invalidInput_SearchRegister:
          printf("¿Desea Buscar otro registro? (Si/No) \n");
          scanf(" %s", restart);

          restart[0] = tolower(restart[0]);
          restart[1] = tolower(restart[1]);

          if (strcmp(restart, "si") == 0) {
              system("clear");
              goto searchAnotherRegister;
          }
          else if (strcmp(restart, "no") == 0) {
              backToMenu_fromSearchRegister_I:

              printf("¿Desea volver al menú principal? (Si/No)\n");
              scanf(" %s", goback);

              goback[0] = tolower(goback[0]);
              goback[1] = tolower(goback[1]);

              if (strcmp(goback, "si") == 0) {
                  system("clear");
                  goto menu;
              }
              else if (strcmp(goback, "no") == 0) {
                 goto closeProgram;
              }
              else {
                  system("clear");
                  printf("opcion no valida \n");
              goto backToMenu_fromSearchRegister_I;
              }
          }
          else {
              system("clear");
              printf("Opcion no valida \n");
              goto invalidInput_SearchRegister;
          }
          break;
      case 5:
          closeProgram:
          send(fd, "ex", 2, 0);
          break;
      default:
          system("clear");
          printf("Opcion no valida \n");
          goto menu;
          break;
      }
}

int main() {
  fd = socket(AF_INET, SOCK_STREAM,0);

	if (fd == -1) {
		perror("error in socket");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	bzero(server.sin_zero, 8);

	size = sizeof(struct sockaddr_in);

	err = connect(fd, (struct sockaddr*)&server, size);
	if (err == -1) {
		perror("error in connect");
		exit(-1);
	}

  menu();
	// printf("%s\n", buffer);
  return 0;
}
