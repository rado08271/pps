#include <stdio.h>
#include <string.h>

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

int generateUiForBruteAttackProceedings() {
    printf("\n\n");

    printf("============== Proceed? y/n ===============\n");

    string answer = '\0';
    scanf("%c", answer);
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

string generateUIForPasswordInput() {
    printf("\n\n");
    printf("====== Please input your password =====\n");
    printf("|     -  Password max length is 10    |\n");
    printf("|     -  Supported chars [AZaz09]     |\n");
    printf("|-------------------------------------|\n");
    printf("|                                     |\n");

    char answer;
//    scanf("%s", &answer);
    return "heslo";
}
