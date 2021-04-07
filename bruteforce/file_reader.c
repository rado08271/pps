#include "file_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

string* readDictionary(string path, uul* entries) {
    FILE *file = fopen(path, "r");

    size_t bufferLength = 1024;
    char buffer[bufferLength];
    ssize_t lineLength = 0;

    uul currentIdx = 0;

    while (fgets(buffer, bufferLength, file)) {
        currentIdx++;
    }

    rewind(file);

    string* dictionary = (string*) calloc(currentIdx , sizeof(string)/sizeof(char)); //= (string*) calloc(5000000, sizeof(string));
    currentIdx = 0;

    while (fgets(buffer, bufferLength, file)) {
        buffer[strcspn(buffer, "\n")] = 0;
        dictionary[currentIdx] = (string) calloc(sizeof(buffer)/sizeof(buffer[0]), sizeof(char));
        strcpy(dictionary[currentIdx++], buffer);
    }

    *entries = currentIdx;
    fclose(file);

    return dictionary;
}

char* readFile(string filePath) {
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

int printToFile(string filePath, string content) {
    FILE *handler = fopen(filePath, "wb+");

    if (handler) {
        fputs(content, handler);

        fclose(handler);
        return 1;
    } else {
        printf("File does not exist");
        return -1;
    }
}