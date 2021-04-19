#ifndef MATRIX_MULTIPLICATOR_FILE_READER_H
#define MATRIX_MULTIPLICATOR_FILE_READER_H

#include "main.h"

string* readDictionary(string path, uul* entries, size_t* memory);
char* readFile(string path);
int printToFile(string filePath, string content);

#endif //MATRIX_MULTIPLICATOR_FILE_READER_H
