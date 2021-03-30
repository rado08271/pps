
#ifndef MATRIX_MULTIPLICATOR_MATRIX_COMPUTER_H
#define MATRIX_MULTIPLICATOR_MATRIX_COMPUTER_H
#include <CL/cl.h>

#include "main.h"

int* computeMatrixIntStrassen(const int* first, const int* second, uul cube, bool isGenerated);
float* computeMatrixFloatStrassen(const float* first, const float* second, uul cube, bool isGenerated);
int* computeMatrixIntIJK(const int* first, const int* second, uul cube, bool isGenerated);
float* computeMatrixFloatIJK(const float* first, const float* second, uul cube, bool isGenerated);
int* computeMatrixIntWithKernel(const int* first, const int* second, uul bigCube, bool isGenerated, cl_device_id deviceId);
float* computeMatrixFloatWithKernel(const float* first, const float* second, uul bigCube, bool isGenerated, cl_device_id deviceId);

#endif //MATRIX_MULTIPLICATOR_MATRIX_COMPUTER_H
