#include <stdio.h>
#include <string.h>
#include "medical.h"


/* ============================================
   CREATE MEDICAL RECORD FILE
   ============================================ */

void createMedicalFile(void)
{
    FILE *file = fopen("medical_records.txt", "r");

    /* File already exists */
    if (file != NULL)
    {
        fclose(file);
        return;
    }

    file = fopen("medical_records.txt", "w");

    if (file == NULL)
    {
        printf("Error: Could not create medical_records.txt\n");
        return;
    }

    /*
       Pet ID | Injury | Illness | Pain | Dehydration |
       Condition | Treatment | Date
    */

    fprintf(file,
            "1|0|0|0|0|0|Medication|01/09/2026\n");

    fprintf(file,
            "2|1|0|1|0|0|Medication|01/09/2026\n");

    fprintf(file,
            "3|0|1|1|0|0|Medication|01/09/2026\n");

    fprintf(file,
            "4|3|0|3|1|2|Surgery|01/09/2026\n");

    fprintf(file,
            "5|0|0|0|0|0|Medication|01/09/2026\n");

    fclose(file);

    printf("medical_records.txt created successfully.\n");
}


/* ============================================
   ADD MEDICAL RECORD
   ============================================ */

void addMedicalRecord(void)
{
    FILE *file;

    int petID;

    int injuryLevel;
    int illnessLevel;
    int painLevel;
    int dehydrationLevel;
    int conditionLevel;

    int treatmentChoice;

    char treatment[30];
    char date[20];


    file = fopen("medical_records.txt", "a");

    if (file == NULL)
    {
        printf("\nError: Could not open medical_records.txt\n");
        return;
    }


    printf("\n========================================\n");
    printf("          ADD MEDICAL RECORD\n");
    printf("========================================\n");


    /* ========================================
       PET ID
       ======================================== */

    printf("Pet ID: ");

    if (scanf("%d", &petID) != 1)
    {
        printf("Invalid Pet ID.\n");

        while (getchar() != '\n')
        {
        }

        fclose(file);
        return;
    }


    /* ========================================
       INJURY SEVERITY
       ======================================== */

    do
    {
        printf("\nInjury Severity\n");
        printf("0 - None\n");
        printf("1 - Minor\n");
        printf("2 - Moderate\n");
        printf("3 - Severe\n");
        printf("4 - Critical\n");

        printf("Enter level: ");
        scanf("%d", &injuryLevel);

        if (injuryLevel < 0 || injuryLevel > 4)
        {
            printf("Invalid choice. Please enter 0-4.\n");
        }

    } while (injuryLevel < 0 || injuryLevel > 4);


    /* ========================================
       ILLNESS SEVERITY
       ======================================== */

    do
    {
        printf("\nIllness Severity\n");
        printf("0 - None\n");
        printf("1 - Minor\n");
        printf("2 - Moderate\n");
        printf("3 - Severe\n");
        printf("4 - Critical\n");

        printf("Enter level: ");
        scanf("%d", &illnessLevel);

        if (illnessLevel < 0 || illnessLevel > 4)
        {
            printf("Invalid choice. Please enter 0-4.\n");
        }

    } while (illnessLevel < 0 || illnessLevel > 4);


    /* ========================================
       PAIN LEVEL
       ======================================== */

    do
    {
        printf("\nPain Level\n");
        printf("0 - None\n");
        printf("1 - Mild\n");
        printf("2 - Moderate\n");
        printf("3 - Severe\n");

        printf("Enter level: ");
        scanf("%d", &painLevel);

        if (painLevel < 0 || painLevel > 3)
        {
            printf("Invalid choice. Please enter 0-3.\n");
        }

    } while (painLevel < 0 || painLevel > 3);


    /* ========================================
       DEHYDRATION LEVEL
       ======================================== */

    do
    {
        printf("\nDehydration Level\n");
        printf("0 - None\n");
        printf("1 - Moderate\n");
        printf("2 - Severe\n");

        printf("Enter level: ");
        scanf("%d", &dehydrationLevel);

        if (dehydrationLevel < 0 || dehydrationLevel > 2)
        {
            printf("Invalid choice. Please enter 0-2.\n");
        }

    } while (dehydrationLevel < 0 || dehydrationLevel > 2);


    /* ========================================
       EXISTING MEDICAL CONDITION
       ======================================== */

    do
    {
        printf("\nExisting Medical Condition\n");
        printf("0 - None\n");
        printf("1 - Minor\n");
        printf("2 - Serious\n");

        printf("Enter level: ");
        scanf("%d", &conditionLevel);

        if (conditionLevel < 0 || conditionLevel > 2)
        {
            printf("Invalid choice. Please enter 0-2.\n");
        }

    } while (conditionLevel < 0 || conditionLevel > 2);


    /* ========================================
       TREATMENT
       ======================================== */

    do
    {
        printf("\nTreatment Required\n");
        printf("1 - Medication\n");
        printf("2 - Surgery\n");

        printf("Enter choice: ");
        scanf("%d", &treatmentChoice);

        if (treatmentChoice != 1 && treatmentChoice != 2)
        {
            printf("Invalid choice. Please enter 1 or 2.\n");
        }

    } while (treatmentChoice != 1 && treatmentChoice != 2);


    if (treatmentChoice == 1)
    {
        strcpy(treatment, "Medication");
    }
    else
    {
        strcpy(treatment, "Surgery");
    }


    /* ========================================
       DATE
       ======================================== */

    while (getchar() != '\n')
    {
    }

    printf("Date (DD/MM/YYYY): ");

    if (fgets(date, sizeof(date), stdin) != NULL)
    {
        date[strcspn(date, "\n")] = '\0';
    }


    /* ========================================
       SAVE RECORD
       ======================================== */

    fprintf(file,
            "%d|%d|%d|%d|%d|%d|%s|%s\n",
            petID,
            injuryLevel,
            illnessLevel,
            painLevel,
            dehydrationLevel,
            conditionLevel,
            treatment,
            date);

    fclose(file);

    printf("\nMedical record saved successfully!\n");
}


/* ============================================
   VIEW MEDICAL RECORDS
   ============================================ */

void viewMedicalRecords(void)
{
    FILE *file;

    int petID;

    int injury;
    int illness;
    int pain;
    int dehydration;
    int condition;

    char treatment[30];
    char date[20];

    int found = 0;


    file = fopen("medical_records.txt", "r");

    if (file == NULL)
    {
        printf("\nNo medical records found.\n");
        return;
    }


    printf("\n========================================\n");
    printf("             MEDICAL RECORDS\n");
    printf("========================================\n");


    while (fscanf(file,
                  " %d|%d|%d|%d|%d|%d|%29[^|]|%19[^\n]",
                  &petID,
                  &injury,
                  &illness,
                  &pain,
                  &dehydration,
                  &condition,
                  treatment,
                  date) == 8)
    {
        found = 1;

        printf("\nPet ID             : %d\n", petID);
        printf("Injury Level       : %d\n", injury);
        printf("Illness Level      : %d\n", illness);
        printf("Pain Level         : %d\n", pain);
        printf("Dehydration Level  : %d\n", dehydration);
        printf("Existing Condition : %d\n", condition);
        printf("Treatment          : %s\n", treatment);
        printf("Date               : %s\n", date);

        printf("----------------------------------------\n");
    }


    fclose(file);


    if (!found)
    {
        printf("No records available.\n");
    }
}
