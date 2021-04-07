//
// Created by rafig on 7. 4. 2021.
//

#ifndef PPS_BRUTE_FORCE_H
#define PPS_BRUTE_FORCE_H

#include <CL/cl.h>

#include "main.h"

string checkPassword(string password, string possibleCharacters, int maxLength, uul* attempts);
double getPossibleInvokes(string possibleCharacters, int maxLength);
string checkDictionary(string password, string* dictionary, uul dicSize, uul* attempts);
string computeMatrixIntWithKernel(string password, string possibleCharacters, uul maxLength, cl_device_id deviceId);

#endif //PPS_BRUTE_FORCE_H
