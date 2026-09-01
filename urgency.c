#include <stdio.h>
#include <string.h>

#include "urgency.h"
#include "pet.h"


/* ============================================
   CALCULATE URGENCY
   ============================================ */

int calculateUrgency(int injury,
                     int illness,
                     int pain,
                     int age,
                     int dehydration,
                     int condition)
{
    int injuryScore;
    int illnessScore;
    int painScore;
    int ageScore;
    int dehydrationScore;
    int conditionScore;

    injuryScore = injury * 7.5;

    illnessScore = illness * 6.25;

    painScore = pain * 5;


    if (age <= 1)
    {
        ageScore = 10;
    }
    else if (age >= 8)
    {
        ageScore = 10;
    }
    else if (age >= 6)
    {
        ageScore = 7;
    }
    else
    {
        ageScore = 3;
    }


    dehydrationScore = dehydration * 5;

    conditionScore = condition * 5;


    return injuryScore +
           illnessScore +
           painScore +
           ageScore +
           dehydrationScore +
           conditionScore;
}


/* ============================================
   DISPLAY URGENCY LEVEL
   ============================================ */

void displayUrgencyLevel(int score)
{
    if (score >= 76)
    {
        printf("Priority : CRITICAL\n");
        printf("Action   : Immediate veterinary attention required.\n");
    }
    else if (score >= 51)
    {
        printf("Priority : HIGH\n");
        printf("Action   : Veterinary attention required soon.\n");
    }
    else if (score >= 26)
    {
        printf("Priority : MODERATE\n");
        printf("Action   : Monitoring and treatment required.\n");
    }
    else
    {
        printf("Priority : LOW\n");
        printf("Action   : Routine monitoring required.\n");
    }
}


/* ============================================
   GET PET AGE
   ============================================ */

int getPetAge(int searchID)
{
    for (int i = 0; i < petCount; i++)
    {
        if (pets[i].ID == searchID)
        {
            return pets[i].Age;
        }
    }

    return -1;
}


/* ============================================
   CALCULATE URGENCY FOR PET
   ============================================ */

void calculatePetUrgency(void)
{
    FILE *file;

    int searchID;
    int recordID;

    int injury;
    int illness;
    int pain;
    int dehydration;
    int condition;

    int age;
    int score;

    char treatment[100];
    char date[20];

    int found = 0;


    printf("\n========================================\n");
    printf("       MEDICAL URGENCY ASSESSMENT\n");
    printf("========================================\n");

    printf("Enter Pet ID: ");

    if (scanf("%d", &searchID) != 1)
    {
        printf("\nInvalid Pet ID.\n");

        while (getchar() != '\n')
        {
        }

        return;
    }


    age = getPetAge(searchID);

    if (age == -1)
    {
        printf("\nPet ID not found.\n");
        return;
    }


    file = fopen("medical_records.txt", "r");

    if (file == NULL)
    {
        printf("\nNo medical records found.\n");
        return;
    }


    while (fscanf(file,
                  " %d|%d|%d|%d|%d|%d|%99[^|]|%19[^\n]",
                  &recordID,
                  &injury,
                  &illness,
                  &pain,
                  &dehydration,
                  &condition,
                  treatment,
                  date) == 8)
    {
        if (recordID == searchID)
        {
            found = 1;

            score = calculateUrgency(
                injury,
                illness,
                pain,
                age,
                dehydration,
                condition
            );


            if (score > 100)
            {
                score = 100;
            }


            printf("\n========================================\n");
            printf("             URGENCY RESULT\n");
            printf("========================================\n");

            printf("Pet ID        : %d\n", searchID);
            printf("Age           : %d years\n", age);
            printf("Urgency Score : %d/100\n", score);

            printf("\n");

            displayUrgencyLevel(score);

            printf("========================================\n");

            break;
        }
    }


    fclose(file);


    if (!found)
    {
        printf("\nNo medical record found for this pet.\n");
    }
}


/* ============================================
   VIEW CRITICAL ANIMALS
   ============================================ */

void viewCriticalAnimals(void)
{
    FILE *file;

    int petID;

    char treatment[100];
    char date[20];

    int injury;
    int illness;
    int pain;
    int dehydration;
    int condition;

    int age;
    int score;

    int found = 0;


    file = fopen("medical_records.txt", "r");

    if (file == NULL)
    {
        printf("\nNo medical records found.\n");
        return;
    }


    printf("\n========================================\n");
    printf("           CRITICAL ANIMALS\n");
    printf("========================================\n");


    while (fscanf(file,
                  " %d|%d|%d|%d|%d|%d|%99[^|]|%19[^\n]",
                  &petID,
                  &injury,
                  &illness,
                  &pain,
                  &dehydration,
                  &condition,
                  treatment,
                  date) == 8)
    {
        age = getPetAge(petID);

        if (age == -1)
        {
            continue;
        }


        score = calculateUrgency(
            injury,
            illness,
            pain,
            age,
            dehydration,
            condition
        );


        if (score > 100)
        {
            score = 100;
        }


        if (score >= 76)
        {
            found = 1;

            printf("\nPet ID        : %d\n", petID);
            printf("Urgency Score : %d/100\n", score);
            printf("Priority      : CRITICAL\n");

            printf("----------------------------------------\n");
        }
    }


    fclose(file);


    if (!found)
    {
        printf("\nNo critical animals found.\n");
    }
}
