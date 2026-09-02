#include <stdio.h>
#include <string.h>
#include "pet.h"
#include "search.h"
#include "medical.h"
#include "urgency.h"


struct Pet pets[MAX_PETS];
int petCount = 0;


/* ============================================
   CREATE PET FILE
   ============================================ */

void createPetFile(void)
{
    FILE *file = fopen("pets.txt", "r");

    if (file != NULL)
    {
        fclose(file);
        return;
    }

    file = fopen("pets.txt", "w");

    if (file == NULL)
    {
        printf("Error: Could not create pets.txt\n");
        return;
    }

    fprintf(file,
            "1|Buddy|Dog|Labrador|3|Male|25.50|Large|Port Louis|Healthy|Available\n");

    fprintf(file,
            "2|Luna|Dog|MixedBreed|4|Male|18.00|Medium|Port Louis|Healthy|Available\n");

    fprintf(file,
            "3|Mimi|Cat|DomesticShortHair|2|Female|4.00|Small|Curepipe|Healthy|Available\n");

    fprintf(file,
            "4|Rex|Dog|LabradorMix|6|Male|24.50|Large|Quatre Bornes|Injured|UnderTreatment\n");

    fprintf(file,
            "5|Lily|Cat|MixedBreed|4|Female|5.00|Medium|Curepipe|Healthy|Available\n");

    fclose(file);

    printf("pets.txt created successfully.\n");
}


/* ============================================
   LOAD PETS
   ============================================ */

void loadPets(void)
{
    FILE *file = fopen("pets.txt", "r");

    if (file == NULL)
    {
        printf("No pet file found. Starting with empty records.\n");
        petCount = 0;
        return;
    }

    petCount = 0;

    while (petCount < MAX_PETS &&
           fscanf(file,
                  " %d|%49[^|]|%29[^|]|%49[^|]|%d|%19[^|]|%f|%19[^|]|%49[^|]|%49[^|]|%49[^\n]",
                  &pets[petCount].ID,
                  pets[petCount].Name,
                  pets[petCount].Species,
                  pets[petCount].Breed,
                  &pets[petCount].Age,
                  pets[petCount].Gender,
                  &pets[petCount].Weight,
                  pets[petCount].Size,
                  pets[petCount].Location,
                  pets[petCount].HealthStatus,
                  pets[petCount].Availability) == 11)
    {
        petCount++;
    }

    fclose(file);

    printf("%d pet(s) loaded successfully.\n", petCount);
}


/* ============================================
   SAVE PETS
   ============================================ */

void savePets(void)
{
    FILE *file = fopen("pets.txt", "w");

    if (file == NULL)
    {
        printf("Error: Could not open pets.txt for writing.\n");
        return;
    }

    for (int i = 0; i < petCount; i++)
    {
        fprintf(file,
                "%d|%s|%s|%s|%d|%s|%.2f|%s|%s|%s|%s\n",
                pets[i].ID,
                pets[i].Name,
                pets[i].Species,
                pets[i].Breed,
                pets[i].Age,
                pets[i].Gender,
                pets[i].Weight,
                pets[i].Size,
                pets[i].Location,
                pets[i].HealthStatus,
                pets[i].Availability);
    }

    fclose(file);

    printf("Pet records saved successfully.\n");
}


/* ============================================
   REGISTER PET
   ============================================ */

