#ifndef URGENCY_H
#define URGENCY_H

int calculateUrgency(int injury,
                     int illness,
                     int pain,
                     int age,
                     int dehydration,
                     int condition);

void displayUrgencyLevel(int score);

int getPetAge(int searchID);

void calculatePetUrgency(void);

void viewCriticalAnimals(void);

#endif
