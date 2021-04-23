
__kernel void matrixMutliplicationSimple(
          __global int* firstMatrix,
          __global int* secondMatrix,
          __global int* resultMatrix,
          int cube) {

    const int row = get_local_id(0);
    const int col = get_local_id(1);

    int cellResult = 0;

    for (int x = 0; x < cube; x++) {
        cellResult += firstMatrix[x * cube + row] * secondMatrix[col * cube + x];
    }

    resultMatrix[col * cube + row] = cellResult;

}

#define MAX_TILE_SIZE 16                // Max size of 1 tile - same as local work size for first work dim
#define WORK_ITEMS_PER_THREAD 8         // The work-per-thread specified constant

__kernel void matrixMultiplication(
        __global int* firstMatrix,
        __global int* secondMatrix,
        __global int* resultMatrix,
        int cube) {

    const int row = get_local_id(0);                // submatrix row
    const int col = get_local_id(1);                // submatrix col
    const int globalRow = MAX_TILE_SIZE * get_group_id(0) + row;    // main matrix row
    const int globalCol = MAX_TILE_SIZE * get_group_id(1) + col;    // main matrix col

    // create submatrixes
    __local int firstSubmatrix[MAX_TILE_SIZE][MAX_TILE_SIZE];
    __local int secondSubmatrix[MAX_TILE_SIZE][MAX_TILE_SIZE];


    // initialize sums for every tile
    int sums[WORK_ITEMS_PER_THREAD];
    for (int wi = 0; wi < WORK_ITEMS_PER_THREAD; wi++) {
        sums[wi] = 0;
    }

    // Perform computations over every tile
    const int numberOfTiles = cube/MAX_TILE_SIZE;
    for (int tile = 0; tile < numberOfTiles; tile++) {

        // Lead tile to local memory
        for (int wi = 0; wi < WORK_ITEMS_PER_THREAD; wi++) {
            const int tiledRow = MAX_TILE_SIZE * tile + row;
            const int tiledCol = MAX_TILE_SIZE * tile + col;
            firstSubmatrix[col + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD)][row] = firstMatrix[(tiledCol + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD))*cube + globalRow];
            secondSubmatrix[col + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD)][row] = secondMatrix[(globalCol + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD))*cube + tiledRow];
        }

        // wait for current tile to be loaded
        barrier(CLK_LOCAL_MEM_FENCE);

        for (int comp = 0; comp < MAX_TILE_SIZE; comp++) {
            for (int wi = 0; wi < WORK_ITEMS_PER_THREAD; wi++) {
                sums[wi] += firstSubmatrix[comp][row] * secondSubmatrix[col + wi * (MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD)][comp];
            }
        }

        // Synchronise before loading the next tile
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Store the final results in resultMatrix
    for (int wi=0; wi<WORK_ITEMS_PER_THREAD; wi++) {
        resultMatrix[(globalCol + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD))*cube + globalRow] = sums[wi];
    }
}


__kernel void matrixMultiplicationFloat(
        __global int* firstMatrix,
        __global int* secondMatrix,
        __global int* resultMatrix,
        int cube) {

    const int row = get_local_id(0);                // submatrix row
    const int col = get_local_id(1);                // submatrix col
    const int globalRow = MAX_TILE_SIZE * get_group_id(0) + row;    // main matrix row
    const int globalCol = MAX_TILE_SIZE * get_group_id(1) + col;    // main matrix col

    // create submatrixes
    __local float firstSubmatrix[MAX_TILE_SIZE][MAX_TILE_SIZE];
    __local float secondSubmatrix[MAX_TILE_SIZE][MAX_TILE_SIZE];


    // initialize sums for every tile
    int sums[WORK_ITEMS_PER_THREAD];
    for (int wi = 0; wi < WORK_ITEMS_PER_THREAD; wi++) {
        sums[wi] = 0;
    }

    // Perform computations over every tile
    const int numberOfTiles = cube/MAX_TILE_SIZE;
    for (int tile = 0; tile < numberOfTiles; tile++) {

        // Lead tile to local memory
        for (int wi = 0; wi < WORK_ITEMS_PER_THREAD; wi++) {
            const int tiledRow = MAX_TILE_SIZE * tile + row;
            const int tiledCol = MAX_TILE_SIZE * tile + col;
            firstSubmatrix[col + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD)][row] = firstMatrix[(tiledCol + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD))*cube + globalRow];
            secondSubmatrix[col + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD)][row] = secondMatrix[(globalCol + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD))*cube + tiledRow];
        }

        // wait for current tile to be loaded
        barrier(CLK_LOCAL_MEM_FENCE);

        for (int comp = 0; comp < MAX_TILE_SIZE; comp++) {
            for (int wi = 0; wi < WORK_ITEMS_PER_THREAD; wi++) {
                sums[wi] += firstSubmatrix[comp][row] * secondSubmatrix[col + wi * (MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD)][comp];
            }
        }

        // Synchronise before loading the next tile
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Store the final results in resultMatrix
    for (int wi=0; wi<WORK_ITEMS_PER_THREAD; wi++) {
        resultMatrix[(globalCol + wi*(MAX_TILE_SIZE/WORK_ITEMS_PER_THREAD))*cube + globalRow] = sums[wi];
    }
}


__kernel void matrixMultiplicationFloatSimple(
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
