//
// Created by rafig on 7. 4. 2021.
//

#ifndef PPS_BRUTE_FORCE_H
#define PPS_BRUTE_FORCE_H

#include <CL/cl.h>

#include "main.h"
#define MAX_VARIATIONS ((int) pow((double) 4, (double) 3))

string checkPassword(string password, string possibleCharacters, int maxLength);
string bruteForcePasswordGuesserSeq(string password, string possibleCharacters, int maxLength, string* dictionary, uul dicSize, uul* attempts);
double getPossibleInvokes(string possibleCharacters, int maxLength);
string bruteForcePasswordGuesserDictionary(string password, int maxLength, string* dictionary, uul dicSize, size_t dicMemory, uul* attempts, cl_device_id deviceId);
string bruteForcePasswordGuesser(string password, string possibleCharacters, int currentLength, uul* attempts, int* variations, cl_device_id deviceId);
int* getAllVariations(string possibleCharacters, int currentLength, cl_device_id deviceId);
int* getAllVariationsSeq(string possibleCharacters, int currentLength);

#endif //PPS_BRUTE_FORCE_H
