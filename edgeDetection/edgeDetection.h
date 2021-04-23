//
// Created by rafig on 21. 4. 2021.
//

#ifndef EDGEDETECTION_EDGEDETECTION_H
#define EDGEDETECTION_EDGEDETECTION_H

#include "CL/cl.h"
#include "sobel.h"


int* edgeDetection( int* inArray,  int* sharpedArray, int* outArray, int widthIn, int heightIn, cl_device_id deviceId);
int* sharpenImage( int* inArray,  int* sharpedArray, int* outArray, int widthIn, int heightIn, cl_device_id deviceId);
int* runImageSharpening( int* inArray,  int* sharpedArray, int* outArray, int widthIn, int heightIn);

#endif //EDGEDETECTION_EDGEDETECTION_H
