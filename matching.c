#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matching.h"
#include "adopter.h"
#include "pet.h"


/* ============================================
   COMPARE STRINGS IGNORING CASE
   ============================================ */

int sameString(const char a[], const char b[])
{
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0')
    {
        char ca = a[i];
        char cb = b[i];

        if (ca >= 'A' && ca <= 'Z')
        {
            ca = ca + ('a' - 'A');
        }

        if (cb >= 'A' && cb <= 'Z')
        {
            cb = cb + ('a' - 'A');
        }

        if (ca != cb)
        {
            return 0;
        }

        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}


/* ============================================
   CALCULATE COMPATIBILITY
   ============================================ */

int calculateCompatibility(
    const char species[],
    const char preferredSpecies[],
    int age,
    int minAge,
    int maxAge,
    const char size[],
    const char preferredSize[],
    const char gender[],
    char preferredGender)
{
    int score = 0;


    /* Species = 30 points */

    if (sameString(species, preferredSpecies))
    {
        score += 30;
    }


    /* Age = 25 points */

    if (age >= minAge && age <= maxAge)
    {
        score += 25;
    }
    else if (age < minAge)
    {
        int difference = minAge - age;

        if (difference <= 2)
        {
            score += 15;
        }
        else
        {
            score += 5;
        }
    }
    else
    {
        int difference = age - maxAge;

        if (difference <= 2)
        {
            score += 15;
        }
        else
        {
            score += 5;
        }
    }


    /* Size = 20 points */

    if (sameString(size, preferredSize))
    {
        score += 20;
    }


    /* Gender = 15 points */

    if (preferredGender == 'A' ||
        preferredGender == 'a')
    {
        score += 15;
    }
    else if (
        (preferredGender == 'M' ||
         preferredGender == 'm') &&
        sameString(gender, "Male"))
    {
        score += 15;
    }
    else if (
        (preferredGender == 'F' ||
         preferredGender == 'f') &&
        sameString(gender, "Female"))
    {
        score += 15;
    }


    return score;
}


/* ============================================
   SHOW TOP 5 RECOMMENDATIONS
   ============================================ */

void showTop5Recommendations(struct Adopter adopter)
{
    PetMatch matches[MAX_MATCHES];

    int matchCount = 0;


    if (petCount == 0)
    {
        printf("\nNo pet records found.\n");
        return;
    }


    /* ----------------------------------------
       CREATE MATCH LIST
       ---------------------------------------- */

    for (int i = 0;
         i < petCount && matchCount < MAX_MATCHES;
         i++)
    {
        if (!sameString(pets[i].Availability,
                        "Available"))
        {
            continue;
        }


        /* Pet ID is an INTEGER */

        sprintf(matches[matchCount].petID,
                "%d",
                pets[i].ID);

        strcpy(matches[matchCount].name,
               pets[i].Name);

        strcpy(matches[matchCount].species,
               pets[i].Species);

        strcpy(matches[matchCount].breed,
               pets[i].Breed);

        matches[matchCount].age =
            pets[i].Age;

        strcpy(matches[matchCount].gender,
               pets[i].Gender);

        matches[matchCount].weight =
            pets[i].Weight;

        strcpy(matches[matchCount].size,
               pets[i].Size);

        strcpy(matches[matchCount].location,
               pets[i].Location);

        strcpy(matches[matchCount].healthStatus,
               pets[i].HealthStatus);

        strcpy(matches[matchCount].availability,
               pets[i].Availability);


        matches[matchCount].score =
            calculateCompatibility(
                pets[i].Species,
                adopter.PreferredSpecies,
                pets[i].Age,
                adopter.minAge,
                adopter.maxAge,
                pets[i].Size,
                adopter.PreferredSize,
                pets[i].Gender,
                adopter.PreferredGender
            );


        /* Healthy pets receive extra points */

        if (sameString(pets[i].HealthStatus,
                       "Healthy"))
        {
            matches[matchCount].score += 10;
        }


        if (matches[matchCount].score > 100)
        {
            matches[matchCount].score = 100;
        }


        matchCount++;
    }


    if (matchCount == 0)
    {
        printf("\nNo available pets found.\n");
        return;
    }


    /* ----------------------------------------
       SORT BY SCORE
       ---------------------------------------- */

    for (int i = 0;
         i < matchCount - 1;
         i++)
    {
        for (int j = 0;
             j < matchCount - i - 1;
             j++)
        {
            if (matches[j].score <
                matches[j + 1].score)
            {
                PetMatch temp;

                temp = matches[j];

                matches[j] = matches[j + 1];

                matches[j + 1] = temp;
            }
        }
    }


    /* ----------------------------------------
       DISPLAY TOP 5
       ---------------------------------------- */

    int limit;

    if (matchCount < 5)
    {
        limit = matchCount;
    }
    else
    {
        limit = 5;
    }


    printf("\n======================================================\n");
    printf("              TOP 5 PET RECOMMENDATIONS\n");
    printf("              FOR %s (ID: %d)\n",
           adopter.Name,
           adopter.ID);
    printf("======================================================\n");


    for (int i = 0; i < limit; i++)
    {
        char label[30];


        if (matches[i].score >= 85)
        {
            strcpy(label, "Excellent Match");
        }
        else if (matches[i].score >= 70)
        {
            strcpy(label, "Good Match");
        }
        else if (matches[i].score >= 50)
        {
            strcpy(label, "Moderate Match");
        }
        else
        {
            strcpy(label, "Low Match");
        }


        printf("\n%d. %s\n",
               i + 1,
               matches[i].name);

        printf("   Pet ID       : %s\n",
               matches[i].petID);

        printf("   Species      : %s\n",
               matches[i].species);

        printf("   Breed        : %s\n",
               matches[i].breed);

        printf("   Age          : %d years\n",
               matches[i].age);

        printf("   Gender       : %s\n",
               matches[i].gender);

        printf("   Size         : %s\n",
               matches[i].size);

        printf("   Health       : %s\n",
               matches[i].healthStatus);

        printf("   Location     : %s\n",
               matches[i].location);

        printf("   Compatibility: %d%%\n",
               matches[i].score);

        printf("   Match Level  : %s\n",
               label);

        printf("----------------------------------------\n");
    }


    printf("======================================================\n");
}


/* ============================================
   CALCULATE ADOPTION FEE
   ============================================ */

float calculateAdoptionFee(
    int age,
    const char healthStatus[])
{
    float baseFee = 150.0f;


    /* Senior pets are free */

    if (age >= 8)
    {
        return 0.0f;
    }


    /* Serious/critical pets are free */

    if (sameString(healthStatus, "Critical") ||
        sameString(healthStatus, "Serious"))
    {
        return 0.0f;
    }


    return baseFee;
}


/* ============================================
   PROCESS ADOPTION APPLICATION
   ============================================ */

void processAndSaveApplication(
    struct Adopter adopter,
    const char petID[])
{
    int found = -1;

    int searchID;


    /* Convert entered Pet ID to integer */

    searchID = atoi(petID);


    /* ----------------------------------------
       FIND PET IN LOADED ARRAY
       ---------------------------------------- */

    for (int i = 0; i < petCount; i++)
    {
        if (pets[i].ID == searchID)
        {
            found = i;
            break;
        }
    }


    if (found == -1)
    {
        printf("\nPet ID %s was not found.\n",
               petID);
        return;
    }


    /* ----------------------------------------
       CHECK AVAILABILITY
       ---------------------------------------- */

    if (!sameString(pets[found].Availability,
                    "Available"))
    {
        printf("\nThis pet is currently not available for adoption.\n");
        return;
    }


    /* ----------------------------------------
       CALCULATE SCORE
       ---------------------------------------- */

    int score =
        calculateCompatibility(
            pets[found].Species,
            adopter.PreferredSpecies,
            pets[found].Age,
            adopter.minAge,
            adopter.maxAge,
            pets[found].Size,
            adopter.PreferredSize,
            pets[found].Gender,
            adopter.PreferredGender
        );


    if (sameString(pets[found].HealthStatus,
                   "Healthy"))
    {
        score += 10;
    }


    if (score > 100)
    {
        score = 100;
    }


    /* ----------------------------------------
       CALCULATE FEE
       ---------------------------------------- */

    float fee =
        calculateAdoptionFee(
            pets[found].Age,
            pets[found].HealthStatus
        );


    /* ----------------------------------------
       APPLICATION STATUS
       ---------------------------------------- */

    char status[20];


    if (score >= 75)
    {
        strcpy(status, "Approved");
    }
    else if (score >= 50)
    {
        strcpy(status, "Waitlist");
    }
    else
    {
        strcpy(status, "Rejected");
    }


    /* ----------------------------------------
       DISPLAY RESULT
       ---------------------------------------- */

    printf("\n========================================\n");
    printf("        ADOPTION APPLICATION\n");
    printf("========================================\n");

    printf("Adopter        : %s\n",
           adopter.Name);

    printf("Adopter ID     : %d\n",
           adopter.ID);

    printf("Pet            : %s\n",
           pets[found].Name);

    printf("Pet ID         : %d\n",
           pets[found].ID);

    printf("Species        : %s\n",
           pets[found].Species);

    printf("Breed          : %s\n",
           pets[found].Breed);

    printf("Compatibility  : %d%%\n",
           score);

    printf("Adoption Fee   : Rs %.2f\n",
           fee);

    printf("Decision       : %s\n",
           status);

    printf("========================================\n");


    /* ----------------------------------------
       SAVE APPLICATION
       ---------------------------------------- */

    FILE *file =
        fopen("adoptions.txt", "a");


    if (file == NULL)
    {
        printf("\nError: Could not open adoptions.txt\n");
        return;
    }


    fprintf(file,
            "%d|%d|%s|%s|%d|%.2f|%s\n",
            adopter.ID,
            pets[found].ID,
            pets[found].Name,
            pets[found].Species,
            score,
            fee,
            status);


    fclose(file);


    /* ----------------------------------------
       IF APPROVED, UPDATE PET STATUS
       ---------------------------------------- */

    if (strcmp(status, "Approved") == 0)
    {
        strcpy(pets[found].Availability,
               "Adopted");

        savePets();

        printf("\nPet has been marked as Adopted.\n");
    }


    printf("\nAdoption application saved successfully.\n");
}


/* ============================================



