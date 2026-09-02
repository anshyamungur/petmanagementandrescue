#ifndef DONATION_H
#define DONATION_H

#define MAX_DONATIONS 100

struct Donation
{
    int donationID;

    char donorname[50];
    char address[100];
    char contactnumber[20];
    char email[50];

    char donationtype[20];

    float amount;

    char itemname[50];
    int quantity;
};


/* Load donations from file */
void loadDonations(
    struct Donation donations[],
    int *count
);


/* Add a new donation */
struct Donation *addDonation(
    struct Donation *donations,
    int *count
);


/* Display all donations */
void displayDonations(
    struct Donation donations[],
    int count
);


/* Search donation by ID */
void searchDonation(
    struct Donation donations[],
    int count,
    int donationID
);


/* Save donation to file */
void saveDonationToFile(
    struct Donation donation
);

#endif

