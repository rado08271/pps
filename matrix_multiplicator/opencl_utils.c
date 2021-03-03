#include <CL/cl.h>
#include "stdio.h"

#include "opencl_utils.h"

cl_command_queue createExecutionOrder(cl_context context, cl_device_id device) {
    printf ("\nCreating order of command execution (queue) ...\n");

    cl_int err;

    // not using profiling of commands nor need of OUT-OF-ORDER - makes kernels run not in queue
    cl_command_queue commandQueue = clCreateCommandQueue(context, device, 0, &err);

    if (err != CL_SUCCESS) {
        printf ("\nQueue initialization failed with %d\n", err);
        return NULL;
    }

    return commandQueue;
}

cl_context createContext(cl_device_id device) {
    printf ("\nCreating context ...\n");

    cl_int err;

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

    if (err != CL_SUCCESS) {
        printf ("\nCreating context failed with %d\n", err);
        return NULL;
    }

    cl_uint contextInfo;
    size_t size;

    err = clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, 0, NULL, &size);
    err = clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, 0, NULL, &size);
    printf("\nNumber of memory leaks on context:\t%d (value should be stale/0)\n", contextInfo);

    return context;
}

cl_device_id* getDevicesInfo(cl_platform_id platformId) {
    printf("\nGetting devices info ... \n");

    cl_int err;
    cl_device_id* deviceId;
    cl_uint devices;

    size_t size;
    string info;
    cl_bool booleanValue;

    err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, 0, NULL, &devices);
    deviceId = (cl_device_id*) malloc(devices);
    printf("==== \n%d available GPU device ====\n", devices);
    err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, devices, deviceId, NULL);

    if (err != CL_SUCCESS) {
        printf("\nDevices failed with %d\n", err);
        return NULL;
    }

    for (int i = 0; i < devices; i++) {

        cl_device_type deviceType;
        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_TYPE, 0, NULL, &size);           // get size of info char array
        deviceType = (cl_device_type) malloc(size);
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_TYPE, size, &deviceType, NULL);
        printf("Dev. Type:\t\t%llu\n", deviceType);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_NAME, 0, NULL, &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_NAME, size, info, NULL);
        printf("Dev. Name:\t\t%s\n", info);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_VENDOR, 0, NULL, &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_VENDOR, size, info, NULL);
        printf("Dev. Vendor:\t\t%s\n", info);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_VERSION, 0, NULL, &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_VERSION, size, info, NULL);
        printf("Version:\t\t%s\n", info);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DRIVER_VERSION, 0, NULL, &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetDeviceInfo(deviceId[i], CL_DRIVER_VERSION, size, info, NULL);
        printf("Dev. Driver:\t\t%s\n", info);

        cl_ulong memory;
        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE , 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE , size, &memory, NULL);
        printf("Max Memory:\t\t%llu\n", memory);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_COMPUTE_UNITS, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_COMPUTE_UNITS, size, &booleanValue, NULL);
        printf("Compute:\t\t%u\n", booleanValue);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_AVAILABLE, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_AVAILABLE, size, &booleanValue, NULL);
        printf("Available:\t\t%u\n", booleanValue);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_COMPILER_AVAILABLE, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_COMPILER_AVAILABLE, size, &booleanValue, NULL);
        printf("Compiler:\t\t%u\n", booleanValue);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_IMAGE_SUPPORT, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_IMAGE_SUPPORT, size, &booleanValue, NULL);
        printf("Sup. Images:\t\t%u\n", booleanValue);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_ERROR_CORRECTION_SUPPORT, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_ERROR_CORRECTION_SUPPORT, size, &booleanValue, NULL);
        printf("Cor. Error:\t\t%u\n", booleanValue);

    }

    return deviceId;
}

cl_platform_id* getPlatformInfo() {
    printf("\nGetting platform info ... \n");

    cl_int err;
    cl_platform_id* platformId;
    cl_uint platforms;

    err = clGetPlatformIDs(0, NULL, &platforms);
    platformId = (cl_platform_id*) malloc(platforms);
    printf("Available platforms %d\n", platforms);
    err = clGetPlatformIDs(platforms, platformId, NULL);
    if (err != CL_SUCCESS) {
        printf("Platform failed with %d\n", err);
        return NULL;
    }

    for (int i = 0; i < platforms; i++) {
        printf("\n==== %d. platform info ====\n", i);
        size_t size;
        string info;

        if (err != CL_SUCCESS) {
            printf("GET Platform info failed with %d\n", err);
            return NULL;
        }

        size = 0;
        err = clGetPlatformInfo(platformId[i], CL_PLATFORM_PROFILE, 0, NULL,
                                &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetPlatformInfo(platformId[i], CL_PLATFORM_PROFILE, size, info, NULL);
        printf("Profile:\t\t%s\n", info);

        size = 0;
        err = clGetPlatformInfo(platformId[i], CL_PLATFORM_VERSION, 0, NULL,
                                &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetPlatformInfo(platformId[i], CL_PLATFORM_VERSION, size, info, NULL);
        printf("Version:\t\t%s\n", info);

        size = 0;
        err = clGetPlatformInfo(platformId[i], CL_PLATFORM_NAME, 0, NULL,
                                &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetPlatformInfo(platformId[i], CL_PLATFORM_NAME, size, info, NULL);
        printf("GPU Name:\t\t%s\n", info);

        size = 0;
        err = clGetPlatformInfo(platformId[i], CL_PLATFORM_VENDOR, 0, NULL,
                                &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetPlatformInfo(platformId[i], CL_PLATFORM_VENDOR, size, info, NULL);
        printf("GPU Vendor:\t\t%s\n", info);

        free(info);
    }
    return platformId;

}

cl_device_id chooseDevice() {
    int platformIdx = 1;
    cl_platform_id platform = getPlatformInfo()[platformIdx];

    int deviceIdx = 0;
    return getDevicesInfo(platform)[deviceIdx];
}