#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "donation.h"


/* =========================================================
   CLEAR INPUT BUFFER
   ========================================================= */

void clearDonationInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* Clear remaining input */
    }
}


/* =========================================================
   READ TEXT INCLUDING SPACES
   ========================================================= */

void readDonationText(char text[], int size)
{
    if (fgets(text, size, stdin) != NULL)
    {
        text[strcspn(text, "\n")] = '\0';
    }
}


/* =========================================================
   LOAD DONATIONS FROM FILE
   ========================================================= */

void loadDonations(
    struct Donation donations[],
    int *count)
{
    FILE *file;
    char line[400];

    *count = 0;

    file = fopen("donations.txt", "r");

    if (file == NULL)
    {
        printf("\ndonations.txt does not exist.\n");
        printf("Starting with no donation records.\n");
        return;
    }

    while (*count < MAX_DONATIONS &&
           fgets(line, sizeof(line), file) != NULL)
    {
        char *part;

        /* Remove newline */
        line[strcspn(line, "\r\n")] = '\0';

        /* Donation ID */
        part = strtok(line, "|");
        if (part == NULL)
            continue;

        donations[*count].donationID = atoi(part);

        /* Donor name */
        part = strtok(NULL, "|");
        if (part == NULL)
            continue;

        strcpy(donations[*count].donorname, part);

        /* Address */
        part = strtok(NULL, "|");
        if (part == NULL)
            continue;

        strcpy(donations[*count].address, part);

        /* Contact number */
        part = strtok(NULL, "|");
        if (part == NULL)
            continue;

        strcpy(donations[*count].contactnumber, part);

        /* Email */
        part = strtok(NULL, "|");
        if (part == NULL)
            continue;

        strcpy(donations[*count].email, part);

        /* Donation type */
        part = strtok(NULL, "|");
        if (part == NULL)
            continue;

        strcpy(donations[*count].donationtype, part);

        /* Amount */
        part = strtok(NULL, "|");
        if (part == NULL)
            continue;

        donations[*count].amount = atof(part);

        /*
           Item name can be EMPTY for Money donations.
           strtok() skips empty fields, so handle the
           remaining part manually.
        */


        if (strcmp(donations[*count].donationtype, "Money") == 0)
        {
            donations[*count].itemname[0] = '\0';

            part = strtok(NULL, "|");

            if (part == NULL)
            {
                donations[*count].quantity = 0;
            }
            else
            {
                donations[*count].quantity = atoi(part);
            }
        }
        else
        {
            part = strtok(NULL, "|");

            if (part == NULL)
                continue;

            strcpy(donations[*count].itemname, part);

            part = strtok(NULL, "|");

            if (part == NULL)
                continue;

            donations[*count].quantity = atoi(part);
        }

        (*count)++;
    }

    fclose(file);

    printf(
        "%d donation(s) loaded successfully.\n",
        *count
    );
}


/* =========================================================
   ADD DONATION
   ========================================================= */

