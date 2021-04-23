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
    // Go through dictionary to find out if such password exists
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

    // If password was not found in dictionary or dictionary is not used iterate through variations untill password is not found
    for (int actualLength = 1; actualLength <= maxLength; actualLength++ ) {
        int* indexes = (int*) calloc(actualLength, sizeof (int));

        const double maxAvailable = pow((double) possibleCharactersAmount, (double) actualLength);
        double max = maxAvailable;

        while (max-- > 0) {
            *attempts += 1;
            string variation = (string) calloc(actualLength, sizeof (char));
            for (int i = 0; i < possibleCharactersAmount - (possibleCharactersAmount - actualLength); i++) {
                variation[i] = possibleCharacters[indexes[i]];
            }

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
        }

//        free(indexes);
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
    }

    return allSolutions;
}

// TODO: Delete
void getVariationsArray(int* indexes, int charactersLength, int currentLength, int currentId) {
    if ((double) currentId < (pow(charactersLength, currentLength) * currentLength)) {
        for (int i = 0; i < currentLength; i++) {
            size_t previousColId = ((currentId - 1) * currentLength) + i;
            size_t currentColId = ((currentId * currentLength) + i);
            if (indexes[previousColId] >= charactersLength -1) {
                indexes[currentColId] = 0;
            } else {
                for (int x = currentColId; x < currentColId + currentLength; x++) {
                    size_t previousColIndex = x - currentLength;
                    size_t currentColIndex = x;
                    indexes[currentColIndex] = indexes[previousColIndex];
                }
                indexes[currentColId] = indexes[previousColId] + 1;
                break;
            }
        }
    }
}

// TODO: Delete
int* getAllVariationsSeq(string possibleCharacters, int currentLength) {
    int *variations = (int*) malloc( sizeof(int) * (size_t) getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength);

    for (int i = 0; i < getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength; i++) {
        variations[i] = 0;
    }

    size_t currentLoop = 1;
    uul loops = getPossibleInvokesForSize(possibleCharacters, currentLength);

    while (loops-- > 0) {
        getVariationsArray(variations, strlen(possibleCharacters), currentLength, currentLoop++);
    }

    return variations;
}

int* getAllVariations(string possibleCharacters, int currentLength, cl_device_id deviceId) {
    string kernelSource = readBruteForceKernel();
    cl_int err;

    int possibleCharactersLength = strlen(possibleCharacters);

    printf("\n===== Initialization and allocation for OpenCl Get all variations =====");

    int *variations = (int*) malloc( sizeof(int) * (size_t) getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength);

    for (int i = 0; i < getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength; i++) {
        variations[i] = 0;
    }

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_event event = createEvent(context);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "getAllVariations", device);

    cl_mem memoryBufferForAllVariations = allocateMemory(context, sizeof(int) * (size_t) getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength, false);
    if (memoryBufferForAllVariations != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForAllVariations, CL_TRUE, 0, sizeof(int) * (size_t) getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength, variations, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating variations error occured: %d\n", err);
        }
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForAllVariations);
    err = clSetKernelArg(kernel, 1, sizeof(int), (void*) &possibleCharactersLength);
    err = clSetKernelArg(kernel, 2, sizeof(int), (void*) &currentLength);

    const size_t local[1] = {(size_t) 1};
    const size_t global[1] = {(size_t) getWorkerGroupSize(getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength)};

    printf("\nExecution of OpenCl\n");

    startTimer();

    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, local, 0, NULL, &event);

    if (err != CL_SUCCESS) {
        printf("\nExecution failed with %d\n", err);
    }

    err = clWaitForEvents(1, &event);
    if (err != CL_SUCCESS) {
        printf("\nWait for event failed with %d", err);
    }

    printf("\nGetting event status\n");

    size_t size = 0;
    cl_int info = 0;
    err = clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, 0, NULL, &size);
    err = clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, size, &info, NULL);
    if (info == CL_QUEUED ) {
        printf("CL_QUEUED: %x\n", info);
    } else if (info == CL_SUBMITTED  ) {
        printf("CL_SUBMITTED : %x\n", info);
    } else if (info == CL_RUNNING  ) {
        printf("CL_RUNNING : %x\n", info);
    } else if (info == CL_COMPLETE  ) {
        printf("CL_COMPLETE : %x\n", info);
    }

    err = clEnqueueReadBuffer(queue, memoryBufferForAllVariations, CL_TRUE, 0, sizeof(int) * (size_t) getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength, variations, 1, &event, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading result error occured: %d\n", err);
    }

    clFinish(queue);

    stopTimer();

    printf("\nReleasing resources");

    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseMemObject(memoryBufferForAllVariations);
    clReleaseEvent(event);

    return variations;
}

