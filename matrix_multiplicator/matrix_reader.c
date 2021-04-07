#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "helper.h"

int* getSimpleMatrix(string matrixData, uul rows, uul cols);

uul getCols(string matrix) {
    uul count = 0;
    while(*matrix) {
        *matrix++;
        if (*matrix == '\t') ++count;
        if (*matrix == ' ') ++count;
        if (*matrix == '\n') break;
    }

    return count +1;
}

uul getRows(string matrix) {
    uul count = 0;
    while(*matrix) if (*matrix++ == '\n') ++count;

    return count;
}

int checkMatrix(string first, string second, int** firstMatrix, int** secondMatrix) {
    // FIXME - this is needed for optimalization of uul processing
    uul firstX = getCols(first), firstY = getRows(first), secondX = getCols(second), secondY = getRows(second);

    printf("%lldx%lld and %lldx%lld\n", firstX, firstY, secondX, secondY);
//    printf("Fist matrix:\n%s\n", first);
//    printf("Second matrix:\n%s\n", second);

    if (firstX * secondX != firstY * secondY ) {
        printf("Matrix sizes must equal (cols of first matrix must equal rows of second matrix)");
        printf("%lld * %lld == %lld * %lld", firstX, secondX, firstY, secondY);
        return -1;
    }

    *firstMatrix = getSimpleMatrix(first, firstY, firstX);
    *secondMatrix = getSimpleMatrix(second, secondY, secondX);

    return 1;
}

int* readMatrix(string matrix, uul rows, uul cols) {
    int addrIdx = 0;
    int *matrixArr = (int *)malloc(rows * cols * sizeof(int));

    string temp, p = strtok_r(matrix, EOL, &temp);
    do {
        int* array = stringToSubsetOfIntegers(p, "\t", cols*2+2*sizeof(string));

        for (int idx = 0 ; idx < cols; idx++) {
            if (addrIdx > rows * cols) {
                printf("An error occurred wile working with matrix");
                return NULL;
            }
            matrixArr[addrIdx++] = array[idx];
        }

        free(array);

    } while ((p = strtok_r(NULL, EOL, &temp)) != NULL);

    free(p);
    free(temp);

    return matrixArr;
}

int* getSimpleMatrix(string matrixData, uul rows, uul cols) {
    return readMatrix(matrixData, rows, cols);
}


int getMatrix(string matrixData, int** firstMatrix, int** secondMatrix) {
    uul result = seek(matrixData, '*');

    string first = getSubstring(matrixData, 0, result);
    string second = getSubstring(matrixData, result+1, strlen(matrixData));

    checkMatrix(first, second, &*firstMatrix, &*secondMatrix);

    if (getCols(first) * getCols(second) != getRows(first) * getRows(second))
        return -1;

    return getCols(first);
}

string convertMatrixToString(int* matrix, uul cube) {
    string translatedMatrix = (string) malloc(sizeof(string) * cube*cube*cube);
    strcpy(translatedMatrix, "");

    for (int row = 0; row < cube;) {
        for (int col = 0; col < cube;) {
            char translatedCell[2];
            itoa(matrix[cube*row + col], translatedCell, 10);

            strcat(translatedMatrix, translatedCell);
            if (col++ < cube-1)
                strcat(translatedMatrix, "\t");
        }
        if (row++ < cube-1)
            strcat(translatedMatrix, "\n");
    }

    return translatedMatrix;
}

int* generateRandomIntMatrix(uul cube) {
    int* matrix = (int*) malloc(sizeof(int*) * cube*cube);

    for (int row = 0; row < cube*cube; row++) {
//        matrix[row]= (int) (rand() % 10);
        matrix[row]= row %10 +1;
    }

    return matrix;
}

float* generateRandomFloatMatrix(uul cube) {
    float* matrix = (float*) malloc(sizeof(float*) * cube*cube);

    for (int row = 0; row < cube*cube; row++) {
//        matrix[row]= (int) (rand() % 10);
        matrix[row]= (float) (row % cube) + 0.1f;
    }

    return matrix;
}

string createRandomMatrix(uul firstCube, uul secondCube) {
    int* firstMatrix = generateRandomIntMatrix(firstCube);
    int* secondMatrix = generateRandomIntMatrix(secondCube);
    string translatedFirstMatrix = convertMatrixToString(firstMatrix, firstCube);
    string translatedSecondMatrix = convertMatrixToString(secondMatrix, secondCube);

    string fileContent = (string) malloc(sizeof(string) * strlen(translatedFirstMatrix) + strlen(translatedSecondMatrix) +2);
    strcpy(fileContent, "");
    strcat(fileContent, translatedFirstMatrix);
    strcat(fileContent, "\n*\n");
    strcat(fileContent, translatedSecondMatrix);

    free(firstMatrix);
    free(secondMatrix);
    free(translatedFirstMatrix);
    free(translatedSecondMatrix);

    return fileContent;
}