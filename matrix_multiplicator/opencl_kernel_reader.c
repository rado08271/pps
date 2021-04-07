#include "opencl_kernel_reader.h"
#include "file_reader.h"

string readKernel(string kernel) {
    return readFile(kernel);
}

string readMatrixMultiplierKernel() {
    return readKernel("../kernel.cl");
}

