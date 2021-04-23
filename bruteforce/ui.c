#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "ui.h"

int generateUiForBruteAttack() {
    printf("\n\n");

    printf("============= Brute Force =============\n");
    printf("| Please select:                      |\n");
    printf("| 1. Compute with OpenCL              |\n");
    printf("| 2. Use all together serially        |\n");
    printf("| 0. Exit                             |\n");
    printf("=======================================\n");

    int answer = -1;
    scanf("%d", &answer);
    return answer;
}

int generateUIForDictionary() {
    printf("\n\n");

    printf("============ Use Dictionary============\n");
    printf("| Please select:                      |\n");
    printf("| 1. Use simple dictionary (4m)       |\n");
    printf("| 2. Use advanced dictionary (64m)    |\n");
    printf("| 3. No dictionary                    |\n");
    printf("| 0. Exit                             |\n");
    printf("=======================================\n");

    int answer = -1;
    scanf("%d", &answer);
    return answer;
}

int generateUiForBruteAttackProceedings() {
    printf("\n\n");

    printf("============== Proceed? y/n ===============\n");

    char answer[20];
    scanf("%s", answer);
    printf("============== Proceed\n");

    if (strcmp(answer, "y") == 0)
        return 1;

    return 2;
}

int generateUIForDeviceChoosing() {
    printf("\n\n");

    printf("============ Choose device ============\n");
    printf("| Please select:                      |\n");
    printf("| 1. Let me decide                    |\n");
    printf("| 2. You suggest                      |\n");
    printf("| 0. Exit                             |\n");
    printf("=======================================\n");

    int answer = -1;
    scanf("%d", &answer);
    return answer;
}

string generateUIForPasswordInput(const int maxPasswordSize, const string possibleCharacters) {
    printf("\n\n");
    printf("====== Please input your password =====\n");
    printf("|     -  Password max length is       |\n");
    printf("\t%d\n", maxPasswordSize);
    printf("|     -  Supported chars              |\n");
    printf("\t%s\n", possibleCharacters);
    printf("|-------------------------------------|\n");
    printf("|                                     |\n");

    char answer[maxPasswordSize];
    scanf("%s", answer);

    string tmpAnswer = (string) calloc(maxPasswordSize, sizeof(char));
    strcpy(tmpAnswer, answer);
    return tmpAnswer;
}
