#ifndef ADOPTER_H
#define ADOPTER_H

#define MAX_ADOPTERS 100

struct Adopter
{
    /* Personal information */
    char Name[50];
    char Phone[20];
    char Gender;
    char Email[50];
    char Address[50];
    int ID;

    char Schedule[20];
    char Hometype[20];
    int Garden;
    int Experience;
    int Children;

    /* Adoption preferences */
    char PreferredSpecies[20];
    int minAge;
    int maxAge;
    char PreferredSize[20];
    char PreferredGender;
};

extern struct Adopter adopters[MAX_ADOPTERS];
extern int adopterCount;

void createAdopterFile(void);
void loadAdopters(void);
void saveAdopters(void);

void addAdopter(void);
void displayAdopter(void);
void searchAdopter(void);
void updateAdopter(void);
void deleteAdopter(void);

#endif