void registerPet(void)
{
    if (petCount >= MAX_PETS)
    {
        printf("Maximum number of pets reached.\n");
        return;
    }

    struct Pet *pet = &pets[petCount];

    printf("\n========================================\n");
    printf("           REGISTER NEW PET\n");
    printf("========================================\n");

    /* Pet ID */

    printf("Pet ID: ");

    if (scanf("%d", &pet->ID) != 1)
    {
        while (getchar() != '\n')
        {
        }

        printf("Invalid Pet ID.\n");
        return;
    }


    /* Name */

    printf("Name: ");
    scanf(" %49[^\n]", pet->Name);


    /* Species */

    printf("Species (Cat/Dog): ");
    scanf(" %29s", pet->Species);


    /* Breed */

    printf("Breed: ");
    scanf(" %49[^\n]", pet->Breed);


    /* Age */

    do
    {
        printf("Age: ");

        if (scanf("%d", &pet->Age) != 1)
        {
            while (getchar() != '\n')
            {
            }

            pet->Age = -1;

            printf("Invalid age.\n");
        }

        if (pet->Age < 0)
        {
            printf("Age cannot be negative.\n");
        }

    } while (pet->Age < 0);


    /* Gender */

    do
    {
        printf("Gender:\n");
        printf("1. Male\n");
        printf("2. Female\n");
        printf("Enter choice: ");

        int genderChoice;

        if (scanf("%d", &genderChoice) != 1)
        {
            while (getchar() != '\n')
            {
            }

            genderChoice = 0;
        }

        if (genderChoice == 1)
        {
            strcpy(pet->Gender, "Male");
        }
        else if (genderChoice == 2)
        {
            strcpy(pet->Gender, "Female");
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }

    } while (strcmp(pet->Gender, "Male") != 0 &&
             strcmp(pet->Gender, "Female") != 0);


    /* Weight */

    do
    {
        printf("Weight (kg): ");

        if (scanf("%f", &pet->Weight) != 1)
        {
            while (getchar() != '\n')
            {
            }

            pet->Weight = -1;

            printf("Invalid weight.\n");
        }

        if (pet->Weight <= 0)
        {
            printf("Weight must be greater than zero.\n");
        }

    } while (pet->Weight <= 0);


    /* Size */

    do
    {
        printf("\nSize:\n");
        printf("1. Small\n");
        printf("2. Medium\n");
        printf("3. Large\n");
        printf("Enter choice: ");

        int sizeChoice;

        if (scanf("%d", &sizeChoice) != 1)
        {
            while (getchar() != '\n')
            {
            }

            sizeChoice = 0;
        }

        if (sizeChoice == 1)
        {
            strcpy(pet->Size, "Small");
        }
        else if (sizeChoice == 2)
        {
            strcpy(pet->Size, "Medium");
        }
        else if (sizeChoice == 3)
        {
            strcpy(pet->Size, "Large");
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }

        if (sizeChoice >= 1 && sizeChoice <= 3)
        {
            break;
        }

    } while (1);


    /* Rescue Location */

    printf("Rescue Location: ");
    scanf(" %49[^\n]", pet->Location);


    /* Health Status */

    do
    {
        printf("\nHealth Status:\n");
        printf("1. Healthy\n");
        printf("2. Injured\n");
        printf("3. Under Treatment\n");
        printf("Enter choice: ");

        int healthChoice;

        if (scanf("%d", &healthChoice) != 1)
        {
            while (getchar() != '\n')
            {
            }

            healthChoice = 0;
        }

        if (healthChoice == 1)
        {
            strcpy(pet->HealthStatus, "Healthy");
        }
        else if (healthChoice == 2)
        {
            strcpy(pet->HealthStatus, "Injured");
        }
        else if (healthChoice == 3)
        {
            strcpy(pet->HealthStatus, "UnderTreatment");
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }

    } while (strcmp(pet->HealthStatus, "Healthy") != 0 &&
             strcmp(pet->HealthStatus, "Injured") != 0 &&
             strcmp(pet->HealthStatus, "UnderTreatment") != 0);


    /* Availability */

    do
    {
        printf("\nAvailability:\n");
        printf("1. Available\n");
        printf("2. Under Treatment\n");
        printf("3. Adopted\n");
        printf("Enter choice: ");

        int availabilityChoice;

        if (scanf("%d", &availabilityChoice) != 1)
        {
            while (getchar() != '\n')
            {
            }

            availabilityChoice = 0;
        }

        if (availabilityChoice == 1)
        {
            strcpy(pet->Availability, "Available");
        }
        else if (availabilityChoice == 2)
        {
            strcpy(pet->Availability, "UnderTreatment");
        }
        else if (availabilityChoice == 3)
        {
            strcpy(pet->Availability, "Adopted");
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }

    } while (strcmp(pet->Availability, "Available") != 0 &&
             strcmp(pet->Availability, "UnderTreatment") != 0 &&
             strcmp(pet->Availability, "Adopted") != 0);


    petCount++;

    savePets();

    printf("\nPet registered successfully!\n");
}


/* ============================================
   VIEW ALL PETS
   ============================================ */

