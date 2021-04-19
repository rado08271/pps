#include <CL/cl.h>
#include <stdbool.h>
#include "stdio.h"

#include "opencl_utils.h"

#define WORK_DIM 2          // WORK_DIM represents how many dimensions should OPEN_CL use for computing and work_sizes

cl_int runOpenClProgram(cl_kernel kernel, cl_command_queue queue, const size_t localWorkSize[1], const size_t globalWorkSize[1]) {
    // global means to specify amount of instances of work-item in kernel source
    // local means to specify how many of work-items should be grouped into one work-group and will spcify how many work groups there are
    // resulting work groups can be specified by work_dim and global work-item instances divided by clusters of these instances into groups
    // result will be in form of ammount of work_groups available in program G[x]/L[x] for any dimension (work_dim)
    cl_int err;
    printf("g[0]: %zu, l[0]: %zu", globalWorkSize[0], localWorkSize[0]);
    printf("\nExecuting specified program using OpenCL\n");
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("\nExecution failed with %d\n", err);
//        return NULL;
    }

    return err;
}

cl_kernel createProgramKernel(cl_program program, string sourceKernelName, cl_device_id deviceId) {
    cl_int err;

    printf("\nCreating and assigning kernel with compiled program\n");
    cl_kernel kernel = clCreateKernel(program, sourceKernelName, &err);

    if (err != CL_SUCCESS) {
        printf("\nKernel creation failed with %d\n", err);
        return NULL;
    }

    printf("\nPrinting kernel info\n");

    size_t size = 0;
    size_t sizeResult[3];
    size_t basicResult;
    cl_ulong longResult;

    size = 0;
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_GLOBAL_WORK_SIZE, 0, NULL, &size);           // get size of info char array
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_GLOBAL_WORK_SIZE, size, &sizeResult, NULL);           // get size of info char array
    printf("Global Work Size: \t\t%zu, %zu, %zu\n", sizeResult[0], sizeResult[1], sizeResult[2]);

    size = 0;
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_WORK_GROUP_SIZE, 0, NULL, &size);           // get size of info char array
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_WORK_GROUP_SIZE, size, &basicResult, NULL);           // get size of info char array
    printf("Work Group Size: \t\t%zu\n", basicResult);

    size = 0;
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, 0, NULL, &size);           // get size of info char array
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, size, &sizeResult, NULL);           // get size of info char array
    printf("Comp. WorkG Size: \t\t%zu, %zu, %zu\n", sizeResult[0], sizeResult[1], sizeResult[2]);

    size = 0;
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_LOCAL_MEM_SIZE, 0, NULL, &size);           // get size of info char array
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_LOCAL_MEM_SIZE, size, &longResult, NULL);           // get size of info char array
    printf("Work Group Size: \t\t%llu\n", longResult);

    size = 0;
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, 0, NULL, &size);           // get size of info char array
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, size, &basicResult, NULL);           // get size of info char array
    printf("Pref WG Size MP: \t\t%zu\n", basicResult);

    size = 0;
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_PRIVATE_MEM_SIZE, 0, NULL, &size);           // get size of info char array
    err = clGetKernelWorkGroupInfo(kernel, deviceId, CL_KERNEL_PRIVATE_MEM_SIZE, size, &longResult, NULL);           // get size of info char array
    printf("Private Mem Size: \t\t%zu\n", longResult);

    return kernel;
}

cl_program createProgram(cl_context context, string sourceCode, cl_device_id device) {
    cl_int err;

    printf("\nTranslating kernel source code to CL Program\n");
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &sourceCode, NULL, &err);

    if (err != CL_SUCCESS) {
        printf("\nCL Program failed with %d\n", err);
        return NULL;
    }

    printf ("\nBuilding CL program (with compiling and linking)\n");
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    printf ("\nPrinting build status\n");

    size_t size = 0;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
    string info = (string) malloc(size);
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, size, info, NULL);
    printf("%s\n", info);

    if (err != CL_SUCCESS) {
        printf("\nCL Program failed with %d\n", err);
        return NULL;
    }

    free(info);
    return program;
}

cl_mem allocateMemory(cl_context context, size_t size, bool readOnly) {
    cl_int err;

    printf ("\nAllocating, %llu bytes of memory\n", size);
    cl_mem allocated_memory = clCreateBuffer(context, readOnly ? CL_MEM_READ_ONLY : CL_MEM_READ_WRITE, size, NULL, &err);

    if (err != CL_SUCCESS) {
        printf("\nMemory allocation failed with %d\n", err);
        return NULL;
    }

    return allocated_memory;
}

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

    err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, 0, NULL, &devices);
    deviceId = (cl_device_id*) malloc(devices);
    printf("==== \n%d available GPU device ====\n", devices);
    err = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, devices, deviceId, NULL);

    if (err != CL_SUCCESS) {
        printf("\nDevices failed with %d\n", err);
        return NULL;
    }

    for (int i = 0; i < devices; i++) {
        cl_ulong memory;
        cl_uint value;
        size_t sizeT;
        cl_device_type deviceType;
        string info;
        cl_bool booleanValue;

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

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_BUILT_IN_KERNELS, 0, NULL, &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_BUILT_IN_KERNELS, size, info, NULL);
        printf("Kernels:\t\t%s\n", info);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE , 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE , size, &memory, NULL);
        printf("Max Memory:\t\t%llu\n", memory);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_COMPUTE_UNITS, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_COMPUTE_UNITS, size, &value, NULL);
        printf("Compute U.:\t\t%d\n", value);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, size, &value, NULL);
        printf("Work Dim.:\t\t%d\n", value);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, size, &sizeT, NULL);
        printf("Work Groups:\t\t%zu\n", sizeT);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, size, &value, NULL);
        printf("Clock Freq.:\t\t%d\n", value);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_ADDRESS_BITS, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_ADDRESS_BITS, size, &value, NULL);
        printf("Address b.:\t\t%d\n", value);

        size = 0;
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_SAMPLERS, 0, NULL, &size);           // get size of info char array
        err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_SAMPLERS, size, &value, NULL);
        printf("Samplers b.:\t\t%d\n", value);

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

        free(info);
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

