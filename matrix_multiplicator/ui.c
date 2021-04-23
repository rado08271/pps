#include <stdio.h>

int generateReturnTypeUI() {
    printf("\n\n");

    printf("============= Choose Type =============\n");
    printf("| Please select:                      |\n");
    printf("| 1. Integer                          |\n");
    printf("| 2. Floating                         |\n");
    printf("| 0. Exit                             |\n");
    printf("=======================================\n");

    int answer = -1;
    scanf("%d", &answer);
    return answer;
}

int generateMainUI() {
    printf("\n\n");

    printf("========== Matrix Multiplier ==========\n");
    printf("| Please select:                      |\n");
    printf("| 1. Compute matrix                   |\n");
    printf("| 2. Compute matrix in './matrix'     |\n");
    printf("| 3. Generate random matrix           |\n");
    printf("| 0. Exit                             |\n");
    printf("=======================================\n");

    int answer = -1;
    scanf("%d", &answer);
    return answer;
}

int generateUiForMatrixComputing() {
    printf("\n\n");

    printf("============ Compute matrix ===========\n");
    printf("| Please select:                      |\n");
    printf("| 1. Sequential algorithm kij         |\n");
    printf("| 2. Parallel algorithm using OpenCL  |\n");
    printf("| 0. Exit                             |\n");
    printf("=======================================\n");

    int answer = -1;
    scanf("%d", &answer);
    return answer;
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

int generateUIForMatrixGenerator() {
    printf("\n\n");

    printf("=========== Generate matrix ===========\n");
    printf("| Please select:                      |\n");
    printf("| 1. 500x500                          |\n");
    printf("| 2. 1000x1000                        |\n");
    printf("| 3. 2000*2000                        |\n");
    printf("| 4. Custom                           |\n");
    printf("| 0. Exit                             |\n");
    printf("=======================================\n");

    int answer = -1;
    scanf("%d", &answer);
    return answer;
}

int generateUIForCustomMatrixGenerator() {
    printf("\n\n");

    printf("======= Pass cube dimensions N*N ======\n");

    int answer = -1;
    scanf("%d", &answer);
    return answer;
}

