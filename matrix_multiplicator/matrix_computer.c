#include <stdio.h>
#include <CL/cl.h>

#include "matrix_computer.h"
#include "opencl_kernel_reader.h"
#include "opencl_utils.h"

int* computeMatrix(int* first, int* second) {
    printf("\n");
    printf("\n");

    int rows = 4, cols = 3;
    for (int i = 0; i < 3; i++){
        for (int x = 0; x < 3; x++) {
            int cellResult = 0;
            for (int z = 0; z < 3; z++) {
                cellResult += first[i*3 + z] * second[x + (z * 3)];
            }
            printf("%d ", cellResult);
            printf("\t\t");
        }
        printf("\n");
    }

}
int* computeMatrixWithKernel(const int* first, const int* second, uul firstCube, uul secondCube) {
    const char* kernelSource = readMockedKernel();
    cl_int err;
    cl_event firstEvent;

    cl_device_id device = chooseDevice();
    cl_context context = createContext(device);
    cl_command_queue queue = createExecutionOrder(context, device);

    printf ("\nAllocating, %llu bytes of memory for first matrix\n", sizeof (first) * firstCube);
    cl_mem memoryBufferForFirstMatrix = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof (first) * firstCube, NULL, &err);
    if (err != CL_SUCCESS)
        printf("\nMemory allocation failed with %d\n", err);
    else
        err = clEnqueueWriteBuffer(queue, memoryBufferForFirstMatrix, CL_FALSE, 0, sizeof(first) * firstCube, first, 0, NULL, &firstEvent);      // CONSIDER Using CL_TRUE to block execution

    printf ("\nAllocating, %llu bytes of memory for second matrix\n", sizeof (second) * secondCube);
    cl_mem memoryBufferForSecondMatrix = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof (second) * secondCube, NULL, &err);
    if (err != CL_SUCCESS)
        printf("\nMemory allocation failed with %d\n", err);
    else
        err = clEnqueueWriteBuffer(queue, memoryBufferForSecondMatrix, CL_FALSE, 0, sizeof(second) * secondCube, second, 0, NULL, &firstEvent);      // CONSIDER Using CL_TRUE to block execution



    clReleaseContext(context);
    clReleaseCommandQueue(queue);
}


void clCountMatrix() {
    const char* kernelSource = readMockedKernel();

    cl_int err;
    cl_platform_id platform = 0;
    cl_device_id  device = 0;
    cl_context  context = 0;
    cl_command_queue queue = 0;
    cl_program  program = NULL;
    cl_kernel kernel = NULL;
    cl_mem buffer;
    float *data = (float*) malloc(sizeof(float*) * 5);

//    data = [0.23f, 0.94f, 4.22f, 3.21f];
    data[0] = 0.43f;
    data[1] = 1.52f;
    data[2] = 4.22f;
    data[3] = 3.11f;

    // Configure the OpenCL environment
    // get single CPU device to work with
    err = clGetPlatformIDs(1, &platform, NULL);
    printf("Platform ERR is: %d\n", err);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, NULL, NULL);
    printf("Check devices ERR is: %d\n", err);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    printf("Get devices ERR is: %d\n", err);

    // create context for device that will be sharing data
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    printf("Context ERR is: %d\n", err);

    // create queues for commands in (queue properties may be set to 0 for no external manipulation)
    queue = clCreateCommandQueue(context, device, (cl_command_queue_properties) 0, &err);
    printf("Queue ERR is: %d\n", err);

    // compile and create program kernels
    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    printf("Program ERR is: %d\n", err);
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    printf("Building ERR is: %d\n", err);

    // initialize kernel for program and it's source
    kernel = clCreateKernel(program, "computeMatrix", &err);
    printf("kernel ERR is: %d\n", err);

    // create memory objects for enqueuing
    buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, 4*sizeof(*data), NULL, &err);
    printf("Buffer ERR is: %d\n", err);

    // write data for GPU to work with (arguments)
    err = clEnqueueWriteBuffer(queue, buffer, CL_FALSE, 0, SIZE_MAX, data, 0, NULL, NULL);
    printf("Write ERR is: %d\n", err);

    // set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(float*), &buffer);
    printf("ERR is: %d\n", err);

    // enqueue execution of kernel
    size_t globalDimensions[] = {100, 0, 0};
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalDimensions, NULL, 0, NULL, NULL);
    printf("Range kernel ERR is: %d\n", err);

    // read the data from gpu to cpu
    err = clEnqueueReadBuffer(queue, buffer, CL_FALSE, 0, sizeof(cl_float) * 100, data, 0, NULL, NULL);
    printf("Read ERR is: %d\n", err);

    // async wait for executed stack
    err = clFinish(queue);
    printf("FINISHED err is: %d\n", err);
}