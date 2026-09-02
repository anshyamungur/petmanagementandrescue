#include <stdio.h>
#include <stdlib.h>

#include "pet.h"
#include "adopter.h"
#include "matching.h"
#include "donation.h"
#include "medical.h"
#include "urgency.h"
#include "reportandstats.h"
#include "search.h"


/* =========================================================
   PET MANAGEMENT MENU
   ========================================================= */

void petManagementMenu(void)
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf("          PET MANAGEMENT\n");
        printf("========================================\n");
        printf("1. Register Pet\n");
        printf("2. View All Pets\n");
        printf("3. Update Pet\n");
        printf("4. Search Pet\n");
        printf("0. Back to Main Menu\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                registerPet();
                break;

            case 2:
                viewAllPets();
                break;

            case 3:
                updatePet();
                break;

            case 4:
                searchPet();
                break;

            case 0:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 0);
}


/* =========================================================
   ADOPTER MANAGEMENT MENU
   ========================================================= */

void adopterManagementMenu(void)
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf("        ADOPTER MANAGEMENT\n");
        printf("========================================\n");
        printf("1. Add Adopter\n");
        printf("2. Display Adopters\n");
        printf("3. Search Adopter\n");
        printf("4. Update Adopter\n");
        printf("5. Delete Adopter\n");
        printf("0. Back to Main Menu\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                addAdopter();
                break;

            case 2:
                displayAdopter();
                break;

            case 3:
                searchAdopter();
                break;

            case 4:
                updateAdopter();
                break;

            case 5:
                deleteAdopter();
                break;

            case 0:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 0);
}


/* =========================================================
   DONATION MENU
   ========================================================= */

void donationMenu(void)
{
    struct Donation *donations;
    int count = 0;
    int choice;
    int donationID;

    donations = malloc(
        MAX_DONATIONS * sizeof(struct Donation)
    );

    if (donations == NULL)
    {
        printf("\nMemory allocation failed.\n");
        return;
    }

    loadDonations(donations, &count);

    do
    {
        printf("\n========================================\n");
        printf("          DONATION MANAGEMENT\n");
        printf("========================================\n");
        printf("1. Add Donation\n");
        printf("2. Display Donations\n");
        printf("3. Search Donation\n");
        printf("0. Back to Main Menu\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                donations = addDonation(
                    donations,
                    &count
                );
                break;

            case 2:
                displayDonations(
                    donations,
                    count
                );
                break;

            case 3:
                printf("\nEnter donation ID: ");
                scanf("%d", &donationID);

                while (getchar() != '\n');

                searchDonation(
                    donations,
                    count,
                    donationID
                );
                break;

            case 0:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 0);

    free(donations);
}


/* =========================================================
   ADOPTION MATCHING MENU
   ========================================================= */

void matchingMenu(void)
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf("         ADOPTION & MATCHING\n");
        printf("========================================\n");
        printf("1. Show Top 5 Recommendations\n");
        printf("2. Process Adoption Application\n");
        printf("0. Back to Main Menu\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
            {
                int adopterID;
                int i;
                int found = 0;

                printf("\nEnter adopter ID: ");
                scanf("%d", &adopterID);

                while (getchar() != '\n');

                for (i = 0; i < adopterCount; i++)
                {
                    if (adopters[i].ID == adopterID)
                    {
                        showTop5Recommendations(
                            adopters[i]
                        );

                        found = 1;
                        break;
                    }
                }

                if (!found)
                {
                    printf("\nAdopter ID not found.\n");
                }

                break;
            }

            case 2:
            {
                int adopterID;
                char petID[20];
                int i;
                int found = 0;

                printf("\nEnter adopter ID: ");
                scanf("%d", &adopterID);

                while (getchar() != '\n');

                printf("Enter pet ID: ");
                scanf("%19s", petID);

                while (getchar() != '\n');

                for (i = 0; i < adopterCount; i++)
                {
                    if (adopters[i].ID == adopterID)
                    {
                        processAndSaveApplication(
                            adopters[i],
                            petID
                        );

                        found = 1;
                        break;
                    }
                }

                if (!found)
                {
                    printf("\nAdopter ID not found.\n");
                }

                break;
            }

            case 0:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 0);
}


/* =========================================================
   MEDICAL RECORD MENU
   ========================================================= */

void medicalMenu(void)
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf("          MEDICAL RECORDS\n");
        printf("========================================\n");
        printf("1. Add Medical Record\n");
        printf("2. View Medical Records\n");
        printf("0. Back to Main Menu\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                addMedicalRecord();
                break;

            case 2:
                viewMedicalRecords();
                break;

            case 0:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 0);
}


/* =========================================================
   URGENCY MENU
   ========================================================= */

void urgencyMenu(void)
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf("          URGENCY ASSESSMENT\n");
        printf("========================================\n");
        printf("1. Calculate Pet Urgency\n");
        printf("2. View Critical Animals\n");
        printf("0. Back to Main Menu\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                calculatePetUrgency();
                break;

            case 2:
                viewCriticalAnimals();
                break;

            case 0:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 0);
}


/* =========================================================
   REPORT MENU
   ========================================================= */

void reportMenu(void)
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf("          REPORTS & STATISTICS\n");
        printf("========================================\n");
        printf("1. Donation Statistics\n");
        printf("2. Pet Statistics\n");
        printf("3. Adoption Statistics\n");
        printf("4. Overall Summary Report\n");
        printf("0. Back to Main Menu\n");
        printf("========================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                displayDonationStatistics();
                break;

            case 2:
                displayPetStatistics();
                break;

            case 3:
                displayAdoptionStatistics();
                break;

            case 4:
                displayOverallReport();
                break;

            case 0:
                break;

            default:
                printf("\nInvalid choice.\n");
        }

    } while (choice != 0);
}


/* =========================================================
   MAIN MENU
   ========================================================= */

int main(void)
{
    int choice;

    /*
       Initialize data files and load existing records.
    */

    createPetFile();
    loadPets();

    createAdopterFile();
    loadAdopters();

    createMedicalFile();

    do
    {
        printf("\n");
        printf("================================================\n");
        printf("       PET MANAGEMENT & RESCUE SYSTEM\n");
        printf("================================================\n");
        printf("1.  Pet Management\n");
        printf("2.  Adopter Management\n");
        printf("3.  Adoption & Matching\n");
        printf("4.  Donations\n");
        printf("5.  Medical Records\n");
        printf("6.  Urgency Assessment\n");
        printf("7.  Reports & Statistics\n");
        printf("0.  Exit\n");
        printf("================================================\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                petManagementMenu();
                break;

            case 2:
                adopterManagementMenu();
                break;

            case 3:
                matchingMenu();
                break;

            case 4:
                donationMenu();
                break;

            case 5:
                medicalMenu();
                break;

            case 6:
                urgencyMenu();
                break;

            case 7:
                reportMenu();
                break;

            case 0:
                printf("\nThank you for using the system.\n");
                break;

            default:
                printf("\nInvalid choice. Please try again.\n");
        }

    } while (choice != 0);

    return 0;
}
