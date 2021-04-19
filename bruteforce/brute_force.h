//
// Created by rafig on 7. 4. 2021.
//

#ifndef PPS_BRUTE_FORCE_H
#define PPS_BRUTE_FORCE_H

#include <CL/cl.h>

#include "main.h"

string checkPassword(string password, string possibleCharacters, int maxLength);
string bruteForcePasswordGuesserSeq(string password, string possibleCharacters, int maxLength, string* dictionary, uul dicSize, uul* attempts);
double getPossibleInvokes(string possibleCharacters, int maxLength);
string checkDictionary(string password, string* dictionary, uul dicSize, uul* attempts);
string bruteForcePasswordGuesser(string password, string possibleCharacters, int maxLength, uul* attempts, cl_device_id deviceId);
string bruteForcePasswordGuesserDictionary(string password, int maxLength, string* dictionary, uul dicSize, size_t dicMemory, uul* attempts, cl_device_id deviceId);
void test( const char* password,
           const char* possibleCharacters,
           char* result,
           unsigned long long* attempts,
           int maxLength);

#endif //PPS_BRUTE_FORCE_H
