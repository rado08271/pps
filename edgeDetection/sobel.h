#ifndef UNTITLED1_SOBEL_H
#define UNTITLED1_SOBEL_H

#include <stdio.h>

#define EOL "\n"
#define TERM '\0'

typedef unsigned long long uul;
typedef char* string;

int isspace(int argument);

typedef struct {
    char version[3];
    int width;
    int height;
    int maxGrayLevel;
    int **imageData;
} PGM;

void read_pgm_file(char* path, PGM* pgm);
void readComments(FILE* file);
void padding(PGM* pgm);
void copyImages(PGM* src, PGM dst);
void initializeImage(PGM* src, PGM dst);
void sobel_edge_detector1D(int width, int height, int* in, int* out, int* sharp);
int* sobelEdgeDetectorInOpenCl(int width, int height, int *array, int *out, int *sharp);
void convert2dto1d(int **matrix1, int width, int height, int *finalArray, int* sharp, int *out);
void convert1dto2d(int **outputMatrix, int width, int height, int *array);
void min_max_normalization(PGM *input_image, int **matrix);
void write_pgm_file(PGM *input_image, char dir[], int **matrix, char name[]);

#endif //UNTITLED1_SOBEL_H