string bruteForcePasswordGuesser(string password, string possibleCharacters, int currentLength, uul* attempts, int* variations, cl_device_id deviceId) {
//    int* variations = getAllVariationsSeq(possibleCharacters, currentLength);
    int possibleCharactersLength = strlen(possibleCharacters);
    int passwordLength = strlen(password);

    string guessedPassword = (string) calloc(passwordLength, sizeof(char));

    uul tmpAttempts = 0;
    int stop = 0;
    string kernelSource = readBruteForceKernel();
    cl_int err;

    printf("\n===== Initialization and allocation for OpenCl Brute Force Password Guesser =====");

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_event event = createEvent(context);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "bruteForceVariations", device);

    cl_mem memoryBufferForOriginalPassword = allocateMemory(context, sizeof (password) * passwordLength, true);
    if (memoryBufferForOriginalPassword != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForOriginalPassword, CL_TRUE, 0, sizeof (password) * passwordLength, password, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating orig password error occured: %d\n", err);
        }
    }

    cl_mem memoryBufferForPossibleCharacters = allocateMemory(context, sizeof (possibleCharacters) * strlen(possibleCharacters), true);
    if (memoryBufferForPossibleCharacters != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForPossibleCharacters, CL_TRUE, 0, sizeof (possibleCharacters) * strlen(possibleCharacters), possibleCharacters, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating %s error occured: %d\n", possibleCharacters, err);
        }
    }

    cl_mem memoryBufferForAllVariations = allocateMemory(context, sizeof(int) * (size_t) getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength, true);
    if (memoryBufferForAllVariations != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForAllVariations, CL_TRUE, 0, sizeof(int) * (size_t) getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength, variations, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating variations error occured: %d\n", err);
        }
    }

    cl_mem memoryBufferForResult = allocateMemory(context, sizeof (string) * passwordLength, false);
    if (memoryBufferForResult != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForResult, CL_TRUE, 0, sizeof (guessedPassword) * passwordLength, guessedPassword, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating result error occured: %d\n", err);
        }
    }

    cl_mem memoryBufferForAttempts = allocateMemory(context, sizeof (unsigned long long*), false);
    if (memoryBufferForAttempts != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (unsigned long long*), attempts, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating attempts error occured: %d\n", err);
        }
    }

    cl_mem memoryBufferForStop = allocateMemory(context, sizeof (int*), false);
    if (memoryBufferForAttempts != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (int*), &stop, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating stop error occured: %d\n", err);
        }
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForOriginalPassword);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferForPossibleCharacters);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &memoryBufferForAllVariations);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &memoryBufferForResult);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*) &memoryBufferForAttempts);
    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*) &memoryBufferForStop);
    err = clSetKernelArg(kernel, 6, sizeof(int), (void*) &currentLength);
    err = clSetKernelArg(kernel, 7, sizeof(int), (void*) &passwordLength);
    err = clSetKernelArg(kernel, 8, sizeof(int), (void*) &possibleCharactersLength);

    const size_t local[1] = {(size_t) 16};
    const size_t global[1] = {(size_t) getWorkerGroupSize(getPossibleInvokesForSize(possibleCharacters, currentLength) * currentLength)};

    printf("\nExecution of OpenCl\n");

    startTimer();

    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, local, 0, NULL, &event);

    if (err != CL_SUCCESS) {
        printf("\nExecution failed with %d\n", err);
    }

    err = clWaitForEvents(1, &event);
    if (err != CL_SUCCESS) {
        printf("\nWait for event failed with %d", err);
    }

    printf("\nGetting event status\n");

    size_t size = 0;
    cl_int info = 0;
    err = clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, 0, NULL, &size);
    err = clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, size, &info, NULL);
    if (info == CL_QUEUED ) {
        printf("CL_QUEUED: %x\n", info);
    } else if (info == CL_SUBMITTED  ) {
        printf("CL_SUBMITTED : %x\n", info);
    } else if (info == CL_RUNNING  ) {
        printf("CL_RUNNING : %x\n", info);
    } else if (info == CL_COMPLETE  ) {
        printf("CL_COMPLETE : %x\n", info);
    }

    err = clEnqueueReadBuffer(queue, memoryBufferForResult, CL_TRUE, 0, sizeof (guessedPassword) * passwordLength, guessedPassword, 1, &event, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading result error occured: %d\n", err);
    }

    err = clEnqueueReadBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (unsigned long long*), &tmpAttempts, 1, &event, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading attempts error occured: %d\n", err);
    }

    err = clEnqueueReadBuffer(queue, memoryBufferForStop, CL_TRUE, 0, sizeof (int*), &stop, 1, &event, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading stop error occured: %d\n", err);
    }

    *attempts += tmpAttempts;

    clFinish(queue);

    stopTimer();

    printf("\nReleasing resources");

    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseMemObject(memoryBufferForOriginalPassword);
    clReleaseMemObject(memoryBufferForPossibleCharacters);
    clReleaseMemObject(memoryBufferForAllVariations);
    clReleaseMemObject(memoryBufferForResult);
    clReleaseMemObject(memoryBufferForAttempts);
    clReleaseMemObject(memoryBufferForStop);
    clReleaseEvent(event);

    if (stop == 0) {
        return NULL;
    }
    return guessedPassword;
}

