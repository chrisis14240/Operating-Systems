#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

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


FILE *file;
struct dogType *data;
int  firstOcurrence[100009], lastOcurrence[100009], maxClinicID, nRegisters, nDeletedRegisters;

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
	return 0;
}

int readRegister(void *ptr) {
  struct dogType *aux;
  aux = ptr;
  file = fopen("dataDogs.dat", "a+");

  int idx;
  printf("Actualmente hay %d registros, por favor ingrese el numero de registro que desea ver.\n", nRegisters);
  scanf("%d", &idx);

  while (idx < 1 || idx > nRegisters) {
      printf("Numero de registro no valido, el numero debe estar entre 1 y  %d (inclusivamente) \n", nRegisters);
      scanf("%d", &idx);
  }

  int counter = 0;

  while (counter != idx) {
      int err = fread(aux, sizeof(struct dogType), 1, file);

      if (err == 0) {
        perror("error while reading");
        exit(-1);
      }

      if (!aux->isDeleted)
        counter++;
  }

  printf("-------------------\n" );
	printf(" name: %s \n", aux->name);
  printf(" type: %s \n", aux->type);
  printf(" breed: %s", aux->breed);
	printf(" age: %d \n", aux->age);
	printf(" height: %d \n", aux->height);
	printf(" weight: %f \n", aux->weight);
  printf(" gender: %c \n", aux->gender);


  char command[64] = "nano ./clinicHistories/clinic_history_";
  char clinicID[10];

  sprintf(clinicID, "%d", aux->clinicID);
  strcat(command, clinicID);
  strcat(command, ".txt");

  char option[2] = "";
  printf("¿Desea ver la historia clinica de %s? Si/No \n", aux->name);
  scanf(" %s", option);

  option[0] = tolower(option[0]);
  option[1] = tolower(option[1]);

  if (strcmp(option, "si") == 0) {
    system(command);
  }

  fclose(file);

	return 0;
}

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


  data.clinicID = maxClinicID+1;
  data.isDeleted = false;
  data.previousRegister = -1;
  long hash = hashIdx(data.name);

  file = fopen("dataDogs.dat", "rb+");
  if (lastOcurrence[hash] != 0) {
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

  int err = fwrite(&data, sizeof(struct dogType), 1, file);
  if (err == 0) {
      perror("error in write");
      exit(-1);
  }

  fclose(file);
  return 0;
}

int searchByName(void *ptr) {
  struct dogType *aux;
  aux = ptr;
  file = fopen("dataDogs.dat", "a+");


  char name[32];
  for (int i = 0; i < 32; i++) {
    name[i] = 0;
  }

  printf("Por favor ingrese el nombre del registro que desea buscar:\n");
  scanf(" %s", name);
  long hash = hashIdx(name);

  bool flag = false;

  if (lastOcurrence[hash] == -1) {
    printf("No hay ningun registro con el nombre recibido\n");
    fclose(file);
    return 0;
  }

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
          printf("-------------------\n" );
          printf(" name: %s \n", aux->name);
          printf(" type: %s \n", aux->type);
          printf(" breed: %s", aux->breed);
          printf(" age: %d \n", aux->age);
          printf(" height: %d \n", aux->height);
          printf(" weight: %f \n", aux->weight);
          printf(" gender: %c \n", aux->gender);
      }


      if (aux->previousRegister == -1)
        break;

      fseek(file, (aux->previousRegister)*sizeof(struct dogType), SEEK_SET);
  } while(aux->previousRegister != -1);

  fclose(file);
  return 0;
}

int deleteRegister(void *ptr) {
  int idx, newIdx = 0, counter = 0;
  struct dogType *aux;
  aux = ptr;

  file = fopen("dataDogs.dat", "rb+");

  printf("Actualmente hay %d registros, por favor ingrese el numero de registro que desea eliminar.\n", nRegisters);

  printf("¿Cual es el numero de registro que desea eliminar?\n");
  scanf(" %d", &idx);

  while (idx < 1 || idx > nRegisters) {
      printf("Numero de registro no valido, debe estar entre 1 y  %d (inclusivamente)\n", nRegisters);
      scanf(" %d", &idx);
  }

  int err = -1;

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

  printf("El siguiente registro has sido removido: \n");

  printf("-----------------------\n" );
  printf(" name: %s \n", aux->name);
  printf(" type: %s \n", aux->type);
  printf(" breed: %s", aux->breed);
  printf(" age: %d \n", aux->age);
  printf(" height: %d \n", aux->height);
  printf(" weight: %f \n", aux->weight);
  printf(" gender: %c \n", aux->gender);

  printf("-----------------------\n" );

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
          break;
      default:
          system("clear");
          printf("Opcion no valida \n");
          goto menu;
          break;
      }
}

int resizeFile(void *ptr, FILE *file) {

  struct dogType *data;
  data = (struct dogType*)ptr;

  int idx = 0, err;
  FILE *auxFile = fopen("aux.dat", "a+");
  fseek(file, 0, SEEK_SET);

  while (fread(data, sizeof(struct dogType), 1, file)) {
    if (!data->isDeleted) {
      err = fwrite(data, sizeof(struct dogType), 1, auxFile);
    }
    idx++;
  }

  char *command = "cp ./dataDogs.dat ./auxDataDogs.dat";
  system(command);

  command = "rm ./dataDogs.dat";
  system(command);
  command = "mv ./aux.dat ./dataDogs.dat";
  system(command);

	return 0;
}

int main() {

  data = (struct dogType*) malloc(sizeof(struct dogType));

  if (data == NULL) {
    perror("error in malloc");
    exit(-1);
  }

  memset(lastOcurrence, -1, 100009*sizeof(lastOcurrence[0]));
  memset(firstOcurrence, -1, 100009*sizeof(firstOcurrence[0]));

  initData(data);
  menu();


  file = fopen("dataDogs.dat", "a+");
  if (nDeletedRegisters != 0)
    resizeFile(data, file);
  fclose(file);

  free(data);

  printf("Muchas gracias por usar nuestro programa \n");

  return 0;
}
