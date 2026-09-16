#include <stdio.h>
#include "raylib.h"
#include "pet.h"
#include "adopter.h"
#include "matching.h"
#include "medical.h"
#include "urgency.h"
#include "donation.h"
#include "reportandstats.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* sameString() is defined (non-static) in matching.c but not
   declared in matching.h; declare it here so the GUI can reuse
   the same case-insensitive comparison used by the matching and
   report logic, without modifying matching.c/matching.h. */
int sameString(const char a[], const char b[]);

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 750

/* =========================================================
   COLOURS
   ========================================================= */

static const Color GUI_BACKGROUND = {245, 247, 250, 255};
static const Color GUI_NAVY = {31, 48, 61, 255};
static const Color GUI_TEAL = {42, 157, 143, 255};
static const Color GUI_TEAL_LIGHT = {88, 190, 180, 255};
static const Color GUI_TEAL_PALE = {232, 246, 243, 255};
static const Color GUI_TEXT = {35, 45, 55, 255};
static const Color GUI_MUTED = {110, 120, 130, 255};
static const Color GUI_BORDER = {220, 225, 230, 255};
static const Color GUI_ERROR = {220, 75, 75, 255};

/* =========================================================
   SCREEN STATES
   ========================================================= */

typedef enum
{
    SCREEN_WELCOME,
    SCREEN_LOGIN,
    SCREEN_DASHBOARD,
    SCREEN_PETS,
    SCREEN_ADOPTERS,
    SCREEN_MATCHING,
    SCREEN_MEDICAL,
    SCREEN_URGENCY,
    SCREEN_DONATIONS,
    SCREEN_REPORTS 
} ScreenState;

typedef enum
{
    ROLE_NONE,
    ROLE_STAFF,
    ROLE_ADOPTER
} UserRole;

UserRole role = ROLE_NONE;
ScreenState previousScreen = SCREEN_WELCOME;

/* =========================================================
   ADOPTER GUI STATE
   ========================================================= */

int adopterAction = 0;
/*
   0 = display
   1 = add
   2 = search
   3 = update
   4 = delete
*/

int selectedAdopter = -1;
char adopterSearchText[50] = "";

char adopterNameInput[50] = "";
char adopterPhoneInput[20] = "";
char adopterEmailInput[50] = "";
char adopterSpeciesInput[20] = "";

char adopterGenderInput[2] = "";
char adopterAddressInput[50] = "";
char adopterScheduleInput[20] = "";
char adopterHomeTypeInput[20] = "";
char adopterMinAgeInput[5] = "";
char adopterMaxAgeInput[5] = "";
char adopterSizeInput[20] = "";
char adopterPreferredGenderInput[2] = "";

char adopterGardenInput[4] = "";
char adopterExperienceInput[30] = "";
char adopterChildrenInput[4] = "";

int adopterFormPage = 1;
int adopterInputField = 0;
int adopterFormField = 0;
int adopterPage = 0;
int adopterPendingDeleteIndex = -1;

/* =========================================================
   PET GUI STATE
   ========================================================= */

int petAction = 0;
/*
   0 = display
   1 = add
   3 = update
*/

int selectedPet = -1;
char petSearchText[50] = "";

char petNameInput[50] = "";
char petSpeciesInput[30] = "";
char petBreedInput[50] = "";
char petAgeInput[5] = "";
char petGenderInput[20] = "";
char petWeightInput[10] = "";
char petSizeInput[20] = "";
char petLocationInput[50] = "";
char petHealthInput[50] = "";
char petAvailabilityInput[50] = "";
char petIDInput[12] = "";
char petFormMessage[100] = "";

int petFormField = 0;
int petInputField = 0;
int petPage = 0;
int petFormStep = 0;
int petPendingDeleteIndex = -1;

/* =========================================================
   MATCHING GUI STATE
   ========================================================= */

int matchingSelectedAdopter = -1;
char matchingMessage[100] = "";
char matchingAdopterIDInput[10] = "";
int matchingIDInputField = 0;

/* =========================================================
   ADOPTER "BROWSE PETS" SPECIES PROMPT STATE
   ========================================================= */

bool adopterSpeciesPromptDone = false;
char adopterSpeciesPromptInput[20] = "";
int adopterSpeciesPromptField = 0;

/* =========================================================
   MEDICAL GUI STATE
   ========================================================= */

typedef struct
{
    int petID;
    int injury;
    int illness;
    int pain;
    int dehydration;
    int condition;
    char treatment[30];
    char date[20];
} MedicalRecordGUI;

#define MAX_MEDICAL_GUI 200

MedicalRecordGUI medicalRecords[MAX_MEDICAL_GUI];
int medicalRecordCount = 0;

int medicalAction = 0;
/*
   0 = view records
   1 = add record
*/

char medicalPetIDInput[10] = "";
int medicalInjuryLevel = 0;
int medicalIllnessLevel = 0;
int medicalPainLevel = 0;
int medicalDehydrationLevel = 0;
int medicalConditionLevel = 0;
int medicalTreatmentChoice = 1;
char medicalDateInput[20] = "";
int medicalInputField = 0;
char medicalMessage[100] = "";

/* =========================================================
   URGENCY GUI STATE
   ========================================================= */

char urgencySearchText[10] = "";
int urgencyInputField = 0;

/* =========================================================
   DONATIONS GUI STATE
   ========================================================= */

struct Donation guiDonations[MAX_DONATIONS];
int guiDonationCount = 0;

int donationAction = 0;
/*
   0 = view donations
   1 = add donation
*/

char donationSearchText[50] = "";
int donationInputField = 0;

char donationNameInput[50] = "";
char donationAddressInput[100] = "";
char donationContactInput[20] = "";
char donationEmailInput[50] = "";
char donationTypeInput[20] = "Money";
char donationAmountInput[20] = "";
char donationItemInput[50] = "";
char donationQuantityInput[10] = "";
char donationMessage[100] = "";

/* =========================================================
   SAVE GUI ADOPTER
   ========================================================= */

void saveAdopterFromGUI(void)
{
    if (adopterCount >= MAX_ADOPTERS)
        return;

    struct Adopter *a = &adopters[adopterCount];

    /* Generate new ID */
    int newID = 1;

    if (adopterCount > 0)
        newID = adopters[adopterCount - 1].ID + 1;

    a->ID = newID;

    /* Personal information */
    strcpy(a->Name, adopterNameInput);
    strcpy(a->Phone, adopterPhoneInput);
    strcpy(a->Email, adopterEmailInput);
    a->Gender = adopterGenderInput[0];
    strcpy(a->Address, adopterAddressInput);

    /* Household information */
    strcpy(a->Schedule, adopterScheduleInput);
    strcpy(a->Hometype, adopterHomeTypeInput);

    a->Garden = (strcmp(adopterGardenInput, "Yes") == 0) ? 1 : 0;
    a->Experience = (strcmp(adopterExperienceInput, "Yes") == 0) ? 1 : 0;

    if (adopterChildrenInput[0] == 'Y' ||
        adopterChildrenInput[0] == 'y')
    {
        a->Children = 1;
    }
    else
    {
        a->Children = 0;
    }

    /* Adoption preferences */
    strcpy(a->PreferredSpecies, adopterSpeciesInput);

    a->minAge = atoi(adopterMinAgeInput);
    a->maxAge = atoi(adopterMaxAgeInput);

    strcpy(a->PreferredSize, adopterSizeInput);
    a->PreferredGender = adopterPreferredGenderInput[0];

    /* Add adopter to array */
    adopterCount++;

    /* Save to adopters.txt */
    saveAdopters();
}

/* =========================================================
   POPULATE GUI FORM FROM EXISTING ADOPTER (for editing)
   ========================================================= */

void populateAdopterForm(int index)
{
    if (index < 0 || index >= adopterCount)
        return;

    struct Adopter *a = &adopters[index];

    strcpy(adopterNameInput, a->Name);
    strcpy(adopterPhoneInput, a->Phone);
    strcpy(adopterEmailInput, a->Email);
    strcpy(adopterAddressInput, a->Address);

    adopterGenderInput[0] = a->Gender;
    adopterGenderInput[1] = '\0';

    strcpy(adopterScheduleInput, a->Schedule);
    strcpy(adopterHomeTypeInput, a->Hometype);

    strcpy(adopterGardenInput, a->Garden ? "Yes" : "No");
    strcpy(adopterExperienceInput, a->Experience ? "Yes" : "No");
    strcpy(adopterChildrenInput, a->Children ? "Yes" : "No");

    strcpy(adopterSpeciesInput, a->PreferredSpecies);

    snprintf(adopterMinAgeInput, sizeof(adopterMinAgeInput), "%d", a->minAge);
    snprintf(adopterMaxAgeInput, sizeof(adopterMaxAgeInput), "%d", a->maxAge);

    strcpy(adopterSizeInput, a->PreferredSize);

    adopterPreferredGenderInput[0] = a->PreferredGender;
    adopterPreferredGenderInput[1] = '\0';
}

/* =========================================================
   SAVE EDITED ADOPTER BACK TO RECORD (update, not add)
   ========================================================= */

void updateAdopterFromGUI(void)
{
    if (selectedAdopter < 0 || selectedAdopter >= adopterCount)
        return;

    struct Adopter *a = &adopters[selectedAdopter];

    /* ID is intentionally left unchanged */

    strcpy(a->Name, adopterNameInput);
    strcpy(a->Phone, adopterPhoneInput);
    strcpy(a->Email, adopterEmailInput);
    a->Gender = adopterGenderInput[0];
    strcpy(a->Address, adopterAddressInput);

    strcpy(a->Schedule, adopterScheduleInput);
    strcpy(a->Hometype, adopterHomeTypeInput);

    a->Garden = (strcmp(adopterGardenInput, "Yes") == 0) ? 1 : 0;
    a->Experience = (strcmp(adopterExperienceInput, "Yes") == 0) ? 1 : 0;

    if (adopterChildrenInput[0] == 'Y' ||
        adopterChildrenInput[0] == 'y')
    {
        a->Children = 1;
    }
    else
    {
        a->Children = 0;
    }

    strcpy(a->PreferredSpecies, adopterSpeciesInput);

    a->minAge = atoi(adopterMinAgeInput);
    a->maxAge = atoi(adopterMaxAgeInput);

    strcpy(a->PreferredSize, adopterSizeInput);
    a->PreferredGender = adopterPreferredGenderInput[0];

    saveAdopters();
}

/* =========================================================
   DELETE ADOPTER FROM GUI (by array index)
   ========================================================= */

void deleteAdopterByIndex(int index)
{
    if (index < 0 || index >= adopterCount)
        return;

    for (int j = index; j < adopterCount - 1; j++)
    {
        adopters[j] = adopters[j + 1];
    }

    adopterCount--;

    if (selectedAdopter == index)
        selectedAdopter = -1;

    saveAdopters();
}

/* =========================================================
   SEARCH FILTER (case-insensitive, matches Name or ID)
   ========================================================= */

int adopterMatchesSearch(int index)
{
    if (adopterSearchText[0] == '\0')
        return 1;

    char nameLower[50];
    char searchLower[50];
    char idText[16];
    int i;

    for (i = 0; adopters[index].Name[i] != '\0' && i < 49; i++)
        nameLower[i] = (char)tolower((unsigned char)adopters[index].Name[i]);
    nameLower[i] = '\0';

    for (i = 0; adopterSearchText[i] != '\0' && i < 49; i++)
        searchLower[i] = (char)tolower((unsigned char)adopterSearchText[i]);
    searchLower[i] = '\0';

    if (strstr(nameLower, searchLower) != NULL)
        return 1;

    snprintf(idText, sizeof(idText), "%d", adopters[index].ID);

    if (strstr(idText, adopterSearchText) != NULL)
        return 1;

    return 0;
}

/* =========================================================
   SAVE GUI PET (add new)
   ========================================================= */

int savePetFromGUI(void)
{
    if (petCount >= MAX_PETS)
        return -1;

    struct Pet *p = &pets[petCount];

    int newID = atoi(petIDInput);

    p->ID = newID;

    strcpy(p->Name, petNameInput);
    strcpy(p->Species, petSpeciesInput);
    strcpy(p->Breed, petBreedInput);
    p->Age = atoi(petAgeInput);
    strcpy(p->Gender, petGenderInput);
    p->Weight = (float)atof(petWeightInput);
    strcpy(p->Size, petSizeInput);
    strcpy(p->Location, petLocationInput);
    strcpy(p->HealthStatus, petHealthInput);
    strcpy(p->Availability, petAvailabilityInput);

    petCount++;

    savePets();

    return newID;
}

/* =========================================================
   POPULATE GUI FORM FROM EXISTING PET (for editing)
   ========================================================= */

void populatePetForm(int index)
{
    if (index < 0 || index >= petCount)
        return;

    struct Pet *p = &pets[index];

    strcpy(petNameInput, p->Name);
    strcpy(petSpeciesInput, p->Species);
    strcpy(petBreedInput, p->Breed);
    snprintf(petAgeInput, sizeof(petAgeInput), "%d", p->Age);
    strcpy(petGenderInput, p->Gender);
    snprintf(petWeightInput, sizeof(petWeightInput), "%.2f", p->Weight);
    strcpy(petSizeInput, p->Size);
    strcpy(petLocationInput, p->Location);
    strcpy(petHealthInput, p->HealthStatus);
    strcpy(petAvailabilityInput, p->Availability);
}

/* =========================================================
   SAVE EDITED PET BACK TO RECORD (update, not add)
   ========================================================= */

void updatePetFromGUI(void)
{
    if (selectedPet < 0 || selectedPet >= petCount)
        return;

    struct Pet *p = &pets[selectedPet];

    /* ID is intentionally left unchanged */

    strcpy(p->Name, petNameInput);
    strcpy(p->Species, petSpeciesInput);
    strcpy(p->Breed, petBreedInput);
    p->Age = atoi(petAgeInput);
    strcpy(p->Gender, petGenderInput);
    p->Weight = (float)atof(petWeightInput);
    strcpy(p->Size, petSizeInput);
    strcpy(p->Location, petLocationInput);
    strcpy(p->HealthStatus, petHealthInput);
    strcpy(p->Availability, petAvailabilityInput);

    savePets();
}

/* =========================================================
   DELETE PET FROM GUI (by array index)
   ========================================================= */

void deletePetByIndex(int index)
{
    if (index < 0 || index >= petCount)
        return;

    for (int j = index; j < petCount - 1; j++)
    {
        pets[j] = pets[j + 1];
    }

    petCount--;

    if (selectedPet == index)
        selectedPet = -1;

    savePets();
}

/* =========================================================
   SEARCH FILTER FOR PETS (case-insensitive, Name or ID)
   ========================================================= */

int petMatchesSearch(int index)
{
    /* Adopters should never see pets that are already adopted */

    if (role == ROLE_ADOPTER &&
        sameString(pets[index].Availability, "Adopted"))
    {
        return 0;
    }

    if (petSearchText[0] == '\0')
        return 1;

    char nameLower[50];
    char searchLower[50];
    char speciesLower[30];
    char idText[16];
    int i;

    for (i = 0; pets[index].Name[i] != '\0' && i < 49; i++)
        nameLower[i] = (char)tolower((unsigned char)pets[index].Name[i]);
    nameLower[i] = '\0';

    for (i = 0; petSearchText[i] != '\0' && i < 49; i++)
        searchLower[i] = (char)tolower((unsigned char)petSearchText[i]);
    searchLower[i] = '\0';

    if (strstr(nameLower, searchLower) != NULL)
        return 1;

    for (i = 0; pets[index].Species[i] != '\0' && i < 29; i++)
        speciesLower[i] = (char)tolower((unsigned char)pets[index].Species[i]);
    speciesLower[i] = '\0';

    if (strstr(speciesLower, searchLower) != NULL)
        return 1;

    snprintf(idText, sizeof(idText), "%d", pets[index].ID);

    if (strstr(idText, petSearchText) != NULL)
        return 1;

    return 0;
}

/* =========================================================
   PAW
   ========================================================= */

void DrawPaw(Vector2 center, float scale, Color color)
{
    DrawCircle(
        (int)center.x,
        (int)center.y,
        30 * scale,
        color
    );

    DrawCircle(
        (int)(center.x - 34 * scale),
        (int)(center.y - 28 * scale),
        13 * scale,
        color
    );

    DrawCircle(
        (int)(center.x - 10 * scale),
        (int)(center.y - 48 * scale),
        13 * scale,
        color
    );

    DrawCircle(
        (int)(center.x + 18 * scale),
        (int)(center.y - 48 * scale),
        13 * scale,
        color
    );

    DrawCircle(
        (int)(center.x + 40 * scale),
        (int)(center.y - 25 * scale),
        13 * scale,
        color
    );
}

/* =========================================================
   CENTRED TEXT
   ========================================================= */

void DrawCenteredText(
    Font font,
    const char *text,
    float y,
    float fontSize,
    float spacing,
    Color color
)
{
    Vector2 size =
        MeasureTextEx(
            font,
            text,
            fontSize,
            spacing
        );

    DrawTextEx(
        font,
        text,
        (Vector2){
            (SCREEN_WIDTH - size.x) / 2.0f,
            y
        },
        fontSize,
        spacing,
        color
    );
}

/* =========================================================
   BUTTON
   ========================================================= */

bool DrawButton(
    Font font,
    Rectangle button,
    const char *text,
    Color normalColor,
    Color hoverColor
)
{
    Vector2 mouse = GetMousePosition();

    bool hovered =
        CheckCollisionPointRec(mouse, button);

    DrawRectangleRounded(
        button,
        0.15f,
        12,
        hovered ? hoverColor : normalColor
    );

    Vector2 textSize =
        MeasureTextEx(
            font,
            text,
            17,
            1
        );

    DrawTextEx(
        font,
        text,
        (Vector2){
            button.x +
            (button.width - textSize.x) / 2.0f,

            button.y +
            (button.height - textSize.y) / 2.0f
        },
        17,
        1,
        WHITE
    );

    return hovered &&
           IsMouseButtonPressed(
               MOUSE_BUTTON_LEFT
           );
}

/* =========================================================
   PET IMAGE DRAWING
   ========================================================= */

void DrawPetImage(
    Texture2D texture,
    Rectangle destination
)
{
    if (texture.id == 0)
    {
        DrawRectangleRounded(
            destination,
            0.12f,
            10,
            GUI_TEAL_PALE
        );

        DrawPaw(
            (Vector2){
                destination.x + destination.width / 2,
                destination.y + destination.height / 2 + 10
            },
            0.30f,
            GUI_TEAL
        );

        return;
    }

    Rectangle source =
    {
        0,
        0,
        (float)texture.width,
        (float)texture.height
    };

    DrawTexturePro(
        texture,
        source,
        destination,
        (Vector2){0, 0},
        0,
        WHITE
    );
}

/* =========================================================
   GENERIC TEXT INPUT BOX
   ========================================================= */

