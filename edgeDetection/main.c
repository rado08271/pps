#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sobel.h"

#define INT_SIZE sizeof(int)

void convert2dto1d(int **matrix1, int width, int height, int *finalArray, int* sharp, int *out) {
    int index = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            finalArray[index] = matrix1[i][j];
            out[index] = matrix1[i][j];
            sharp[index] = matrix1[i][j];
            index += 1;
        }
    }
}

void convert1dto2d(int **outputMatrix, int width, int height, int *array) {
    int index = 0, row = 0;
    for (int i = 0; i < height * width; i += width) {
        for (int j = i; j < i + width; j++) {
            outputMatrix[row][index++] = array[j];
        }
        index = 0;
        row++;
    }
}

int main() {
    int *finalArray, *outputArray, *sharp;
    PGM input_image, output_image;
    char dir[] = "C:\\Users\\Paly\\Desktop\\skicar.pgm";

    read_pgm_file(dir, &input_image);
    padding(&input_image);

    initialize_image(&output_image, input_image);

    int width = input_image.width, height = input_image.height;

    finalArray = (int *) calloc(width * height, sizeof(float));
    outputArray = (int *) calloc(width * height, sizeof(float));
    sharp = (int *) calloc(width * height, sizeof(float));

    convert2dto1d(input_image.imageData, width, height, finalArray,sharp, outputArray);
    sobel_edge_detector1D(width, height, finalArray, outputArray, sharp);
    convert1dto2d(output_image.imageData, width, height, outputArray);

    min_max_normalization(&output_image, output_image.imageData);
    write_pgm_file(&output_image, dir, output_image.imageData, "_sharsssp.pgm");
    printf("\nGradient saved: %s \n", dir);

    free(finalArray);
    free(outputArray);
    free(sharp);
    free(input_image.imageData);
    free(output_image.imageData);

    return 0;
}

void read_pgm_file(char *dir, PGM *input_image) {
    FILE *input_file;
    int i, j, num;

    input_file = fopen(dir, "rb");
    if (input_file == NULL) {
        printf("File could not opened!");
        return;
    }

    fgets(input_image->version, sizeof(input_image->version), input_file);
    read_comments(input_file);

    fscanf(input_file, "%d %d %d", &input_image->width, &input_image->height, &input_image->maxGrayLevel);

    input_image->imageData = (int **) calloc(input_image->height, sizeof(int *));
    for (i = 0; i < input_image->height; i++) {
        input_image->imageData[i] = calloc(input_image->width, sizeof(int));
    }

    if (strcmp(input_image->version, "P2") == 0) {
        for (i = 0; i < input_image->height; i++) {
            for (j = 0; j < input_image->width; j++) {
                fscanf(input_file, "%d", &num);
                input_image->imageData[i][j] = num;
            }
        }
    } else if (strcmp(input_image->version, "P5") == 0) {
        char *buffer;
        int buffer_size = input_image->height * input_image->width;
        buffer = (char *) malloc((buffer_size + 1) * sizeof(char));

        if (buffer == NULL) {
            printf("Can not allocate memory for buffer! \n");
            return;
        }
        fread(buffer, sizeof(char), input_image->width * input_image->height, input_file);
        for (i = 0; i < input_image->height * input_image->width; i++) {
            input_image->imageData[i / (input_image->width)][i % input_image->width] = buffer[i];
        }
        free(buffer);
    }
    fclose(input_file);
    printf("____IMAGE INFO____\n");
    printf("Version: %s \nWidth: %d \nHeight: %d \nMaximum Gray Level: %d \n",
           input_image->version, input_image->width, input_image->height, input_image->maxGrayLevel);
}

void read_comments(FILE *input_file) {
    char ch;
    char line[100];

    while ((ch = fgetc(input_file)) != EOF && (isspace(ch))) { ;
    }
    if (ch == '#') {
        fgets(line, sizeof(line), input_file);
    } else {
        fseek(input_file, -2L, SEEK_CUR);
    }
}

void padding(PGM *input_image) {
    int i;
    for (i = 0; i < input_image->width; i++) {
        input_image->imageData[0][i] = 0;
        input_image->imageData[input_image->height - 1][i] = 0;
    }
    for (i = 0; i < input_image->height; i++) {
        input_image->imageData[i][0] = 0;
        input_image->imageData[i][input_image->width - 1] = 0;
    }
}

void copyImages(PGM *output_image, PGM input_image) {
    int i, j;
    strcpy(output_image->version, input_image.version);
    output_image->width = input_image.width;
    output_image->height = input_image.height;
    output_image->maxGrayLevel = input_image.maxGrayLevel;
    for (i = 0; i < output_image->height; i++) {
        for (j = 0; j < output_image->width; j++) {
            output_image->imageData[i][j] = input_image.imageData[i][j];
        }
    }
}

