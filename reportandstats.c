#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reportandstats.h"


/* =========================================================
   HELPER FUNCTION
   Compare two strings without considering uppercase/lowercase
   ========================================================= */

int sameReportString(const char a[], const char b[])
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


/* =========================================================
   DISPLAY DONATION STATISTICS
   ========================================================= */

void displayDonationStatistics(void)
{
    FILE *file;

    int totalDonations = 0;
    float totalMoney = 0.0f;
    int totalFood = 0;
    int totalSupplies = 0;

    char line[500];


    file = fopen("donations.txt", "r");

    if (file == NULL)
    {
        printf("\nNo donation records found.\n");
        return;
    }


    /*
       donations.txt format:

       0 = Donation ID
       1 = Donor Name
       2 = Address
       3 = Contact Number
       4 = Email
       5 = Donation Type
       6 = Amount
       7 = Item Name
       8 = Quantity
    */

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *fields[9];

        int fieldIndex = 1;

        fields[0] = line;


        for (int i = 0;
             line[i] != '\0' && fieldIndex < 9;
             i++)
        {
            if (line[i] == '|')
            {
                line[i] = '\0';

                fields[fieldIndex] = &line[i + 1];

                fieldIndex++;
            }
        }


        if (fieldIndex == 9)
        {
            totalDonations++;

            fields[8][strcspn(fields[8], "\n")] = '\0';


            /*
               Money donation
            */

            if (sameReportString(fields[5], "Money"))
            {
                totalMoney += atof(fields[6]);
            }


            /*
               Food donation
            */

            else if (sameReportString(fields[5], "Food"))
            {
                totalFood += atoi(fields[8]);
            }


            /*
               Supplies donation
            */

            else if (sameReportString(fields[5], "Supplies"))
            {
                totalSupplies += atoi(fields[8]);
            }
        }
    }


    fclose(file);


    printf("\n========================================\n");
    printf("          DONATION STATISTICS\n");
    printf("========================================\n");

    printf("Total Donations Received : %d\n",
           totalDonations);

    printf("Total Money Donated      : Rs %.2f\n",
           totalMoney);

    printf("Total Food Quantity      : %d\n",
           totalFood);

    printf("Total Supplies Quantity  : %d\n",
           totalSupplies);

    printf("========================================\n");
}


/* =========================================================
   DISPLAY PET STATISTICS
   ========================================================= */

void displayPetStatistics(void)
{
    FILE *file;

    int totalPets = 0;
    int dogs = 0;
    int cats = 0;
    int male = 0;
    int female = 0;
    int available = 0;
    int adopted = 0;

    char line[500];


    file = fopen("pets.txt", "r");

    if (file == NULL)
    {
        printf("\nNo pet records found.\n");
        return;
    }


    /*
       pets.txt format:

       0 = ID
       1 = Name
       2 = Species
       3 = Breed
       4 = Age
       5 = Gender
       6 = Weight
       7 = Size
       8 = Location
       9 = Health Status
       10 = Availability

       This matches the Pet structure used by the
       matching module.
    */

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *fields[11];

        int fieldIndex = 1;

        fields[0] = line;


        for (int i = 0;
             line[i] != '\0' && fieldIndex < 11;
             i++)
        {
            if (line[i] == '|')
            {
                line[i] = '\0';

                fields[fieldIndex] = &line[i + 1];

                fieldIndex++;
            }
        }


        if (fieldIndex == 11)
        {
            totalPets++;


            fields[10][strcspn(fields[10], "\n")] = '\0';


            /*
               Count species
            */

            if (sameReportString(fields[2], "Dog"))
            {
                dogs++;
            }
            else if (sameReportString(fields[2], "Cat"))
            {
                cats++;
            }


            /*
               Count gender
            */

            if (sameReportString(fields[5], "Male") ||
                sameReportString(fields[5], "M"))
            {
                male++;
            }
            else if (sameReportString(fields[5], "Female") ||
                     sameReportString(fields[5], "F"))
            {
                female++;
            }


            /*
               Count availability
            */

            if (sameReportString(fields[10], "Available"))
            {
                available++;
            }
            else if (sameReportString(fields[10], "Adopted"))
            {
                adopted++;
            }
        }
    }


    fclose(file);


    printf("\n========================================\n");
    printf("             PET STATISTICS\n");
    printf("========================================\n");

    printf("Total Pets Registered : %d\n",
           totalPets);

    printf("Dogs                  : %d\n",
           dogs);

    printf("Cats                  : %d\n",
           cats);

    printf("Male Pets             : %d\n",
           male);

    printf("Female Pets           : %d\n",
           female);

    printf("Available Pets        : %d\n",
           available);

    printf("Adopted Pets          : %d\n",
           adopted);

    printf("========================================\n");
}