void DrawTextInputBox(
    Font font,
    Rectangle box,
    char *buffer,
    int maxLen,
    const char *placeholder,
    int fieldID,
    int *activeField,
    Vector2 mouse
)
{
    bool active = (*activeField == fieldID);

    DrawRectangleRounded(box, 0.2f, 10, WHITE);

    DrawRectangleRoundedLinesEx(
        box,
        0.2f,
        10,
        2,
        active ? GUI_TEAL : GUI_BORDER
    );

    DrawTextEx(
        font,
        buffer[0] == '\0' ? placeholder : buffer,
        (Vector2){box.x + 12, box.y + (box.height - 14) / 2},
        14,
        1,
        buffer[0] == '\0' ? GUI_MUTED : GUI_TEXT
    );

    if (CheckCollisionPointRec(mouse, box) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        *activeField = fieldID;
    }

    if (active)
    {
        int key = GetCharPressed();

        while (key > 0)
        {
            if (key >= 32 && key <= 125)
            {
                int len = strlen(buffer);

                if (len < maxLen - 1)
                {
                    buffer[len] = (char)key;
                    buffer[len + 1] = '\0';
                }
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            int len = strlen(buffer);

            if (len > 0)
                buffer[len - 1] = '\0';
        }
    }
}

/* =========================================================
   MEDICAL RECORDS FILE IO (GUI SIDE)
   ========================================================= */

void loadMedicalRecordsGUI(void)
{
    FILE *file = fopen("medical_records.txt", "r");

    medicalRecordCount = 0;

    if (file == NULL)
        return;

    while (medicalRecordCount < MAX_MEDICAL_GUI &&
           fscanf(file,
                  " %d|%d|%d|%d|%d|%d|%29[^|]|%19[^\n]",
                  &medicalRecords[medicalRecordCount].petID,
                  &medicalRecords[medicalRecordCount].injury,
                  &medicalRecords[medicalRecordCount].illness,
                  &medicalRecords[medicalRecordCount].pain,
                  &medicalRecords[medicalRecordCount].dehydration,
                  &medicalRecords[medicalRecordCount].condition,
                  medicalRecords[medicalRecordCount].treatment,
                  medicalRecords[medicalRecordCount].date) == 8)
    {
        medicalRecordCount++;
    }

    fclose(file);
}

void saveMedicalRecordGUI(
    int petID,
    int injury,
    int illness,
    int pain,
    int dehydration,
    int condition,
    const char *treatment,
    const char *date
)
{
    FILE *file = fopen("medical_records.txt", "a");

    if (file == NULL)
        return;

    fprintf(
        file,
        "%d|%d|%d|%d|%d|%d|%s|%s\n",
        petID,
        injury,
        illness,
        pain,
        dehydration,
        condition,
        treatment,
        date
    );

    fclose(file);
}

/* =========================================================
   DONATION SEARCH FILTER
   ========================================================= */

int donationMatchesSearch(int index)
{
    if (donationSearchText[0] == '\0')
        return 1;

    char nameLower[50];
    char searchLower[50];
    char idText[16];
    int i;

    for (i = 0; guiDonations[index].donorname[i] != '\0' && i < 49; i++)
        nameLower[i] = (char)tolower((unsigned char)guiDonations[index].donorname[i]);
    nameLower[i] = '\0';

    for (i = 0; donationSearchText[i] != '\0' && i < 49; i++)
        searchLower[i] = (char)tolower((unsigned char)donationSearchText[i]);
    searchLower[i] = '\0';

    if (strstr(nameLower, searchLower) != NULL)
        return 1;

    snprintf(idText, sizeof(idText), "%d", guiDonations[index].donationID);

    if (strstr(idText, donationSearchText) != NULL)
        return 1;

    return 0;
}

/* =========================================================
   REPORTS: KPI STAT CARD
   ========================================================= */

void DrawStatCard(
    Font labelFont,
    Font valueFont,
    Rectangle box,
    const char *label,
    const char *value,
    Color accent
)
{
    DrawRectangleRounded(box, 0.18f, 10, WHITE);

    DrawRectangleRounded(
        (Rectangle){box.x + 16, box.y + 16, 34, 6},
        0.5f,
        6,
        accent
    );

    DrawTextEx(
        labelFont,
        label,
        (Vector2){box.x + 16, box.y + box.height - 46},
        13,
        1,
        GUI_MUTED
    );

    DrawTextEx(
        valueFont,
        value,
        (Vector2){box.x + 16, box.y + box.height - 26},
        22,
        1,
        GUI_NAVY
    );
}

/* =========================================================
   REPORTS: SIMPLE HAND-DRAWN BAR CHART
   ========================================================= */

void DrawSimpleBarChart(
    Font titleFont,
    Font labelFont,
    Rectangle area,
    const char *title,
    const char *labels[],
    int values[],
    Color colors[],
    int count
)
{
    DrawRectangleRounded(area, 0.06f, 10, WHITE);

    DrawTextEx(
        titleFont,
        title,
        (Vector2){area.x + 18, area.y + 16},
        16,
        1,
        GUI_NAVY
    );

    int maxVal = 1;

    for (int i = 0; i < count; i++)
    {
        if (values[i] > maxVal)
            maxVal = values[i];
    }

    float chartBottom = area.y + area.height - 34;
    float chartTop = area.y + 60;
    float chartHeight = chartBottom - chartTop;

    float barAreaWidth = area.width - 40;
    float gap = 18;
    float barWidth = (barAreaWidth - gap * (count - 1)) / count;

    DrawLine(
        (int)(area.x + 20),
        (int)chartBottom,
        (int)(area.x + area.width - 20),
        (int)chartBottom,
        GUI_BORDER
    );

    for (int i = 0; i < count; i++)
    {
        float barHeight =
            ((float)values[i] / (float)maxVal) * chartHeight;

        if (barHeight < 3 && values[i] > 0)
            barHeight = 3;

        Rectangle bar =
        {
            area.x + 20 + i * (barWidth + gap),
            chartBottom - barHeight,
            barWidth,
            barHeight
        };

        DrawRectangleRounded(bar, 0.35f, 8, colors[i]);

        char valText[16];

        snprintf(valText, sizeof(valText), "%d", values[i]);

        Vector2 valSize = MeasureTextEx(labelFont, valText, 13, 1);

        DrawTextEx(
            labelFont,
            valText,
            (Vector2){
                bar.x + (bar.width - valSize.x) / 2,
                bar.y - 20
            },
            13,
            1,
            GUI_TEXT
        );

        Vector2 labelSize = MeasureTextEx(labelFont, labels[i], 12, 1);

        DrawTextEx(
            labelFont,
            labels[i],
            (Vector2){
                bar.x + (bar.width - labelSize.x) / 2,
                chartBottom + 10
            },
            12,
            1,
            GUI_MUTED
        );
    }
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "PetRescue Management System"
    );

    SetTargetFPS(60);

    Font regularFont =
        LoadFont("assets/fonts/Inter-Regular.otf");

    Font semiBoldFont =
        LoadFont("assets/fonts/Inter-SemiBold.otf");

    /* =====================================================
       PET IMAGES
       ===================================================== */

    Texture2D petImage1 =
        LoadTexture("assets/pets/Buddy.png");

    Texture2D petImage2 =
        LoadTexture("assets/pets/Luna.png");

Texture2D petImage3 =
        LoadTexture("assets/pets/Mimi.png");
Texture2D petImage4 =
    LoadTexture("assets/pets/Rex.png");

Texture2D petImage5 =
    LoadTexture("assets/pets/Lily.png");

Texture2D petImage6 =
    LoadTexture("assets/pets/Yolk.png");

Texture2D petImage7 =
    LoadTexture("assets/pets/Filou.png");
loadPets();
createAdopterFile();
loadAdopters();
createMedicalFile();
loadDonations(guiDonations, &guiDonationCount);
    ScreenState screen =
        SCREEN_WELCOME;

    float elapsed = 0.0f;

    /* Login fields */

    char username[64] = "";
    char password[64] = "";

    int activeField = 0;

    bool loginError = false;

    /* =====================================================
       MAIN LOOP
       ===================================================== */

    while (!WindowShouldClose())
    {
        float delta =
            GetFrameTime();

        elapsed += delta;

        Vector2 mouse =
            GetMousePosition();

        /* =================================================
           WELCOME SCREEN
           ================================================= */

        if (screen == SCREEN_WELCOME)
        {
            BeginDrawing();

            ClearBackground(
                GUI_BACKGROUND
            );

            DrawCircle(
                100,
                100,
                190,
                (Color){
                    235,
                    244,
                    243,
                    255
                }
            );

            DrawCircle(
                1100,
                650,
                240,
                (Color){
                    235,
                    244,
                    243,
                    255
                }
            );

            float fade =
                elapsed / 1.5f;

            if (fade > 1.0f)
                fade = 1.0f;

            float pawY =
                285.0f +
                sinf(elapsed * 2.0f) *
                8.0f;

            float pulse =
                1.0f +
                sinf(elapsed * 2.0f) *
                0.04f;

            Color pawColor =
                GUI_TEAL;

            pawColor.a =
                (unsigned char)
                (255 * fade);

            DrawPaw(
                (Vector2){
                    600,
                    pawY
                },
                pulse,
                pawColor
            );

            Color titleColor =
                GUI_NAVY;

            titleColor.a =
                (unsigned char)
                (255 * fade);

            DrawCenteredText(
                semiBoldFont,
                "MauPaws",
                380,
                48,
                1,
                titleColor
            );

            Color subtitleColor =
                GUI_MUTED;

            subtitleColor.a =
                (unsigned char)
                (255 * fade);

            DrawCenteredText(
                regularFont,
                "Pet Management & Rescue System",
                440,
                19,
                1,
                subtitleColor
            );

            float messageFade =
                (elapsed - 0.8f) / 1.0f;

            if (messageFade < 0.0f)
                messageFade = 0.0f;

            if (messageFade > 1.0f)
                messageFade = 1.0f;

            Color messageColor =
                GUI_TEXT;

            messageColor.a =
                (unsigned char)
                (255 * messageFade);

            DrawCenteredText(
                regularFont,
                "Connecting people with animals in need.",
                500,
                16,
                1,
                messageColor
            );

            if (elapsed > 2.5f)
            {
                Rectangle button =
                {
                    475,
                    570,
                    250,
                    55
                };

                bool hovered =
                    CheckCollisionPointRec(
                        mouse,
                        button
                    );

                DrawRectangleRounded(
                    button,
                    0.15f,
                    12,
                    hovered
                    ? GUI_TEAL_LIGHT
                    : GUI_TEAL
                );

                DrawCenteredText(
                    semiBoldFont,
                    "Continue",
                    588,
                    17,
                    1,
                    WHITE
                );

                if (hovered &&
                    IsMouseButtonPressed(
                        MOUSE_BUTTON_LEFT))
                {
                    screen =
                        SCREEN_LOGIN;

                    elapsed = 0.0f;
                }
            }

            /* Bottom tagline, using small paw icons as separators
               instead of a bullet character (the custom fonts
               loaded for this app don't include that glyph). */

            {
                const char *tagPart1 = "Animal welfare";
                const char *tagPart2 = "Adoption";
                const char *tagPart3 = "Rescue";

                float tagFontSize = 13.0f;
                float tagSpacing = 1.0f;
                float tagGap = 14.0f;
                float tagIconWidth = 22.0f;

                Vector2 tagSize1 = MeasureTextEx(regularFont, tagPart1, tagFontSize, tagSpacing);
                Vector2 tagSize2 = MeasureTextEx(regularFont, tagPart2, tagFontSize, tagSpacing);
                Vector2 tagSize3 = MeasureTextEx(regularFont, tagPart3, tagFontSize, tagSpacing);

                float tagTotalWidth =
                    tagSize1.x + tagGap + tagIconWidth + tagGap +
                    tagSize2.x + tagGap + tagIconWidth + tagGap +
                    tagSize3.x;

                float tagX = (SCREEN_WIDTH - tagTotalWidth) / 2.0f;
                float tagY = 700.0f;

                DrawTextEx(regularFont, tagPart1, (Vector2){tagX, tagY}, tagFontSize, tagSpacing, GUI_MUTED);
                tagX += tagSize1.x + tagGap;

                DrawPaw((Vector2){tagX + tagIconWidth / 2.0f, tagY + tagFontSize / 2.0f}, 0.13f, GUI_MUTED);
                tagX += tagIconWidth + tagGap;

                DrawTextEx(regularFont, tagPart2, (Vector2){tagX, tagY}, tagFontSize, tagSpacing, GUI_MUTED);
                tagX += tagSize2.x + tagGap;

                DrawPaw((Vector2){tagX + tagIconWidth / 2.0f, tagY + tagFontSize / 2.0f}, 0.13f, GUI_MUTED);
                tagX += tagIconWidth + tagGap;

                DrawTextEx(regularFont, tagPart3, (Vector2){tagX, tagY}, tagFontSize, tagSpacing, GUI_MUTED);
            }

            EndDrawing();
        }

        /* =================================================
           LOGIN SCREEN
           ================================================= */

        else if (screen == SCREEN_LOGIN)
        {
            BeginDrawing();

            ClearBackground(
                GUI_BACKGROUND
            );

            DrawCircle(
                1050,
                100,
                190,
                (Color){
                    235,
                    244,
                    243,
                    255
                }
            );

            DrawCircle(
                100,
                680,
                170,
                (Color){
                    235,
                    244,
                    243,
                    255
                }
            );

            DrawPaw(
                (Vector2){
                    600,
                    110
                },
                0.6f,
                GUI_TEAL
            );

            DrawCenteredText(
                semiBoldFont,
                "Welcome to MauPaws",
                165,
                30,
                1,
                GUI_NAVY
            );

            DrawCenteredText(
                regularFont,
                "Choose your account type",
                210,
                16,
                1,
                GUI_MUTED
            );

            /* =================================================
               ROLE BUTTONS
               ================================================= */

            Rectangle staffButton =
            {
                395,
                260,
                195,
                55
            };

            Rectangle adopterButton =
            {
                610,
                260,
                195,
                55
            };

            bool staffHover =
                CheckCollisionPointRec(
                    mouse,
                    staffButton
                );

            bool adopterHover =
                CheckCollisionPointRec(
                    mouse,
                    adopterButton
                );

            Color staffColor =
                role == ROLE_STAFF
                ? GUI_TEAL
                : (staffHover
                   ? GUI_TEAL_PALE
                   : WHITE);

            Color adopterColor =
                role == ROLE_ADOPTER
                ? GUI_TEAL
                : (adopterHover
                   ? GUI_TEAL_PALE
                   : WHITE);

            DrawRectangleRounded(
                staffButton,
                0.15f,
                12,
                staffColor
            );

            DrawRectangleRounded(
                adopterButton,
                0.15f,
                12,
                adopterColor
            );

            DrawRectangleRoundedLinesEx(
                staffButton,
                0.15f,
                12,
                2,
                role == ROLE_STAFF
                ? GUI_TEAL
                : GUI_BORDER
            );

            DrawRectangleRoundedLinesEx(
                adopterButton,
                0.15f,
                12,
                2,
                role == ROLE_ADOPTER
                ? GUI_TEAL
                : GUI_BORDER
            );

            /* =================================================
               STAFF TEXT
               ================================================= */

            Vector2 staffTextSize =
                MeasureTextEx(
                    semiBoldFont,
                    "STAFF",
                    16,
                    1
                );

            DrawTextEx(
                semiBoldFont,
                "STAFF",
                (Vector2){
                    staffButton.x +
                    (staffButton.width -
                     staffTextSize.x) / 2.0f,

                    staffButton.y +
                    (staffButton.height -
                     staffTextSize.y) / 2.0f
                },
                16,
                1,
                role == ROLE_STAFF
                ? WHITE
                : GUI_TEXT
            );

            /* =================================================
               ADOPTER TEXT
               ================================================= */

            Vector2 adopterTextSize =
                MeasureTextEx(
                    semiBoldFont,
                    "ADOPTER",
                    16,
                    1
                );

            DrawTextEx(
                semiBoldFont,
                "ADOPTER",
                (Vector2){
                    adopterButton.x +
                    (adopterButton.width -
                     adopterTextSize.x) / 2.0f,

                    adopterButton.y +
                    (adopterButton.height -
                     adopterTextSize.y) / 2.0f
                },
                16,
                1,
                role == ROLE_ADOPTER
                ? WHITE
                : GUI_TEXT
            );

            /* =================================================
               ROLE CLICK
               ================================================= */

            if (staffHover &&
                IsMouseButtonPressed(
                    MOUSE_BUTTON_LEFT))
            {
                role =
                    ROLE_STAFF;

                loginError =
                    false;
            }

            if (adopterHover &&
                IsMouseButtonPressed(
                    MOUSE_BUTTON_LEFT))
            {
                role =
                    ROLE_ADOPTER;

                loginError =
                    false;
            }

            /* =================================================
               USERNAME
               ================================================= */

            DrawTextEx(
                semiBoldFont,
                "Username",
                (Vector2){
                    395,
                    340
                },
                14,
                1,
                GUI_TEXT
            );

            Rectangle usernameBox =
            {
                395,
                360,
                410,
                55
            };

            DrawRectangleRounded(
                usernameBox,
                0.12f,
                10,
                WHITE
            );

            DrawRectangleRoundedLinesEx(
                usernameBox,
                0.12f,
                10,
                2,
                activeField == 1
                ? GUI_TEAL
                : GUI_BORDER
            );

            DrawTextEx(
                regularFont,
                username,
                (Vector2){
                    415,
                    378
                },
                16,
                1,
                GUI_TEXT
            );

            /* =================================================
               PASSWORD
               ================================================= */

            DrawTextEx(
                semiBoldFont,
                "Password",
                (Vector2){
                    395,
                    430
                },
                14,
                1,
                GUI_TEXT
            );

            Rectangle passwordBox =
            {
                395,
                450,
                410,
                55
            };

            DrawRectangleRounded(
                passwordBox,
                0.12f,
                10,
                WHITE
            );

            DrawRectangleRoundedLinesEx(
                passwordBox,
                0.12f,
                10,
                2,
                activeField == 2
                ? GUI_TEAL
                : GUI_BORDER
            );

            char passwordDisplay[64];

            int passwordLength =
                (int)strlen(password);


for (int i = 0;
                 i < passwordLength;
                 i++)
            {
                passwordDisplay[i] =
                    '*';
            }

            passwordDisplay[
                passwordLength
            ] = '\0';

            DrawTextEx(
                regularFont,
                passwordDisplay,
                (Vector2){
                    415,
                    468
                },
                16,
                1,
                GUI_TEXT
            );

            /* =================================================
               FIELD CLICKS
               ================================================= */

            if (CheckCollisionPointRec(
                    mouse,
                    usernameBox) &&
                IsMouseButtonPressed(
                    MOUSE_BUTTON_LEFT))
            {
                activeField = 1;
            }

            if (CheckCollisionPointRec(
                    mouse,
                    passwordBox) &&
                IsMouseButtonPressed(
                    MOUSE_BUTTON_LEFT))
            {
                activeField = 2;
            }

            /* =================================================
               TEXT INPUT
               ================================================= */

            int key =
                GetCharPressed();

            while (key > 0)
            {
                if (key >= 32 &&
                    key <= 125)
                {
                    if (activeField == 1)
                    {
                        int length =
                            (int)strlen(username);

                        if (length < 63)
                        {
                            username[length] =
                                (char)key;

                            username[length + 1] =
                                '\0';
                        }
                    }
                    else if (activeField == 2)
                    {
                        int length =
                            (int)strlen(password);

                        if (length < 63)
                        {
                            password[length] =
                                (char)key;

                            password[length + 1] =
                                '\0';
                        }
                    }
                }

                key =
                    GetCharPressed();
            }

            if (IsKeyPressed(
                    KEY_BACKSPACE))
            {
                if (activeField == 1)
                {
                    int length =
                        (int)strlen(username);

                    if (length > 0)
                        username[length - 1] =
                            '\0';
                }
                else if (activeField == 2)
                {
                    int length =
                        (int)strlen(password);

                    if (length > 0)
                        password[length - 1] =
                            '\0';
                }
            }

            /* =================================================
               LOGIN BUTTON
               ================================================= */

            Rectangle loginButton =
            {
                395,
                535,
                410,
                55
            };

            bool loginHover =
                CheckCollisionPointRec(
                    mouse,
                    loginButton
                );

            DrawRectangleRounded(
                loginButton,
                0.15f,
                12,
                loginHover
                ? GUI_TEAL_LIGHT
                : GUI_TEAL
            );

            DrawCenteredText(
                semiBoldFont,
                "LOGIN",
                553,
                17,
                1,
                WHITE
            );

            if (loginHover &&
                IsMouseButtonPressed(
                    MOUSE_BUTTON_LEFT))
            {
                if (role == ROLE_NONE)
                {
                    loginError = true;
                }
                else
                {
                    screen =
                        SCREEN_DASHBOARD;

                    elapsed = 0.0f;
                }
            }

            if (loginError)
            {
                DrawCenteredText(
                    regularFont,
                    "Please select Staff or Adopter.",
                    610,
                    14,
                    1,
                    GUI_ERROR
                );
            }

            DrawCenteredText(
                regularFont,
                "GUI authentication trial",
                700,
                12,
                1,
                GUI_MUTED
            );

            EndDrawing();
        }

        /* =================================================
           DASHBOARD
           ================================================= */

        else if (screen == SCREEN_DASHBOARD)
        {
            BeginDrawing();

            ClearBackground(
                GUI_BACKGROUND
            );

            /* =================================================
               TOP BAR
               ================================================= */

            DrawRectangle(
                0,
                0,
                SCREEN_WIDTH,
                78,
                GUI_NAVY
            );

            DrawPaw(
                (Vector2){
                    45,
                    40
                },
                0.30f,
                GUI_TEAL_LIGHT
            );

            DrawTextEx(
                semiBoldFont,
                "MauPaws",
                (Vector2){
                    75,
                    24
                },
                24,
                1,
                WHITE
            );

            DrawTextEx(
                regularFont,
                role == ROLE_STAFF
                ? "Staff Portal"
                : "Adopter Portal",
                (Vector2){
                    870,
                    28
                },
                15,
                1,
                (Color){
                    210,
                    225,
                    225,
                    255
                }
            );

            /* =================================================
               SIDEBAR
               ================================================= */

            DrawRectangle(
                0,
                78,
                230,
                SCREEN_HEIGHT - 78,
                WHITE
            );

            DrawLine(
                230,
                78,
                230,
                SCREEN_HEIGHT,
                GUI_BORDER
            );

            DrawTextEx(
                semiBoldFont,
                role == ROLE_STAFF
                ? "STAFF MENU"
                : "ADOPTER MENU",
                (Vector2){
                    30,
                    110
                },
                13,
                1,
                GUI_MUTED
            );

            const char *staffMenuItems[] =
            {
                "Dashboard",
                "Pets",
                "Adopters",
                "Matching",
                "Medical",
                "Urgency",
                "Donations",
                "Reports"
            };

            ScreenState staffMenuScreens[] =
            {
                SCREEN_DASHBOARD,
                SCREEN_PETS,
                SCREEN_ADOPTERS,
                SCREEN_MATCHING,
                SCREEN_MEDICAL,
                SCREEN_URGENCY,
                SCREEN_DONATIONS,
                SCREEN_REPORTS
            };

            /* Adopters only get a simplified menu: Pets and Matching */

            const char *adopterMenuItems[] =
            {
                "Pets",
                "Matching"
            };

            ScreenState adopterMenuScreens[] =
            {
                SCREEN_PETS,
                SCREEN_MATCHING
            };

            const char **menuItems =
                role == ROLE_STAFF
                ? staffMenuItems
                : adopterMenuItems;

            ScreenState *menuScreens =
                role == ROLE_STAFF
                ? staffMenuScreens
                : adopterMenuScreens;

            int menuCount =
                role == ROLE_STAFF
                ? 8
                : 2;

            for (int i = 0;
                 i < menuCount;
                 i++)
            {
                Rectangle item =
                {
                    18,
                    145 + i * 52,
                    194,
                    42
                };

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                    CheckCollisionPointRec(mouse, item))
                {
                    screen = menuScreens[i];
                }

                bool hover =
                    CheckCollisionPointRec(
                        mouse,
                        item
                    );

                bool isCurrent =
                    (menuScreens[i] == screen);

                Color itemColor =
                    isCurrent
                    ? GUI_TEAL_PALE
                    : (hover
                       ? GUI_TEAL_PALE
                       : WHITE);

                DrawRectangleRounded(
                    item,
                    0.15f,
                    10,
                    itemColor
                );

                DrawTextEx(
                    regularFont,
                    menuItems[i],
                    (Vector2){
                        38,
                        157 + i * 52
                    },
                    15,
                    1,
                    isCurrent
                    ? GUI_TEAL
                    : GUI_TEXT
                );
            }

            /* =================================================
               MAIN TITLE
               ================================================= */

            DrawTextEx(
                semiBoldFont,
                role == ROLE_STAFF
                ? "Good morning, Staff"
                : "Welcome back!",
                (Vector2){
                    270,
                    115
                },
                28,
                1,
                GUI_NAVY
            );

            DrawTextEx(
                regularFont,
                role == ROLE_STAFF
                ? "Here's an overview of the rescue centre."
                : "Find your perfect companion.",
                (Vector2){
                    270,
                    155
                },
                15,
                1,
                GUI_MUTED
            );

            /* =================================================
               STAFF DASHBOARD
               ================================================= */

            if (role == ROLE_STAFF)
            {
                /* =========================================
                   GATHER REAL-TIME STATS FOR THIS DASHBOARD
                   ========================================= */

                loadMedicalRecordsGUI();

                int urgEntryIndex[MAX_MEDICAL_GUI];
                int urgEntryScore[MAX_MEDICAL_GUI];
                int urgEntryCount = 0;
                int urgentCaseCount = 0;
                float urgencyScoreSum = 0.0f;

                for (int i = 0; i < medicalRecordCount; i++)
                {
                    int age = getPetAge(medicalRecords[i].petID);

                    if (age == -1)
                        continue;

                    int petIdx = -1;

                    for (int p = 0; p < petCount; p++)
                    {
                        if (pets[p].ID == medicalRecords[i].petID)
                        {
                            petIdx = p;
                            break;
                        }
                    }

                    if (petIdx == -1)
                        continue;

                    int score = calculateUrgency(
                        medicalRecords[i].injury,
                        medicalRecords[i].illness,
                        medicalRecords[i].pain,
                        age,
                        medicalRecords[i].dehydration,
                        medicalRecords[i].condition
                    );

                    if (score > 100)
                        score = 100;

                    urgEntryIndex[urgEntryCount] = petIdx;
                    urgEntryScore[urgEntryCount] = score;
                    urgEntryCount++;

                    urgencyScoreSum += (float)score;

                    if (score >= 76)
                        urgentCaseCount++;
                }

                /* Sort descending by urgency score (most urgent first) */

                for (int i = 0; i < urgEntryCount - 1; i++)
                {
                    for (int j = 0; j < urgEntryCount - i - 1; j++)
                    {
                        if (urgEntryScore[j] < urgEntryScore[j + 1])
                        {
                            int ts = urgEntryScore[j];
                            urgEntryScore[j] = urgEntryScore[j + 1];
                            urgEntryScore[j + 1] = ts;

                            int ti = urgEntryIndex[j];
                            urgEntryIndex[j] = urgEntryIndex[j + 1];
                            urgEntryIndex[j + 1] = ti;
                        }
                    }
                }

                float averageUrgency =
                    urgEntryCount > 0
                    ? (urgencyScoreSum / urgEntryCount)
                    : 0.0f;

                char totalPetsText[16];
                char adoptersCountText[16];
                char urgentCaseText[16];

                snprintf(totalPetsText, sizeof(totalPetsText), "%d", petCount);
                snprintf(adoptersCountText, sizeof(adoptersCountText), "%d", adopterCount);
                snprintf(urgentCaseText, sizeof(urgentCaseText), "%d", urgentCaseCount);

                const char *statTitles[] =
                {
                    "Total Pets",
                    "Adopters",
                    "Urgent Cases"
                };

                const char *statValues[] =
                {
                    totalPetsText,
                    adoptersCountText,
                    urgentCaseText
                };

                Color statColors[] =
                {
                    GUI_TEAL,
                    (Color){
                        80,
                        130,
                        190,
                        255
                    },
                    GUI_ERROR
                };

                for (int i = 0;
                     i < 3;
                     i++)
                {
                    Rectangle card =
                    {
                        270 + i * 205,
                        195,
                        185,
                        100
                    };

                    DrawRectangleRounded(
                        card,
                        0.12f,
                        12,
                        WHITE
                    );

                    DrawRectangle(
                        (int)card.x,
                        (int)card.y,
                        5,
                        (int)card.height,
                        statColors[i]
                    );

                    DrawTextEx(
                        regularFont,
                        statTitles[i],
                        (Vector2){
                            card.x + 20,
                            card.y + 18
                        },
                        13,
                        1,
                        GUI_MUTED
                    );

                    DrawTextEx(
                        semiBoldFont,
                        statValues[i],
                        (Vector2){
                            card.x + 20,
                            card.y + 43
                        },
                        28,
                        1,
                        GUI_NAVY
                    );
                }

                /* =================================================
                   URGENCY
                   ================================================= */

                DrawTextEx(
                    semiBoldFont,
                    "Urgency Overview",
                    (Vector2){
                        270,
                        330
                    },
                    19,
                    1,
                    GUI_NAVY
                );

                char urgencySubtitle[80];

                if (urgEntryCount > 0)
                {
                    snprintf(
                        urgencySubtitle,
                        sizeof(urgencySubtitle),
                        "Average urgency across %d medical record(s): %.0f/100",
                        urgEntryCount,
                        averageUrgency
                    );
                }
                else
                {
                    strcpy(urgencySubtitle, "No medical records on file yet.");
                }

                DrawTextEx(
                    regularFont,
                    urgencySubtitle,
                    (Vector2){
                        270,
                        360
                    },
                    13,
                    1,
                    GUI_MUTED
                );

                Rectangle urgencyBar =
                {
                    290,
                    410,
                    600,
                    22
                };

                DrawRectangleRounded(
                    (Rectangle){
                        290,
                        410,
                        150,
                        22
                    },
                    0.4f,
                    8,
                    (Color){
                        76,
                        175,
                        80,
                        255
                    }
                );

                DrawRectangle(
                    440,
                    410,
                    150,
                    22,
                    (Color){
                        255,
                        193,
                        7,
                        255
                    }
                );

                DrawRectangle(
                    590,
                    410,
                    150,
                    22,
                    (Color){
                        255,
                        152,
                        0,
                        255
                    }
                );

                DrawRectangleRounded(
                    (Rectangle){
                        740,
                        410,
                        150,
                        22
                    },
                    0.4f,
                    8,
                    (Color){
                        220,
                        53,
                        69,
                        255
                    }
                );

                /* Keep the gentle live-motion feel, but now the
                   indicator oscillates around the REAL average
                   urgency instead of a fully made-up number. */

                float urgency =
                    averageUrgency +
                    sinf(elapsed * 1.5f) * 5.0f;

                if (urgency < 0.0f)
                    urgency = 0.0f;

                if (urgency > 100.0f)
                    urgency = 100.0f;

                float indicatorX =
                    urgencyBar.x +
                    (urgency / 100.0f) *
                    urgencyBar.width;

                DrawCircle(
                    (int)indicatorX,
                    421,
                    10,
                    GUI_NAVY
                );

                DrawTextEx(
                    regularFont,
                    "LOW",
                    (Vector2){
                        290,
                        445
                    },
                    12,
                    1,
                    (Color){
                        76,
                        150,
                        80,
                        255
                    }
                );

                DrawTextEx(
                    regularFont,
                    "MODERATE",
                    (Vector2){
                        430,
                        445
                    },
                    12,
                    1,
                    (Color){
                        190,
                        145,
                        0,
                        255
                    }
                );

                DrawTextEx(
                    regularFont,
                    "HIGH",
                    (Vector2){
                        610,
                        445
                    },
                    12,
                    1,
                    (Color){
                        220,
                        120,
                        0,
                        255
                    }
                );

                DrawTextEx(
                    regularFont,
                    "CRITICAL",
                    (Vector2){
                        755,
                        445
                    },
                    12,
                    1,
                    GUI_ERROR
                );

                /* =================================================
                   ANIMALS REQUIRING ATTENTION
                   ================================================= */

                DrawTextEx(
                    semiBoldFont,
                    "Animals Requiring Attention",
                    (Vector2){
                        270,
                        495
                    },
                    19,
                    1,
                    GUI_NAVY
                );

                int attentionCount =
                    urgEntryCount < 3 ? urgEntryCount : 3;

                if (attentionCount == 0)
                {
                    DrawTextEx(
                        regularFont,
                        "No pets currently require urgent attention.",
                        (Vector2){
                            270,
                            535
                        },
                        14,
                        1,
                        GUI_MUTED
                    );
                }

                for (int i = 0;
                     i < attentionCount;
                     i++)
                {
                    int p = urgEntryIndex[i];
                    int score = urgEntryScore[i];

                    const char *levelLabel;
                    Color levelColor;

                    if (score >= 76)
                    {
                        levelLabel = "CRITICAL";
                        levelColor = GUI_ERROR;
                    }
                    else if (score >= 51)
                    {
                        levelLabel = "HIGH";
                        levelColor = (Color){255, 152, 0, 255};
                    }
                    else if (score >= 26)
                    {
                        levelLabel = "MODERATE";
                        levelColor = (Color){255, 193, 7, 255};
                    }
                    else
                    {
                        levelLabel = "LOW";
                        levelColor = (Color){76, 175, 80, 255};
                    }

                    Rectangle petCard =
                    {
                        270 + i * 205,
                        530,
                        185,
                        125
                    };

                    DrawRectangleRounded(
                        petCard,
                        0.12f,
                        12,
                        WHITE
                    );

                    DrawCircle(
                        (int)(
                            petCard.x + 45
                        ),
                        (int)(
                            petCard.y + 45
                        ),
                        28,
                        GUI_TEAL_PALE
                    );

                    DrawPaw(
                        (Vector2){
                            petCard.x + 45,
                            petCard.y + 47
                        },
                        0.22f,
                        GUI_TEAL
                    );

                    DrawTextEx(
                        semiBoldFont,
                        pets[p].Name,
                        (Vector2){
                            petCard.x + 85,
                            petCard.y + 25
                        },
                        16,
                        1,
                        GUI_TEXT
                    );

                    DrawTextEx(
                        regularFont,
                        "Urgency:",
                        (Vector2){
                            petCard.x + 85,
                            petCard.y + 52
                        },
                        12,
                        1,
                        GUI_MUTED
                    );

                    DrawTextEx(
                        semiBoldFont,
                        levelLabel,
                        (Vector2){
                            petCard.x + 85,
                            petCard.y + 70
                        },
                        13,
                        1,
                        levelColor
                    );
                }
            }

            /* =================================================
               ADOPTER DASHBOARD
               ================================================= */

            else
            {
                Rectangle searchCard =
                {
                    270,
                    210,
                    820,
                    130
                };

                DrawRectangleRounded(
                    searchCard,
                    0.12f,
                    12,
                    WHITE
                );

                DrawTextEx(
                    semiBoldFont,
                    "Find Your Companion",
                    (Vector2){
                        295,
                        235
                    },
                    20,
                    1,
                    GUI_NAVY
                );

                DrawTextEx(
                    regularFont,
                    "Browse available animals and discover your perfect match.",
                    (Vector2){
                        295,
                        270
                    },
                    14,
                    1,
                    GUI_MUTED
                );

                Rectangle browseButton =
                {
                    295,
                    300,
                    180,
                    42
                };

                if (DrawButton(
                        semiBoldFont,
                        browseButton,
                        "Browse Pets",
                        GUI_TEAL,
                        GUI_TEAL_LIGHT))
                {
                    screen = SCREEN_PETS;
                }

                /* =================================================
                   FEATURED PETS
                   ================================================= */

                DrawTextEx(
                    semiBoldFont,
                    "Featured Pets",
                    (Vector2){
                        270,
                        385
                    },
                    20,
                    1,
                    GUI_NAVY
                );

                /*
                   These three have real images.
                   The other three are additional pet cards
                   with a paw placeholder until image files
                   are available.
                */

const char *featured[] =
                {
                    "Buddy",
                    "Luna",
                    "Mimi",
                    "Rex",
                    "Yolk",
                    "Filou",
                    "Lily" 
                };

                for (int i = 0;
                     i < 7;
                     i++)
                {
                    int column =
                        i % 4;

                    int row =
                        i / 4;

                    Rectangle card =
                    {
                        270 + column * 225,
                        415 + row * 150,
                        205,
                        130
                    };

                    DrawRectangleRounded(
                        card,
                        0.12f,
                        12,
                        WHITE
                    );

                    
Texture2D currentPetImage;
    
if (i == 0)
    currentPetImage = petImage1;
else if (i == 1)
    currentPetImage = petImage2;
else if (i == 2)
    currentPetImage = petImage3;
else if (i == 3)
    currentPetImage = petImage4;
else if (i == 4)
    currentPetImage = petImage5;
else if (i == 5)
    currentPetImage = petImage6;
else
    currentPetImage = petImage7;

DrawPetImage(
    currentPetImage,
    (Rectangle){
        card.x + 12,
        card.y + 12,
        75,
        75
    }
);

                    DrawTextEx(
                        semiBoldFont,
                        featured[i],
                        (Vector2){
                            card.x + 100,
                            card.y + 25
                        },
                        16,
                        1,
                        GUI_TEXT
                    );

                    DrawTextEx(
                        regularFont,
                        "Available",
                        (Vector2){
                            card.x + 100,
                            card.y + 50
                        },
                        11,
                        1,
                        GUI_TEAL
                    );

                    DrawTextEx(
                        regularFont,
                        "for adoption",
                        (Vector2){
                            card.x + 100,
                            card.y + 67
                        },
                        11,
                        1,
                        GUI_MUTED
                    );
                }
            }

            /* =================================================
               LOGOUT
               ================================================= */

            Rectangle logoutButton =
            {
                30,
                675,
                170,
                42
            };

            if (DrawButton(
                    semiBoldFont,
                    logoutButton,
                    "LOG OUT",
                    GUI_NAVY,
                    GUI_TEAL))
            {
                screen =
                    SCREEN_LOGIN;

                role =
                    ROLE_NONE;

                username[0] =
                    '\0';

                password[0] =
                    '\0';

                activeField =
                    0;

                loginError =
                    false;

                elapsed =
                    0.0f;
            }

            EndDrawing();
        }
    
        /* =================================================
           PETS SCREEN
           ================================================= */

        else if (screen == SCREEN_PETS)
        {
            /* Always start back on page 1 when freshly
               navigating onto this screen. */

            if (previousScreen != SCREEN_PETS)
            {
                petPage = 0;
                petPendingDeleteIndex = -1;
            }

            /* Reset the adopter species prompt each time an
               adopter freshly navigates onto this screen. */

            if (role == ROLE_ADOPTER &&
                previousScreen != SCREEN_PETS)
            {
                adopterSpeciesPromptDone = false;
                adopterSpeciesPromptInput[0] = '\0';
                petSearchText[0] = '\0';
            }

            BeginDrawing();

            ClearBackground(
                GUI_BACKGROUND
            );

            /* TOP BAR */

            DrawRectangle(
                0,
                0,
                SCREEN_WIDTH,
                78,
                GUI_NAVY
            );

            DrawPaw(
                (Vector2){
                    45,
                    40
                },
                0.30f,
                GUI_TEAL_LIGHT
            );

            DrawTextEx(
                semiBoldFont,
                "MauPaws",
                (Vector2){
                    75,
                    24
                },
                24,
                1,
                WHITE
            );

            DrawTextEx(
                regularFont,
                "Pets",
                (Vector2){
                    950,
                    28
                },
                15,
                1,
                WHITE
            );

            /* SIDEBAR */

            DrawRectangle(
                0,
                78,
                230,
                SCREEN_HEIGHT - 78,
                WHITE
            );

            DrawLine(
                230,
                78,
                230,
                SCREEN_HEIGHT,
                GUI_BORDER
            );

            DrawTextEx(
                semiBoldFont,
                "PET MANAGEMENT",
                (Vector2){
                    30,
                    110
                },
                13,
                1,
                GUI_MUTED
            );

            /* BACK TO DASHBOARD */

            Rectangle backButton =
            {
                18,
                145,
                194,
                42
            };

            if (DrawButton(
                    semiBoldFont,
                    backButton,
                    "Dashboard",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                screen =
                    SCREEN_DASHBOARD;
            }

            /* TITLE */

            DrawTextEx(
                semiBoldFont,
                "All Pets",
                (Vector2){
                    270,
                    115
                },
                24,
                1,
                GUI_NAVY
            );

            DrawTextEx(
                regularFont,
                "View all rescued animals",
                (Vector2){
                    270,
                    150
                },
                14,
                1,
                GUI_MUTED
            );

            /* PET COUNT */

            char petCountText[64];

            snprintf(
                petCountText,
                sizeof(petCountText),
                "%d pets registered",
                petCount
            );

            DrawTextEx(
                regularFont,
                petCountText,
                (Vector2){
                    270,
                    185
                },
                13,
                1,
                GUI_TEAL
            );

            /* =================================================
               ADD PET BUTTON
               ================================================= */

            Rectangle addPetButton =
            {
                900,
                110,
                190,
                45
            };

            if (DrawButton(
                    semiBoldFont,
                    addPetButton,
                    "+ Add Pet",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                petAction = 1;
                selectedPet = -1;
                petFormField = 0;
                petFormStep = 0;

                petNameInput[0] = '\0';
                petSpeciesInput[0] = '\0';
                petBreedInput[0] = '\0';
                petAgeInput[0] = '\0';
                petGenderInput[0] = '\0';
                petWeightInput[0] = '\0';
                petSizeInput[0] = '\0';
                petLocationInput[0] = '\0';
                petHealthInput[0] = '\0';
                petAvailabilityInput[0] = '\0';
                petFormMessage[0] = '\0';

                /* Suggest the next available ID, but let staff
                   change it to any other ID that isn't taken. */

                int suggestedPetID =
                    petCount > 0
                    ? pets[petCount - 1].ID + 1
                    : 1;

                snprintf(petIDInput, sizeof(petIDInput), "%d", suggestedPetID);

                /* Reset the medical-record fields too, since
                   registering a new pet will also ask about
                   its medical record on the next step. */

                medicalInjuryLevel = 0;
                medicalIllnessLevel = 0;
                medicalPainLevel = 0;
                medicalDehydrationLevel = 0;
                medicalConditionLevel = 0;
                medicalTreatmentChoice = 1;
                medicalDateInput[0] = '\0';
                medicalInputField = 0;
            }

            /* =================================================
               PET DISPLAY (cards + search)
               ================================================= */

            if (petAction == 0)
            {
                /* =========================================
                   ADOPTERS ARE ASKED WHICH SPECIES THEY WANT
                   BEFORE THEY CAN BROWSE THE PET LIST
                   ========================================= */

                bool needsSpeciesPrompt =
                    (role == ROLE_ADOPTER &&
                     !adopterSpeciesPromptDone);

                if (needsSpeciesPrompt)
                {
                    Rectangle promptCard =
                    {
                        270,
                        210,
                        640,
                        230
                    };

                    DrawRectangleRounded(
                        promptCard,
                        0.08f,
                        10,
                        WHITE
                    );

                    DrawTextEx(
                        semiBoldFont,
                        "What kind of pet are you looking for?",
                        (Vector2){promptCard.x + 25, promptCard.y + 25},
                        20,
                        1,
                        GUI_NAVY
                    );

                    DrawTextEx(
                        regularFont,
                        "Enter a species (e.g. Dog, Cat) to see matching pets.",
                        (Vector2){promptCard.x + 25, promptCard.y + 58},
                        14,
                        1,
                        GUI_MUTED
                    );

                    Rectangle speciesBox =
                    {
                        promptCard.x + 25,
                        promptCard.y + 95,
                        300,
                        42
                    };

                    DrawTextInputBox(
                        regularFont,
                        speciesBox,
                        adopterSpeciesPromptInput,
                        sizeof(adopterSpeciesPromptInput),
                        "e.g. Dog",
                        1,
                        &adopterSpeciesPromptField,
                        mouse
                    );

                    Rectangle searchSpeciesButton =
                    {
                        promptCard.x + 25,
                        promptCard.y + 155,
                        180,
                        42
                    };

                    if (DrawButton(
                            semiBoldFont,
                            searchSpeciesButton,
                            "Show Pets",
                            GUI_TEAL,
                            GUI_TEAL_LIGHT))
                    {
                        strcpy(petSearchText, adopterSpeciesPromptInput);
                        adopterSpeciesPromptDone = true;
                        petPage = 0;
                    }

                    Rectangle showAllButton =
                    {
                        promptCard.x + 225,
                        promptCard.y + 155,
                        180,
                        42
                    };

                    if (DrawButton(
                            regularFont,
                            showAllButton,
                            "Show All Pets",
                            GUI_TEAL_PALE,
                            GUI_TEAL_LIGHT))
                    {
                        petSearchText[0] = '\0';
                        adopterSpeciesPromptDone = true;
                        petPage = 0;
                    }
                }
                else
                {
                Rectangle petSearchBox =
                {
                    650,
                    175,
                    240,
                    34
                };

                DrawRectangleRounded(
                    petSearchBox,
                    0.2f,
                    10,
                    WHITE
                );

                DrawRectangleRoundedLinesEx(
                    petSearchBox,
                    0.2f,
                    10,
                    2,
                    petInputField == 1
                    ? GUI_TEAL
                    : GUI_BORDER
                );

                DrawTextEx(
                    regularFont,
                    petSearchText[0] == '\0'
                    ? "Search by name or ID..."
                    : petSearchText,
                    (Vector2){
                        petSearchBox.x + 12,
                        petSearchBox.y + 9
                    },
                    13,
                    1,
                    petSearchText[0] == '\0'
                    ? GUI_MUTED
                    : GUI_TEXT
                );

                if (CheckCollisionPointRec(
                        mouse,
                        petSearchBox) &&
                    IsMouseButtonPressed(
                        MOUSE_BUTTON_LEFT))
                {
                    petInputField = 1;
                }

                if (petInputField == 1)
                {
                    int searchKey = GetCharPressed();

                    while (searchKey > 0)
                    {
                        if (searchKey >= 32 && searchKey <= 125)
                        {
                            int length = strlen(petSearchText);

                            if (length < 49)
                            {
                                petSearchText[length] = (char)searchKey;
                                petSearchText[length + 1] = '\0';
                                petPage = 0;
                            }
                        }

                        searchKey = GetCharPressed();
                    }

                    if (IsKeyPressed(KEY_BACKSPACE))
                    {
                        int length = strlen(petSearchText);

                        if (length > 0)
                        {
                            petSearchText[length - 1] = '\0';
                            petPage = 0;
                        }
                    }
                }

                const int petsPerPage = 6;

                int totalMatches = 0;

                for (int i = 0; i < petCount; i++)
                {
                    if (petMatchesSearch(i))
                        totalMatches++;
                }

                int totalPetPages =
                    (totalMatches + petsPerPage - 1) / petsPerPage;

                if (totalPetPages < 1)
                    totalPetPages = 1;

                if (petPage >= totalPetPages)
                    petPage = totalPetPages - 1;

                if (petPage < 0)
                    petPage = 0;

                /* =========================================
                   PAGE UP / PAGE DOWN CONTROLS
                   ========================================= */

                if (totalMatches > petsPerPage)
                {
                    Rectangle prevPageButton = {900, 175, 55, 34};
                    Rectangle nextPageButton = {1105, 175, 55, 34};

                    bool prevAllowed = petPage > 0;
                    bool nextAllowed = petPage < totalPetPages - 1;

                    if (DrawButton(
                            regularFont,
                            prevPageButton,
                            "< Prev",
                            prevAllowed ? GUI_TEAL : GUI_BORDER,
                            prevAllowed ? GUI_TEAL_LIGHT : GUI_BORDER) &&
                        prevAllowed)
                    {
                        petPage--;
                    }

                    char pageText[32];

                    snprintf(
                        pageText,
                        sizeof(pageText),
                        "Page %d of %d",
                        petPage + 1,
                        totalPetPages
                    );

                    Vector2 pageTextSize =
                        MeasureTextEx(regularFont, pageText, 13, 1);

                    DrawTextEx(
                        regularFont,
                        pageText,
                        (Vector2){
                            965 + (130 - pageTextSize.x) / 2,
                            184
                        },
                        13,
                        1,
                        GUI_MUTED
                    );

                    if (DrawButton(
                            regularFont,
                            nextPageButton,
                            "Next >",
                            nextAllowed ? GUI_TEAL : GUI_BORDER,
                            nextAllowed ? GUI_TEAL_LIGHT : GUI_BORDER) &&
                        nextAllowed)
                    {
                        petPage++;
                    }
                }

                int shown = 0;
                int matchedIndex = 0;

            for (int i = 0;
                 i < petCount;
                 i++)
            {
                if (!petMatchesSearch(i))
                    continue;

                int pageOfMatch = matchedIndex / petsPerPage;
                matchedIndex++;

                if (pageOfMatch != petPage)
                    continue;

                int column =
                    shown % 3;

                int row =
                    shown / 3;

                shown++;

                Rectangle card =
                {
                    270 + column * 250,
                    220 + row * 195,
                    225,
                    175
                };

                DrawRectangleRounded(
                    card,
                    0.12f,
                    12,
                    WHITE
                );
               /* PET PHOTO
                  Only the originally registered pets have a
                  real photo on file. Any newly added pet gets
                  a paw-print placeholder instead. */

Texture2D currentPetImage;
bool hasPetPhoto = true;

if (sameString(pets[i].Name, "Buddy"))
    currentPetImage = petImage1;
else if (sameString(pets[i].Name, "Luna"))
    currentPetImage = petImage2;
else if (sameString(pets[i].Name, "Mimi"))
    currentPetImage = petImage3;
else if (sameString(pets[i].Name, "Rex"))
    currentPetImage = petImage4;
else if (sameString(pets[i].Name, "Lily"))
    currentPetImage = petImage5;
else if (sameString(pets[i].Name, "Yolk"))
    currentPetImage = petImage6;
else if (sameString(pets[i].Name, "Filou"))
    currentPetImage = petImage7;
else
    hasPetPhoto = false;

if (hasPetPhoto)
{
    DrawPetImage(
        currentPetImage,
        (Rectangle){
            card.x + 12,
            card.y + 20,
            70,
            70
        }
    );
}
else
{
    DrawRectangleRounded(
        (Rectangle){card.x + 12, card.y + 20, 70, 70},
        0.5f,
        10,
        GUI_TEAL_PALE
    );

    DrawPaw(
        (Vector2){card.x + 47, card.y + 55},
        0.45f,
        GUI_TEAL
    );
}

                char petCardNameLabel[70];

                snprintf(
                    petCardNameLabel,
                    sizeof(petCardNameLabel),
                    "%s (ID %d)",
                    pets[i].Name,
                    pets[i].ID
                );

                DrawTextEx(
                    semiBoldFont,
                    petCardNameLabel,
                    (Vector2){
                        card.x + 95,
                        card.y + 20
                    },
                    17,
                    1,
                    GUI_TEXT
                );

                DrawTextEx(
                    regularFont,
                    pets[i].Species,
                    (Vector2){
                        card.x + 95,
                        card.y + 45
                    },
                    12,
                    1,
                    GUI_MUTED
                );

                DrawTextEx(
                    regularFont,
                    pets[i].Breed,
                    (Vector2){
                        card.x + 95,
                        card.y + 65
                    },
                    11,
                    1,
                    GUI_MUTED
                );

                DrawTextEx(
                    regularFont,
                    pets[i].Availability,
                    (Vector2){
                        card.x + 95,
                        card.y + 95
                    },
                    12,
                    1,
                    GUI_TEAL
                );

                DrawTextEx(
                    regularFont,
                    pets[i].HealthStatus,
                    (Vector2){
                        card.x + 95,
                        card.y + 118
                    },
                    11,
                    1,
                    GUI_MUTED
                );

                Rectangle editPetButton =
                {
                    card.x + 12,
                    card.y + 145,
                    95,
                    26
                };

                Rectangle deletePetButton =
                {
                    card.x + 118,
                    card.y + 145,
                    95,
                    26
                };

                if (DrawButton(
                        regularFont,
                        editPetButton,
                        "Edit",
                        GUI_TEAL,
                        GUI_TEAL_LIGHT) &&
                    petPendingDeleteIndex == -1)
                {
                    populatePetForm(i);

                    selectedPet = i;
                    petAction = 3;
                    petFormField = 0;
                    petFormStep = 0;
                }

                if (DrawButton(
                        regularFont,
                        deletePetButton,
                        "Delete",
                        GUI_ERROR,
                        GUI_ERROR) &&
                    petPendingDeleteIndex == -1)
                {
                    petPendingDeleteIndex = i;
                }
            }

                /* =========================================
                   DELETE CONFIRMATION DIALOG
                   ========================================= */

                if (petPendingDeleteIndex >= 0 &&
                    petPendingDeleteIndex < petCount)
                {
                    DrawRectangle(
                        0, 0,
                        SCREEN_WIDTH, SCREEN_HEIGHT,
                        (Color){0, 0, 0, 140}
                    );

                    Rectangle confirmCard =
                    {
                        (SCREEN_WIDTH - 420) / 2.0f,
                        (SCREEN_HEIGHT - 210) / 2.0f,
                        420,
                        210
                    };

                    DrawRectangleRounded(confirmCard, 0.08f, 12, WHITE);

                    DrawTextEx(
                        semiBoldFont,
                        "Delete this pet?",
                        (Vector2){confirmCard.x + 30, confirmCard.y + 26},
                        20,
                        1,
                        GUI_NAVY
                    );

                    char petDeleteMsg[120];

                    snprintf(
                        petDeleteMsg,
                        sizeof(petDeleteMsg),
                        "%s will be permanently removed.",
                        pets[petPendingDeleteIndex].Name
                    );

                    DrawTextEx(
                        regularFont,
                        petDeleteMsg,
                        (Vector2){confirmCard.x + 30, confirmCard.y + 62},
                        13,
                        1,
                        GUI_MUTED
                    );

                    DrawTextEx(
                        regularFont,
                        "This action cannot be undone.",
                        (Vector2){confirmCard.x + 30, confirmCard.y + 84},
                        13,
                        1,
                        GUI_MUTED
                    );

                    Rectangle cancelDeletePetButton =
                    {
                        confirmCard.x + 30,
                        confirmCard.y + 140,
                        165,
                        45
                    };

                    Rectangle confirmDeletePetButton =
                    {
                        confirmCard.x + 225,
                        confirmCard.y + 140,
                        165,
                        45
                    };

                    if (DrawButton(
                            semiBoldFont,
                            cancelDeletePetButton,
                            "Cancel",
                            GUI_NAVY,
                            GUI_TEAL))
                    {
                        petPendingDeleteIndex = -1;
                    }

                    if (DrawButton(
                            semiBoldFont,
                            confirmDeletePetButton,
                            "Delete",
                            GUI_ERROR,
                            GUI_ERROR))
                    {
                        deletePetByIndex(petPendingDeleteIndex);
                        petPendingDeleteIndex = -1;
                    }
                }

                if (petCount > 0 && shown == 0)
                {
                    DrawTextEx(
                        regularFont,
                        "No pets match your search.",
                        (Vector2){
                            270,
                            220
                        },
                        14,
                        1,
                        GUI_MUTED
                    );
                }
                }
            }
            /* =================================================
               ADD / EDIT PET FORM
               ================================================= */

            else if (petAction == 1 || petAction == 3)
            {
                if (petFormStep == 0)
                {
                DrawTextEx(
                    semiBoldFont,
                    petAction == 3
                    ? "Edit Pet"
                    : "Register New Pet (Step 1 of 2: Pet Info)",
                    (Vector2){
                        270,
                        205
                    },
                    24,
                    1,
                    GUI_NAVY
                );

                /* NAME */

                DrawTextEx(
                    semiBoldFont,
                    "Name",
                    (Vector2){270, 250},
                    13, 1, GUI_TEXT
                );

                Rectangle petNameBox = {270, 270, 350, 42};

                DrawRectangleRounded(petNameBox, 0.12f, 10, WHITE);
                DrawRectangleRoundedLinesEx(
                    petNameBox, 0.12f, 10, 2,
                    petFormField == 1 ? GUI_TEAL : GUI_BORDER);
                DrawTextEx(regularFont, petNameInput,
                    (Vector2){285, 282}, 14, 1, GUI_TEXT);

                /* BREED */

                DrawTextEx(
                    semiBoldFont,
                    "Breed",
                    (Vector2){650, 250},
                    13, 1, GUI_TEXT
                );

                Rectangle petBreedBox = {650, 270, 350, 42};

                DrawRectangleRounded(petBreedBox, 0.12f, 10, WHITE);
                DrawRectangleRoundedLinesEx(
                    petBreedBox, 0.12f, 10, 2,
                    petFormField == 2 ? GUI_TEAL : GUI_BORDER);
                DrawTextEx(regularFont, petBreedInput,
                    (Vector2){665, 282}, 14, 1, GUI_TEXT);

                /* LOCATION */

                DrawTextEx(
                    semiBoldFont,
                    "Rescue Location",
                    (Vector2){270, 335},
                    13, 1, GUI_TEXT
                );

                Rectangle petLocationBox = {270, 355, 350, 42};

                DrawRectangleRounded(petLocationBox, 0.12f, 10, WHITE);
                DrawRectangleRoundedLinesEx(
                    petLocationBox, 0.12f, 10, 2,
                    petFormField == 3 ? GUI_TEAL : GUI_BORDER);
                DrawTextEx(regularFont, petLocationInput,
                    (Vector2){285, 367}, 14, 1, GUI_TEXT);

                /* SPECIES */

                DrawTextEx(
                    semiBoldFont,
                    "Species",
                    (Vector2){650, 335},
                    13, 1, GUI_TEXT
                );

                Rectangle petSpeciesCat = {650, 355, 160, 42};
                Rectangle petSpeciesDog = {840, 355, 160, 42};

                bool petIsCat = strcmp(petSpeciesInput, "Cat") == 0;
                bool petIsDog = strcmp(petSpeciesInput, "Dog") == 0;

                DrawRectangleRounded(petSpeciesCat, 0.12f, 10,
                    petIsCat ? GUI_TEAL : GUI_TEAL_PALE);
                DrawRectangleRounded(petSpeciesDog, 0.12f, 10,
                    petIsDog ? GUI_TEAL : GUI_TEAL_PALE);

                DrawTextEx(semiBoldFont, "Cat",
                    (Vector2){petSpeciesCat.x + 60, petSpeciesCat.y + 13},
                    13, 1, petIsCat ? WHITE : GUI_TEXT);
                DrawTextEx(semiBoldFont, "Dog",
                    (Vector2){petSpeciesDog.x + 60, petSpeciesDog.y + 13},
                    13, 1, petIsDog ? WHITE : GUI_TEXT);

                /* AGE */

                DrawTextEx(
                    semiBoldFont,
                    "Age (years)",
                    (Vector2){270, 420},
                    13, 1, GUI_TEXT
                );

                Rectangle petAgeBox = {270, 440, 160, 42};

                DrawRectangleRounded(petAgeBox, 0.12f, 10, WHITE);
                DrawRectangleRoundedLinesEx(
                    petAgeBox, 0.12f, 10, 2,
                    petFormField == 4 ? GUI_TEAL : GUI_BORDER);
                DrawTextEx(regularFont, petAgeInput,
                    (Vector2){285, 452}, 14, 1, GUI_TEXT);

                /* WEIGHT */

                DrawTextEx(
                    semiBoldFont,
                    "Weight (kg)",
                    (Vector2){460, 420},
                    13, 1, GUI_TEXT
                );

                Rectangle petWeightBox = {460, 440, 160, 42};

                DrawRectangleRounded(petWeightBox, 0.12f, 10, WHITE);
                DrawRectangleRoundedLinesEx(
                    petWeightBox, 0.12f, 10, 2,
                    petFormField == 5 ? GUI_TEAL : GUI_BORDER);
                DrawTextEx(regularFont, petWeightInput,
                    (Vector2){475, 452}, 14, 1, GUI_TEXT);

                /* GENDER */

                DrawTextEx(
                    semiBoldFont,
                    "Gender",
                    (Vector2){650, 420},
                    13, 1, GUI_TEXT
                );

                Rectangle petGenderMale = {650, 440, 160, 42};
                Rectangle petGenderFemale = {840, 440, 160, 42};

                bool petIsMale = strcmp(petGenderInput, "Male") == 0;
                bool petIsFemale = strcmp(petGenderInput, "Female") == 0;

                DrawRectangleRounded(petGenderMale, 0.12f, 10,
                    petIsMale ? GUI_TEAL : GUI_TEAL_PALE);
                DrawRectangleRounded(petGenderFemale, 0.12f, 10,
                    petIsFemale ? GUI_TEAL : GUI_TEAL_PALE);

                DrawTextEx(semiBoldFont, "Male",
                    (Vector2){petGenderMale.x + 55, petGenderMale.y + 13},
                    13, 1, petIsMale ? WHITE : GUI_TEXT);
                DrawTextEx(semiBoldFont, "Female",
                    (Vector2){petGenderFemale.x + 48, petGenderFemale.y + 13},
                    13, 1, petIsFemale ? WHITE : GUI_TEXT);

                /* SIZE */

                DrawTextEx(
                    semiBoldFont,
                    "Size",
                    (Vector2){270, 505},
                    13, 1, GUI_TEXT
                );

                Rectangle petSizeSmall = {270, 525, 105, 42};
                Rectangle petSizeMedium = {385, 525, 105, 42};
                Rectangle petSizeLarge = {500, 525, 105, 42};

                bool petIsSmall = strcmp(petSizeInput, "Small") == 0;
                bool petIsMedium = strcmp(petSizeInput, "Medium") == 0;
                bool petIsLarge = strcmp(petSizeInput, "Large") == 0;

                DrawRectangleRounded(petSizeSmall, 0.12f, 10,
                    petIsSmall ? GUI_TEAL : GUI_TEAL_PALE);
                DrawRectangleRounded(petSizeMedium, 0.12f, 10,
                    petIsMedium ? GUI_TEAL : GUI_TEAL_PALE);
                DrawRectangleRounded(petSizeLarge, 0.12f, 10,
                    petIsLarge ? GUI_TEAL : GUI_TEAL_PALE);

                DrawTextEx(semiBoldFont, "Small",
                    (Vector2){petSizeSmall.x + 25, petSizeSmall.y + 13},
                    13, 1, petIsSmall ? WHITE : GUI_TEXT);
                DrawTextEx(semiBoldFont, "Medium",
                    (Vector2){petSizeMedium.x + 15, petSizeMedium.y + 13},
                    13, 1, petIsMedium ? WHITE : GUI_TEXT);
                DrawTextEx(semiBoldFont, "Large",
                    (Vector2){petSizeLarge.x + 25, petSizeLarge.y + 13},
                    13, 1, petIsLarge ? WHITE : GUI_TEXT);

                /* HEALTH STATUS */

                DrawTextEx(
                    semiBoldFont,
                    "Health Status",
                    (Vector2){650, 505},
                    13, 1, GUI_TEXT
                );

                Rectangle petHealthy = {650, 525, 115, 40};
                Rectangle petInjured = {775, 525, 115, 40};
                Rectangle petTreatment = {900, 525, 130, 40};

                bool petIsHealthy = strcmp(petHealthInput, "Healthy") == 0;
                bool petIsInjured = strcmp(petHealthInput, "Injured") == 0;
                bool petIsUnderTreatment = strcmp(petHealthInput, "UnderTreatment") == 0;

                DrawRectangleRounded(petHealthy, 0.12f, 10,
                    petIsHealthy ? GUI_TEAL : GUI_TEAL_PALE);
                DrawRectangleRounded(petInjured, 0.12f, 10,
                    petIsInjured ? GUI_TEAL : GUI_TEAL_PALE);
                DrawRectangleRounded(petTreatment, 0.12f, 10,
                    petIsUnderTreatment ? GUI_TEAL : GUI_TEAL_PALE);

                DrawTextEx(regularFont, "Healthy",
                    (Vector2){petHealthy.x + 20, petHealthy.y + 12},
                    12, 1, petIsHealthy ? WHITE : GUI_TEXT);
                DrawTextEx(regularFont, "Injured",
                    (Vector2){petInjured.x + 22, petInjured.y + 12},
                    12, 1, petIsInjured ? WHITE : GUI_TEXT);
                DrawTextEx(regularFont, "Treatment",
                    (Vector2){petTreatment.x + 18, petTreatment.y + 12},
                    12, 1, petIsUnderTreatment ? WHITE : GUI_TEXT);

                /* AVAILABILITY */

                DrawTextEx(
                    semiBoldFont,
                    "Availability",
                    (Vector2){270, 590},
                    13, 1, GUI_TEXT
                );

                Rectangle petAvailable = {270, 610, 115, 40};
                Rectangle petUnderTreatment2 = {395, 610, 130, 40};
                Rectangle petAdopted = {535, 610, 115, 40};

                bool petIsAvailable = strcmp(petAvailabilityInput, "Available") == 0;
                bool petIsAvailUnderTreatment = strcmp(petAvailabilityInput, "UnderTreatment") == 0;
                bool petIsAdopted = strcmp(petAvailabilityInput, "Adopted") == 0;

                DrawRectangleRounded(petAvailable, 0.12f, 10,
                    petIsAvailable ? GUI_TEAL : GUI_TEAL_PALE);
                DrawRectangleRounded(petUnderTreatment2, 0.12f, 10,
                    petIsAvailUnderTreatment ? GUI_TEAL : GUI_TEAL_PALE);
                DrawRectangleRounded(petAdopted, 0.12f, 10,
                    petIsAdopted ? GUI_TEAL : GUI_TEAL_PALE);

                DrawTextEx(regularFont, "Available",
                    (Vector2){petAvailable.x + 15, petAvailable.y + 12},
                    12, 1, petIsAvailable ? WHITE : GUI_TEXT);
                DrawTextEx(regularFont, "Treatment",
                    (Vector2){petUnderTreatment2.x + 20, petUnderTreatment2.y + 12},
                    12, 1, petIsAvailUnderTreatment ? WHITE : GUI_TEXT);
                DrawTextEx(regularFont, "Adopted",
                    (Vector2){petAdopted.x + 22, petAdopted.y + 12},
                    12, 1, petIsAdopted ? WHITE : GUI_TEXT);

                /* PET ID (only asked when registering a brand-new
                   pet; an existing pet's ID cannot be changed) */

                Rectangle petIDBoxInForm = {0, 0, 0, 0};

                if (petAction == 1)
                {
                    DrawTextEx(
                        semiBoldFont,
                        "Pet ID",
                        (Vector2){700, 590},
                        13, 1, GUI_TEXT
                    );

                    petIDBoxInForm = (Rectangle){700, 610, 150, 40};

                    DrawRectangleRounded(petIDBoxInForm, 0.12f, 10, WHITE);
                    DrawRectangleRoundedLinesEx(
                        petIDBoxInForm, 0.12f, 10, 2,
                        petFormField == 6 ? GUI_TEAL : GUI_BORDER);
                    DrawTextEx(regularFont, petIDInput,
                        (Vector2){715, 622}, 14, 1, GUI_TEXT);

                    DrawTextEx(
                        regularFont,
                        "Suggested next available ID - you can change it.",
                        (Vector2){700, 655},
                        11, 1, GUI_MUTED
                    );
                }

                if (petFormMessage[0] != '\0')
                {
                    DrawTextEx(
                        regularFont,
                        petFormMessage,
                        (Vector2){270, 655},
                        13, 1, GUI_ERROR
                    );
                }

                /* =================================================
                   TEXT FIELD CLICK-TO-FOCUS
                   ================================================= */

                if (petAction == 1 &&
                    CheckCollisionPointRec(mouse, petIDBoxInForm) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    petFormField = 6;
                }

                if (CheckCollisionPointRec(mouse, petNameBox) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    petFormField = 1;
                }

                if (CheckCollisionPointRec(mouse, petBreedBox) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    petFormField = 2;
                }

                if (CheckCollisionPointRec(mouse, petLocationBox) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    petFormField = 3;
                }

                if (CheckCollisionPointRec(mouse, petAgeBox) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    petFormField = 4;
                }

                if (CheckCollisionPointRec(mouse, petWeightBox) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    petFormField = 5;
                }

                /* =================================================
                   BUTTON GROUP CLICKS
                   ================================================= */

                if (CheckCollisionPointRec(mouse, petSpeciesCat) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petSpeciesInput, "Cat");
                }

                if (CheckCollisionPointRec(mouse, petSpeciesDog) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petSpeciesInput, "Dog");
                }

                if (CheckCollisionPointRec(mouse, petGenderMale) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petGenderInput, "Male");
                }

                if (CheckCollisionPointRec(mouse, petGenderFemale) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petGenderInput, "Female");
                }

                if (CheckCollisionPointRec(mouse, petSizeSmall) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petSizeInput, "Small");
                }

                if (CheckCollisionPointRec(mouse, petSizeMedium) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petSizeInput, "Medium");
                }

                if (CheckCollisionPointRec(mouse, petSizeLarge) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petSizeInput, "Large");
                }

                if (CheckCollisionPointRec(mouse, petHealthy) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petHealthInput, "Healthy");
                }

                if (CheckCollisionPointRec(mouse, petInjured) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petHealthInput, "Injured");
                }

                if (CheckCollisionPointRec(mouse, petTreatment) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petHealthInput, "UnderTreatment");
                }

                if (CheckCollisionPointRec(mouse, petAvailable) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petAvailabilityInput, "Available");
                }

                if (CheckCollisionPointRec(mouse, petUnderTreatment2) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petAvailabilityInput, "UnderTreatment");
                }

                if (CheckCollisionPointRec(mouse, petAdopted) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    strcpy(petAvailabilityInput, "Adopted");
                }

                /* =================================================
                   TEXT INPUT (typing + backspace)
                   ================================================= */

                int petKey = GetCharPressed();

                while (petKey > 0)
                {
                    if (petKey >= 32 && petKey <= 125)
                    {
                        if (petFormField == 1)
                        {
                            int length = strlen(petNameInput);

                            if (length < 49)
                            {
                                petNameInput[length] = (char)petKey;
                                petNameInput[length + 1] = '\0';
                            }
                        }
                        else if (petFormField == 2)
                        {
                            int length = strlen(petBreedInput);

                            if (length < 49)
                            {
                                petBreedInput[length] = (char)petKey;
                                petBreedInput[length + 1] = '\0';
                            }
                        }
                        else if (petFormField == 3)
                        {
                            int length = strlen(petLocationInput);

                            if (length < 49)
                            {
                                petLocationInput[length] = (char)petKey;
                                petLocationInput[length + 1] = '\0';
                            }
                        }
                        else if (petFormField == 4)
                        {
                            if (petKey >= '0' && petKey <= '9')
                            {
                                int length = strlen(petAgeInput);

                                if (length < 3)
                                {
                                    petAgeInput[length] = (char)petKey;
                                    petAgeInput[length + 1] = '\0';
                                }
                            }
                        }
                        else if (petFormField == 5)
                        {
                            if ((petKey >= '0' && petKey <= '9') || petKey == '.')
                            {
                                int length = strlen(petWeightInput);

                                if (length < 8)
                                {
                                    petWeightInput[length] = (char)petKey;
                                    petWeightInput[length + 1] = '\0';
                                }
                            }
                        }
                        else if (petFormField == 6)
                        {
                            if (petKey >= '0' && petKey <= '9')
                            {
                                int length = strlen(petIDInput);

                                if (length < 6)
                                {
                                    petIDInput[length] = (char)petKey;
                                    petIDInput[length + 1] = '\0';
                                }
                            }
                        }
                    }

                    petKey = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    if (petFormField == 1)
                    {
                        int length = strlen(petNameInput);
                        if (length > 0) petNameInput[length - 1] = '\0';
                    }
                    else if (petFormField == 2)
                    {
                        int length = strlen(petBreedInput);
                        if (length > 0) petBreedInput[length - 1] = '\0';
                    }
                    else if (petFormField == 3)
                    {
                        int length = strlen(petLocationInput);
                        if (length > 0) petLocationInput[length - 1] = '\0';
                    }
                    else if (petFormField == 4)
                    {
                        int length = strlen(petAgeInput);
                        if (length > 0) petAgeInput[length - 1] = '\0';
                    }
                    else if (petFormField == 5)
                    {
                        int length = strlen(petWeightInput);
                        if (length > 0) petWeightInput[length - 1] = '\0';
                    }
                    else if (petFormField == 6)
                    {
                        int length = strlen(petIDInput);
                        if (length > 0) petIDInput[length - 1] = '\0';
                    }
                }
                } /* end petFormStep == 0 */

                if (petAction == 1 && petFormStep == 1)
                {
                    /* =====================================
                       STEP 2: MEDICAL RECORD FOR NEW PET
                       ===================================== */

                    DrawTextEx(
                        semiBoldFont,
                        "Register New Pet (Step 2 of 2: Medical Record)",
                        (Vector2){270, 205},
                        22,
                        1,
                        GUI_NAVY
                    );

                    DrawTextEx(
                        regularFont,
                        "This creates a baseline medical record for the pet you just entered.",
                        (Vector2){270, 235},
                        13,
                        1,
                        GUI_MUTED
                    );

                    const char *petFormSeverityLabels[] =
                        {"None", "Minor", "Moderate", "Severe", "Critical"};

                    /* Injury severity (0-4) */

                    DrawTextEx(regularFont, "Injury Severity", (Vector2){270, 275}, 13, 1, GUI_MUTED);

                    for (int lvl = 0; lvl <= 4; lvl++)
                    {
                        Rectangle lvlBox = {270.0f + lvl * 90, 297, 82, 34};

                        if (DrawButton(
                                regularFont,
                                lvlBox,
                                petFormSeverityLabels[lvl],
                                medicalInjuryLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                                GUI_TEAL_LIGHT))
                        {
                            medicalInjuryLevel = lvl;
                        }
                    }

                    /* Illness severity (0-4) */

                    DrawTextEx(regularFont, "Illness Severity", (Vector2){270, 345}, 13, 1, GUI_MUTED);

                    for (int lvl = 0; lvl <= 4; lvl++)
                    {
                        Rectangle lvlBox = {270.0f + lvl * 90, 367, 82, 34};

                        if (DrawButton(
                                regularFont,
                                lvlBox,
                                petFormSeverityLabels[lvl],
                                medicalIllnessLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                                GUI_TEAL_LIGHT))
                        {
                            medicalIllnessLevel = lvl;
                        }
                    }

                    /* Pain level (0-3) */

                    DrawTextEx(regularFont, "Pain Level", (Vector2){270, 415}, 13, 1, GUI_MUTED);

                    for (int lvl = 0; lvl <= 3; lvl++)
                    {
                        Rectangle lvlBox = {270.0f + lvl * 90, 437, 82, 34};

                        if (DrawButton(
                                regularFont,
                                lvlBox,
                                petFormSeverityLabels[lvl],
                                medicalPainLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                                GUI_TEAL_LIGHT))
                        {
                            medicalPainLevel = lvl;
                        }
                    }

                    /* Dehydration level (0-2) */

                    DrawTextEx(regularFont, "Dehydration Level", (Vector2){270, 485}, 13, 1, GUI_MUTED);

                    for (int lvl = 0; lvl <= 2; lvl++)
                    {
                        Rectangle lvlBox = {270.0f + lvl * 90, 507, 82, 34};

                        if (DrawButton(
                                regularFont,
                                lvlBox,
                                petFormSeverityLabels[lvl],
                                medicalDehydrationLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                                GUI_TEAL_LIGHT))
                        {
                            medicalDehydrationLevel = lvl;
                        }
                    }

                    /* Existing condition (0-2) */

                    DrawTextEx(regularFont, "Existing Condition", (Vector2){700, 415}, 13, 1, GUI_MUTED);

                    for (int lvl = 0; lvl <= 2; lvl++)
                    {
                        Rectangle lvlBox = {700.0f + lvl * 90, 437, 82, 34};

                        if (DrawButton(
                                regularFont,
                                lvlBox,
                                petFormSeverityLabels[lvl],
                                medicalConditionLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                                GUI_TEAL_LIGHT))
                        {
                            medicalConditionLevel = lvl;
                        }
                    }

                    /* Treatment */

                    DrawTextEx(regularFont, "Treatment", (Vector2){700, 485}, 13, 1, GUI_MUTED);

                    Rectangle petFormMedicationBox = {700, 507, 130, 34};
                    Rectangle petFormSurgeryBox = {840, 507, 130, 34};

                    if (DrawButton(
                            regularFont,
                            petFormMedicationBox,
                            "Medication",
                            medicalTreatmentChoice == 1 ? GUI_TEAL : GUI_TEAL_PALE,
                            GUI_TEAL_LIGHT))
                    {
                        medicalTreatmentChoice = 1;
                    }

                    if (DrawButton(
                            regularFont,
                            petFormSurgeryBox,
                            "Surgery",
                            medicalTreatmentChoice == 2 ? GUI_TEAL : GUI_TEAL_PALE,
                            GUI_TEAL_LIGHT))
                    {
                        medicalTreatmentChoice = 2;
                    }

                    /* Date */

                    DrawTextEx(regularFont, "Date (DD/MM/YYYY)", (Vector2){270, 555}, 13, 1, GUI_MUTED);

                    Rectangle petFormDateBox = {270, 577, 200, 38};

                    DrawTextInputBox(
                        regularFont,
                        petFormDateBox,
                        medicalDateInput,
                        sizeof(medicalDateInput),
                        "e.g. 13/09/2026",
                        2,
                        &medicalInputField,
                        mouse
                    );

                    if (medicalMessage[0] != '\0')
                    {
                        DrawTextEx(
                            regularFont,
                            medicalMessage,
                            (Vector2){270, 625},
                            13,
                            1,
                            GUI_ERROR
                        );
                    }
                }

                /* =================================================
                   CANCEL / SAVE / STEP NAVIGATION BUTTONS
                   ================================================= */

                Rectangle petCancelButton = {650, 675, 150, 45};
                Rectangle petSaveButton = {820, 675, 180, 45};

                if (petAction == 1 && petFormStep == 1)
                {
                    /* Step 2 of the Add Pet wizard: Back / Add Pet */

                    if (DrawButton(
                            regularFont,
                            petCancelButton,
                            "< Back",
                            GUI_NAVY,
                            GUI_TEAL))
                    {
                        petFormStep = 0;
                        medicalMessage[0] = '\0';
                    }

                    if (DrawButton(
                            semiBoldFont,
                            petSaveButton,
                            "Add Pet",
                            GUI_TEAL,
                            GUI_TEAL_LIGHT))
                    {
                        if (medicalDateInput[0] == '\0')
                        {
                            strcpy(medicalMessage, "Please enter a date for the medical record.");
                        }
                        else
                        {
                            int newPetID = savePetFromGUI();

                            if (newPetID != -1)
                            {
                                saveMedicalRecordGUI(
                                    newPetID,
                                    medicalInjuryLevel,
                                    medicalIllnessLevel,
                                    medicalPainLevel,
                                    medicalDehydrationLevel,
                                    medicalConditionLevel,
                                    medicalTreatmentChoice == 1 ? "Medication" : "Surgery",
                                    medicalDateInput
                                );
                            }

                            petAction = 0;
                            petFormStep = 0;
                            selectedPet = -1;
                            petFormField = 0;
                            medicalMessage[0] = '\0';
                        }
                    }
                }
                else
                {
                    /* Editing a pet, or step 1 of Add Pet: normal
                       Cancel / (Next or Save Changes) buttons */

                    if (DrawButton(
                            regularFont,
                            petCancelButton,
                            "Cancel",
                            GUI_NAVY,
                            GUI_TEAL))
                    {
                        petAction = 0;
                        petFormStep = 0;
                        selectedPet = -1;
                        petFormField = 0;
                        petFormMessage[0] = '\0';
                    }

                    if (DrawButton(
                            semiBoldFont,
                            petSaveButton,
                            petAction == 3
                                ? "Save Changes"
                                : "Next: Medical Info >",
                            GUI_TEAL,
                            GUI_TEAL_LIGHT))
                    {
                        if (petAction == 3)
                        {
                            updatePetFromGUI();

                            petAction = 0;
                            petFormStep = 0;
                            selectedPet = -1;
                            petFormField = 0;
                        }
                        else
                        {
                            if (petIDInput[0] == '\0')
                            {
                                strcpy(petFormMessage, "Please enter a Pet ID.");
                            }
                            else
                            {
                                int typedID = atoi(petIDInput);
                                bool idTaken = false;

                                for (int k = 0; k < petCount; k++)
                                {
                                    if (pets[k].ID == typedID)
                                    {
                                        idTaken = true;
                                        break;
                                    }
                                }

                                if (idTaken)
                                {
                                    strcpy(petFormMessage, "That Pet ID is already in use - please choose another.");
                                }
                                else
                                {
                                    petFormMessage[0] = '\0';
                                    petFormStep = 1;
                                }
                            }
                        }
                    }
                }
            }

            EndDrawing();
        }
        /* =================================================
           ADOPTERS SCREEN
           ================================================= */

        else if (screen == SCREEN_ADOPTERS)
        {
            /* Always start back on page 1 when freshly
               navigating onto this screen. */

            if (previousScreen != SCREEN_ADOPTERS)
            {
                adopterPage = 0;
                adopterPendingDeleteIndex = -1;
            }

            BeginDrawing();

            ClearBackground(
                GUI_BACKGROUND
            );

            /* =================================================
               TOP BAR
               ================================================= */

            DrawRectangle(
                0,
                0,
                SCREEN_WIDTH,
                78,
                GUI_NAVY
            );

            DrawPaw(
                (Vector2){
                    45,
                    40
                },
                0.30f,
                GUI_TEAL_LIGHT
            );

            DrawTextEx(
                semiBoldFont,
                "MauPaws",
                (Vector2){
                    75,
                    24
                },
                24,
                1,
                WHITE
            );

            DrawTextEx(
                regularFont,
                "Adopters",
                (Vector2){
                    930,
                    28
                },
                15,
                1,
                WHITE
            );

            /* =================================================
               SIDEBAR
               ================================================= */

            DrawRectangle(
                0,
                78,
                230,
                SCREEN_HEIGHT - 78,
                WHITE
            );

            DrawLine(
                230,
                78,
                230,
                SCREEN_HEIGHT,
                GUI_BORDER
            );

            DrawTextEx(
                semiBoldFont,
                "ADOPTER MANAGEMENT",
                (Vector2){
                    30,
                    110
                },
                13,
                1,
                GUI_MUTED
            );

            /* BACK TO DASHBOARD */

            Rectangle backButton =
            {
                18,
                145,
                194,
                42
            };

            if (DrawButton(
                    semiBoldFont,
                    backButton,
                    "Dashboard",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                screen =
                    SCREEN_DASHBOARD;
            }

            /* =================================================
               MAIN CONTENT
               ================================================= */

            DrawTextEx(
                semiBoldFont,
                "All Adopters",
                (Vector2){
                    270,
                    115
                },
                24,
                1,
                GUI_NAVY
            );

            DrawTextEx(
                regularFont,
                "Registered adoption applicants",
                (Vector2){
                    270,
                    150
                },
                14,
                1,
                GUI_MUTED
            );

            /* =================================================
               ADOPTER COUNT
               ================================================= */

            char adopterCountText[64];

            snprintf(
                adopterCountText,
                sizeof(adopterCountText),
                "%d adopters registered",
                adopterCount
            );

            DrawTextEx(
                regularFont,
                adopterCountText,
                (Vector2){
                    270,
                    185
                },
                13,
                1,
                GUI_TEAL
            );
/* =================================================
   ADD ADOPTER BUTTON
   ================================================= */

Rectangle addAdopterButton =
{
    880,
    110,
    210,
    45
};

if (DrawButton(
        semiBoldFont,
        addAdopterButton,
        "+ Add Adopter",
        GUI_TEAL,
        GUI_TEAL_LIGHT))
{
    adopterAction = 1;

    adopterNameInput[0] = '\0';
    adopterPhoneInput[0] = '\0';
    adopterEmailInput[0] = '\0';
    adopterSpeciesInput[0] = '\0';

    adopterInputField = 0;
}

/* =================================================
   ADOPTER CARDS
   ================================================= */

if (adopterAction == 0)
{
    /* =================================================
       SEARCH BOX
       ================================================= */

    Rectangle adopterSearchBox =
    {
        650,
        175,
        240,
        34
    };

    DrawRectangleRounded(
        adopterSearchBox,
        0.2f,
        10,
        WHITE
    );

    DrawRectangleRoundedLinesEx(
        adopterSearchBox,
        0.2f,
        10,
        2,
        adopterInputField == 1
        ? GUI_TEAL
        : GUI_BORDER
    );

    DrawTextEx(
        regularFont,
        adopterSearchText[0] == '\0'
        ? "Search by name or ID..."
        : adopterSearchText,
        (Vector2){
            adopterSearchBox.x + 12,
            adopterSearchBox.y + 9
        },
        13,
        1,
        adopterSearchText[0] == '\0'
        ? GUI_MUTED
        : GUI_TEXT
    );

    if (CheckCollisionPointRec(
            mouse,
            adopterSearchBox) &&
        IsMouseButtonPressed(
            MOUSE_BUTTON_LEFT))
    {
        adopterInputField = 1;
    }

    if (adopterInputField == 1)
    {
        int searchKey = GetCharPressed();

        while (searchKey > 0)
        {
            if (searchKey >= 32 && searchKey <= 125)
            {
                int length = strlen(adopterSearchText);

                if (length < 49)
                {
                    adopterSearchText[length] = (char)searchKey;
                    adopterSearchText[length + 1] = '\0';
                    adopterPage = 0;
                }
            }

            searchKey = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            int length = strlen(adopterSearchText);

            if (length > 0)
            {
                adopterSearchText[length - 1] = '\0';
                adopterPage = 0;
            }
        }
    }

    /* =================================================
       ADOPTER CARD GRID (filtered by search)
       ================================================= */

    const int adoptersPerPage = 6;

    int totalAdopterMatches = 0;

    for (int i = 0; i < adopterCount; i++)
    {
        if (adopterMatchesSearch(i))
            totalAdopterMatches++;
    }

    int totalAdopterPages =
        (totalAdopterMatches + adoptersPerPage - 1) / adoptersPerPage;

    if (totalAdopterPages < 1)
        totalAdopterPages = 1;

    if (adopterPage >= totalAdopterPages)
        adopterPage = totalAdopterPages - 1;

    if (adopterPage < 0)
        adopterPage = 0;

    /* PAGE UP / PAGE DOWN CONTROLS */

    if (totalAdopterMatches > adoptersPerPage)
    {
        Rectangle prevAdopterPageButton = {900, 175, 55, 34};
        Rectangle nextAdopterPageButton = {1105, 175, 55, 34};

        bool adopterPrevAllowed = adopterPage > 0;
        bool adopterNextAllowed = adopterPage < totalAdopterPages - 1;

        if (DrawButton(
                regularFont,
                prevAdopterPageButton,
                "< Prev",
                adopterPrevAllowed ? GUI_TEAL : GUI_BORDER,
                adopterPrevAllowed ? GUI_TEAL_LIGHT : GUI_BORDER) &&
            adopterPrevAllowed)
        {
            adopterPage--;
        }

        char adopterPageText[32];

        snprintf(
            adopterPageText,
            sizeof(adopterPageText),
            "Page %d of %d",
            adopterPage + 1,
            totalAdopterPages
        );

        Vector2 adopterPageTextSize =
            MeasureTextEx(regularFont, adopterPageText, 13, 1);

        DrawTextEx(
            regularFont,
            adopterPageText,
            (Vector2){
                965 + (130 - adopterPageTextSize.x) / 2,
                184
            },
            13,
            1,
            GUI_MUTED
        );

        if (DrawButton(
                regularFont,
                nextAdopterPageButton,
                "Next >",
                adopterNextAllowed ? GUI_TEAL : GUI_BORDER,
                adopterNextAllowed ? GUI_TEAL_LIGHT : GUI_BORDER) &&
            adopterNextAllowed)
        {
            adopterPage++;
        }
    }

    int shown = 0;
    int matchedAdopterIndex = 0;

    for (int i = 0;
         i < adopterCount;
         i++)
    {
        if (!adopterMatchesSearch(i))
            continue;

        int pageOfAdopterMatch = matchedAdopterIndex / adoptersPerPage;
        matchedAdopterIndex++;

        if (pageOfAdopterMatch != adopterPage)
            continue;

                int column =
                    shown % 3;

                int row =
                    shown / 3;

                shown++;

                Rectangle card =
                {
                    270 + column * 295,
                    220 + row * 195,
                    275,
                    175
                };

                DrawRectangleRounded(
                    card,
                    0.12f,
                    12,
                    WHITE
                );

                /* CARD TITLE */

                DrawTextEx(
                    semiBoldFont,
                    adopters[i].Name,
                    (Vector2){
                        card.x + 18,
                        card.y + 18
                    },
                    18,
                    1,
                    GUI_TEXT
                );

                /* ID */

                char idText[32];

                snprintf(
                    idText,
                    sizeof(idText),
                    "ID: %d",
                    adopters[i].ID
                );

                DrawTextEx(
                    regularFont,
                    idText,
                    (Vector2){
                        card.x + 18,
                        card.y + 47
                    },
                    12,
                    1,
                    GUI_MUTED
                );

                /* PHONE */

                DrawTextEx(
                    regularFont,
                    adopters[i].Phone,
                    (Vector2){
                        card.x + 18,
                        card.y + 68
                    },
                    12,
                    1,
                    GUI_MUTED
                );

                /* PREFERRED SPECIES */

                char speciesText[64];

                snprintf(
                    speciesText,
                    sizeof(speciesText),
                    "Looking for: %s",
                    adopters[i].PreferredSpecies
                );

                DrawTextEx(
                    regularFont,
                    speciesText,
                    (Vector2){
                        card.x + 18,
                        card.y + 91
                    },
                    12,
                    1,
                    GUI_TEAL
                );

                /* AGE RANGE */

                char ageText[64];

                snprintf(
                    ageText,
                    sizeof(ageText),
                    "Age: %d - %d",
                    adopters[i].minAge,
                    adopters[i].maxAge
                );

                DrawTextEx(
                    regularFont,
                    ageText,
                    (Vector2){
                        card.x + 18,
                        card.y + 113
                    },
                    11,
                    1,
                    GUI_MUTED
                );

                /* SIZE */

                DrawTextEx(
                    regularFont,
                    adopters[i].PreferredSize,
                    (Vector2){
                        card.x + 185,
                        card.y + 113
                    },
                    11,
                    1,
                    GUI_MUTED
                );

                /* EDIT / DELETE */

                Rectangle editAdopterButton =
                {
                    card.x + 18,
                    card.y + 145,
                    115,
                    26
                };

                Rectangle deleteAdopterButton =
                {
                    card.x + 142,
                    card.y + 145,
                    115,
                    26
                };

                if (DrawButton(
                        regularFont,
                        editAdopterButton,
                        "Edit",
                        GUI_TEAL,
                        GUI_TEAL_LIGHT) &&
                    adopterPendingDeleteIndex == -1)
                {
                    populateAdopterForm(i);

                    selectedAdopter = i;
                    adopterAction = 3;
                    adopterFormPage = 1;
                    adopterFormField = 0;
                }

                if (DrawButton(
                        regularFont,
                        deleteAdopterButton,
                        "Delete",
                        GUI_ERROR,
                        GUI_ERROR) &&
                    adopterPendingDeleteIndex == -1)
                {
                    adopterPendingDeleteIndex = i;
                }
            }

    /* =============================================
       DELETE CONFIRMATION DIALOG
       ============================================= */

    if (adopterPendingDeleteIndex >= 0 &&
        adopterPendingDeleteIndex < adopterCount)
    {
        DrawRectangle(
            0, 0,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            (Color){0, 0, 0, 140}
        );

        Rectangle adopterConfirmCard =
        {
            (SCREEN_WIDTH - 420) / 2.0f,
            (SCREEN_HEIGHT - 210) / 2.0f,
            420,
            210
        };

        DrawRectangleRounded(adopterConfirmCard, 0.08f, 12, WHITE);

        DrawTextEx(
            semiBoldFont,
            "Delete this adopter?",
            (Vector2){adopterConfirmCard.x + 30, adopterConfirmCard.y + 26},
            20,
            1,
            GUI_NAVY
        );

        char adopterDeleteMsg[120];

        snprintf(
            adopterDeleteMsg,
            sizeof(adopterDeleteMsg),
            "%s will be permanently removed.",
            adopters[adopterPendingDeleteIndex].Name
        );

        DrawTextEx(
            regularFont,
            adopterDeleteMsg,
            (Vector2){adopterConfirmCard.x + 30, adopterConfirmCard.y + 62},
            13,
            1,
            GUI_MUTED
        );

        DrawTextEx(
            regularFont,
            "This action cannot be undone.",
            (Vector2){adopterConfirmCard.x + 30, adopterConfirmCard.y + 84},
            13,
            1,
            GUI_MUTED
        );

        Rectangle cancelDeleteAdopterButton =
        {
            adopterConfirmCard.x + 30,
            adopterConfirmCard.y + 140,
            165,
            45
        };

        Rectangle confirmDeleteAdopterButton =
        {
            adopterConfirmCard.x + 225,
            adopterConfirmCard.y + 140,
            165,
            45
        };

        if (DrawButton(
                semiBoldFont,
                cancelDeleteAdopterButton,
                "Cancel",
                GUI_NAVY,
                GUI_TEAL))
        {
            adopterPendingDeleteIndex = -1;
        }

        if (DrawButton(
                semiBoldFont,
                confirmDeleteAdopterButton,
                "Delete",
                GUI_ERROR,
                GUI_ERROR))
        {
            deleteAdopterByIndex(adopterPendingDeleteIndex);
            adopterPendingDeleteIndex = -1;
        }
    }

    if (adopterCount > 0 && shown == 0)
    {
        DrawTextEx(
            regularFont,
            "No adopters match your search.",
            (Vector2){
                270,
                220
            },
            14,
            1,
            GUI_MUTED
        );
    }
}
/* =================================================
   ADD ADOPTER FORM
   ================================================= */

if (adopterAction == 1 || adopterAction == 3)
{/* =================================================
   ADD ADOPTER FORM
   ================================================= */

if (adopterFormPage == 1)
{
    /* =================================================
       PAGE TITLE
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        adopterAction == 3
        ? "Edit Adopter"
        : "Add New Adopter",
        (Vector2){
            270,
            205
        },
        24,
        1,
        GUI_NAVY
    );

    DrawTextEx(
        regularFont,
        "Personal and household information",
        (Vector2){
            270,
            240
        },
        14,
        1,
        GUI_MUTED
    );

    /* =================================================
       NAME
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Name",
        (Vector2){
            270,
            280
        },
        13,
        1,
        GUI_TEXT
    );

    Rectangle nameBox =
    {
        270,
        300,
        350,
        42
    };

    DrawRectangleRounded(
        nameBox,
        0.12f,
        10,
        WHITE
    );

    DrawRectangleRoundedLinesEx(
        nameBox,
        0.12f,
        10,
        2,
        adopterFormField == 1
        ? GUI_TEAL
        : GUI_BORDER
    );

    DrawTextEx(
        regularFont,
        adopterNameInput,
        (Vector2){
            285,
            312
        },
        14,
        1,
        GUI_TEXT
    );

    /* =================================================
       PHONE
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Phone",
        (Vector2){
            650,
            280
        },
        13,
        1,
        GUI_TEXT
    );

    Rectangle phoneBox =
    {
        650,
        300,
        350,
        42
    };

    DrawRectangleRounded(
        phoneBox,
        0.12f,
        10,
        WHITE
    );

    DrawRectangleRoundedLinesEx(
        phoneBox,
        0.12f,
        10,
        2,
        adopterFormField == 2
        ? GUI_TEAL
        : GUI_BORDER
    );

    DrawTextEx(
        regularFont,
        adopterPhoneInput,
        (Vector2){
            665,
            312
        },
        14,
        1,
        GUI_TEXT
    );

    /* =================================================
       EMAIL
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Email",
        (Vector2){
            270,
            365
        },
        13,
        1,
        GUI_TEXT
    );

    Rectangle emailBox =
    {
        270,
        385,
        350,
        42
    };

    DrawRectangleRounded(
        emailBox,
        0.12f,
        10,
        WHITE
    );

    DrawRectangleRoundedLinesEx(
        emailBox,
        0.12f,
        10,
        2,
        adopterFormField == 3
        ? GUI_TEAL
        : GUI_BORDER
    );

    DrawTextEx(
        regularFont,
        adopterEmailInput,
        (Vector2){
            285,
            397
        },
        14,
        1,
        GUI_TEXT
    );

/* =================================================
   GENDER
   ================================================= */

DrawTextEx(
    semiBoldFont,
    "Gender",
    (Vector2){
        650,
        365
    },
    13,
    1,
    GUI_TEXT
);

Rectangle genderMale =
{
    650,
    385,
    80,
    42
};

Rectangle genderFemale =
{
    740,
    385,
    80,
    42
};

bool genderMaleSelected =
    strcmp(adopterGenderInput, "M") == 0;

bool genderFemaleSelected =
    strcmp(adopterGenderInput, "F") == 0;

DrawRectangleRounded(
    genderMale,
    0.12f,
    10,
    genderMaleSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawRectangleRounded(
    genderFemale,
    0.12f,
    10,
    genderFemaleSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawTextEx(
    semiBoldFont,
    "Male",
    (Vector2){
        667,
        397
    },
    13,
    1,
    genderMaleSelected
        ? WHITE
        : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "Female",
    (Vector2){
        748,
        397
    },
    13,
    1,
    genderFemaleSelected
        ? WHITE
        : GUI_TEXT
);
    /* =================================================
       ADDRESS
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Address",
        (Vector2){
            270,
            450
        },
        13,
        1,
        GUI_TEXT
    );

    Rectangle addressBox =
    {
        270,
        470,
        730,
        42
    };

    DrawRectangleRounded(
        addressBox,
        0.12f,
        10,
        WHITE
    );

    DrawRectangleRoundedLinesEx(
        addressBox,
        0.12f,
        10,
        2,
        adopterFormField == 5
        ? GUI_TEAL
        : GUI_BORDER
    );

    DrawTextEx(
        regularFont,
        adopterAddressInput,
        (Vector2){
            285,
            482
        },
        14,
        1,
        GUI_TEXT
    );
/* =================================================
   SCHEDULE
   ================================================= */

DrawTextEx(
    semiBoldFont,
    "Schedule",
    (Vector2){
        270,
        535
    },
    13,
    1,
    GUI_TEXT
);

Rectangle scheduleBusy =
{
    270,
    555,
    65,
    42
};

Rectangle scheduleModerate =
{
    345,
    555,
    90,
    42
};

Rectangle scheduleLight =
{
    445,
    555,
    65,
    42
};

bool scheduleBusySelected =
    strcmp(adopterScheduleInput, "Busy") == 0;

bool scheduleModerateSelected =
    strcmp(adopterScheduleInput, "Moderate") == 0;

bool scheduleLightSelected =
    strcmp(adopterScheduleInput, "Light") == 0;

DrawRectangleRounded(
    scheduleBusy,
    0.12f,
    10,
    scheduleBusySelected
    ? GUI_TEAL
    : GUI_TEAL_PALE
);

DrawRectangleRounded(
    scheduleModerate,
    0.12f,
    10,
    scheduleModerateSelected
    ? GUI_TEAL
    : GUI_TEAL_PALE
);

DrawRectangleRounded(
    scheduleLight,
    0.12f,
    10,
    scheduleLightSelected
    ? GUI_TEAL
    : GUI_TEAL_PALE
);

DrawTextEx(
    semiBoldFont,
    "Busy",
    (Vector2){
        285,
        567
    },
    13,
    1,
    scheduleBusySelected
    ? WHITE
    : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "Moderate",
    (Vector2){
        353,
        567
    },
    13,
    1,
    scheduleModerateSelected
    ? WHITE
    : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "Light",
    (Vector2){
        460,
        567
    },
    13,
    1,
    scheduleLightSelected
    ? WHITE
    : GUI_TEXT
);
/* =================================================
   HOME TYPE
   ================================================= */

DrawTextEx(
    semiBoldFont,
    "Home Type",
    (Vector2){
        530,
        535
    },
    13,
    1,
    GUI_TEXT
);

Rectangle homeHouse =
{
    530,
    555,
    80,
    42
};

Rectangle homeApartment =
{
    620,
    555,
    100,
    42
};
bool homeHouseSelected =
    strcmp(adopterHomeTypeInput, "House") == 0;

bool homeApartmentSelected =
    strcmp(adopterHomeTypeInput, "Apartment") == 0;

DrawRectangleRounded(
    homeHouse,
    0.12f,
    10,
    homeHouseSelected
    ? GUI_TEAL
    : GUI_TEAL_PALE
);

DrawRectangleRounded(
    homeApartment,
    0.12f,
    10,
    homeApartmentSelected
    ? GUI_TEAL
    : GUI_TEAL_PALE
);

DrawTextEx(
    semiBoldFont,
    "House",
    (Vector2){
        548,
        567
    },
    13,
    1,
    homeHouseSelected
    ? WHITE
    : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "Apartment",
    (Vector2){
        630,
        567
    },
    13,
    1,
    homeApartmentSelected
    ? WHITE
    : GUI_TEXT
);
    /* =================================================
       GARDEN
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Garden",
        (Vector2){
            750,
            535
        },
        13,
        1,
        GUI_TEXT
    );

Rectangle gardenYes =
{
    750,
    555,
    75,
    42
};

Rectangle gardenNo =
{
    835,
    555,
    75,
    42
};
    bool gardenYesSelected =
        strcmp(
            adopterGardenInput,
            "Yes"
        ) == 0;

    bool gardenNoSelected =
        strcmp(
            adopterGardenInput,
            "No"
        ) == 0;

    DrawRectangleRounded(
        gardenYes,
        0.12f,
        10,
        gardenYesSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
    );

    DrawRectangleRounded(
        gardenNo,
        0.12f,
        10,
        gardenNoSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
    );

    DrawTextEx(
        semiBoldFont,
        "Yes",
        (Vector2){
            775,
            567
        },
        13,
        1,
        gardenYesSelected
        ? WHITE
        : GUI_TEXT
    );

    DrawTextEx(
        semiBoldFont,
        "No",
        (Vector2){
            861,
            567
        },
        13,
        1,
        gardenNoSelected
        ? WHITE
        : GUI_TEXT
    );

/* =================================================
   EXPERIENCE
   ================================================= */

DrawTextEx(
    semiBoldFont,
    "Experience",
    (Vector2){
        600,
        625
    },
    13,
    1,
    GUI_TEXT
);

Rectangle experienceYes =
{
    685,
    615,
    75,
    42
};

Rectangle experienceNo =
{
    770,
    615,
    75,
    42
};

bool experienceYesSelected =
    strcmp(adopterExperienceInput, "Yes") == 0;

bool experienceNoSelected =
    strcmp(adopterExperienceInput, "No") == 0;

DrawRectangleRounded(
    experienceYes,
    0.12f,
    10,
    experienceYesSelected
    ? GUI_TEAL
    : GUI_TEAL_PALE
);

DrawRectangleRounded(
    experienceNo,
    0.12f,
    10,
    experienceNoSelected
    ? GUI_TEAL
    : GUI_TEAL_PALE
);

DrawTextEx(
    semiBoldFont,
    "Yes",
    (Vector2){
        710,
        627
    },
    13,
    1,
    experienceYesSelected
    ? WHITE
    : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "No",
    (Vector2){
        797,
        627
    },
    13,
    1,
    experienceNoSelected
    ? WHITE
    : GUI_TEXT
);
    /* =================================================
       CHILDREN
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Children",
        (Vector2){
            270,
            625
        },
        13,
        1,
        GUI_TEXT
    );

    Rectangle childrenYes =
    {
        350,
        615,
        75,
        42
    };

    Rectangle childrenNo =
    {
        435,
        615,
        75,
        42
    };

    bool childrenYesSelected =
        strcmp(
            adopterChildrenInput,
            "Yes"
        ) == 0;

    bool childrenNoSelected =
        strcmp(
            adopterChildrenInput,
            "No"
        ) == 0;

    DrawRectangleRounded(
        childrenYes,
        0.12f,
        10,
        childrenYesSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
    );

    DrawRectangleRounded(
        childrenNo,
        0.12f,
        10,
        childrenNoSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
    );

    DrawTextEx(
        semiBoldFont,
        "Yes",
        (Vector2){
            375,
            627
        },
        13,
        1,
        childrenYesSelected
        ? WHITE
        : GUI_TEXT
    );

    DrawTextEx(
        semiBoldFont,
        "No",
        (Vector2){
            461,
            627
        },
        13,
        1,
        childrenNoSelected
        ? WHITE
        : GUI_TEXT
    );
    /* =================================================
   PAGE 1 TEXT FIELD CLICKS
   ================================================= */
   /* =================================================
   PAGE 1 FIELD CLICKS
   ================================================= */

if (CheckCollisionPointRec(mouse, nameBox) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    adopterFormField = 1;
}

if (CheckCollisionPointRec(mouse, phoneBox) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    adopterFormField = 2;
}

if (CheckCollisionPointRec(mouse, emailBox) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    adopterFormField = 3;
}


/* ADDRESS */
if (CheckCollisionPointRec(mouse, addressBox) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    adopterFormField = 5;
}



/* GARDEN */
if (CheckCollisionPointRec(mouse, gardenYes) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    strcpy(adopterGardenInput, "Yes");
}

if (CheckCollisionPointRec(mouse, gardenNo) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    strcpy(adopterGardenInput, "No");
}

/* CHILDREN */
if (CheckCollisionPointRec(mouse, childrenYes) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    strcpy(adopterChildrenInput, "Yes");
}

if (CheckCollisionPointRec(mouse, childrenNo) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    strcpy(adopterChildrenInput, "No");
}
/* =================================================
   PAGE 1 TEXT INPUT
   ================================================= */

int key = GetCharPressed();

while (key > 0)
{
    if (key >= 32 && key <= 125)
    {
        if (adopterFormField == 1)
        {
            int length = strlen(adopterNameInput);

            if (length < 49)
            {
                adopterNameInput[length] = (char)key;
                adopterNameInput[length + 1] = '\0';
            }
        }

        else if (adopterFormField == 2)
        {
            int length = strlen(adopterPhoneInput);

            if (length < 19)
            {
                adopterPhoneInput[length] = (char)key;
                adopterPhoneInput[length + 1] = '\0';
            }
        }

        else if (adopterFormField == 3)
        {
            int length = strlen(adopterEmailInput);

            if (length < 49)
            {
                adopterEmailInput[length] = (char)key;
                adopterEmailInput[length + 1] = '\0';
            }
        }

        else if (adopterFormField == 5)
        {
            int length = strlen(adopterAddressInput);

            if (length < 49)
            {
                adopterAddressInput[length] = (char)key;
                adopterAddressInput[length + 1] = '\0';
            }
        }
        else if (adopterFormField == 10)
{
    if (key >= '0' && key <= '9')
    {
        int length = strlen(adopterMinAgeInput);

        if (length < 3)
        {
            adopterMinAgeInput[length] = (char)key;
            adopterMinAgeInput[length + 1] = '\0';
        }
    }
}
else if (adopterFormField == 11)
{
    if (key >= '0' && key <= '9')
    {
        int length = strlen(adopterMaxAgeInput);

        if (length < 3)
        {
            adopterMaxAgeInput[length] = (char)key;
            adopterMaxAgeInput[length + 1] = '\0';
        }
    }
}
    }

    key = GetCharPressed();
}


    /* =================================================
       PAGE 1 BACKSPACE
       ================================================= */

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        if (adopterFormField == 1)
        {
            int length = strlen(adopterNameInput);

            if (length > 0)
                adopterNameInput[length - 1] = '\0';
        }
        else if (adopterFormField == 2)
        {
            int length = strlen(adopterPhoneInput);

            if (length > 0)
                adopterPhoneInput[length - 1] = '\0';
        }
        else if (adopterFormField == 3)
        {
            int length = strlen(adopterEmailInput);

            if (length > 0)
                adopterEmailInput[length - 1] = '\0';
        }
        else if (adopterFormField == 5)
        {
            int length = strlen(adopterAddressInput);

            if (length > 0)
                adopterAddressInput[length - 1] = '\0';
        }
        else if (adopterFormField == 10)
{
    int length = strlen(adopterMinAgeInput);

    if (length > 0)
        adopterMinAgeInput[length - 1] = '\0';
}
else if (adopterFormField == 11)
{
    int length = strlen(adopterMaxAgeInput);

    if (length > 0)
        adopterMaxAgeInput[length - 1] = '\0';
}
    }
    /* =================================================
       NAVIGATION BUTTONS
       ================================================= */

    Rectangle cancelButton =
    {
        650,
        665,
        150,
        42
    };

    Rectangle nextButton =
    {
        820,
        665,
        180,
        42
    };

    if (DrawButton(
            regularFont,
            cancelButton,
            "Cancel",
            GUI_NAVY,
            GUI_TEAL))
    {
        adopterAction = 0;
        selectedAdopter = -1;
        adopterFormPage = 1;
        adopterFormField = 0;
    }

    if (DrawButton(
            semiBoldFont,
            nextButton,
            "Next",
            GUI_TEAL,
            GUI_TEAL_LIGHT))
    {
        adopterFormPage = 2;
        adopterFormField = 0;
    }
/* =================================================
   GENDER CLICK
   ================================================= */

if (CheckCollisionPointRec(
        mouse,
        genderMale) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterGenderInput,
        "M"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        genderFemale) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterGenderInput,
        "F"
    );

    adopterFormField = 0;
}
/* =================================================
   HOME TYPE CLICK
   ================================================= */

if (CheckCollisionPointRec(
        mouse,
        homeHouse) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterHomeTypeInput,
        "House"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        homeApartment) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterHomeTypeInput,
        "Apartment"
    );

    adopterFormField = 0;
}
    /* =================================================
       GARDEN CLICK
       ================================================= */

    if (CheckCollisionPointRec(
            mouse,
            gardenYes) &&
        IsMouseButtonPressed(
            MOUSE_BUTTON_LEFT))
    {
        strcpy(
            adopterGardenInput,
            "Yes"
        );
    }

    if (CheckCollisionPointRec(
            mouse,
            gardenNo) &&
        IsMouseButtonPressed(
            MOUSE_BUTTON_LEFT))
    {
        strcpy(
            adopterGardenInput,
            "No"
        );
    }

/* =================================================
   SCHEDULE CLICK
   ================================================= */

if (CheckCollisionPointRec(
        mouse,
        scheduleBusy) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterScheduleInput,
        "Busy"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        scheduleModerate) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterScheduleInput,
        "Moderate"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        scheduleLight) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterScheduleInput,
        "Light"
    );

    adopterFormField = 0;
}   
/* =================================================
   EXPERIENCE CLICK
   ================================================= */

