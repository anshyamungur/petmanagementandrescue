#include <stdio.h>
#include <string.h>
#include "adopter.h"

struct Adopter adopters[MAX_ADOPTERS];
int adopterCount = 0;


/* ============================================
   CREATE DEFAULT ADOPTER FILE
   ============================================ */

void createAdopterFile(void)
{
    FILE *file = fopen("adopters.txt", "r");

    if (file != NULL)
    {
        fclose(file);
        return;
    }

    file = fopen("adopters.txt", "w");

    if (file == NULL)
    {
        printf("Error: Could not create adopters.txt\n");
        return;
    }

    fprintf(file,
            "236|Cole|57575128|M|cole@mail.com|Long Mountain|Moderate|House|1|1|0|Cat|4|9|Large|A\n");

    fprintf(file,
            "237|Sarah|51234567|F|sarah@mail.com|Curepipe|Light|House|1|1|1|Dog|2|7|Medium|F\n");

    fprintf(file,
            "238|John|59876543|M|john@mail.com|Quatre Bornes|Busy|Apartment|0|0|0|Cat|1|5|Small|M\n");

    fprintf(file,
            "239|Emily|52345678|F|emily@mail.com|Grand Baie|Light|House|1|1|1|Dog|3|10|Large|A\n");

    fprintf(file,
            "240|David|53456789|M|david@mail.com|Rose Hill|Moderate|Apartment|1|0|0|Dog|1|6|Medium|F\n");

    fclose(file);

    printf("adopters.txt created successfully.\n");
}


/* ============================================
   LOAD ADOPTERS
   ============================================ */

void loadAdopters(void)
{
    FILE *file = fopen("adopters.txt", "r");

    if (file == NULL)
    {
        printf("No adopter file found. Starting with empty records.\n");
        adopterCount = 0;
        return;
    }

    adopterCount = 0;

    while (adopterCount < MAX_ADOPTERS &&
           fscanf(file,
                  " %d|%49[^|]|%19[^|]| %c|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%d|%d|%19[^|]|%d|%d|%19[^|]| %c",
                  &adopters[adopterCount].ID,
                  adopters[adopterCount].Name,
                  adopters[adopterCount].Phone,
                  &adopters[adopterCount].Gender,
                  adopters[adopterCount].Email,
                  adopters[adopterCount].Address,
                  adopters[adopterCount].Schedule,
                  adopters[adopterCount].Hometype,
                  &adopters[adopterCount].Garden,
                  &adopters[adopterCount].Experience,
                  &adopters[adopterCount].Children,
                  adopters[adopterCount].PreferredSpecies,
                  &adopters[adopterCount].minAge,
                  &adopters[adopterCount].maxAge,
                  adopters[adopterCount].PreferredSize,
                  &adopters[adopterCount].PreferredGender) == 16)
    {
        adopterCount++;
    }

    fclose(file);

    printf("%d adopter(s) loaded successfully.\n", adopterCount);
}


/* ============================================
   SAVE ADOPTERS
   ============================================ */
void saveAdopters(void)
{
    FILE *file = fopen("adopters.txt", "w");

    if (file == NULL)
    {
        printf("Error: Could not open adopters.txt for writing.\n");
        return;
    }

    for (int i = 0; i < adopterCount; i++)
    {
        fprintf(file,
                "%d|%s|%s|%c|%s|%s|%s|%s|%d|%d|%d|%s|%d|%d|%s|%c\n",
                adopters[i].ID,
                adopters[i].Name,
                adopters[i].Phone,
                adopters[i].Gender,
                adopters[i].Email,
                adopters[i].Address,
                adopters[i].Schedule,
                adopters[i].Hometype,
                adopters[i].Garden,
                adopters[i].Experience,
                adopters[i].Children,
                adopters[i].PreferredSpecies,
                adopters[i].minAge,
                adopters[i].maxAge,
                adopters[i].PreferredSize,
                adopters[i].PreferredGender);
    }

    fclose(file);

    printf("Adopters saved successfully to adopters.txt\n");
}



