#include "file_reader.h"
#include <stdio.h>
#include <stdlib.h>

char* readFile(const char* filePath) {
    char *buffer = NULL;
    int string_size, read_size;
    FILE *handler = fopen(filePath, "r");

    if (handler) {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);

        buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
        read_size = fread(buffer, sizeof(char), string_size, handler);

        buffer[string_size] = '\0';

        fclose(handler);
    } else {
        printf("File does not exist");
    }

    return buffer;
}

