#include <wait.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

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

int nRegister;

// Arrays to load the data use in the registers generation
char types[6][32];
char names[1000][32];

typedef char (*baseData_p)[32];
baseData_p baseData[2] = {names, types};

char cat_breeds[40][16];
char dog_breeds[107][16];
char bird_breeds[27][16];
char fish_breeds[107][16];
char rabbit_breeds[68][16];
char turtle_breeds[46][16];

typedef char (*breedsData_p)[16];
breedsData_p breedsData[6] = {dog_breeds, cat_breeds, bird_breeds, fish_breeds, rabbit_breeds, turtle_breeds};

// Filenames
const char *namesFile = "./data/names.txt";
const char *typesFile = "./data/types.txt";
const char *dogsFile = "./data/dog_breeds.txt";
const char *catsFile = "./data/cat_breeds.txt";
const char *birdsFile = "./data/bird_breeds.txt";
const char *fishesFile = "./data/fish_breeds.txt";
const char *rabbitsFile = "./data/rabbit_breeds.txt";
const char *turtlesFile = "./data/turtle_breeds.txt";

int loadData(const char *filename, int dataType, int fileIdx) {
    char data[32];
    int idx = 0;
    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL) {
      perror("error while opening the file");
      exit(-1);
    }

    /* Checks the type of data, if data Type == 0 then the data to be loaded is either names or types.
     If data Type == 1 then the data to be loaded is a breed name from the different breed files.
    */
    if (dataType == 0) {
      while (fgets(data, 32, (FILE *)file)) {
          strcpy(baseData[fileIdx][idx], data);
          idx++;
      }

      return 0;
    }

    while (fgets(data, 32, (FILE *)file)) {
        strcpy(breedsData[fileIdx][idx], data);
        idx++;
    }

    fclose(file);

    return 0;
}

int writeRegister(FILE *file) {
  struct dogType data;

  int idx = 0;
  float weight;
  int name_idx, type_idx, breed_idx, age, height;

  char gender;
  char name[32];
  char breed[161][16];

  name_idx = rand() % 1000;

  //name declaration
  while (names[name_idx][idx] != ',') {
      name[idx] = names[name_idx][idx];
      idx++;
  }

  idx++;

  //complete the rest of the name with empty characters
  for (size_t i = idx; i < 32; i++) {
    name[i] = 0;
  }

  //gender declaration
  if (name[idx] == '1') {
      gender = 'M';
  }
  else {
      gender = 'F';
  }


  type_idx = rand() % 6;
  age = rand() % (20 - 1 + 1) + 1;
  height = rand() % (50 - 10 + 1) + 10;
  weight = drand48() * (60.0 - 20.0) + 20.0;

  switch (type_idx) {
  case 0:
      breed_idx = rand() % 107;
      strcpy(data.breed, dog_breeds[breed_idx]);
      break;
  case 1:
      breed_idx = rand() % 41;
      strcpy(data.breed, cat_breeds[breed_idx]);
      break;
  case 2:
      breed_idx = rand() % 47;
      strcpy(data.breed, turtle_breeds[breed_idx]);
      break;
  case 3:
      breed_idx = rand() % 107;
      strcpy(data.breed, fish_breeds[breed_idx]);
      break;
  case 4:
      breed_idx = rand() % 69;
      strcpy(data.breed, rabbit_breeds[breed_idx]);
      break;
  case 5:
      breed_idx = rand() % 28;
      strcpy(data.breed, turtle_breeds[breed_idx]);
  default:
      break;

      return 0;
  }

  strcpy(data.name, name);
  strcpy(data.type, types[type_idx]);
  strtok(data.type, "\n");
  data.clinicID = nRegister;
  data.age = age;
  data.height = height;
  data.weight = weight;
  data.gender = gender;
  data.isDeleted = false;
  data.previousRegister = -1;

  nRegister++;
  int err = fwrite(&data, sizeof(struct dogType), 1, file);
  if (err == 0) {
      perror("error in write");
      exit(-1);
  }

  return 0;
}

int main() {
    // load the data from file into the program's memory to then use it in the registers' generation
    loadData(namesFile, 0, 0);
    loadData(typesFile, 0, 1);
    loadData(dogsFile, 1, 0);
    loadData(catsFile, 1, 1);
    loadData(birdsFile, 1, 2);
    loadData(fishesFile, 1, 3);
    loadData(rabbitsFile, 1, 4);
    loadData(turtlesFile, 1, 5);

    FILE *file = fopen("auxDataDogs.dat", "w+");
       if (file == NULL) {
           perror("error while opening file");
           exit(-1);
       }

       for (int idx = 0; idx < 1e7; idx++) {
         writeRegister(file);
       }

}