struct Donation *addDonation(
    struct Donation *donations,
    int *count)
{
    struct Donation *temp;

    int choice;
    int result;


    /* -----------------------------------------------------
       CHECK MAXIMUM NUMBER OF DONATIONS
       ----------------------------------------------------- */

    if (*count >= MAX_DONATIONS)
    {
        printf("\nMaximum number of donations reached.\n");
        return donations;
    }


    /* -----------------------------------------------------
       INCREASE DYNAMIC ARRAY
       ----------------------------------------------------- */

    temp = realloc(
        donations,
        (*count + 1) * sizeof(struct Donation)
    );


    if (temp == NULL)
    {
        printf("\nMemory allocation failed.\n");
        printf("Donation could not be recorded.\n");

        return donations;
    }


    donations = temp;


    /* -----------------------------------------------------
       AUTOMATICALLY ASSIGN DONATION ID
       ----------------------------------------------------- */

    if (*count == 0)
    {
        donations[*count].donationID = 1;
    }
    else
    {
        donations[*count].donationID =
            donations[*count - 1].donationID + 1;
    }


    /* -----------------------------------------------------
       DONOR DETAILS
       ----------------------------------------------------- */

    printf("\n========================================\n");
    printf("          ADD NEW DONATION\n");
    printf("========================================\n");


    printf("\nEnter your name: ");

    readDonationText(
        donations[*count].donorname,
        sizeof(donations[*count].donorname)
    );


    printf("Enter your address: ");

    readDonationText(
        donations[*count].address,
        sizeof(donations[*count].address)
    );


    printf("Enter your contact number: ");

    readDonationText(
        donations[*count].contactnumber,
        sizeof(donations[*count].contactnumber)
    );


    printf("Enter your email address: ");

    readDonationText(
        donations[*count].email,
        sizeof(donations[*count].email)
    );


    /* -----------------------------------------------------
       DONATION TYPE
       ----------------------------------------------------- */

    printf("\n========================================\n");
    printf("           DONATION TYPE\n");
    printf("========================================\n");

    printf("1. Money\n");
    printf("2. Animal Food\n");
    printf("3. Animal Supplies\n");


    do
    {
        printf("Kindly specify your choice number: ");

        result = scanf("%d", &choice);

        clearDonationInputBuffer();


        if (result != 1)
        {
            printf("\nInvalid input.\n");
            choice = 0;
        }
        else if (choice < 1 || choice > 3)
        {
            printf("\nInvalid choice.\n");
            printf("Please enter 1, 2 or 3.\n");
        }

    } while (choice < 1 || choice > 3);


    /* -----------------------------------------------------
       MONEY DONATION
       ----------------------------------------------------- */

    if (choice == 1)
    {
        strcpy(
            donations[*count].donationtype,
            "Money"
        );


        printf("\nEnter donation amount: Rs ");


        while (1)
        {
            result = scanf(
                "%f",
                &donations[*count].amount
            );

            clearDonationInputBuffer();


            if (result != 1)
            {
                printf("Invalid amount.\n");
                printf("Please enter a valid amount: ");
            }
            else if (donations[*count].amount <= 0)
            {
                printf("Invalid amount.\n");
                printf(
                    "Please enter an amount greater than Rs 0: "
                );
            }
            else
            {
                break;
            }
        }


        donations[*count].itemname[0] = '\0';

        donations[*count].quantity = 0;
    }


    /* -----------------------------------------------------
       FOOD DONATION
       ----------------------------------------------------- */

    else if (choice == 2)
    {
        strcpy(
            donations[*count].donationtype,
            "Food"
        );


        printf("\nEnter food item: ");

        readDonationText(
            donations[*count].itemname,
            sizeof(donations[*count].itemname)
        );


        while (strlen(donations[*count].itemname) == 0)
        {
            printf("Food item cannot be empty.\n");
            printf("Enter food item: ");

            readDonationText(
                donations[*count].itemname,
                sizeof(donations[*count].itemname)
            );
        }


        printf("Enter quantity: ");


        while (1)
        {
            result = scanf(
                "%d",
                &donations[*count].quantity
            );

            clearDonationInputBuffer();


            if (result != 1)
            {
                printf("Invalid quantity.\n");
                printf("Please enter a valid quantity: ");
            }
            else if (donations[*count].quantity <= 0)
            {
                printf("Invalid quantity.\n");
                printf(
                    "Please enter a quantity greater than 0: "
                );
            }
            else
            {
                break;
            }
        }


        donations[*count].amount = 0.0f;
    }


    /* -----------------------------------------------------
       SUPPLIES DONATION
       ----------------------------------------------------- */

    else
    {
        strcpy(
            donations[*count].donationtype,
            "Supplies"
        );


        printf("\nEnter supply item: ");

        readDonationText(
            donations[*count].itemname,
            sizeof(donations[*count].itemname)
        );


        while (strlen(donations[*count].itemname) == 0)
        {
            printf("Supply item cannot be empty.\n");
            printf("Enter supply item: ");

            readDonationText(
                donations[*count].itemname,
                sizeof(donations[*count].itemname)
            );
        }


        printf("Enter quantity: ");


        while (1)
        {
            result = scanf(
                "%d",
                &donations[*count].quantity
            );

            clearDonationInputBuffer();


            if (result != 1)
            {
                printf("Invalid quantity.\n");
                printf("Please enter a valid quantity: ");
            }
            else if (donations[*count].quantity <= 0)
            {
                printf("Invalid quantity.\n");
                printf(
                    "Please enter a quantity greater than 0: "
                );
            }
            else
            {
                break;
            }
        }


        donations[*count].amount = 0.0f;
    }


    /* -----------------------------------------------------
       SAVE DONATION
       ----------------------------------------------------- */

    saveDonationToFile(
        donations[*count]
    );


    /* -----------------------------------------------------
       CONFIRMATION
       ----------------------------------------------------- */

    printf("\n========================================\n");
    printf("Donation recorded successfully.\n");

    printf("Donation ID : %d\n",
           donations[*count].donationID);

    printf("Thank you for your support!\n");
    printf("========================================\n");


    (*count)++;


    return donations;
}


