#ifndef MATRIX_MULTIPLICATOR_OPENCL_UTILS_H
#define MATRIX_MULTIPLICATOR_OPENCL_UTILS_H

#include <stdbool.h>
#include "sobel.h"

cl_device_id chooseDevice();
cl_device_id chooseBestDevice();
cl_event createEvent(cl_context context);
cl_context createContext(cl_device_id device);
cl_command_queue createExecutionOrder(cl_context context, cl_device_id device);
cl_mem allocateMemory(cl_context context, size_t size, bool readOnly);
cl_program createProgram(cl_context context, string sourceCode, cl_device_id device);
cl_kernel createProgramKernel(cl_program program, string sourceKernelName, cl_device_id deviceId);
cl_int runOpenClProgram(cl_kernel kernel, cl_command_queue queue, const size_t localWorkSize[1], const size_t globalWorkSize[1], cl_event waitList);
size_t getWorkerGroupSize(uul pixels);

#endif //MATRIX_MULTIPLICATOR_OPENCL_UTILS_H
