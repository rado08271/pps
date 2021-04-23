#define MAX_BUFFERS 128
#define WORKER_THREAD 32

__kernel void sharpenImage(__global int *array,
                      __global int *out,
                      int width,
                      int height ) {
    const int row = WORKER_THREAD * get_group_id(0) + get_local_id(0);
    const int col = WORKER_THREAD * get_group_id(1) + get_local_id(1);
    const int currentRow = get_local_id(0);
    const int currentCol = get_local_id(1);

    if (currentRow > 0 && currentRow < height -1 && currentCol > 0 && currentCol < width -1 ) {
        int sharpenKernel[9] = {
                0, -1, 0,
                -1, 5, -1,
                0, -1, 0
        };

        //getting position index of element
        int position = currentCol + currentRow;

        //sharpen
        int sharpen = (array[position - width] * sharpenKernel[1]) + (array[position - 1] * sharpenKernel[3]) +
                      (array[position] * sharpenKernel[4]) + (array[position + 1] * sharpenKernel[5]) +
                      (array[position + width] * sharpenKernel[7]);

        /*implementation of finalSquaredNumber*/

        //number to be squared is calculated as follows (sobel algorithm)
        double number = (double) (sharpen * sharpen);
        out[position] = sqrt(number);
    }

}


__kernel void edgeDetect(__global int *array,
                         __global int *out,
                         int width,
                         int height ){

    const int currentRow = WORKER_THREAD * get_group_id(0) + get_local_id(0);
    const int currentCol = WORKER_THREAD * get_group_id(1) + get_local_id(1);
    const int row = get_local_id(0);
    const int col = get_local_id(1);

    if (currentRow > 0 && currentRow < height -1 && currentCol > 0 && currentCol < width -1 ) {
        int xKernel[9] = {
                -1, 0, 1,
                -2, 0, 2,
                -1, 0, 1
        };
        int yKernel[9] = {
                -1, -2, -1,
                0, 0, 0,
                1, 2, 1
        };

        //getting position index of element
        int position = (currentRow * width) + currentCol;

        //xdimension calculated value
        int xDimension = (array[position - 1 - width] * xKernel[0]) + (array[position + 1 - width] * xKernel[2]) +
                         (array[position - 1] * xKernel[3]) + (array[position + 1] * xKernel[5]) +
                         (array[position - 1 + width] * xKernel[6]) + (array[position + 1 + width] * xKernel[8]);

        //xdimension calculated value
        int yDimension = (array[position - 1 - width] * yKernel[0]) + (array[position - width] * yKernel[1]) +
                         (array[position + 1 - width] * yKernel[2]) + (array[position - 1 + width] * yKernel[6]) +
                         (array[position + width] * yKernel[7]) + (array[position + 1 + width] * yKernel[8]);


        //number to be squared is calculated as follows (sobel algorithm)
        double number = (double) (xDimension * xDimension) + (double) (yDimension * yDimension);
        out[position] = sqrt(number);

    }
}