void viewAllPets(void)
{
    if (petCount == 0)
    {
        printf("\nNo pet records found.\n");
        return;
    }

    printf("\n============================================================\n");
    printf("                    ALL RESCUED PETS\n");
    printf("============================================================\n");

    for (int i = 0; i < petCount; i++)
    {
        printf("\nPet ID        : %d\n", pets[i].ID);
        printf("Name          : %s\n", pets[i].Name);
        printf("Species       : %s\n", pets[i].Species);
        printf("Breed         : %s\n", pets[i].Breed);
        printf("Age           : %d years\n", pets[i].Age);
        printf("Gender        : %s\n", pets[i].Gender);
        printf("Weight        : %.2f kg\n", pets[i].Weight);
        printf("Size          : %s\n", pets[i].Size);
        printf("Location      : %s\n", pets[i].Location);
        printf("Health Status : %s\n", pets[i].HealthStatus);
        printf("Availability  : %s\n", pets[i].Availability);

        printf("--------------------------------------------\n");
    }
}


/* ============================================
   UPDATE PET
   ============================================ */

void updatePet(void)
{
    int searchID;
    int found = 0;

    if (petCount == 0)
    {
        printf("\nNo pet records found.\n");
        return;
    }

    printf("\nEnter Pet ID: ");

    if (scanf("%d", &searchID) != 1)
    {
        while (getchar() != '\n')
        {
        }

        printf("Invalid Pet ID.\n");
        return;
    }

    for (int i = 0; i < petCount; i++)
    {
        if (pets[i].ID == searchID)
        {
            found = 1;

            printf("\nPet found: %s\n", pets[i].Name);


            /* Update Health Status */

            do
            {
                printf("\nNew Health Status:\n");
                printf("1. Healthy\n");
                printf("2. Injured\n");
                printf("3. Under Treatment\n");
                printf("Enter choice: ");

                int healthChoice;

                if (scanf("%d", &healthChoice) != 1)
                {
                    while (getchar() != '\n')
                    {
                    }

                    healthChoice = 0;
                }

                if (healthChoice == 1)
                {
                    strcpy(pets[i].HealthStatus, "Healthy");
                }
                else if (healthChoice == 2)
                {
                    strcpy(pets[i].HealthStatus, "Injured");
                }
                else if (healthChoice == 3)
                {
                    strcpy(pets[i].HealthStatus, "UnderTreatment");
                }
                else
                {
                    printf("Invalid choice. Please try again.\n");
                }

                if (healthChoice >= 1 && healthChoice <= 3)
                {
                    break;
                }

            } while (1);


            /* Update Availability */

            do
            {
                printf("\nNew Availability:\n");
                printf("1. Available\n");
                printf("2. Under Treatment\n");
                printf("3. Adopted\n");
                printf("Enter choice: ");

                int availabilityChoice;

                if (scanf("%d", &availabilityChoice) != 1)
                {
                    while (getchar() != '\n')
                    {
                    }

                    availabilityChoice = 0;
                }

                if (availabilityChoice == 1)
                {
                    strcpy(pets[i].Availability, "Available");
                }
                else if (availabilityChoice == 2)
                {
                    strcpy(pets[i].Availability, "UnderTreatment");
                }
                else if (availabilityChoice == 3)
                {
                    strcpy(pets[i].Availability, "Adopted");
                }
                else
                {
                    printf("Invalid choice. Please try again.\n");
                }

                if (availabilityChoice >= 1 && availabilityChoice <= 3)
                {
                    break;
                }

            } while (1);


            savePets();

            printf("\nPet updated successfully!\n");

            break;
        }
    }

    if (!found)
    {
        printf("\nPet ID not found.\n");
    }
}


/* ============================================
   TEST MAIN


int main(void)
{
    createPetFile();

    loadPets();

    createMedicalFile();

    

    viewMedicalRecords();

    printf("\n=== ALL PETS ===\n");
    viewAllPets();

    printf("\n=== SEARCH PET ===\n");
    searchPet();

    printf("\n=== URGENCY ASSESSMENT ===\n");
    calculatePetUrgency();

    printf("\n=== CRITICAL ANIMALS ===\n");
    viewCriticalAnimals();

    return 0;
}


*/