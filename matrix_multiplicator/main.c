#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "file_reader.h"
#include "matrix_reader.h"
#include "matrix_computer.h"

int main() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    }

    char* fileContent = readFile("../matrix");
    int *firstMatrix = (int*) malloc(sizeof (int)), *secondMatrix = (int*) malloc(sizeof (int));

    getMatrix(fileContent, &firstMatrix, &secondMatrix);
    computeMatrixWithKernel(firstMatrix, secondMatrix, 3,3 );

    free(fileContent);
    free(firstMatrix);
    free(secondMatrix);
}
