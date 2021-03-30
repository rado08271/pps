
#ifndef MATRIX_MULTIPLICATOR_MATRIX_READER_H
#define MATRIX_MULTIPLICATOR_MATRIX_READER_H

#include "main.h"

int getMatrix(string matrixData, int** firstMatrix, int** secondMatrix);
string createRandomMatrix(uul firstCube, uul secondCube);
int* generateRandomIntMatrix(uul cube);
float* generateRandomFloatMatrix(uul cube);

#endif //MATRIX_MULTIPLICATOR_MATRIX_READER_H
