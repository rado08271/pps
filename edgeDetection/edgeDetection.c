//
// Created by rafig on 21. 4. 2021.
//
#include <CL/cl.h>
#include <stdbool.h>

#include "edgeDetection.h"
#include "opencl_kernel_reader.h"
#include "opencl_utils.h"
#include "simple_timer.h"

#define WORKER_THREAD 32

int* runImageSharpening( int* inArray,  int* sharpedArray, int* outArray, int widthIn, int heightIn) {
    cl_device_id device = chooseBestDevice();

    int* sharpedImage = sharpenImage(inArray, sharpedArray, outArray, widthIn, heightIn, device);
    int* edgesDetected = edgeDetection(inArray, sharpedArray, outArray, widthIn, heightIn, device);

    clReleaseDevice(device);

    return edgesDetected;
}

int* sharpenImage( int* inArray,  int* sharpedArray, int* outArray, int widthIn, int heightIn, cl_device_id deviceId) {
    size_t width = (size_t) widthIn;
    size_t height = (size_t) heightIn;
    int* tmpOutArray = (int*) calloc(width*height, sizeof(int));

    uul tmpAttempts = 0;
    string kernelSource = readBruteForceKernel();
    cl_int err;

    printf("\nInitialization and allocation for OpenCl");

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_event* waitList = (cl_event*) calloc(1, sizeof(cl_event));
    cl_event event = createEvent(context);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "sharpenImage", device);

    printf("\nWidth: %zu and Height: %zu to allocate %llu\n", width, height, sizeof (inArray) * width*height);

    size_t c = 0;
    for (int y = 0; y < height; y++ ) {
        for (int x = 0; x < width; x++) {
            c++;
        }
    }

    cl_mem memoryBufferForInImage = allocateMemory(context, sizeof(int) * c, false);
    if (memoryBufferForInImage != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForInImage, CL_TRUE, 0,sizeof(int) * c, inArray, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating orig image error occured: %d\n", err);
        }
    }

    cl_mem memoryBufferForOutImage = allocateMemory(context, sizeof(int) * c, false);
    if (memoryBufferForInImage != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForOutImage, CL_TRUE, 0, sizeof(int) * c, outArray, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating orig image error occured: %d\n", err);
        }
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForInImage);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferForOutImage);
    err = clSetKernelArg(kernel, 2, sizeof(int), (void*) &width);
    err = clSetKernelArg(kernel, 3, sizeof(int), (void*) &height);

    const size_t local[2] = {(size_t) WORKER_THREAD, (size_t) WORKER_THREAD};
    const size_t global[2] = {(size_t) getWorkerGroupSize(height), (size_t) getWorkerGroupSize(width)};

    printf("\nExecution of OpenCl");

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &event);

    if (err != CL_SUCCESS) {
        printf("\nExecution failed with %d\n", err);
    }

    err = clWaitForEvents(1, &event);

    err = clEnqueueReadBuffer(queue, memoryBufferForOutImage, CL_TRUE, 0, sizeof(int) * c, outArray, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading recreated image error occured: %d\n", err);
    }

    clFinish(queue);

    printf("\nReleasing resources\n");

    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseMemObject(memoryBufferForInImage);
    clReleaseMemObject(memoryBufferForOutImage);

    return outArray;
}

int* edgeDetection( int* inArray,  int* sharpedArray, int* outArray, int widthIn, int heightIn, cl_device_id deviceId) {
    size_t width = (size_t) widthIn;
    size_t height = (size_t) heightIn;
    int* tmpOutArray = (int*) calloc(width*height, sizeof(int));

    uul tmpAttempts = 0;
    string kernelSource = readBruteForceKernel();
    cl_int err;

    printf("\nInitialization and allocation for OpenCl");

    cl_device_id device = deviceId;
    cl_context context = createContext(device);
    cl_command_queue queue = createExecutionOrder(context, device);
    cl_event* waitList = (cl_event*) calloc(1, sizeof(cl_event));
    cl_event event = createEvent(context);
    cl_program program = createProgram(context, kernelSource, device);
    cl_kernel kernel = createProgramKernel(program, "edgeDetect", device);

    printf("\nWidth: %zu and Height: %zu to allocate %llu\n", width, height, sizeof (inArray) * width*height);

    size_t c = 0;
    for (int y = 0; y < height; y++ ) {
        for (int x = 0; x < width; x++) {
            c++;
        }
    }

    cl_mem memoryBufferForInImage = allocateMemory(context, sizeof(int) * c, true);
    if (memoryBufferForInImage != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForInImage, CL_TRUE, 0,sizeof(int) * c, inArray, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating orig image error occured: %d\n", err);
        }
    }

    cl_mem memoryBufferForOutImage = allocateMemory(context, sizeof(int) * c, false);
    if (memoryBufferForInImage != NULL) {
        err = clEnqueueWriteBuffer(queue, memoryBufferForOutImage, CL_TRUE, 0, sizeof(int) * c, outArray, 0, NULL, NULL);      // CL_TRUE = clEnqueueReadBuffer does not return until the buffer data has been read and copied into memory pointed to by ptr.
        if (err != CL_SUCCESS) {
            printf("\nWhile allocating orig image error occured: %d\n", err);
        }
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*) &memoryBufferForInImage);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*) &memoryBufferForOutImage);
    err = clSetKernelArg(kernel, 2, sizeof(int), (void*) &width);
    err = clSetKernelArg(kernel, 3, sizeof(int), (void*) &height);

    const size_t local[2] = {(size_t) WORKER_THREAD, (size_t) WORKER_THREAD};
    const size_t global[2] = {(size_t) getWorkerGroupSize(height), (size_t) getWorkerGroupSize(width)};

    startTimer();

    printf("\nExecution of OpenCl");

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &event);

    if (err != CL_SUCCESS) {
        printf("\nExecution failed with %d\n", err);
    }

    err = clWaitForEvents(1, &event);

    err = clEnqueueReadBuffer(queue, memoryBufferForOutImage, CL_TRUE, 0, sizeof(int) * c, outArray, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nWhile reading recreated image error occured: %d\n", err);
    }

    stopTimer();

    clFinish(queue);

    printf("\nReleasing resources\n");

    clReleaseContext(context);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    clReleaseMemObject(memoryBufferForInImage);
    clReleaseMemObject(memoryBufferForOutImage);

    return outArray;
}