//
// Created by Daniel on 18. 4. 2021.
//

#ifndef UNTITLED1_SOBEL_H
#define UNTITLED1_SOBEL_H

#endif //UNTITLED1_SOBEL_H


int isspace(int argument);

typedef struct {
    char version[3];
    int width;
    int height;
    int maxGrayLevel;
    int **imageData;
} PGM;

void read_pgm_file(char*, PGM*);
void read_comments(FILE*);
void padding(PGM*);
void initialize_image(PGM*, PGM);
void sobel_edge_detector(PGM*, PGM*);
void sobel_edge_detector1D(int, int, int*, int*, int*);
int convolution(PGM*, int[3][3], int, int);
void min_max_normalization(PGM*, int**);
void write_pgm_file(PGM*, char[], int**, char[]);
void printNeighbours(int[], int, int, int[]);