/* ============================================
   CHECK IF ADOPTER ID EXISTS
   ============================================ */

int adopterIDExists(int id)
{
    for (int i = 0; i < adopterCount; i++)
    {
        if (adopters[i].ID == id)
        {
            return 1;
        }
    }

    return 0;
}


/* ============================================
   ADD ADOPTER
   ============================================ */

void addAdopter(void)
{
    if (adopterCount >= MAX_ADOPTERS)
    {
        printf("Maximum number of adopters reached.\n");
        return;
    }

    struct Adopter *adopter = &adopters[adopterCount];

    printf("\n========================================\n");
    printf("          REGISTER ADOPTER\n");
    printf("========================================\n");

    do
    {
        printf("Enter adopter ID: ");
        scanf("%d", &adopter->ID);

        if (adopterIDExists(adopter->ID))
        {
            printf("This ID already exists. Please use another ID.\n");
        }

    } while (adopterIDExists(adopter->ID));

    printf("Enter name: ");
    scanf(" %49[^\n]", adopter->Name);

    printf("Enter phone: ");
    scanf(" %19s", adopter->Phone);

    do
    {
        printf("Enter gender (M/F): ");
        scanf(" %c", &adopter->Gender);

        if (adopter->Gender != 'M' &&
            adopter->Gender != 'm' &&
            adopter->Gender != 'F' &&
            adopter->Gender != 'f')
        {
            printf("Invalid gender.\n");
        }

    } while (adopter->Gender != 'M' &&
             adopter->Gender != 'm' &&
             adopter->Gender != 'F' &&
             adopter->Gender != 'f');

    printf("Enter email: ");
    scanf(" %49s", adopter->Email);

    printf("Enter address: ");
    scanf(" %49[^\n]", adopter->Address);

    printf("Enter home type (House/Apartment): ");
    scanf(" %19s", adopter->Hometype);

    printf("Garden? (1 = Yes, 0 = No): ");
    scanf("%d", &adopter->Garden);

    printf("Previous pet experience? (1 = Yes, 0 = No): ");
    scanf("%d", &adopter->Experience);

    printf("Enter schedule (Busy/Moderate/Light): ");
    scanf(" %19s", adopter->Schedule);

    printf("Do you have children? (1 = Yes, 0 = No): ");
    scanf("%d", &adopter->Children);

    printf("What animal are you looking for? (Cat/Dog): ");
    scanf(" %19s", adopter->PreferredSpecies);

    do
    {
        printf("Minimum preferred pet age: ");
        scanf("%d", &adopter->minAge);

        printf("Maximum preferred pet age: ");
        scanf("%d", &adopter->maxAge);

        if (adopter->minAge < 0 ||
            adopter->maxAge < adopter->minAge)
        {
            printf("Invalid age range.\n");
        }

    } while (adopter->minAge < 0 ||
             adopter->maxAge < adopter->minAge);

    printf("Preferred size (Large/Medium/Small): ");
    scanf(" %19s", adopter->PreferredSize);

    do
    {
        printf("Preferred gender (M/F/A): ");
        scanf(" %c", &adopter->PreferredGender);

        if (adopter->PreferredGender != 'M' &&
            adopter->PreferredGender != 'm' &&
            adopter->PreferredGender != 'F' &&
            adopter->PreferredGender != 'f' &&
            adopter->PreferredGender != 'A' &&
            adopter->PreferredGender != 'a')
        {
            printf("Invalid choice.\n");
        }

    } while (adopter->PreferredGender != 'M' &&
             adopter->PreferredGender != 'm' &&
             adopter->PreferredGender != 'F' &&
             adopter->PreferredGender != 'f' &&
             adopter->PreferredGender != 'A' &&
             adopter->PreferredGender != 'a');

    adopterCount++;

    saveAdopters();

    printf("\nAdopter registered successfully!\n");
}


