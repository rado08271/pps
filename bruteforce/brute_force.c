#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "brute_force.h"
#include "main.h"
#include "opencl_utils.h"
#include "opencl_kernel_reader.h"
#include "simple_timer.h"

string checkPassword(string password, string possibleCharacters, int maxLength) {
    if (maxLength < strlen(password)) {
        return NULL;
    }
    for (int pass = 0; pass < strlen(password); pass++) {
        int isOk = 0;
        for (int ch = 0; ch < strlen(possibleCharacters); ch++) {
            printf("%c and %c\n", password[pass], possibleCharacters[ch]);
            if (password[pass] == possibleCharacters[ch]) {
                isOk = 1;
            }
        }

        if (isOk == 0) {
            return NULL;
        }
    }

    return password;
}

string bruteForcePasswordGuesserSeq(string password, string possibleCharacters, int maxLength, string* dictionary, uul dicSize, uul* attempts) {
    if (dictionary != NULL) {
        for (int i = 0; i < dicSize; i++) {
            string currentVariation = dictionary[i];

            if (strcmp(currentVariation, password) == 0) {
                *attempts = i;
                return currentVariation;
            }
        }
    }

    unsigned long possibleCharactersAmount = strlen(possibleCharacters);

    for (int actualLength = 1; actualLength < maxLength; actualLength++ ) {
        int* indexes = (int*) calloc(actualLength, sizeof (int));

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

//            free(variation);
        }

        free(indexes);
    }

    return NULL;
}

double getPossibleInvokesForSize(string possibleCharacters, int size) {
    return pow(strlen(possibleCharacters), size);
}

double getPossibleInvokes(string possibleCharacters, int maxLength) {
    unsigned long possibleCharactersAmount = strlen(possibleCharacters);

    double allSolutions = 0;

    for (int actualLength = 1; actualLength < maxLength; actualLength++ ) {
        allSolutions += getPossibleInvokesForSize(possibleCharacters, actualLength);
//        printf("%.2f\t%.2f\n", pow(possibleCharactersAmount, actualLength), allSolutions);
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

//(string password, string possibleCharacters, int maxLength, string* dictionary, uul dicSize, uul* attempts)
string bruteForcePasswordGuesser(string password, string possibleCharacters, int maxLength, uul* attempts, cl_device_id deviceId) {
    string guessedPassword = (string) calloc(maxLength, sizeof(char));

    // TODO Delete
//    unsigned long long logLength = (unsigned long long) getPossibleInvokesForSize(possibleCharacters, maxLength) * (maxLength + 1);
//    string log = (string) calloc(logLength, sizeof(char));

    int tmpAttempts = 0;
    string kernelSource = readBruteForceKernel();
    cl_int err;

    printf("\nInitialization and allocation for OpenCl");

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "bruteForce", device);

    cl_mem memoryBufferForOriginalPassword = allocateMemory(context, sizeof (password) * maxLength, true);
    if (memoryBufferForOriginalPassword != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForOriginalPassword, CL_TRUE, 0, sizeof (password) * maxLength, password, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferFprPossibleCharacters = allocateMemory(context, sizeof (possibleCharacters) * strlen(possibleCharacters), true);
    if (memoryBufferFprPossibleCharacters != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferFprPossibleCharacters, CL_TRUE, 0, sizeof (possibleCharacters) * strlen(possibleCharacters), possibleCharacters, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForResult = allocateMemory(context, sizeof (guessedPassword) * maxLength, false);
    if (memoryBufferForResult != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForResult, CL_TRUE, 0, sizeof (guessedPassword) * maxLength, guessedPassword, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForAttempts = allocateMemory(context, sizeof(uul), false);
    if (memoryBufferForAttempts != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (uul), attempts, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    // TODO Delete
//    cl_mem memoryBufferForLog = allocateMemory(context, sizeof(log) * logLength, false);
//    if (memoryBufferForLog != NULL)
//        err = clEnqueueWriteBuffer(queue, memoryBufferForLog, CL_TRUE, 0, sizeof(log) * logLength, log, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.


    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForOriginalPassword);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferFprPossibleCharacters);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &memoryBufferForResult);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &memoryBufferForAttempts);
    err = clSetKernelArg(kernel, 4, sizeof(int), (void*) &maxLength);
    // TODO Delete
//    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*) &memoryBufferForLog);

    const size_t local[1] = {(size_t) 1};
    const size_t global[1] = {(size_t) 1};
//    const size_t global[2] = {(size_t) getWorkerGroupSize(getPossibleInvokes(possibleCharacters, maxLength)), getWorkerGroupSize(getPossibleInvokes(possibleCharacters, maxLength))};

    printf("\nExecution of OpenCl");

    startTimer();

    runOpenClProgram(kernel, queue, local, global);

    err = clEnqueueReadBuffer(queue, memoryBufferForResult, CL_TRUE, 0, sizeof (guessedPassword) * maxLength, guessedPassword, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading result error occured: %d\n", err);
    }

    err = clEnqueueReadBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (uul), &tmpAttempts, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading attempts error occured: %d\n", err);
    }

    // TODO Delete
//    err = clEnqueueReadBuffer(queue, memoryBufferForLog, CL_TRUE, 0, sizeof(log) * logLength, &log, 0, NULL, NULL);
//    if (err != CL_SUCCESS) {
//        printf("\nWhile reading attempts error occured: %d\n", err);
//    }

//    *attempts = tmpAttempts;
    printf("\nAttempts = %d", tmpAttempts);
    printf("\nLogged:\n %s||\t%llu\n", guessedPassword, maxLength * sizeof guessedPassword);

    clFinish(queue);

    stopTimer();

    printf("\nReleasing resources");

    clReleaseDevice(device);
    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    printf("\nReleasing resources for password");
    clReleaseMemObject(memoryBufferForOriginalPassword);
    printf("\nReleasing resources for chars");
    clReleaseMemObject(memoryBufferFprPossibleCharacters);
    printf("\nReleasing resources for result");
    clReleaseMemObject(memoryBufferForResult);
    printf("\nReleasing resources for attempts");
    clReleaseMemObject(memoryBufferForAttempts);
//    printf("\nReleasing resources for log");
//    clReleaseMemObject(memoryBufferForLog);
    printf("\nreturning guessed password!, %s", guessedPassword);

    return guessedPassword;
}