if (CheckCollisionPointRec(
        mouse,
        experienceYes) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterExperienceInput,
        "Yes"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        experienceNo) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterExperienceInput,
        "No"
    );

    adopterFormField = 0;
}
 /* =================================================
       CHILDREN CLICK
       ================================================= */

    if (CheckCollisionPointRec(
            mouse,
            childrenYes) &&
        IsMouseButtonPressed(
            MOUSE_BUTTON_LEFT))
    {
        strcpy(
            adopterChildrenInput,
            "Yes"
        );
    }

    if (CheckCollisionPointRec(
            mouse,
            childrenNo) &&
        IsMouseButtonPressed(
            MOUSE_BUTTON_LEFT))
    {
        strcpy(
            adopterChildrenInput,
            "No"
        );
    }
}
else if (adopterFormPage == 2)
{
    /* =================================================
       PAGE 2 — ADOPTION PREFERENCES
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Adoption Preferences",
        (Vector2){
            270,
            205
        },
        24,
        1,
        GUI_NAVY
    );

    DrawTextEx(
        regularFont,
        "Tell us what kind of companion the adopter is looking for.",
        (Vector2){
            270,
            240
        },
        14,
        1,
        GUI_MUTED
    );

/* =================================================
   PREFERRED SPECIES
   ================================================= */

DrawTextEx(
    semiBoldFont,
    "Preferred Species",
    (Vector2){
        800,
        280
    },
    13,
    1,
    GUI_TEXT
);

Rectangle speciesCat =
{
    800,
    300,
    90,
    42
};

Rectangle speciesDog =
{
    900,
    300,
    90,
    42
};

bool catSelected =
    strcmp(adopterSpeciesInput, "Cat") == 0;

bool dogSelected =
    strcmp(adopterSpeciesInput, "Dog") == 0;

DrawRectangleRounded(
    speciesCat,
    0.12f,
    10,
    catSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawRectangleRounded(
    speciesDog,
    0.12f,
    10,
    dogSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawTextEx(
    semiBoldFont,
    "Cat",
    (Vector2){
        830,
        312
    },
    13,
    1,
    catSelected
        ? WHITE
        : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "Dog",
    (Vector2){
        930,
        312
    },
    13,
    1,
    dogSelected
        ? WHITE
        : GUI_TEXT
);
    /* =================================================
       MINIMUM AGE
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Minimum Age",
        (Vector2){
            270,
            280
        },
        13,
        1,
        GUI_TEXT
    );

    Rectangle minAgeBox =
    {
        270,
        300,
        210,
        42
    };

    DrawRectangleRounded(
        minAgeBox,
        0.12f,
        10,
        WHITE
    );

    DrawRectangleRoundedLinesEx(
        minAgeBox,
        0.12f,
        10,
        2,
        adopterFormField == 10
        ? GUI_TEAL
        : GUI_BORDER
    );

    DrawTextEx(
        regularFont,
        adopterMinAgeInput,
        (Vector2){
            285,
            312
        },
        14,
        1,
        GUI_TEXT
    );

    /* =================================================
       MAXIMUM AGE
       ================================================= */

    DrawTextEx(
        semiBoldFont,
        "Maximum Age",
        (Vector2){
            500,
            280
        },
        13,
        1,
        GUI_TEXT
    );

    Rectangle maxAgeBox =
    {
        500,
        300,
        210,
        42
    };

    DrawRectangleRounded(
        maxAgeBox,
        0.12f,
        10,
        WHITE
    );

    DrawRectangleRoundedLinesEx(
        maxAgeBox,
        0.12f,
        10,
        2,
        adopterFormField == 11
        ? GUI_TEAL
        : GUI_BORDER
    );

    DrawTextEx(
        regularFont,
        adopterMaxAgeInput,
        (Vector2){
            515,
            312
        },
        14,
        1,
        GUI_TEXT
    );

/* =================================================
   PREFERRED SIZE
   ================================================= */

DrawTextEx(
    semiBoldFont,
    "Preferred Size",
    (Vector2){
        270,
        390
    },
    13,
    1,
    GUI_TEXT
);

Rectangle sizeSmall =
{
    270,
    415,
    120,
    42
};

Rectangle sizeMedium =
{
    400,
    415,
    120,
    42
};

Rectangle sizeLarge =
{
    530,
    415,
    120,
    42
};

bool smallSelected =
    strcmp(adopterSizeInput, "Small") == 0;

bool mediumSelected =
    strcmp(adopterSizeInput, "Medium") == 0;

bool largeSelected =
    strcmp(adopterSizeInput, "Large") == 0;

DrawRectangleRounded(
    sizeSmall,
    0.12f,
    10,
    smallSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawRectangleRounded(
    sizeMedium,
    0.12f,
    10,
    mediumSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawRectangleRounded(
    sizeLarge,
    0.12f,
    10,
    largeSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawTextEx(
    semiBoldFont,
    "Small",
    (Vector2){
        305,
        427
    },
    13,
    1,
    smallSelected
        ? WHITE
        : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "Medium",
    (Vector2){
        425,
        427
    },
    13,
    1,
    mediumSelected
        ? WHITE
        : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "Large",
    (Vector2){
        565,
        427
    },
    13,
    1,
    largeSelected
        ? WHITE
        : GUI_TEXT
);
/* =================================================
   PREFERRED GENDER
   ================================================= */

DrawTextEx(
    semiBoldFont,
    "Preferred Gender",
    (Vector2){
        680,
        390
    },
    13,
    1,
    GUI_TEXT
);

Rectangle preferredGenderMale =
{
    680,
    415,
    95,
    42
};

Rectangle preferredGenderFemale =
{
    785,
    415,
    95,
    42
};

Rectangle preferredGenderAny =
{
    890,
    415,
    95,
    42
};

bool maleSelected =
    strcmp(adopterPreferredGenderInput, "M") == 0;

bool femaleSelected =
    strcmp(adopterPreferredGenderInput, "F") == 0;

bool anySelected =
    strcmp(adopterPreferredGenderInput, "A") == 0;

DrawRectangleRounded(
    preferredGenderMale,
    0.12f,
    10,
    maleSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawRectangleRounded(
    preferredGenderFemale,
    0.12f,
    10,
    femaleSelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawRectangleRounded(
    preferredGenderAny,
    0.12f,
    10,
    anySelected
        ? GUI_TEAL
        : GUI_TEAL_PALE
);

DrawTextEx(
    semiBoldFont,
    "Male",
    (Vector2){
        705,
        427
    },
    13,
    1,
    maleSelected
        ? WHITE
        : GUI_TEXT
);
DrawTextEx(
    semiBoldFont,
    "Female",
    (Vector2){802, 427},
    13,
    1,
    femaleSelected ? WHITE : GUI_TEXT
);

DrawTextEx(
    semiBoldFont,
    "Any",
    (Vector2){920, 427},
    13,
    1,
    anySelected ? WHITE : GUI_TEXT
);
    /* =================================================
       FORM SUMMARY
       ================================================= */

DrawRectangleRounded(
    (Rectangle){
        270,
        490,
        730,
        100
    },
    0.12f,
    10,
    GUI_TEAL_PALE
);

DrawTextEx(
    semiBoldFont,
    "Adoption requirements",
    (Vector2){295, 515},
    15,
    1,
    GUI_NAVY
);

DrawTextEx(
    regularFont,
    "The information above will be used when matching",
    (Vector2){295, 540},
    13,
    1,
    GUI_MUTED
);

DrawTextEx(
    regularFont,
    "the adopter with suitable pets.",
    (Vector2){295, 560},
    13,
    1,
    GUI_MUTED
);
    /* =================================================
       BACK BUTTON
       ================================================= */

    Rectangle backFormButton =
    {
        650,
        620,
        150,
        45
    };

    if (DrawButton(
            regularFont,
            backFormButton,
            "Back",
            GUI_NAVY,
            GUI_TEAL))
    {
        adopterFormPage = 1;
        adopterFormField = 0;
    }

    /* =================================================
       ADD ADOPTER BUTTON
       ================================================= */

    Rectangle saveAdopterButton =
    {
        820,
        620,
        180,
        45
    };

    if (DrawButton(
            semiBoldFont,
            saveAdopterButton,
           adopterAction == 3
               ? "Save Changes"
               : "Add Adopter",
            GUI_TEAL,
            GUI_TEAL_LIGHT))
    {
    if (adopterAction == 3)
    {
        updateAdopterFromGUI();
    }
    else
    {
        saveAdopterFromGUI();
    }

    adopterAction = 0;
    selectedAdopter = -1;
    adopterFormPage = 1;
    adopterFormField = 0;
    adopterInputField = 0;

    /* Clear form */
    adopterNameInput[0] = '\0';
    adopterPhoneInput[0] = '\0';
    adopterEmailInput[0] = '\0';
    adopterGenderInput[0] = '\0';
    adopterAddressInput[0] = '\0';
    adopterScheduleInput[0] = '\0';
    adopterHomeTypeInput[0] = '\0';
    adopterMinAgeInput[0] = '\0';
    adopterMaxAgeInput[0] = '\0';
    adopterSpeciesInput[0] = '\0';
    adopterSizeInput[0] = '\0';
    adopterPreferredGenderInput[0] = '\0';
    adopterGardenInput[0] = '\0';
    adopterExperienceInput[0] = '\0';
    adopterChildrenInput[0] = '\0';
}
/* =================================================
   PAGE 2 FIELD CLICKS
   ================================================= */

/* MINIMUM AGE */

if ((adopterAction == 1 || adopterAction == 3) &&
    adopterFormPage == 2 &&
    CheckCollisionPointRec(mouse, minAgeBox) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    adopterFormField = 10;
}
/* Max age */
if ((adopterAction == 1 || adopterAction == 3) &&
    adopterFormPage == 2 &&
    CheckCollisionPointRec(mouse, maxAgeBox) &&
    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    adopterFormField = 11;
}

/* =================================================
   PREFERRED SPECIES
   ================================================= */

if (CheckCollisionPointRec(
        mouse,
        speciesCat) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterSpeciesInput,
        "Cat"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        speciesDog) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterSpeciesInput,
        "Dog"
    );

    adopterFormField = 0;
}


/* =================================================
   PREFERRED SIZE
   ================================================= */

if (CheckCollisionPointRec(
        mouse,
        sizeSmall) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterSizeInput,
        "Small"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        sizeMedium) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterSizeInput,
        "Medium"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        sizeLarge) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterSizeInput,
        "Large"
    );

    adopterFormField = 0;
}


/* =================================================
   PREFERRED GENDER
   ================================================= */

if (CheckCollisionPointRec(
        mouse,
        preferredGenderMale) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterPreferredGenderInput,
        "M"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        preferredGenderFemale) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterPreferredGenderInput,
        "F"
    );

    adopterFormField = 0;
}

if (CheckCollisionPointRec(
        mouse,
        preferredGenderAny) &&
    IsMouseButtonPressed(
        MOUSE_BUTTON_LEFT))
{
    strcpy(
        adopterPreferredGenderInput,
        "A"
    );

    adopterFormField = 0;
}
}
}
            EndDrawing();
        }

        /* =================================================
           MATCHING SCREEN
           ================================================= */

        else if (screen == SCREEN_MATCHING)
        {
            BeginDrawing();

            ClearBackground(GUI_BACKGROUND);

            /* TOP BAR */

            DrawRectangle(0, 0, SCREEN_WIDTH, 78, GUI_NAVY);

            DrawPaw((Vector2){45, 40}, 0.30f, GUI_TEAL_LIGHT);

            DrawTextEx(
                semiBoldFont,
                "MauPaws",
                (Vector2){75, 24},
                24,
                1,
                WHITE
            );

            DrawTextEx(
                regularFont,
                "Matching",
                (Vector2){950, 28},
                15,
                1,
                WHITE
            );

            /* SIDEBAR */

            DrawRectangle(0, 78, 230, SCREEN_HEIGHT - 78, WHITE);

            DrawLine(230, 78, 230, SCREEN_HEIGHT, GUI_BORDER);

            DrawTextEx(
                semiBoldFont,
                "PET MATCHING",
                (Vector2){30, 110},
                13,
                1,
                GUI_MUTED
            );

            Rectangle matchingBackButton = {18, 145, 194, 42};

            if (DrawButton(
                    semiBoldFont,
                    matchingBackButton,
                    "Dashboard",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                screen = SCREEN_DASHBOARD;
                matchingSelectedAdopter = -1;
                matchingMessage[0] = '\0';
                matchingAdopterIDInput[0] = '\0';
            }

            /* TITLE */

            DrawTextEx(
                semiBoldFont,
                "Adopter - Pet Matching",
                (Vector2){270, 115},
                24,
                1,
                GUI_NAVY
            );

            DrawTextEx(
                regularFont,
                "Select an adopter to see recommended pets",
                (Vector2){270, 150},
                14,
                1,
                GUI_MUTED
            );

            /* =============================================
               LEFT COLUMN: STAFF PICK FROM A LIST,
               ADOPTERS ENTER THEIR OWN ID
               ============================================= */

            if (role == ROLE_STAFF)
            {
            DrawRectangleRounded(
                (Rectangle){270, 185, 260, 530},
                0.05f,
                10,
                WHITE
            );

            DrawTextEx(
                semiBoldFont,
                "Adopters",
                (Vector2){290, 200},
                15,
                1,
                GUI_NAVY
            );

            if (adopterCount == 0)
            {
                DrawTextEx(
                    regularFont,
                    "No adopters registered.",
                    (Vector2){290, 235},
                    13,
                    1,
                    GUI_MUTED
                );
            }

            for (int i = 0; i < adopterCount && i < 10; i++)
            {
                Rectangle adopterRow =
                {
                    285,
                    230.0f + i * 47,
                    230,
                    40
                };

                bool selected = (matchingSelectedAdopter == i);

                DrawRectangleRounded(
                    adopterRow,
                    0.2f,
                    10,
                    selected ? GUI_TEAL : GUI_TEAL_PALE
                );

                char adopterLabel[80];

                snprintf(
                    adopterLabel,
                    sizeof(adopterLabel),
                    "%s (ID %d)",
                    adopters[i].Name,
                    adopters[i].ID
                );

                DrawTextEx(
                    regularFont,
                    adopterLabel,
                    (Vector2){adopterRow.x + 12, adopterRow.y + 12},
                    13,
                    1,
                    selected ? WHITE : GUI_TEXT
                );

                if (CheckCollisionPointRec(mouse, adopterRow) &&
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    matchingSelectedAdopter = i;
                    matchingMessage[0] = '\0';
                }
            }
            }
            else
            {
                /* =========================================
                   ADOPTER: ENTER YOUR OWN ID
                   ========================================= */

                Rectangle idCard =
                {
                    270,
                    185,
                    260,
                    230
                };

                DrawRectangleRounded(
                    idCard,
                    0.06f,
                    10,
                    WHITE
                );

                DrawTextEx(
                    semiBoldFont,
                    "Find Your Matches",
                    (Vector2){idCard.x + 20, idCard.y + 20},
                    16,
                    1,
                    GUI_NAVY
                );

                DrawTextEx(
                    regularFont,
                    "Enter your Adopter ID",
                    (Vector2){idCard.x + 20, idCard.y + 52},
                    13,
                    1,
                    GUI_MUTED
                );

                Rectangle idBox =
                {
                    idCard.x + 20,
                    idCard.y + 75,
                    220,
                    40
                };

                DrawTextInputBox(
                    regularFont,
                    idBox,
                    matchingAdopterIDInput,
                    sizeof(matchingAdopterIDInput),
                    "e.g. 1",
                    1,
                    &matchingIDInputField,
                    mouse
                );

                Rectangle findButton =
                {
                    idCard.x + 20,
                    idCard.y + 130,
                    220,
                    42
                };

                if (DrawButton(
                        semiBoldFont,
                        findButton,
                        "Find My Matches",
                        GUI_TEAL,
                        GUI_TEAL_LIGHT))
                {
                    int enteredID = atoi(matchingAdopterIDInput);
                    int foundIndex = -1;

                    for (int i = 0; i < adopterCount; i++)
                    {
                        if (adopters[i].ID == enteredID)
                        {
                            foundIndex = i;
                            break;
                        }
                    }

                    if (matchingAdopterIDInput[0] == '\0')
                    {
                        strcpy(matchingMessage, "Please enter your Adopter ID.");
                        matchingSelectedAdopter = -1;
                    }
                    else if (foundIndex == -1)
                    {
                        strcpy(matchingMessage, "Adopter ID not found. Please register first.");
                        matchingSelectedAdopter = -1;
                    }
                    else
                    {
                        matchingSelectedAdopter = foundIndex;
                        matchingMessage[0] = '\0';
                    }
                }

                if (matchingSelectedAdopter == -1 && matchingMessage[0] != '\0')
                {
                    DrawTextEx(
                        regularFont,
                        matchingMessage,
                        (Vector2){idCard.x + 20, idCard.y + 185},
                        12,
                        1,
                        GUI_ERROR
                    );
                }
            }

            /* =============================================
               RECOMMENDATIONS (RIGHT COLUMN)
               ============================================= */

            if (matchingSelectedAdopter < 0 ||
                matchingSelectedAdopter >= adopterCount)
            {
                DrawTextEx(
                    regularFont,
                    role == ROLE_STAFF
                    ? "Select an adopter from the list to view matches."
                    : "Enter your Adopter ID on the left to view matches.",
                    (Vector2){560, 250},
                    14,
                    1,
                    GUI_MUTED
                );
            }
            else
            {
                struct Adopter selectedAdopterData =
                    adopters[matchingSelectedAdopter];

                int scoreList[MAX_PETS];
                int indexList[MAX_PETS];
                int candidateCount = 0;

                for (int i = 0; i < petCount; i++)
                {
                    if (!sameString(pets[i].Availability, "Available"))
                        continue;

                    int score = calculateCompatibility(
                        pets[i].Species,
                        selectedAdopterData.PreferredSpecies,
                        pets[i].Age,
                        selectedAdopterData.minAge,
                        selectedAdopterData.maxAge,
                        pets[i].Size,
                        selectedAdopterData.PreferredSize,
                        pets[i].Gender,
                        selectedAdopterData.PreferredGender
                    );

                    if (sameString(pets[i].HealthStatus, "Healthy"))
                        score += 10;

                    if (score > 100)
                        score = 100;

                    indexList[candidateCount] = i;
                    scoreList[candidateCount] = score;
                    candidateCount++;
                }

                /* Bubble sort descending by score */

                for (int i = 0; i < candidateCount - 1; i++)
                {
                    for (int j = 0; j < candidateCount - i - 1; j++)
                    {
                        if (scoreList[j] < scoreList[j + 1])
                        {
                            int ts = scoreList[j];
                            scoreList[j] = scoreList[j + 1];
                            scoreList[j + 1] = ts;

                            int ti = indexList[j];
                            indexList[j] = indexList[j + 1];
                            indexList[j + 1] = ti;
                        }
                    }
                }

                char summary[80];

                snprintf(
                    summary,
                    sizeof(summary),
                    "Top matches for %s",
                    selectedAdopterData.Name
                );

                DrawTextEx(
                    semiBoldFont,
                    summary,
                    (Vector2){560, 195},
                    17,
                    1,
                    GUI_NAVY
                );

                if (candidateCount == 0)
                {
                    DrawTextEx(
                        regularFont,
                        "No available pets found.",
                        (Vector2){560, 230},
                        14,
                        1,
                        GUI_MUTED
                    );
                }

                int limit = candidateCount < 5 ? candidateCount : 5;

                for (int i = 0; i < limit; i++)
                {
                    int p = indexList[i];

                    Rectangle matchCard =
                    {
                        560,
                        225.0f + i * 92,
                        440,
                        80
                    };

                    DrawRectangleRounded(matchCard, 0.1f, 10, WHITE);

                    char petLine1[160];

                    snprintf(
                        petLine1,
                        sizeof(petLine1),
                        "%s  -  %s (%s)",
                        pets[p].Name,
                        pets[p].Species,
                        pets[p].Breed
                    );

                    DrawTextEx(
                        semiBoldFont,
                        petLine1,
                        (Vector2){matchCard.x + 15, matchCard.y + 12},
                        15,
                        1,
                        GUI_TEXT
                    );

                    char petLine2[100];

                    snprintf(
                        petLine2,
                        sizeof(petLine2),
                        "ID %d  |  Age %d  |  %s  |  Compatibility %d%%",
                        pets[p].ID,
                        pets[p].Age,
                        pets[p].Size,
                        scoreList[i]
                    );

                    DrawTextEx(
                        regularFont,
                        petLine2,
                        (Vector2){matchCard.x + 15, matchCard.y + 36},
                        13,
                        1,
                        GUI_MUTED
                    );

                    Rectangle applyButton =
                    {
                        matchCard.x + 320,
                        matchCard.y + 22,
                        105,
                        36
                    };

                    if (DrawButton(
                            regularFont,
                            applyButton,
                            "Apply",
                            GUI_TEAL,
                            GUI_TEAL_LIGHT))
                    {
                        char petIDText[16];

                        snprintf(
                            petIDText,
                            sizeof(petIDText),
                            "%d",
                            pets[p].ID
                        );

                        processAndSaveApplication(
                            selectedAdopterData,
                            petIDText
                        );

                        snprintf(
                            matchingMessage,
                            sizeof(matchingMessage),
                            "Application submitted for %s.",
                            pets[p].Name
                        );

                        break;
                    }
                }

                if (matchingMessage[0] != '\0')
                {
                    DrawTextEx(
                        regularFont,
                        matchingMessage,
                        (Vector2){560, 640},
                        14,
                        1,
                        GUI_TEAL
                    );
                }
            }

            EndDrawing();
        }

        /* =================================================
           MEDICAL SCREEN
           ================================================= */

        else if (screen == SCREEN_MEDICAL)
        {
            if (medicalAction == 0)
                loadMedicalRecordsGUI();

            BeginDrawing();

            ClearBackground(GUI_BACKGROUND);

            /* TOP BAR */

            DrawRectangle(0, 0, SCREEN_WIDTH, 78, GUI_NAVY);

            DrawPaw((Vector2){45, 40}, 0.30f, GUI_TEAL_LIGHT);

            DrawTextEx(
                semiBoldFont,
                "MauPaws",
                (Vector2){75, 24},
                24,
                1,
                WHITE
            );

            DrawTextEx(
                regularFont,
                "Medical",
                (Vector2){960, 28},
                15,
                1,
                WHITE
            );

            /* SIDEBAR */

            DrawRectangle(0, 78, 230, SCREEN_HEIGHT - 78, WHITE);

            DrawLine(230, 78, 230, SCREEN_HEIGHT, GUI_BORDER);

            DrawTextEx(
                semiBoldFont,
                "MEDICAL RECORDS",
                (Vector2){30, 110},
                13,
                1,
                GUI_MUTED
            );

            Rectangle medicalBackButton = {18, 145, 194, 42};

            if (DrawButton(
                    semiBoldFont,
                    medicalBackButton,
                    "Dashboard",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                screen = SCREEN_DASHBOARD;
                medicalAction = 0;
            }

            /* TITLE */

            DrawTextEx(
                semiBoldFont,
                medicalAction == 0
                    ? "Medical Records"
                    : "Add Medical Record",
                (Vector2){270, 115},
                24,
                1,
                GUI_NAVY
            );

            DrawTextEx(
                regularFont,
                "Track injuries, illness and treatment",
                (Vector2){270, 150},
                14,
                1,
                GUI_MUTED
            );

            Rectangle addMedicalButton = {950, 110, 140, 45};

            if (DrawButton(
                    semiBoldFont,
                    addMedicalButton,
                    medicalAction == 0 ? "+ Add Record" : "Cancel",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                if (medicalAction == 0)
                {
                    medicalAction = 1;
                    medicalPetIDInput[0] = '\0';
                    medicalInjuryLevel = 0;
                    medicalIllnessLevel = 0;
                    medicalPainLevel = 0;
                    medicalDehydrationLevel = 0;
                    medicalConditionLevel = 0;
                    medicalTreatmentChoice = 1;
                    medicalDateInput[0] = '\0';
                    medicalInputField = 0;
                    medicalMessage[0] = '\0';
                }
                else
                {
                    medicalAction = 0;
                }
            }

            if (medicalAction == 0)
            {
                /* =========================================
                   VIEW RECORDS
                   ========================================= */

                if (medicalRecordCount == 0)
                {
                    DrawTextEx(
                        regularFont,
                        "No medical records found.",
                        (Vector2){270, 220},
                        14,
                        1,
                        GUI_MUTED
                    );
                }

                int visibleRows = 0;

                for (int i = 0; i < medicalRecordCount && visibleRows < 6; i++)
                {
                    Rectangle row =
                    {
                        270,
                        210.0f + visibleRows * 78,
                        890,
                        68
                    };

                    visibleRows++;

                    DrawRectangleRounded(row, 0.08f, 10, WHITE);

                    char petAge[32] = "unknown";
                    int age = getPetAge(medicalRecords[i].petID);

                    if (age >= 0)
                        snprintf(petAge, sizeof(petAge), "%d yrs", age);

                    char petNameForRecord[50] = "Unknown Pet";

                    for (int p = 0; p < petCount; p++)
                    {
                        if (pets[p].ID == medicalRecords[i].petID)
                        {
                            strcpy(petNameForRecord, pets[p].Name);
                            break;
                        }
                    }

                    char line1[180];

                    snprintf(
                        line1,
                        sizeof(line1),
                        "%s (ID %d)  (%s)  -  Treatment: %s",
                        petNameForRecord,
                        medicalRecords[i].petID,
                        petAge,
                        medicalRecords[i].treatment
                    );

                    DrawTextEx(
                        semiBoldFont,
                        line1,
                        (Vector2){row.x + 15, row.y + 10},
                        14,
                        1,
                        GUI_TEXT
                    );

                    char line2[150];

                    snprintf(
                        line2,
                        sizeof(line2),
                        "Injury %d | Illness %d | Pain %d | Dehydration %d | Condition %d | Date: %s",
                        medicalRecords[i].injury,
                        medicalRecords[i].illness,
                        medicalRecords[i].pain,
                        medicalRecords[i].dehydration,
                        medicalRecords[i].condition,
                        medicalRecords[i].date
                    );

                    DrawTextEx(
                        regularFont,
                        line2,
                        (Vector2){row.x + 15, row.y + 36},
                        12,
                        1,
                        GUI_MUTED
                    );
                }
            }
            else
            {
                /* =========================================
                   ADD RECORD FORM
                   ========================================= */

                DrawTextEx(
                    regularFont,
                    "Pet ID",
                    (Vector2){270, 210},
                    13,
                    1,
                    GUI_MUTED
                );

                Rectangle petIDBox = {270, 232, 200, 38};

                DrawTextInputBox(
                    regularFont,
                    petIDBox,
                    medicalPetIDInput,
                    sizeof(medicalPetIDInput),
                    "Enter pet ID",
                    1,
                    &medicalInputField,
                    mouse
                );

                const char *severityLabels[] = {"None", "Minor", "Moderate", "Severe", "Critical"};

                /* Injury severity (0-4) */

                DrawTextEx(regularFont, "Injury Severity", (Vector2){270, 285}, 13, 1, GUI_MUTED);

                for (int lvl = 0; lvl <= 4; lvl++)
                {
                    Rectangle lvlBox = {270.0f + lvl * 90, 307, 82, 34};

                    if (DrawButton(
                            regularFont,
                            lvlBox,
                            severityLabels[lvl],
                            medicalInjuryLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                            GUI_TEAL_LIGHT))
                    {
                        medicalInjuryLevel = lvl;
                    }
                }

                /* Illness severity (0-4) */

                DrawTextEx(regularFont, "Illness Severity", (Vector2){270, 355}, 13, 1, GUI_MUTED);

                for (int lvl = 0; lvl <= 4; lvl++)
                {
                    Rectangle lvlBox = {270.0f + lvl * 90, 377, 82, 34};

                    if (DrawButton(
                            regularFont,
                            lvlBox,
                            severityLabels[lvl],
                            medicalIllnessLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                            GUI_TEAL_LIGHT))
                    {
                        medicalIllnessLevel = lvl;
                    }
                }

                /* Pain level (0-3) */

                DrawTextEx(regularFont, "Pain Level", (Vector2){270, 425}, 13, 1, GUI_MUTED);

                for (int lvl = 0; lvl <= 3; lvl++)
                {
                    Rectangle lvlBox = {270.0f + lvl * 90, 447, 82, 34};

                    if (DrawButton(
                            regularFont,
                            lvlBox,
                            severityLabels[lvl],
                            medicalPainLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                            GUI_TEAL_LIGHT))
                    {
                        medicalPainLevel = lvl;
                    }
                }

                /* Dehydration level (0-2) */

                DrawTextEx(regularFont, "Dehydration Level", (Vector2){270, 495}, 13, 1, GUI_MUTED);

                for (int lvl = 0; lvl <= 2; lvl++)
                {
                    Rectangle lvlBox = {270.0f + lvl * 90, 517, 82, 34};

                    if (DrawButton(
                            regularFont,
                            lvlBox,
                            severityLabels[lvl],
                            medicalDehydrationLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                            GUI_TEAL_LIGHT))
                    {
                        medicalDehydrationLevel = lvl;
                    }
                }

                /* Existing condition (0-2) */

                DrawTextEx(regularFont, "Existing Condition", (Vector2){700, 425}, 13, 1, GUI_MUTED);

                for (int lvl = 0; lvl <= 2; lvl++)
                {
                    Rectangle lvlBox = {700.0f + lvl * 90, 447, 82, 34};

                    if (DrawButton(
                            regularFont,
                            lvlBox,
                            severityLabels[lvl],
                            medicalConditionLevel == lvl ? GUI_TEAL : GUI_TEAL_PALE,
                            GUI_TEAL_LIGHT))
                    {
                        medicalConditionLevel = lvl;
                    }
                }

                /* Treatment */

                DrawTextEx(regularFont, "Treatment", (Vector2){700, 495}, 13, 1, GUI_MUTED);

                Rectangle medicationBox = {700, 517, 130, 34};
                Rectangle surgeryBox = {840, 517, 130, 34};

                if (DrawButton(
                        regularFont,
                        medicationBox,
                        "Medication",
                        medicalTreatmentChoice == 1 ? GUI_TEAL : GUI_TEAL_PALE,
                        GUI_TEAL_LIGHT))
                {
                    medicalTreatmentChoice = 1;
                }

                if (DrawButton(
                        regularFont,
                        surgeryBox,
                        "Surgery",
                        medicalTreatmentChoice == 2 ? GUI_TEAL : GUI_TEAL_PALE,
                        GUI_TEAL_LIGHT))
                {
                    medicalTreatmentChoice = 2;
                }

                /* Date */

                DrawTextEx(regularFont, "Date (DD/MM/YYYY)", (Vector2){270, 565}, 13, 1, GUI_MUTED);

                Rectangle dateBox = {270, 587, 200, 38};

                DrawTextInputBox(
                    regularFont,
                    dateBox,
                    medicalDateInput,
                    sizeof(medicalDateInput),
                    "e.g. 09/09/2026",
                    2,
                    &medicalInputField,
                    mouse
                );

                Rectangle saveMedicalButton = {850, 640, 180, 45};

                if (DrawButton(
                        semiBoldFont,
                        saveMedicalButton,
                        "Save Record",
                        GUI_TEAL,
                        GUI_TEAL_LIGHT))
                {
                    if (medicalPetIDInput[0] == '\0' || medicalDateInput[0] == '\0')
                    {
                        strcpy(medicalMessage, "Pet ID and date are required.");
                    }
                    else
                    {
                        saveMedicalRecordGUI(
                            atoi(medicalPetIDInput),
                            medicalInjuryLevel,
                            medicalIllnessLevel,
                            medicalPainLevel,
                            medicalDehydrationLevel,
                            medicalConditionLevel,
                            medicalTreatmentChoice == 1 ? "Medication" : "Surgery",
                            medicalDateInput
                        );

                        medicalAction = 0;
                        loadMedicalRecordsGUI();
                        medicalMessage[0] = '\0';
                    }
                }

                if (medicalMessage[0] != '\0')
                {
                    DrawTextEx(
                        regularFont,
                        medicalMessage,
                        (Vector2){270, 650},
                        13,
                        1,
                        GUI_ERROR
                    );
                }
            }

            EndDrawing();
        }

        /* =================================================
           URGENCY SCREEN
           ================================================= */

        else if (screen == SCREEN_URGENCY)
        {
            loadMedicalRecordsGUI();

            BeginDrawing();

            ClearBackground(GUI_BACKGROUND);

            /* TOP BAR */

            DrawRectangle(0, 0, SCREEN_WIDTH, 78, GUI_NAVY);

            DrawPaw((Vector2){45, 40}, 0.30f, GUI_TEAL_LIGHT);

            DrawTextEx(
                semiBoldFont,
                "MauPaws",
                (Vector2){75, 24},
                24,
                1,
                WHITE
            );

            DrawTextEx(
                regularFont,
                "Urgency",
                (Vector2){960, 28},
                15,
                1,
                WHITE
            );

            /* SIDEBAR */

            DrawRectangle(0, 78, 230, SCREEN_HEIGHT - 78, WHITE);

            DrawLine(230, 78, 230, SCREEN_HEIGHT, GUI_BORDER);

            DrawTextEx(
                semiBoldFont,
                "URGENCY TRIAGE",
                (Vector2){30, 110},
                13,
                1,
                GUI_MUTED
            );

            Rectangle urgencyBackButton = {18, 145, 194, 42};

            if (DrawButton(
                    semiBoldFont,
                    urgencyBackButton,
                    "Dashboard",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                screen = SCREEN_DASHBOARD;
            }

            /* TITLE */

            DrawTextEx(
                semiBoldFont,
                "Medical Urgency Assessment",
                (Vector2){270, 115},
                24,
                1,
                GUI_NAVY
            );

            DrawTextEx(
                regularFont,
                "Pets are ranked by how urgently they need care",
                (Vector2){270, 150},
                14,
                1,
                GUI_MUTED
            );

            /* Compute urgency for every medical record with a matching pet */

            int urgIndex[MAX_MEDICAL_GUI];
            int urgScore[MAX_MEDICAL_GUI];
            int urgCount = 0;

            for (int i = 0; i < medicalRecordCount; i++)
            {
                int age = getPetAge(medicalRecords[i].petID);

                if (age == -1)
                    continue;

                int score = calculateUrgency(
                    medicalRecords[i].injury,
                    medicalRecords[i].illness,
                    medicalRecords[i].pain,
                    age,
                    medicalRecords[i].dehydration,
                    medicalRecords[i].condition
                );

                if (score > 100)
                    score = 100;

                urgIndex[urgCount] = i;
                urgScore[urgCount] = score;
                urgCount++;
            }

            /* Bubble sort descending by urgency score */

            for (int i = 0; i < urgCount - 1; i++)
            {
                for (int j = 0; j < urgCount - i - 1; j++)
                {
                    if (urgScore[j] < urgScore[j + 1])
                    {
                        int ts = urgScore[j];
                        urgScore[j] = urgScore[j + 1];
                        urgScore[j + 1] = ts;

                        int ti = urgIndex[j];
                        urgIndex[j] = urgIndex[j + 1];
                        urgIndex[j + 1] = ti;
                    }
                }
            }

            if (urgCount == 0)
            {
                DrawTextEx(
                    regularFont,
                    "No medical records found for registered pets.",
                    (Vector2){270, 210},
                    14,
                    1,
                    GUI_MUTED
                );
            }

            int urgShown = 0;

            for (int i = 0; i < urgCount && urgShown < 7; i++)
            {
                int rec = urgIndex[i];
                int score = urgScore[i];

                const char *priority;
                Color priorityColor;

                if (score >= 76)
                {
                    priority = "CRITICAL";
                    priorityColor = GUI_ERROR;
                }
                else if (score >= 51)
                {
                    priority = "HIGH";
                    priorityColor = (Color){230, 150, 40, 255};
                }
                else if (score >= 26)
                {
                    priority = "MODERATE";
                    priorityColor = (Color){210, 180, 30, 255};
                }
                else
                {
                    priority = "LOW";
                    priorityColor = GUI_TEAL;
                }

                Rectangle urgRow =
                {
                    270,
                    205.0f + urgShown * 70,
                    890,
                    60
                };

                urgShown++;

                DrawRectangleRounded(urgRow, 0.08f, 10, WHITE);

                DrawRectangle(
                    (int)urgRow.x,
                    (int)urgRow.y,
                    6,
                    (int)urgRow.height,
                    priorityColor
                );

                char urgLine1[100];

                snprintf(
                    urgLine1,
                    sizeof(urgLine1),
                    "Pet ID %d  -  Urgency %d/100",
                    medicalRecords[rec].petID,
                    score
                );

                DrawTextEx(
                    semiBoldFont,
                    urgLine1,
                    (Vector2){urgRow.x + 20, urgRow.y + 10},
                    15,
                    1,
                    GUI_TEXT
                );

                DrawTextEx(
                    regularFont,
                    priority,
                    (Vector2){urgRow.x + 20, urgRow.y + 34},
                    13,
                    1,
                    priorityColor
                );

                /* =========================================
                   MINI URGENCY SCALE FOR THIS PET
                   ========================================= */

                Rectangle miniBar =
                {
                    urgRow.x + 460,
                    urgRow.y + 22,
                    380,
                    16
                };

                float miniSegWidth = miniBar.width / 4.0f;

                DrawRectangleRounded(
                    (Rectangle){
                        miniBar.x,
                        miniBar.y,
                        miniSegWidth,
                        miniBar.height
                    },
                    0.4f,
                    8,
                    (Color){76, 175, 80, 255}
                );

                DrawRectangle(
                    (int)(miniBar.x + miniSegWidth),
                    (int)miniBar.y,
                    (int)miniSegWidth,
                    (int)miniBar.height,
                    (Color){255, 193, 7, 255}
                );

                DrawRectangle(
                    (int)(miniBar.x + 2 * miniSegWidth),
                    (int)miniBar.y,
                    (int)miniSegWidth,
                    (int)miniBar.height,
                    (Color){255, 152, 0, 255}
                );

                DrawRectangleRounded(
                    (Rectangle){
                        miniBar.x + 3 * miniSegWidth,
                        miniBar.y,
                        miniSegWidth,
                        miniBar.height
                    },
                    0.4f,
                    8,
                    (Color){220, 53, 69, 255}
                );

                float miniIndicatorX =
                    miniBar.x +
                    ((float)score / 100.0f) *
                    miniBar.width;

                DrawCircle(
                    (int)miniIndicatorX,
                    (int)(miniBar.y + miniBar.height / 2),
                    8,
                    GUI_NAVY
                );

                char miniScoreText[16];

                snprintf(miniScoreText, sizeof(miniScoreText), "%d", score);

                Vector2 miniScoreSize =
                    MeasureTextEx(regularFont, miniScoreText, 11, 1);

                DrawTextEx(
                    regularFont,
                    miniScoreText,
                    (Vector2){
                        miniIndicatorX - miniScoreSize.x / 2,
                        miniBar.y - 16
                    },
                    11,
                    1,
                    GUI_MUTED
                );
            }

            EndDrawing();
        }

        /* =================================================
           DONATIONS SCREEN
           ================================================= */

        else if (screen == SCREEN_DONATIONS)
        {
            BeginDrawing();

            ClearBackground(GUI_BACKGROUND);

            /* TOP BAR */

            DrawRectangle(0, 0, SCREEN_WIDTH, 78, GUI_NAVY);

            DrawPaw((Vector2){45, 40}, 0.30f, GUI_TEAL_LIGHT);

            DrawTextEx(
                semiBoldFont,
                "MauPaws",
                (Vector2){75, 24},
                24,
                1,
                WHITE
            );

            DrawTextEx(
                regularFont,
                "Donations",
                (Vector2){950, 28},
                15,
                1,
                WHITE
            );

            /* SIDEBAR */

            DrawRectangle(0, 78, 230, SCREEN_HEIGHT - 78, WHITE);

            DrawLine(230, 78, 230, SCREEN_HEIGHT, GUI_BORDER);

            DrawTextEx(
                semiBoldFont,
                "DONATIONS",
                (Vector2){30, 110},
                13,
                1,
                GUI_MUTED
            );

            Rectangle donationBackButton = {18, 145, 194, 42};

            if (DrawButton(
                    semiBoldFont,
                    donationBackButton,
                    "Dashboard",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                screen = SCREEN_DASHBOARD;
                donationAction = 0;
            }

            /* TITLE */

            DrawTextEx(
                semiBoldFont,
                donationAction == 0 ? "Donations" : "Record Donation",
                (Vector2){270, 115},
                24,
                1,
                GUI_NAVY
            );

            char donationCountText[64];

            snprintf(
                donationCountText,
                sizeof(donationCountText),
                "%d donations recorded",
                guiDonationCount
            );

            DrawTextEx(
                regularFont,
                donationAction == 0 ? donationCountText : "Enter the donor and gift details",
                (Vector2){270, 150},
                14,
                1,
                donationAction == 0 ? GUI_TEAL : GUI_MUTED
            );

            Rectangle addDonationButton = {950, 110, 190, 45};

            if (DrawButton(
                    semiBoldFont,
                    addDonationButton,
                    donationAction == 0 ? "+ Add Donation" : "Cancel",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                if (donationAction == 0)
                {
                    donationAction = 1;
                    donationNameInput[0] = '\0';
                    donationAddressInput[0] = '\0';
                    donationContactInput[0] = '\0';
                    donationEmailInput[0] = '\0';
                    strcpy(donationTypeInput, "Money");
                    donationAmountInput[0] = '\0';
                    donationItemInput[0] = '\0';
                    donationQuantityInput[0] = '\0';
                    donationInputField = 0;
                    donationMessage[0] = '\0';
                }
                else
                {
                    donationAction = 0;
                }
            }

            if (donationAction == 0)
            {
                /* =========================================
                   VIEW DONATIONS
                   ========================================= */

                Rectangle donationSearchBox = {650, 175, 240, 34};

                DrawTextInputBox(
                    regularFont,
                    donationSearchBox,
                    donationSearchText,
                    sizeof(donationSearchText),
                    "Search by donor or ID...",
                    1,
                    &donationInputField,
                    mouse
                );

                if (guiDonationCount == 0)
                {
                    DrawTextEx(
                        regularFont,
                        "No donations have been recorded yet.",
                        (Vector2){270, 230},
                        14,
                        1,
                        GUI_MUTED
                    );
                }

                int donationShown = 0;

                for (int i = 0; i < guiDonationCount && donationShown < 6; i++)
                {
                    if (!donationMatchesSearch(i))
                        continue;

                    Rectangle donationRow =
                    {
                        270,
                        220.0f + donationShown * 78,
                        890,
                        68
                    };

                    donationShown++;

                    DrawRectangleRounded(donationRow, 0.08f, 10, WHITE);

                    char donLine1[150];

                    snprintf(
                        donLine1,
                        sizeof(donLine1),
                        "#%d  %s  -  %s",
                        guiDonations[i].donationID,
                        guiDonations[i].donorname,
                        guiDonations[i].donationtype
                    );

                    DrawTextEx(
                        semiBoldFont,
                        donLine1,
                        (Vector2){donationRow.x + 15, donationRow.y + 10},
                        14,
                        1,
                        GUI_TEXT
                    );

                    char donLine2[150];

                    if (strcmp(guiDonations[i].donationtype, "Money") == 0)
                    {
                        snprintf(
                            donLine2,
                            sizeof(donLine2),
                            "Amount: Rs %.2f  |  Contact: %s",
                            guiDonations[i].amount,
                            guiDonations[i].contactnumber
                        );
                    }
                    else
                    {
                        snprintf(
                            donLine2,
                            sizeof(donLine2),
                            "Item: %s  |  Quantity: %d  |  Contact: %s",
                            guiDonations[i].itemname,
                            guiDonations[i].quantity,
                            guiDonations[i].contactnumber
                        );
                    }

                    DrawTextEx(
                        regularFont,
                        donLine2,
                        (Vector2){donationRow.x + 15, donationRow.y + 36},
                        12,
                        1,
                        GUI_MUTED
                    );
                }

                if (guiDonationCount > 0 && donationShown == 0)
                {
                    DrawTextEx(
                        regularFont,
                        "No donations match your search.",
                        (Vector2){270, 230},
                        14,
                        1,
                        GUI_MUTED
                    );
                }
            }
            else
            {
                /* =========================================
                   ADD DONATION FORM
                   ========================================= */

                DrawTextEx(regularFont, "Donor Name", (Vector2){270, 205}, 13, 1, GUI_MUTED);

                Rectangle nameBox = {270, 227, 400, 38};

                DrawTextInputBox(
                    regularFont, nameBox, donationNameInput,
                    sizeof(donationNameInput), "Full name", 1,
                    &donationInputField, mouse
                );

                DrawTextEx(regularFont, "Address", (Vector2){270, 275}, 13, 1, GUI_MUTED);

                Rectangle addressBox = {270, 297, 400, 38};

                DrawTextInputBox(
                    regularFont, addressBox, donationAddressInput,
                    sizeof(donationAddressInput), "Address", 2,
                    &donationInputField, mouse
                );

                DrawTextEx(regularFont, "Contact Number", (Vector2){700, 205}, 13, 1, GUI_MUTED);

                Rectangle contactBox = {700, 227, 260, 38};

                DrawTextInputBox(
                    regularFont, contactBox, donationContactInput,
                    sizeof(donationContactInput), "Phone number", 3,
                    &donationInputField, mouse
                );

                DrawTextEx(regularFont, "Email", (Vector2){700, 275}, 13, 1, GUI_MUTED);

                Rectangle emailBox = {700, 297, 260, 38};

                DrawTextInputBox(
                    regularFont, emailBox, donationEmailInput,
                    sizeof(donationEmailInput), "Email address", 4,
                    &donationInputField, mouse
                );

                DrawTextEx(regularFont, "Donation Type", (Vector2){270, 350}, 13, 1, GUI_MUTED);

                Rectangle moneyTypeBox = {270, 372, 130, 36};
                Rectangle foodTypeBox = {410, 372, 130, 36};
                Rectangle suppliesTypeBox = {550, 372, 130, 36};

                if (DrawButton(
                        regularFont, moneyTypeBox, "Money",
                        strcmp(donationTypeInput, "Money") == 0 ? GUI_TEAL : GUI_TEAL_PALE,
                        GUI_TEAL_LIGHT))
                {
                    strcpy(donationTypeInput, "Money");
                }

                if (DrawButton(
                        regularFont, foodTypeBox, "Food",
                        strcmp(donationTypeInput, "Food") == 0 ? GUI_TEAL : GUI_TEAL_PALE,
                        GUI_TEAL_LIGHT))
                {
                    strcpy(donationTypeInput, "Food");
                }

                if (DrawButton(
                        regularFont, suppliesTypeBox, "Supplies",
                        strcmp(donationTypeInput, "Supplies") == 0 ? GUI_TEAL : GUI_TEAL_PALE,
                        GUI_TEAL_LIGHT))
                {
                    strcpy(donationTypeInput, "Supplies");
                }

                if (strcmp(donationTypeInput, "Money") == 0)
                {
                    DrawTextEx(regularFont, "Amount (Rs)", (Vector2){270, 425}, 13, 1, GUI_MUTED);

                    Rectangle amountBox = {270, 447, 260, 38};

                    DrawTextInputBox(
                        regularFont, amountBox, donationAmountInput,
                        sizeof(donationAmountInput), "e.g. 500", 5,
                        &donationInputField, mouse
                    );
                }
                else
                {
                    DrawTextEx(regularFont, "Item Name", (Vector2){270, 425}, 13, 1, GUI_MUTED);

                    Rectangle itemBox = {270, 447, 260, 38};

                    DrawTextInputBox(
                        regularFont, itemBox, donationItemInput,
                        sizeof(donationItemInput), "Item donated", 6,
                        &donationInputField, mouse
                    );

                    DrawTextEx(regularFont, "Quantity", (Vector2){560, 425}, 13, 1, GUI_MUTED);

                    Rectangle qtyBox = {560, 447, 150, 38};

                    DrawTextInputBox(
                        regularFont, qtyBox, donationQuantityInput,
                        sizeof(donationQuantityInput), "e.g. 10", 7,
                        &donationInputField, mouse
                    );
                }

                Rectangle saveDonationButton = {850, 640, 190, 45};

                if (DrawButton(
                        semiBoldFont, saveDonationButton,
                        "Save Donation", GUI_TEAL, GUI_TEAL_LIGHT))
                {
                    if (donationNameInput[0] == '\0' ||
                        (strcmp(donationTypeInput, "Money") == 0 &&
                         (donationAmountInput[0] == '\0' || atof(donationAmountInput) <= 0)) ||
                        (strcmp(donationTypeInput, "Money") != 0 &&
                         (donationItemInput[0] == '\0' ||
                          donationQuantityInput[0] == '\0' ||
                          atoi(donationQuantityInput) <= 0)))
                    {
                        strcpy(donationMessage, "Please fill in all required fields.");
                    }
                    else if (guiDonationCount < MAX_DONATIONS)
                    {
                        struct Donation newDonation;

                        newDonation.donationID =
                            guiDonationCount == 0
                                ? 1
                                : guiDonations[guiDonationCount - 1].donationID + 1;

                        strcpy(newDonation.donorname, donationNameInput);
                        strcpy(newDonation.address, donationAddressInput);
                        strcpy(newDonation.contactnumber, donationContactInput);
                        strcpy(newDonation.email, donationEmailInput);
                        strcpy(newDonation.donationtype, donationTypeInput);

                        if (strcmp(donationTypeInput, "Money") == 0)
                        {
                            newDonation.amount = (float)atof(donationAmountInput);
                            newDonation.itemname[0] = '\0';
                            newDonation.quantity = 0;
                        }
                        else
                        {
                            newDonation.amount = 0.0f;
                            strcpy(newDonation.itemname, donationItemInput);
                            newDonation.quantity = atoi(donationQuantityInput);
                        }

                        saveDonationToFile(newDonation);

                        guiDonations[guiDonationCount] = newDonation;
                        guiDonationCount++;

                        donationAction = 0;
                        donationMessage[0] = '\0';
                    }
                }

                if (donationMessage[0] != '\0')
                {
                    DrawTextEx(
                        regularFont,
                        donationMessage,
                        (Vector2){270, 650},
                        13,
                        1,
                        GUI_ERROR
                    );
                }
            }

            EndDrawing();
        }

        /* =================================================
           REPORTS SCREEN
           ================================================= */

        else if (screen == SCREEN_REPORTS)
        {
            BeginDrawing();

            ClearBackground(GUI_BACKGROUND);

            /* TOP BAR */

            DrawRectangle(0, 0, SCREEN_WIDTH, 78, GUI_NAVY);

            DrawPaw((Vector2){45, 40}, 0.30f, GUI_TEAL_LIGHT);

            DrawTextEx(
                semiBoldFont,
                "MauPaws",
                (Vector2){75, 24},
                24,
                1,
                WHITE
            );

            DrawTextEx(
                regularFont,
                "Reports",
                (Vector2){960, 28},
                15,
                1,
                WHITE
            );

            /* SIDEBAR */

            DrawRectangle(0, 78, 230, SCREEN_HEIGHT - 78, WHITE);

            DrawLine(230, 78, 230, SCREEN_HEIGHT, GUI_BORDER);

            DrawTextEx(
                semiBoldFont,
                "REPORTS & STATS",
                (Vector2){30, 110},
                13,
                1,
                GUI_MUTED
            );

            Rectangle reportsBackButton = {18, 145, 194, 42};

            if (DrawButton(
                    semiBoldFont,
                    reportsBackButton,
                    "Dashboard",
                    GUI_TEAL,
                    GUI_TEAL_LIGHT))
            {
                screen = SCREEN_DASHBOARD;
            }

            /* TITLE */

            DrawTextEx(
                semiBoldFont,
                "Overall Summary Report",
                (Vector2){270, 115},
                24,
                1,
                GUI_NAVY
            );

            DrawTextEx(
                regularFont,
                "Pets, donations and adoption outcomes at a glance",
                (Vector2){270, 150},
                14,
                1,
                GUI_MUTED
            );

            /* =============================================
               GATHER ALL STATISTICS FIRST
               ============================================= */

            /* --- Pets (from in-memory pets[]) --- */

            int dogCount = 0, catCount = 0;
            int maleCount = 0, femaleCount = 0;
            int availableCount = 0, adoptedCount = 0;

            for (int i = 0; i < petCount; i++)
            {
                if (sameString(pets[i].Species, "Dog")) dogCount++;
                else if (sameString(pets[i].Species, "Cat")) catCount++;

                if (sameString(pets[i].Gender, "Male")) maleCount++;
                else if (sameString(pets[i].Gender, "Female")) femaleCount++;

                if (sameString(pets[i].Availability, "Available")) availableCount++;
                else if (sameString(pets[i].Availability, "Adopted")) adoptedCount++;
            }

            /* --- Donations (from in-memory guiDonations[]) --- */

            int moneyDonationCount = 0, foodDonationCount = 0, suppliesDonationCount = 0;
            float totalMoneyAmount = 0.0f;
            int totalFoodQty = 0;
            int totalSuppliesQty = 0;

            for (int i = 0; i < guiDonationCount; i++)
            {
                if (strcmp(guiDonations[i].donationtype, "Money") == 0)
                {
                    moneyDonationCount++;
                    totalMoneyAmount += guiDonations[i].amount;
                }
                else if (strcmp(guiDonations[i].donationtype, "Food") == 0)
                {
                    foodDonationCount++;
                    totalFoodQty += guiDonations[i].quantity;
                }
                else if (strcmp(guiDonations[i].donationtype, "Supplies") == 0)
                {
                    suppliesDonationCount++;
                    totalSuppliesQty += guiDonations[i].quantity;
                }
            }

            /* --- Adoptions (parsed from adoptions.txt) --- */

            int totalApps = 0, approved = 0, waitlisted = 0, rejected = 0;
            int dogApps = 0, catApps = 0;
            float totalFees = 0.0f;

            FILE *adoptionFile = fopen("adoptions.txt", "r");

            if (adoptionFile != NULL)
            {
                char line[500];

                while (fgets(line, sizeof(line), adoptionFile) != NULL)
                {
                    char *fields[7];
                    int fieldIndex = 1;

                    fields[0] = line;

                    for (int i = 0; line[i] != '\0' && fieldIndex < 7; i++)
                    {
                        if (line[i] == '|')
                        {
                            line[i] = '\0';
                            fields[fieldIndex] = &line[i + 1];
                            fieldIndex++;
                        }
                    }

                    if (fieldIndex == 7)
                    {
                        totalApps++;

                        fields[6][strcspn(fields[6], "\n")] = '\0';

                        totalFees += (float)atof(fields[5]);

                        if (sameString(fields[6], "Approved")) approved++;
                        else if (sameString(fields[6], "Waitlist")) waitlisted++;
                        else if (sameString(fields[6], "Rejected")) rejected++;

                        if (sameString(fields[3], "Dog")) dogApps++;
                        else if (sameString(fields[3], "Cat")) catApps++;
                    }
                }

                fclose(adoptionFile);
            }

            float successRate =
                (approved + rejected) > 0
                ? ((float)approved / (approved + rejected)) * 100.0f
                : 0.0f;

            bool hasDecidedApplications = (approved + rejected) > 0;

            /* =============================================
               KPI CARDS
               ============================================= */

            char kpiPetsValue[16], kpiAvailableValue[16];
            char kpiDonationsValue[16], kpiSuccessValue[16];

            snprintf(kpiPetsValue, sizeof(kpiPetsValue), "%d", petCount);
            snprintf(kpiAvailableValue, sizeof(kpiAvailableValue), "%d", availableCount);
            snprintf(kpiDonationsValue, sizeof(kpiDonationsValue), "%d", guiDonationCount);

            if (hasDecidedApplications)
                snprintf(kpiSuccessValue, sizeof(kpiSuccessValue), "%.0f%%", successRate);
            else
                strcpy(kpiSuccessValue, "N/A");

            Color kpiAccentTeal = GUI_TEAL;
            Color kpiAccentBlue = (Color){80, 130, 190, 255};
            Color kpiAccentAmber = (Color){230, 150, 40, 255};
            Color kpiAccentGreen = (Color){46, 175, 120, 255};

            DrawStatCard(
                regularFont, semiBoldFont,
                (Rectangle){270, 185, 205, 90},
                "Total Pets", kpiPetsValue, kpiAccentTeal
            );

            DrawStatCard(
                regularFont, semiBoldFont,
                (Rectangle){490, 185, 205, 90},
                "Available Now", kpiAvailableValue, kpiAccentGreen
            );

            DrawStatCard(
                regularFont, semiBoldFont,
                (Rectangle){710, 185, 205, 90},
                "Total Donations", kpiDonationsValue, kpiAccentBlue
            );

            DrawStatCard(
                regularFont, semiBoldFont,
                (Rectangle){930, 185, 205, 90},
                "Approval Rate", kpiSuccessValue, kpiAccentAmber
            );

            /* =============================================
               CHART 1: PETS BY SPECIES & STATUS
               ============================================= */

            const char *petChartLabels[] = {"Dogs", "Cats", "Available", "Adopted"};
            int petChartValues[] = {dogCount, catCount, availableCount, adoptedCount};

            Color petChartColors[] =
            {
                GUI_TEAL,
                (Color){80, 130, 190, 255},
                (Color){46, 175, 120, 255},
                (Color){149, 165, 166, 255}
            };

            DrawSimpleBarChart(
                semiBoldFont,
                regularFont,
                (Rectangle){270, 295, 430, 260},
                "Pets by Species & Status",
                petChartLabels,
                petChartValues,
                petChartColors,
                4
            );

            /* =============================================
               CHART 2: ADOPTION OUTCOMES
               ============================================= */

            const char *adoptionChartLabels[] = {"Approved", "In Review", "Not Matched"};
            int adoptionChartValues[] = {approved, waitlisted, rejected};

            Color adoptionChartColors[] =
            {
                (Color){46, 175, 120, 255},
                (Color){230, 150, 40, 255},
                (Color){149, 165, 166, 255}
            };

            DrawSimpleBarChart(
                semiBoldFont,
                regularFont,
                (Rectangle){730, 295, 430, 190},
                "Adoption Outcomes",
                adoptionChartLabels,
                adoptionChartValues,
                adoptionChartColors,
                3
            );

            char adoptionSummaryLine1[100];
            char adoptionSummaryLine2[100];

            snprintf(
                adoptionSummaryLine1, sizeof(adoptionSummaryLine1),
                "%d pet(s) have found loving homes so far!",
                adoptedCount
            );

            const char *mostCommonSpeciesLabel =
                dogApps == catApps
                    ? "Tied"
                    : (dogApps > catApps ? "Dogs" : "Cats");

            if (hasDecidedApplications)
            {
                snprintf(
                    adoptionSummaryLine2, sizeof(adoptionSummaryLine2),
                    "Approval Rate: %.0f%%   |   Fees Raised: Rs %.2f",
                    successRate, totalFees
                );
            }
            else
            {
                snprintf(
                    adoptionSummaryLine2, sizeof(adoptionSummaryLine2),
                    "Fees Raised: Rs %.2f   |   Most Common: %s",
                    totalFees, mostCommonSpeciesLabel
                );
            }

            DrawTextEx(
                semiBoldFont,
                adoptionSummaryLine1,
                (Vector2){750, 495},
                13,
                1,
                (Color){46, 175, 120, 255}
            );

            DrawTextEx(
                regularFont,
                adoptionSummaryLine2,
                (Vector2){750, 518},
                13,
                1,
                GUI_MUTED
            );

            /* =============================================
               CHART 3 + SUMMARY: DONATIONS
               ============================================= */

            Rectangle donationPanel = {270, 575, 890, 150};

            DrawRectangleRounded(donationPanel, 0.06f, 10, WHITE);

            const char *donationChartLabels[] = {"Money", "Food", "Supplies"};

            int donationChartValues[] =
            {
                moneyDonationCount,
                foodDonationCount,
                suppliesDonationCount
            };

            Color donationChartColors[] =
            {
                GUI_TEAL,
                (Color){80, 130, 190, 255},
                (Color){230, 150, 40, 255}
            };

            DrawSimpleBarChart(
                semiBoldFont,
                regularFont,
                (Rectangle){270, 575, 470, 150},
                "Donations by Type",
                donationChartLabels,
                donationChartValues,
                donationChartColors,
                3
            );

            DrawLine(760, 595, 760, 705, GUI_BORDER);

            DrawTextEx(
                semiBoldFont,
                "Donation Summary",
                (Vector2){780, 595},
                15,
                1,
                GUI_NAVY
            );

            char donSummaryLine1[80], donSummaryLine2[80], donSummaryLine3[80];

            snprintf(donSummaryLine1, sizeof(donSummaryLine1), "Total Money Raised: Rs %.2f", totalMoneyAmount);
            snprintf(donSummaryLine2, sizeof(donSummaryLine2), "Food Donated: %d units", totalFoodQty);
            snprintf(donSummaryLine3, sizeof(donSummaryLine3), "Supplies Donated: %d units", totalSuppliesQty);

            DrawTextEx(regularFont, donSummaryLine1, (Vector2){780, 630}, 13, 1, GUI_TEXT);
            DrawTextEx(regularFont, donSummaryLine2, (Vector2){780, 655}, 13, 1, GUI_TEXT);
            DrawTextEx(regularFont, donSummaryLine3, (Vector2){780, 680}, 13, 1, GUI_TEXT);

            EndDrawing();
        }

        previousScreen = screen;

}
    /* =====================================================
       CLEANUP
       ===================================================== */

UnloadTexture(
        petImage1
    );

    UnloadTexture(
        petImage2
    );

    UnloadTexture(
        petImage3
    );
    UnloadTexture(
    petImage4
);

UnloadTexture(
    petImage5
);

UnloadTexture(
    petImage6
);

UnloadTexture(
    petImage7
);


    UnloadFont(
        regularFont
    );

    UnloadFont(
        semiBoldFont
    );

    CloseWindow();

    return 0;
}