/* =========================================================
   DISPLAY ADOPTION STATISTICS
   ========================================================= */

void displayAdoptionStatistics(void)
{
    FILE *file;

    int totalApplications = 0;
    int totalApproved = 0;
    int totalWaitlisted = 0;
    int totalRejected = 0;

    int dogApplications = 0;
    int catApplications = 0;

    float totalFees = 0.0f;

    char line[500];


    file = fopen("adoptions.txt", "r");

    if (file == NULL)
    {
        printf("\nNo adoption records found.\n");
        return;
    }


    /*
       matching.c saves:

       0 = Adopter ID
       1 = Pet ID
       2 = Pet Name
       3 = Species
       4 = Compatibility Score
       5 = Adoption Fee
       6 = Status
    */

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *fields[7];

        int fieldIndex = 1;

        fields[0] = line;


        for (int i = 0;
             line[i] != '\0' && fieldIndex < 7;
             i++)
        {
            if (line[i] == '|')
            {
                line[i] = '\0';

                fields[fieldIndex] = &line[i + 1];

                fieldIndex++;
            }
        }


        /*
           Valid adoption record must contain 7 fields.
        */

        if (fieldIndex == 7)
        {
            totalApplications++;


            fields[6][strcspn(fields[6], "\n")] = '\0';


            /*
               Add adoption fee
            */

            totalFees += atof(fields[5]);


            /*
               Count application status
            */

            if (sameReportString(fields[6], "Approved"))
            {
                totalApproved++;
            }
            else if (sameReportString(fields[6], "Waitlist"))
            {
                totalWaitlisted++;
            }
            else if (sameReportString(fields[6], "Rejected"))
            {
                totalRejected++;
            }


            /*
               Count species
            */

            if (sameReportString(fields[3], "Dog"))
            {
                dogApplications++;
            }
            else if (sameReportString(fields[3], "Cat"))
            {
                catApplications++;
            }
        }
    }


    fclose(file);


    /*
       Calculate success rate.
    */

    float successRate = 0.0f;

    if (totalApplications > 0)
    {
        successRate =
            ((float)totalApproved /
             totalApplications) * 100.0f;
    }


    printf("\n========================================\n");
    printf("           ADOPTION STATISTICS\n");
    printf("========================================\n");

    printf("Total Applications : %d\n",
           totalApplications);

    printf("Approved            : %d\n",
           totalApproved);

    printf("Waitlisted          : %d\n",
           totalWaitlisted);

    printf("Rejected            : %d\n",
           totalRejected);

    printf("Success Rate        : %.2f%%\n",
           successRate);

    printf("Total Adoption Fees : Rs %.2f\n",
           totalFees);


    /*
       Determine most commonly involved species.
    */

    if (dogApplications > catApplications)
    {
        printf("Most Common Species : Dogs\n");
    }
    else if (catApplications > dogApplications)
    {
        printf("Most Common Species : Cats\n");
    }
    else if (dogApplications == 0 &&
             catApplications == 0)
    {
        printf("Most Common Species : No dog/cat applications\n");
    }
    else
    {
        printf("Most Common Species : Dogs and Cats are equal\n");
    }


    printf("========================================\n");
}


/* =========================================================
   DISPLAY OVERALL REPORT
   ========================================================= */

void displayOverallReport(void)
{
    printf("\n");
    printf("================================================\n");
    printf("             OVERALL SUMMARY REPORT\n");
    printf("================================================\n");


    /*
       Donation statistics
    */

    displayDonationStatistics();


    /*
       Pet statistics
    */

    displayPetStatistics();


    /*
       Adoption statistics
    */

    displayAdoptionStatistics();


    printf("\n================================================\n");
    printf("                 END OF REPORT\n");
    printf("================================================\n");
}