//(string password, string possibleCharacters, int maxLength, string* dictionary, uul dicSize, uul* attempts)
string bruteForcePasswordGuesserDictionary(string password, int maxLength, string* dictionary, uul dicSize, size_t dicMemory, uul* attempts, cl_device_id deviceId) {
    string guessedPassword = (string) calloc(maxLength, sizeof(char));

    string recreatedDictionary = recreateDictionary(dictionary, dicSize, dicMemory);

    printf("\nDictionary was flattened and delimited:\n%s\n", recreatedDictionary);

    int tmpAttempts = 0;
    string kernelSource = readBruteForceKernel();
    cl_int err;

    printf("\nInitialization and allocation for OpenCl");

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "bruteForceWithDictionary", device);

    cl_mem memoryBufferForOriginalPassword = allocateMemory(context, sizeof (password) * maxLength, true);
    if (memoryBufferForOriginalPassword != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForOriginalPassword, CL_TRUE, 0, sizeof (password) * maxLength, password, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForDictionary = allocateMemory(context,  sizeof(char) * (dicMemory + dicSize), true);
    if (memoryBufferForDictionary != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForDictionary, CL_TRUE, 0, sizeof(char) * (dicMemory + dicSize), recreatedDictionary, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForResult = allocateMemory(context, sizeof (guessedPassword) * maxLength, false);
    if (memoryBufferForResult != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForResult, CL_TRUE, 0, sizeof (guessedPassword) * maxLength, guessedPassword, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForAttempts = allocateMemory(context, sizeof(uul), false);
    if (memoryBufferForAttempts != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (uul), attempts, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForOriginalPassword);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferForDictionary);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &memoryBufferForResult);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &memoryBufferForAttempts);
    err = clSetKernelArg(kernel, 4, sizeof(int), (void*) &maxLength);

    const size_t local[1] = {(size_t) 1};
    const size_t global[1] = {(size_t) 1};

    printf("\nExecution of OpenCl");

    startTimer();

    runOpenClProgram(kernel, queue, local, global);

    err = clEnqueueReadBuffer(queue, memoryBufferForResult, CL_TRUE, 0, sizeof (guessedPassword) * maxLength, guessedPassword, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading result error occured: %d\n", err);
    }

    err = clEnqueueReadBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (uul), &tmpAttempts, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading attempts error occured: %d\n", err);
    }

    printf("\nAttempts = %d", tmpAttempts);
    printf("\nLogged:\n %s||\t%llu\n", guessedPassword, maxLength * sizeof guessedPassword);

    clFinish(queue);

    stopTimer();

    printf("\nReleasing resources");

    clReleaseDevice(device);
    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseMemObject(memoryBufferForOriginalPassword);
    clReleaseMemObject(memoryBufferForDictionary);
    clReleaseMemObject(memoryBufferForResult);
    clReleaseMemObject(memoryBufferForAttempts);

    return guessedPassword;
}

void test( const char* password,
           const char* possibleCharacters,
           char* result,
           unsigned long long* attempts,
           int maxLength) {
    //result[0] = possibleCharacters[id];
    //*attempts += globalId;

    int possibleCharactersSize = 0;

    while(possibleCharacters[possibleCharactersSize] != '\0') {
        possibleCharactersSize++;
    }

    long long logLength = 0;
    long long maxAvailableVariations = 1;
    int exp = maxLength;

    while (exp-- > 0) {
        maxAvailableVariations *= possibleCharactersSize;
    }

    long long currentAttempt = 0;

    int indexes[128];

    int found = 0;
    while (maxAvailableVariations-- > 0 && found != 1) {
        currentAttempt++;

        int lengthOfString = possibleCharactersSize - (possibleCharactersSize - maxLength);
        char variation[128];

        for (int i = 0; i < lengthOfString; i++) {
            variation[i] = possibleCharacters[indexes[i]];
        }

        for (int i = 0; i < lengthOfString; i++) {
//            result[logLength++] = variation[i];
            if (variation[i] != password[i]){
                found = -1;
            } else {
                result[i] = variation[i];
            }
        }

        if (found == -1) {
            found = 0;
        } else {
            found = 1;
        }

        for (int i = 0; i < possibleCharactersSize; i++) {
            if (indexes[i] >= possibleCharactersSize - 1) {
                indexes[i] = 0;
            } else {
                indexes[i]++;
                break;
            }
        }
    }
    *attempts = currentAttempt;
}