//(string password, string possibleCharacters, int maxLength, string* dictionary, uul dicSize, uul* attempts)
string bruteForcePasswordGuesserDictionary(string password, int maxLength, string* dictionary, uul dicSize, size_t dicMemory, uul* attempts, cl_device_id deviceId) {
    string guessedPassword = (string) calloc(maxLength, sizeof(char));

    string recreatedDictionary = recreateDictionary(dictionary, dicSize, dicMemory);

    printf("\nDictionary was flattened and delimited:\n");

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

    cl_mem memoryBufferForResult = allocateMemory(context, strlen(recreatedDictionary) * sizeof(char), false);
    if (memoryBufferForResult != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForResult, CL_TRUE, 0, sizeof (guessedPassword) * maxLength, guessedPassword, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForAttempts = allocateMemory(context, sizeof(uul), false);
    if (memoryBufferForAttempts != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (uul), attempts, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForOriginalPassword);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferForDictionary);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &memoryBufferForResult);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*) &memoryBufferForAttempts);
    err = clSetKernelArg(kernel, 4, sizeof(int), (void*) &dicSize);

    const size_t local[1] = {(size_t) 1};
    const size_t global[1] = {(size_t) 1};

    printf("\nExecution of OpenCl");

    startTimer();

    runOpenClProgram(kernel, queue, local, global, NULL);

    err = clEnqueueReadBuffer(queue, memoryBufferForResult, CL_TRUE, 0, sizeof (guessedPassword) * maxLength, guessedPassword, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading result error occured: %d\n", err);
    }

    err = clEnqueueReadBuffer(queue, memoryBufferForAttempts, CL_TRUE, 0, sizeof (uul), &tmpAttempts, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading attempts error occured: %d\n", err);
    }

    *attempts += tmpAttempts;
    clFinish(queue);

    stopTimer();

    printf("\nReleasing resources");

    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseMemObject(memoryBufferForOriginalPassword);
    clReleaseMemObject(memoryBufferForDictionary);
    clReleaseMemObject(memoryBufferForResult);
    clReleaseMemObject(memoryBufferForAttempts);

    if (guessedPassword[0] == '\0') {
        return NULL;
    }
    return guessedPassword;
}