cl_device_id chooseBestDevice() {
    cl_platform_id* platformId;

    cl_int err;
    cl_uint platforms;
    size_t size;

    clGetPlatformIDs(0, NULL, &platforms);
    platformId = (cl_platform_id*) malloc(platforms);
    err = clGetPlatformIDs(platforms, platformId, NULL);

    cl_uint currentGreatest = 0;
    cl_device_id currentDevice;
    string info;

    for (int x = 0; x < platforms; x++) {
        cl_uint devices;

        cl_device_id* deviceId;
        err = clGetDeviceIDs(platformId[x], CL_DEVICE_TYPE_ALL, 0, NULL, &devices);
        deviceId = (cl_device_id*) malloc(devices);
        err = clGetDeviceIDs(platformId[x], CL_DEVICE_TYPE_ALL, devices, deviceId, NULL);

        printf("\n==========\n");
        for (int i = 0; i < devices; i++) {
            cl_ulong memory;
            cl_uint value;
            size_t sizeT;
            cl_device_type deviceType;
            cl_bool booleanValue;

            cl_uint resultingValue = 0;

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

            size = 0;
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_BUILT_IN_KERNELS, 0, NULL, &size);           // get size of info char array
            info = (string) malloc(size);
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_BUILT_IN_KERNELS, size, info, NULL);
            printf("Kernels:\t\t%s\n", info);

            size = 0;
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE , 0, NULL, &size);           // get size of info char array
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE , size, &memory, NULL);
            printf("Max Memory:\t\t%llu\n", memory);

            size = 0;
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_COMPUTE_UNITS, 0, NULL, &size);           // get size of info char array
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_COMPUTE_UNITS, size, &value, NULL);
            printf("Compute U.:\t\t%d\n", value);
            resultingValue += value;

            size = 0;
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 0, NULL, &size);           // get size of info char array
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, size, &value, NULL);
            printf("Work Dim.:\t\t%d\n", value);
            resultingValue += value;

            size = 0;
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, 0, NULL, &size);           // get size of info char array
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, size, &sizeT, NULL);
            printf("Work Groups:\t\t%zu\n", sizeT);
            resultingValue += (cl_uint) sizeT;

            size = 0;
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, 0, NULL, &size);           // get size of info char array
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, size, &value, NULL);
            printf("Clock Freq.:\t\t%d\n", value);
            resultingValue += value;

            size = 0;
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_ADDRESS_BITS, 0, NULL, &size);           // get size of info char array
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_ADDRESS_BITS, size, &value, NULL);
            printf("Address b.:\t\t%d\n", value);
            resultingValue += value;

            size = 0;
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_SAMPLERS, 0, NULL, &size);           // get size of info char array
            err = clGetDeviceInfo(deviceId[i], CL_DEVICE_MAX_SAMPLERS, size, &value, NULL);
            printf("Samplers b.:\t\t%d\n", value);
            resultingValue += value;

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

            if (resultingValue > currentGreatest) {
                currentGreatest = resultingValue;
                currentDevice = deviceId[i];
            }
        }
    }

    if (currentDevice != NULL) {

        size = 0;
        err = clGetDeviceInfo(currentDevice, CL_DEVICE_NAME, 0, NULL, &size);           // get size of info char array
        info = (string) malloc(size);
        err = clGetDeviceInfo(currentDevice, CL_DEVICE_NAME, size, info, NULL);

        printf("\nWe chose, %s\n", info);
    }

    free(info);

    return currentDevice;
}

cl_device_id chooseDevice() {
    int platformIdx = 1;

    cl_platform_id* platforms = getPlatformInfo();
    scanf("%d", &platformIdx);
    cl_platform_id platform = platforms[platformIdx];
//    cl_platform_id platform = getPlatformInfo()[platformIdx];


    int deviceIdx = 0;

    cl_device_id* devices = getDevicesInfo(platform);
    scanf("%d", &deviceIdx);
    cl_device_id device = devices[deviceIdx];
//    cl_device_id device = getDevicesInfo(platform)[deviceIdx];

    return device;
}

size_t getWorkerGroupSize(uul invokes) {
    for (size_t i = 1 ; ; i*=2) {
        if (invokes < i) {
            printf("\nClosest to %d is %zu", invokes, i);
            return i;
        }
    }

}

string recreateDictionary(string* dictionary, int dicSize, size_t memory) {
    string result = calloc(memory + dicSize, sizeof(char));

    for (int i = 0; i < dicSize; i++) {
        string currentVariation = dictionary[i];

        strcat(result, currentVariation);
        strcat(result, "#");
    }

    return result;
}