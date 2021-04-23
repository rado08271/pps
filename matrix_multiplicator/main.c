#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "file_reader.h"
#include "matrix_reader.h"
#include "matrix_computer.h"
#include "ui.h"
#include "simple_timer.h"
#include "opencl_utils.h"

#define INCLUDE_UI 1

int main() {
    srand(time(0));
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    }

    bool isInteger = false;
    bool showResult = false;
    int *firstMatrixInt = (int *) malloc(sizeof(int)), *secondMatrixInt = (int *) malloc(sizeof(int)), *resultMatrixInt = (int *) malloc(sizeof(int));
    float *firstMatrixFloat = (float *) malloc(sizeof(float)), *secondMatrixFloat = (float *) malloc(sizeof(float)), *resultMatrixFloat = (float *) malloc(sizeof(float));
    uul cubeSize = 0;

    if (INCLUDE_UI == 1) {

        int answer = generateMainUI();

        if (answer == 1) {
            answer = generateReturnTypeUI();

            if (answer == 1) {
                isInteger = true;
            } else if (answer == 2) {
                isInteger = false;
            } else {
                printf("\nProgram failed\n");
                return -1;
            }

            answer = generateUIForMatrixGenerator();
           
            if (answer == 1) {
                cubeSize = 500;
            } else if (answer == 2) {
                cubeSize = 1000;
            } else if (answer == 3) {
                cubeSize = 2000;
            } else if (answer == 4) {
                answer = generateUIForCustomMatrixGenerator();

                if (answer <= 10000) {
                    cubeSize = answer;
                } else {
                    printf("\nValue is too high\n");
                }
            }

            if (cubeSize > 0) {
                answer = generateUiForMatrixComputing();

                if (isInteger) {
                    firstMatrixInt = generateRandomIntMatrix(cubeSize);
                    secondMatrixInt = generateRandomIntMatrix(cubeSize);
                } else {
                    firstMatrixFloat = generateRandomFloatMatrix(cubeSize);
                    secondMatrixFloat = generateRandomFloatMatrix(cubeSize);
                }

                if (answer == 1) {
                    if (isInteger) {
                        resultMatrixInt = computeMatrixIntIJK(firstMatrixInt, secondMatrixInt, cubeSize, true);
                        showResult = true;
                    } else {
                        resultMatrixFloat = computeMatrixFloatIJK(firstMatrixFloat, secondMatrixFloat, cubeSize, true);
                        showResult = true;
                    }
                } else if (answer == 2) {
                    answer = generateUIForDeviceChoosing();

                    if (isInteger) {
                        if (answer == 1) {
                            resultMatrixInt = computeMatrixIntWithKernel(firstMatrixInt, secondMatrixInt, cubeSize, true, chooseDevice());
                            showResult = true;
                        } else {
                            resultMatrixInt = computeMatrixIntWithKernel(firstMatrixInt, secondMatrixInt, cubeSize, true, chooseBestDevice());
                            showResult = true;
                        }
                    } else {
                        if (answer == 1) {
                            resultMatrixFloat = computeMatrixFloatWithKernel(firstMatrixFloat, secondMatrixFloat, cubeSize, true, chooseDevice());
                            showResult = true;
                        } else {
                            resultMatrixFloat = computeMatrixFloatWithKernel(firstMatrixFloat, secondMatrixFloat, cubeSize, true, chooseBestDevice());
                            showResult = true;
                        }
                    }

                }
            }
        } else if (answer == 2) {
            answer = generateUiForMatrixComputing();

            char *fileContent = readFile("../matrix");

            cubeSize = getMatrix(fileContent, &firstMatrixInt, &secondMatrixInt);

            if (answer == 1) {
                computeMatrixIntIJK(firstMatrixInt, secondMatrixInt, cubeSize, true);
                showResult = true;
            } else if (answer == 2) {
                answer = generateUIForDeviceChoosing();

                if (answer == 1) {
                    computeMatrixIntWithKernel(firstMatrixInt, secondMatrixInt, cubeSize, true, chooseDevice());
                    showResult = true;
                } else {
                    computeMatrixIntWithKernel(firstMatrixInt, secondMatrixInt, cubeSize, true, chooseBestDevice());
                    showResult = true;
                }
            }

            free(fileContent);
        } else if (answer == 3) {
            answer = generateUIForMatrixGenerator();
            string fileContent = "";
            if (answer == 1) {
                fileContent = createRandomMatrix(500, 500);
            } else if (answer == 2) {
                fileContent = createRandomMatrix(1000, 1000);
            } else if (answer == 3) {
                fileContent = createRandomMatrix(2000, 2000);
            } else if (answer == 4) {
                answer = generateUIForCustomMatrixGenerator();

                if (answer <= 10000) {
                    fileContent = createRandomMatrix(answer, answer);
                } else {
                    printf("Value is too high");
                }
            }

            printToFile("../matrix", fileContent);
            free(fileContent);
        }
    } else if (INCLUDE_UI == 0) {
        cubeSize = 500;

        firstMatrixInt = generateRandomIntMatrix(cubeSize);
        secondMatrixInt = generateRandomIntMatrix(cubeSize);

        resultMatrixInt = computeMatrixIntIJK(firstMatrixInt, secondMatrixInt, cubeSize, true);

        showResult = false;
        isInteger = true;


    }

    showResult = false;

    if (getTimerResult() >= 0) {
        printf("\n\n========= Your algorithm took %ld [us] to compute =========", getTimerResult());
    }


    free(firstMatrixInt);
    free(firstMatrixFloat);
    free(secondMatrixInt);
    free(secondMatrixFloat);
    free(resultMatrixInt);
    free(resultMatrixFloat);

    printf("\nGood job! Shutting down...\n");
    return 0;
}
