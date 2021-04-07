#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "brute_force.h"
#include "main.h"
#include "opencl_utils.h"
#include "opencl_kernel_reader.h"

string checkPassword(string password, string possibleCharacters, int maxLength, uul* attempts) {
    printf("\nhereherehe\n");
    unsigned long possibleCharactersAmount = strlen(possibleCharacters);

    for (int actualLength = 1; actualLength < maxLength; actualLength++ ) {
        int* indexes = (int*) calloc(actualLength, sizeof (int));
        printf("\nhereherehe\n");

        const double maxAvailable = pow((double) possibleCharactersAmount, (double) actualLength);
        double max = maxAvailable;

        while (max-- > 0) {
            *attempts += 1;
            string variation = (string) calloc(actualLength, sizeof (char));
            for (int i = 0; i < possibleCharactersAmount - (possibleCharactersAmount - actualLength); i++) {
                variation[i] = possibleCharacters[indexes[i]];
            }

//            printf("%s\n", variation);

            if (strcmp(password, variation) == 0) {
                return variation;
            }

            for (int i = 0; i < possibleCharactersAmount; i++) {
                if (indexes[i] >= possibleCharactersAmount - 1) {
                    indexes[i] = 0;
                } else {
                    indexes[i]++;
                    break;
                }
            }

            free(variation);
        }

        free(indexes);
    }

    return NULL;
}

double getPossibleInvokes(string possibleCharacters, int maxLength) {
    unsigned long possibleCharactersAmount = strlen(possibleCharacters);

    double allSolutions = 0;

    for (int actualLength = 1; actualLength < maxLength; actualLength++ ) {
        allSolutions += pow(possibleCharactersAmount, actualLength);
        printf("%.2f\t%.2f\n", pow(possibleCharactersAmount, actualLength), allSolutions);
    }

    return allSolutions;
}

string checkDictionary(string password, string* dictionary, uul dicSize, uul* attempts) {
    for (int i = 0; i < dicSize; i++) {
        string currentVariation = dictionary[i];

        if (strcmp(currentVariation, password) == 0) {
            *attempts = i;
            return currentVariation;
        }

        free(currentVariation);
    }

    return NULL;
}

string computeMatrixIntWithKernel(string password, string possibleCharacters, uul maxLength, cl_device_id deviceId) {
    string guessedPassword = (string) calloc(maxLength, sizeof(char));
    string kernelSource = readBruteForceKernel();
    cl_int err;

    printf("\nInitialization and allocation for OpenCl");

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "matrixMul", device);


//    cl_mem memoryBufferForFirstMatrix = allocateMemory(context, sizeof (first) * cube * cube, true);
//    if (memoryBufferForFirstMatrix != NULL)
//        err = clEnqueueWriteBuffer(queue, memoryBufferForFirstMatrix, CL_TRUE, 0, sizeof (first) * cube * cube, first, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
//
//    cl_mem memoryBufferForSecondMatrix = allocateMemory(context, sizeof (second) * cube * cube, true);
//    if (memoryBufferForSecondMatrix != NULL)
//        err = clEnqueueWriteBuffer(queue, memoryBufferForSecondMatrix, CL_TRUE, 0, sizeof (second) * cube * cube, second, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
//
//    cl_mem memoryBufferForResultMatrix = allocateMemory(context, sizeof (resultMatrix) * cube * cube, false);
//    if (memoryBufferForSecondMatrix != NULL)
//        err = clEnqueueWriteBuffer(queue, memoryBufferForResultMatrix, CL_TRUE, 0, sizeof (resultMatrix) * cube * cube, resultMatrix, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
//
//    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForFirstMatrix);
//    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferForSecondMatrix);
//    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &memoryBufferForResultMatrix);
//    err = clSetKernelArg(kernel, 3, sizeof(int), (void*) &cube);
//    err = clSetKernelArg(kernel, 4, sizeof(int), (void*) &cube);
//
//    const size_t local[2] = {(size_t) 32, (size_t) 32};
//    const size_t global[2] = {(size_t) getWorkerGroupSize(cube), (size_t) getWorkerGroupSize(cube)};
//
//    printf("\nExecution of OpenCl");
//
//    startTimer();
//
//    runOpenClProgram(kernel, queue, local, global);
//
//    err = clEnqueueReadBuffer(queue, memoryBufferForResultMatrix, CL_TRUE, 0, sizeof (resultMatrix) * cube * cube, resultMatrix, 0, NULL, NULL);
//
//    if (err != CL_SUCCESS) {
//        printf("\nWhile reading error occured: %d\n", err);
//    }
//
//    clFinish(queue);
//
//    stopTimer();
//
//    printf("\nReleasing resources");
//
    clReleaseDevice(device);
    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
//
//    clReleaseMemObject(memoryBufferForFirstMatrix);
//    clReleaseMemObject(memoryBufferForSecondMatrix);
//    clReleaseMemObject(memoryBufferForResultMatrix);

    return guessedPassword;
}
