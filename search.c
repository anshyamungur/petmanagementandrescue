#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "search.h"
#include "pet.h"


/* ============================================
   SEARCH PET BY SPECIES
   ============================================ */

void searchPet(void)
{
    char searchSpecies[30];
    int found = 0;

    if (petCount == 0)
    {
        printf("\nNo pet records found.\n");
        return;
    }

    printf("\n========================================\n");
    printf("              SEARCH PET\n");
    printf("========================================\n");

    printf("Enter species (Cat/Dog): ");
    scanf(" %29s", searchSpecies);

    /* Convert search input to lowercase */
    for (int i = 0; searchSpecies[i] != '\0'; i++)
    {
        searchSpecies[i] = tolower(searchSpecies[i]);
    }

    for (int i = 0; i < petCount; i++)
    {
        char species[30];

        /* Copy species and convert it to lowercase */
        strcpy(species, pets[i].Species);

        for (int j = 0; species[j] != '\0'; j++)
        {
            species[j] = tolower(species[j]);
        }

        if (strcmp(species, searchSpecies) == 0)
        {
            found = 1;

            printf("\nPet ID       : %d\n", pets[i].ID);
            printf("Name         : %s\n", pets[i].Name);
            printf("Species      : %s\n", pets[i].Species);
            printf("Breed        : %s\n", pets[i].Breed);
            printf("Age          : %d years\n", pets[i].Age);
            printf("Gender       : %s\n", pets[i].Gender);
            printf("Size         : %s\n", pets[i].Size);
            printf("Health       : %s\n", pets[i].HealthStatus);
            printf("Availability : %s\n", pets[i].Availability);

            printf("----------------------------------------\n");
        }
    }

    if (!found)
    {
        printf("\nNo %s found.\n", searchSpecies);
    }
}
