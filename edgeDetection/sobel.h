
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
void readComments(FILE*);
void padding(PGM*);
void copyImages(PGM*, PGM);
void initializeImage(PGM*, PGM);
void sobel_edge_detector1D(int, int, int*, int*, int*);
void convert2dto1d(int**, int , int , int*, int*, int*);
void convert1dto2d(int**, int, int, int*);
void min_max_normalization(PGM*, int**);
void write_pgm_file(PGM*, char[], int**, char[]);