/* ============================================
   DISPLAY ADOPTERS
   ============================================ */

void displayAdopter(void)
{
    if (adopterCount == 0)
    {
        printf("\nNo adopters registered.\n");
        return;
    }

    printf("\n============================================================\n");
    printf("                    ADOPTER LIST\n");
    printf("============================================================\n");

    printf("%-5s %-15s %-7s %-12s %-10s %-12s\n",
           "ID",
           "Name",
           "Gender",
           "Phone",
           "Species",
           "Age Range");

    printf("------------------------------------------------------------\n");

    for (int i = 0; i < adopterCount; i++)
    {
        printf("%-5d %-15s %-7c %-12s %-10s %d-%-7d\n",
               adopters[i].ID,
               adopters[i].Name,
               adopters[i].Gender,
               adopters[i].Phone,
               adopters[i].PreferredSpecies,
               adopters[i].minAge,
               adopters[i].maxAge);
    }

    printf("============================================================\n");
}


/* ============================================
   SEARCH ADOPTER
   ============================================ */

void searchAdopter(void)
{
    int ID;
    int found = 0;

    if (adopterCount == 0)
    {
        printf("\nNo adopters registered.\n");
        return;
    }

    printf("\nEnter adopter ID to search: ");
    scanf("%d", &ID);

    for (int i = 0; i < adopterCount; i++)
    {
        if (adopters[i].ID == ID)
        {
            printf("\n========================================\n");
            printf("             ADOPTER FOUND\n");
            printf("========================================\n");

            printf("ID: %d\n", adopters[i].ID);
            printf("Name: %s\n", adopters[i].Name);
            printf("Phone: %s\n", adopters[i].Phone);
            printf("Gender: %c\n", adopters[i].Gender);
            printf("Email: %s\n", adopters[i].Email);
            printf("Address: %s\n", adopters[i].Address);
            printf("Home Type: %s\n", adopters[i].Hometype);
            printf("Garden: %s\n",
                   adopters[i].Garden ? "Yes" : "No");
            printf("Experience: %s\n",
                   adopters[i].Experience ? "Yes" : "No");
            printf("Schedule: %s\n", adopters[i].Schedule);
            printf("Children: %s\n",
                   adopters[i].Children ? "Yes" : "No");
            printf("Preferred Species: %s\n",
                   adopters[i].PreferredSpecies);
            printf("Preferred Age: %d - %d\n",
                   adopters[i].minAge,
                   adopters[i].maxAge);
            printf("Preferred Size: %s\n",
                   adopters[i].PreferredSize);
            printf("Preferred Gender: %c\n",
                   adopters[i].PreferredGender);

            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("\nNo adopter found with ID %d.\n", ID);
    }
}


/* ============================================
   UPDATE ADOPTER
   ============================================ */

void updateAdopter(void)
{
    int ID;
    int found = 0;
    int choice;
    char again;

    if (adopterCount == 0)
    {
        printf("\nNo adopters registered.\n");
        return;
    }

    printf("\nEnter adopter ID to update: ");
    scanf("%d", &ID);

    for (int i = 0; i < adopterCount; i++)
    {
        if (adopters[i].ID == ID)
        {
            found = 1;

            do
            {
                printf("\n========================================\n");
                printf("             UPDATE ADOPTER\n");
                printf("========================================\n");

                printf("1. Name\n");
                printf("2. Phone\n");
                printf("3. Gender\n");
                printf("4. Email\n");
                printf("5. Address\n");
                printf("6. Home Type\n");
                printf("7. Garden\n");
                printf("8. Pet Experience\n");
                printf("9. Schedule\n");
                printf("10. Children\n");
                printf("11. Preferred Species\n");
                printf("12. Preferred Age Range\n");
                printf("13. Preferred Size\n");
                printf("14. Preferred Gender\n");
                printf("0. Finish\n");

                printf("Choose: ");
                scanf("%d", &choice);

                switch (choice)
                {
                    case 1:
                        printf("New name: ");
                        scanf(" %49[^\n]", adopters[i].Name);
                        break;

                    case 2:
                        printf("New phone: ");
                        scanf(" %19s", adopters[i].Phone);
                        break;

                    case 3:
                        printf("New gender (M/F): ");
                        scanf(" %c", &adopters[i].Gender);
                        break;

                    case 4:
                        printf("New email: ");
                        scanf(" %49s", adopters[i].Email);
                        break;

                    case 5:
                        printf("New address: ");
                        scanf(" %49[^\n]", adopters[i].Address);
                        break;

                    case 6:
                        printf("New home type: ");
                        scanf(" %19s", adopters[i].Hometype);
                        break;

                    case 7:
                        printf("Garden? (1/0): ");
                        scanf("%d", &adopters[i].Garden);
                        break;

                    case 8:
                        printf("Experience? (1/0): ");
                        scanf("%d", &adopters[i].Experience);
                        break;

                    case 9:
                        printf("New schedule: ");
                        scanf(" %19s", adopters[i].Schedule);
                        break;

                    case 10:
                        printf("Children? (1/0): ");
                        scanf("%d", &adopters[i].Children);
                        break;

                    case 11:
                        printf("Preferred species: ");
                        scanf(" %19s",
                              adopters[i].PreferredSpecies);
                        break;

                    case 12:
                        printf("Minimum age: ");
                        scanf("%d", &adopters[i].minAge);

                        printf("Maximum age: ");
                        scanf("%d", &adopters[i].maxAge);
                        break;

                    case 13:
                        printf("Preferred size: ");
                        scanf(" %19s",
                              adopters[i].PreferredSize);
                        break;

                    case 14:
                        printf("Preferred gender (M/F/A): ");
                        scanf(" %c",
                              &adopters[i].PreferredGender);
                        break;

                    case 0:
                        break;

                    default:
                        printf("Invalid choice.\n");
                        continue;
                }

                if (choice >= 1 && choice <= 14)
                {
                    saveAdopters();
                    printf("Adopter updated successfully.\n");

                    printf("Update another field? (Y/N): ");
                    scanf(" %c", &again);
                }
                else if (choice == 0)
                {
                    again = 'N';
                }
                else
                {
                    again = 'Y';
                }

            } while (again == 'Y' || again == 'y');

            break;
        }
    }

    if (!found)
    {
        printf("\nNo adopter found with ID %d.\n", ID);
    }
}


/* ============================================
   DELETE ADOPTER
   ============================================ */

void deleteAdopter(void)
{
    int ID;
    int found = 0;

    printf("\nEnter adopter ID to delete: ");
    scanf("%d", &ID);

    for (int i = 0; i < adopterCount; i++)
    {
        if (adopters[i].ID == ID)
        {
            for (int j = i; j < adopterCount - 1; j++)
            {
                adopters[j] = adopters[j + 1];
            }

            adopterCount--;

            saveAdopters();

            printf("\nAdopter deleted successfully!\n");

            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("\nNo adopter found with ID %d.\n", ID);
    }
}
#include <stdio.h>
#include "adopter.h"

/*int main(void)
{
    int choice;

    createAdopterFile();
    loadAdopters();

    do
    {
        printf("\n========================================\n");
        printf("          ADOPTER MANAGEMENT\n");
        printf("========================================\n");
        printf("1. Add adopter\n");
        printf("2. Display adopters\n");
        printf("3. Search adopter\n");
        printf("4. Update adopter\n");
        printf("5. Delete adopter\n");
        printf("0. Exit\n");
        printf("========================================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

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
                printf("\nExiting program...\n");
                break;

            default:
                printf("\nInvalid choice. Please try again.\n");
        }

    } while (choice != 0);

    return 0;
}
*/