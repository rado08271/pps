#include <stdio.h>
#include <CL/cl.h>
#include <stdbool.h>
#include <time.h>

#include "matrix_computer.h"
#include "opencl_kernel_reader.h"
#include "opencl_utils.h"
#include "simple_timer.h"

#define MAX_TILE_SIZE 16
#define WORK_ITEMS_PER_THREAD 8

int* computeMatrixIntIJK(const int* first, const int* second, uul cube, bool isGenerated) {
    printf("Computing matrix using IJK ...\n");


    int* result = (int*) malloc(sizeof(int*) * cube*cube);

    if (isGenerated) {
        startTimer();

        for (int k = 0; k < cube; k++) {
            for (int i = 0; i < cube; i++) {
                for (int j = 0; j < cube; j++) {
                    result[i * cube + j] += first[i * cube + k] * second[k * cube + j];
                }
            }
        }


        stopTimer();
    } else {
        printf("this won't work for this algorithm");
    }


    return result;
}

float* computeMatrixFloatIJK(const float* first, const float* second, uul cube, bool isGenerated) {
    printf("Computing matrix using IJK ...\n");

    float* result = (float*) malloc(sizeof(int*) * cube*cube);

    if (isGenerated) {
        startTimer();

        for (int i = 0; i < cube; i++){
            for (int x = 0; x < cube; x++) {
                float cellResult = 0;
                for (int z = 0; z < cube; z++) {
                    cellResult += first[i*cube + z] * second[x + (z * cube)];
                }
                result[cube*i+x] = cellResult;
            }
        }

        stopTimer();
    } else {
        printf("this won't work for this algorithm");
    }

    return result;
}

int* computeMatrixIntWithKernel(const int* first, const int* second, uul bigCube, bool isGenerated, cl_device_id deviceId) {
    int cube = (int) bigCube;
    int* resultMatrix = (int*) malloc(sizeof(int*) * cube * cube);

    string kernelSource = readMatrixMultiplierKernel();
    cl_int err;

    printf("\nInitialization and allocation for OpenCl");

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "matrixMultiplication", device);


    cl_mem memoryBufferForFirstMatrix = allocateMemory(context, sizeof (first) * cube * cube, true);
    if (memoryBufferForFirstMatrix != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForFirstMatrix, CL_TRUE, 0, sizeof (first) * cube * cube, first, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForSecondMatrix = allocateMemory(context, sizeof (second) * cube * cube, true);
    if (memoryBufferForSecondMatrix != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForSecondMatrix, CL_TRUE, 0, sizeof (second) * cube * cube, second, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForResultMatrix = allocateMemory(context, sizeof (resultMatrix) * cube * cube, false);
    if (memoryBufferForSecondMatrix != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForResultMatrix, CL_TRUE, 0, sizeof (resultMatrix) * cube * cube, resultMatrix, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForFirstMatrix);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferForSecondMatrix);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &memoryBufferForResultMatrix);
    err = clSetKernelArg(kernel, 3, sizeof(int), (void*) &cube);
    err = clSetKernelArg(kernel, 4, sizeof(int), (void*) &cube);

    const size_t local[2] = {(size_t) MAX_TILE_SIZE, (size_t) MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD};
    const size_t global[2] = {(size_t) getWorkerGroupSize(cube), (size_t) getWorkerGroupSize(cube)/WORK_ITEMS_PER_THREAD};

    printf("\nExecution of OpenCl");

    startTimer();

    runOpenClProgram(kernel, queue, local, global);

    err = clEnqueueReadBuffer(queue, memoryBufferForResultMatrix, CL_TRUE, 0, sizeof (resultMatrix) * cube * cube, resultMatrix, 0, NULL, NULL);

    if (err != CL_SUCCESS) {
        printf("\nWhile reading error occured: %d\n", err);
    }

    clFinish(queue);

    stopTimer();

    printf("\nReleasing resources");

    clReleaseDevice(device);
    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseMemObject(memoryBufferForFirstMatrix);
    clReleaseMemObject(memoryBufferForSecondMatrix);
    clReleaseMemObject(memoryBufferForResultMatrix);

    return resultMatrix;
}


float* computeMatrixFloatWithKernel(const float* first, const float* second, uul bigCube, bool isGenerated, cl_device_id deviceId) {
    int cube = (int) bigCube;
    float* resultMatrix = (float*) malloc(sizeof(float*) * cube * cube);

    string kernelSource = readMatrixMultiplierKernel();
    cl_int err;

    printf("\nInitialization and allocation for OpenCl");

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "matrixMultiplicationFloat", device);

    cl_mem memoryBufferForFirstMatrix = allocateMemory(context, sizeof (first) * cube * cube, true);
    if (memoryBufferForFirstMatrix != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForFirstMatrix, CL_TRUE, 0, sizeof (first) * cube * cube, first, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForSecondMatrix = allocateMemory(context, sizeof (second) * cube * cube, true);
    if (memoryBufferForSecondMatrix != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForSecondMatrix, CL_TRUE, 0, sizeof (second) * cube * cube, second, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    cl_mem memoryBufferForResultMatrix = allocateMemory(context, sizeof (resultMatrix) * cube * cube, false);
    if (memoryBufferForSecondMatrix != NULL)
        err = clEnqueueWriteBuffer(queue, memoryBufferForResultMatrix, CL_TRUE, 0, sizeof (resultMatrix) * cube * cube, resultMatrix, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForFirstMatrix);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferForSecondMatrix);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*) &memoryBufferForResultMatrix);
    err = clSetKernelArg(kernel, 3, sizeof(int), (void*) &cube);
    err = clSetKernelArg(kernel, 4, sizeof(int), (void*) &cube);

    const size_t local[2] = {(size_t) MAX_TILE_SIZE, (size_t) MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD};
    const size_t global[2] = {(size_t) getWorkerGroupSize(cube), (size_t) getWorkerGroupSize(cube)/WORK_ITEMS_PER_THREAD};

    printf("\nExecution of OpenCl");

    startTimer();

    runOpenClProgram(kernel, queue, local, global);

    err = clEnqueueReadBuffer(queue, memoryBufferForResultMatrix, CL_TRUE, 0, sizeof (resultMatrix) * cube * cube, resultMatrix, 0, NULL, NULL);

    if (err != CL_SUCCESS) {
        printf("\nWhile reading error occured: %d\n", err);
    }

    clFinish(queue);

    stopTimer();

    printf("\nReleasing resources");

    clReleaseDevice(device);
    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseMemObject(memoryBufferForFirstMatrix);
    clReleaseMemObject(memoryBufferForSecondMatrix);
    clReleaseMemObject(memoryBufferForResultMatrix);

    return resultMatrix;
}