void initialize_image(PGM *output_image, PGM input_image) {
    int i;
    //allocte memory
    output_image->imageData = (int **) calloc(input_image.height, sizeof(int *));
    for (i = 0; i < input_image.height; i++) {
        output_image->imageData[i] = calloc(input_image.width, INT_SIZE);
    }
    copyImages(output_image, input_image);

}

void sobel_edge_detector1D(int width, int height, int *array, int *out, int*sharp) {
    int sharpenKernel[9] = {
            0, -1, 0,
            -1, 5, -1,
            0, -1, 0
    };

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

    //sharpening algorithm
    for (int i = width - 1; i < height * width - width - 1; i += width) {
        for (int j = i + 1; j < i + width - 1; j++) {

            //getting position index of element
            int position = j + 1;

            //sharpen
            int sharpen = (array[position - width] * sharpenKernel[1]) +
                             (array[position - 1] * sharpenKernel[3]) + (array[position] * sharpenKernel[4]) +(array[position + 1] * sharpenKernel[5]) +
                    (array[position + width] * sharpenKernel[7]);

            /*implementation of finalSquaredNumber*/

            //number to be squared is calculated as follows (sobel algorithm)
            int number = (sharpen * sharpen);
            float temp, finalSquaredNumber;
            finalSquaredNumber = number / 2;
            temp = 0;

            while (finalSquaredNumber != temp) {
                temp = finalSquaredNumber;
                finalSquaredNumber = (number / temp + temp) / 2;
            }
            sharp[position] = finalSquaredNumber;
        }
    }

    for (int i = width - 1; i < height * width - width - 1; i += width) {
        for (int j = i + 1; j < i + width - 1; j++) {

            //getting position index of element
            int position = j + 1;

            //xdimension calculated value
            int xDimension = (sharp[position - 1 - width] * xKernel[0]) + (sharp[position + 1 - width] * xKernel[2]) +
                             (sharp[position - 1] * xKernel[3]) + (sharp[position + 1] * xKernel[5]) +
                             (sharp[position - 1 + width] * xKernel[6]) + (sharp[position + 1 + width] * xKernel[8]);

            //xdimension calculated value
            int yDimension = (sharp[position - 1 - width] * yKernel[0]) + (sharp[position - width] * yKernel[1]) +
                             (sharp[position + 1 - width] * yKernel[2]) +
                             (sharp[position - 1 + width] * yKernel[6]) + (sharp[position + width] * yKernel[7]) +
                             (sharp[position + 1 + width] * yKernel[8]);

            /*implementation of finalSquaredNumber*/

            //number to be squared is calculated as follows (sobel algorithm)
            int number = (xDimension * xDimension) + (yDimension * yDimension);
            float temp, finalSquaredNumber;
            finalSquaredNumber = number / 2;
            temp = 0;

            while (finalSquaredNumber != temp) {
                temp = finalSquaredNumber;
                finalSquaredNumber = (number / temp + temp) / 2;
            }
            out[position] = finalSquaredNumber;
        }
    }
}

void min_max_normalization(PGM *input_image, int **matrix) {
    int min = 1000000, max = 0, i, j;

    for (i = 0; i < input_image->height; i++) {
        for (j = 0; j < input_image->width; j++) {
            if (matrix[i][j] < min) {
                min = matrix[i][j];
            } else if (matrix[i][j] > max) {
                max = matrix[i][j];
            }
        }
    }
    for (i = 0; i < input_image->height; i++) {
        for (j = 0; j < input_image->width; j++) {
            double ratio = (double) (matrix[i][j] - min) / (max - min);
            matrix[i][j] = ratio * 255;
        }
    }
}

void write_pgm_file(PGM *input_image, char dir[], int **matrix, char name[]) {
    FILE *output_image;
    int i, j, count = 0;

    char *token = strtok(dir, ".");
    if (token != NULL) {
        strcat(token, name);
        output_image = fopen(token, "wb");
    }

    output_image = fopen(dir, "wb");
    fprintf(output_image, "%s\n", input_image->version);
    fprintf(output_image, "%d %d\n", input_image->width, input_image->height);
    fprintf(output_image, "%d\n", input_image->maxGrayLevel);

    if (strcmp(input_image->version, "P2") == 0) {
        for (i = 0; i < input_image->height; i++) {
            for (j = 0; j < input_image->width; j++) {
                fprintf(output_image, "%d", matrix[i][j]);
                if (count % 17 == 0)
                    fprintf(output_image, "\n");
                else
                    fprintf(output_image, " ");
                count++;
            }
        }
    } else if (strcmp(input_image->version, "P5") == 0) {
        for (i = 0; i < input_image->height; i++) {
            for (j = 0; j < input_image->width; j++) {
                char num = matrix[i][j];
                fprintf(output_image, "%c", num);
            }
        }
    }
    fclose(output_image);
}