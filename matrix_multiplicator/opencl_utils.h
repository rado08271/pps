#ifndef MATRIX_MULTIPLICATOR_OPENCL_UTILS_H
#define MATRIX_MULTIPLICATOR_OPENCL_UTILS_H

#include "main.h"

cl_device_id chooseDevice();
cl_context createContext(cl_device_id device);
cl_command_queue createExecutionOrder(cl_context context, cl_device_id device);

#endif //MATRIX_MULTIPLICATOR_OPENCL_UTILS_H
