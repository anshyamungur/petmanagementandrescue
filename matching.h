#ifndef MATCHING_H
#define MATCHING_H

#include "adopter.h"
#include "pet.h"

#define MAX_MATCHES 100

typedef struct
{
    char petID[20];
    char name[50];
    char species[30];
    char breed[50];
    int age;
    char gender[20];
    float weight;
    char size[20];
    char location[50];
    char healthStatus[50];
    char availability[50];

    int score;

} PetMatch;


/* Matching functions */

int calculateCompatibility(
    const char species[],
    const char preferredSpecies[],
    int age,
    int minAge,
    int maxAge,
    const char size[],
    const char preferredSize[],
    const char gender[],
    char preferredGender
);

void showTop5Recommendations(struct Adopter adopter);

float calculateAdoptionFee(
    int age,
    const char healthStatus[]
);

void processAndSaveApplication(
    struct Adopter adopter,
    const char petID[]
);

#endif

