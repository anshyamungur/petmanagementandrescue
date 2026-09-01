#ifndef PET_H
#define PET_H

#define MAX_PETS 100

struct Pet
{
    int ID;
    char Name[50];
    char Species[30];
    char Breed[50];

    int Age;

    char Gender[20];

    float Weight;

    char Size[20];

    char Location[50];
    char HealthStatus[50];
    char Availability[50];
};

extern struct Pet pets[MAX_PETS];
extern int petCount;

void createPetFile(void);
void loadPets(void);
void savePets(void);

void registerPet(void);
void viewAllPets(void);
void updatePet(void);

#endif
