#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

uul seek(string stringToBeSearch, char stringOcc) {
    string e = strchr(stringToBeSearch, stringOcc);

    return e != NULL ? e - stringToBeSearch : -1;
}

string getSubstring(string stringToSplit, uul startIndex, uul finishIndex) {
    string substr = malloc(finishIndex - startIndex +1);
    strncpy(substr, stringToSplit+startIndex, finishIndex - startIndex +1);
    substr[finishIndex - startIndex] = TERM;

    return substr;
}

int* stringToSubsetOfIntegers(string stringToSplit, string delimiter, int arraySize) {
    int i = 0;
    int *array = (int *)malloc(arraySize * sizeof(int));

    string temp, p = strtok_r(stringToSplit, delimiter, &temp);

    do {
        char *ptr;
        array[i++] = strtol(p, &ptr, 10);
    } while ((p = strtok_r(NULL, delimiter, &temp)) != NULL);

    free(temp);

    return array;
}

int getDigitalInput(string message) {
    int number = -1;

    printf("%s", message);
    scanf("%d", &number);

    return number;
}

void exitWithReason(string message) {
    printf("%s", message);

    exit(-1);
}

void printMatrix(int* matrix, uul cube) {
    for (int i = 0; i < cube; i++) {
        for (int x = 0; x < cube; x++) {
            printf("%d ", matrix[cube*i + x]);
        }
        printf("\n");
    }
}