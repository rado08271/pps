/* kernel.cl 
 * Matrix multiplication: C = A * B.
 * Device code.
 */
 
// OpenCL Kernel
__kernel void matrixMul(
          __global int* firstMatrix,
          __global int* secondMatrix,
          __global int* resultMatrix,
          int cube) {
  
    int row = get_global_id(0);
    int col = get_global_id(1);
    int cellResult = 0;

    for (int x = 0; x < cube; x++) {
        cellResult += firstMatrix[x * cube + row] * secondMatrix[col * cube + x];
    }
    resultMatrix[col * cube + row] = cellResult;
}

__kernel void matrixMulFloat(
          __global float* firstMatrix,
          __global float* secondMatrix,
          __global float* resultMatrix,
          int cube) {

    int row = get_global_id(0);
    int col = get_global_id(1);
    float cellResult = 0;

    for (int x = 0; x < cube; x++) {
        cellResult += firstMatrix[x * cube + row] * secondMatrix[col * cube + x];
    }
    resultMatrix[col * cube + row] = cellResult;
}
