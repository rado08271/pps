#include "opencl_kernel_reader.h"
#include "file_reader.h"

string readKernel(string kernel) {
    return readFile(kernel);
}

string readMatrixMultiplierKernel() {
    return readKernel("../matrix_mult");
}

string readMockedKernel() {
    return (string) {
        "kernel void computeMatrix(global float *data) {\n"
        "   int id = get_global_id(0);\n"
        "   data[id] = sin(data[id]);\n"
        "}\n"
    };
}