/* =========================================================
   DISPLAY ALL DONATIONS
   ========================================================= */

void displayDonations(
    struct Donation donations[],
    int count)
{
    if (count == 0)
    {
        printf("\nNo donations have been recorded.\n");
        return;
    }


    printf("\n========================================\n");
    printf("           DONATION RECORDS\n");
    printf("========================================\n");


    for (int i = 0; i < count; i++)
    {
        printf("\nDonation ID     : %d\n",
               donations[i].donationID);

        printf("Donor Name      : %s\n",
               donations[i].donorname);

        printf("Address         : %s\n",
               donations[i].address);

        printf("Contact Number  : %s\n",
               donations[i].contactnumber);

        printf("Email           : %s\n",
               donations[i].email);

        printf("Donation Type   : %s\n",
               donations[i].donationtype);


        if (strcmp(
                donations[i].donationtype,
                "Money") == 0)
        {
            printf("Amount          : Rs %.2f\n",
                   donations[i].amount);
        }
        else
        {
            printf("Item Donated    : %s\n",
                   donations[i].itemname);

            printf("Quantity        : %d\n",
                   donations[i].quantity);
        }


        printf("----------------------------------------\n");
    }
}


/* =========================================================
   SEARCH DONATION
   ========================================================= */

void searchDonation(
    struct Donation donations[],
    int count,
    int donationID)
{
    if (count == 0)
    {
        printf("\nNo donations have been recorded.\n");
        return;
    }


    for (int i = 0; i < count; i++)
    {
        if (donations[i].donationID == donationID)
        {
            printf("\n========================================\n");
            printf("            DONATION FOUND\n");
            printf("========================================\n");

            printf("Donation ID     : %d\n",
                   donations[i].donationID);

            printf("Donor Name      : %s\n",
                   donations[i].donorname);

            printf("Address         : %s\n",
                   donations[i].address);

            printf("Contact Number  : %s\n",
                   donations[i].contactnumber);

            printf("Email           : %s\n",
                   donations[i].email);

            printf("Donation Type   : %s\n",
                   donations[i].donationtype);


            if (strcmp(
                    donations[i].donationtype,
                    "Money") == 0)
            {
                printf("Amount          : Rs %.2f\n",
                       donations[i].amount);
            }
            else
            {
                printf("Item Donated    : %s\n",
                       donations[i].itemname);

                printf("Quantity        : %d\n",
                       donations[i].quantity);
            }


            printf("========================================\n");

            return;
        }
    }


    printf(
        "\nDonation ID %d was not found.\n",
        donationID
    );
}


/* =========================================================
   SAVE DONATION TO FILE
   ========================================================= */

void saveDonationToFile(
    struct Donation donation)
{
    FILE *file;


    file = fopen("donations.txt", "a");


    if (file == NULL)
    {
        printf("\nError opening donations.txt.\n");
        return;
    }


    fprintf(
        file,
        "%d|%s|%s|%s|%s|%s|%.2f|%s|%d\n",
        donation.donationID,
        donation.donorname,
        donation.address,
        donation.contactnumber,
        donation.email,
        donation.donationtype,
        donation.amount,
        donation.itemname,
        donation.quantity
    );


    fclose(file);
}

