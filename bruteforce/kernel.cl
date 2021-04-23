#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
#define MAX_BUFFERS 128
#define CHARACTERS_SUM 10
#define GUESSED_LENGTH 5
#define MAX_SIZE_VARIATIONS 100000
#define MAX_VARIATIONS ((int) pow((double) CHARACTERS_SUM, (double) GUESSED_LENGTH))
#define SIZE 3
#define WORK_THREADS 16

__kernel void getAllVariations(
            __global int* variations,
            int charactersLength,
            int currentLength) {

    size_t maxId = pow((double)charactersLength, (double)currentLength) * currentLength;
    size_t currentRow = get_global_id(0);

    if (currentRow > 0 && (double) currentRow < maxId) {
        for (int i = 0; i < currentLength; i++) {
            size_t previousColId = ((currentRow - 1) * currentLength) + i;
            size_t currentColId = ((currentRow * currentLength) + i);
            if (variations[previousColId] >= charactersLength -1) {
                barrier(CLK_GLOBAL_MEM_FENCE);
                variations[currentColId] = 0;
                barrier(CLK_GLOBAL_MEM_FENCE);
            } else {
                for (int x = currentColId; x < currentColId + currentLength; x++) {
                    size_t previousColIndex = x - currentLength;
                    size_t currentColIndex = x;
                    variations[currentColIndex] = variations[previousColIndex];
                    barrier(CLK_GLOBAL_MEM_FENCE);
                }
                variations[currentColId] = variations[previousColId] + 1;
                barrier(CLK_GLOBAL_MEM_FENCE);
                break;
            }
        }
    }
}

__kernel void bruteForceVariations(
            __global char* password,
            __global char* possibleCharacters,
            __global int* variations,
            __global char* result,
            __global unsigned long long* attempts,
            __global int* stop,
            int currentLength,
            int passwordLength,
            int possibleCharactersLength){

    const int currentRow = WORK_THREADS * get_group_id(0) + get_local_id(0);

    size_t possibleVariations = pow((double)possibleCharactersLength, (double)currentLength) * currentLength;

    int found = 0;
    if (*stop == 0 && currentRow < possibleVariations) {
        *attempts += 1;

        // iterate password;
        for (int col = 0; col < currentLength; col++) {
            if (currentLength < passwordLength) {
                found = -1;
                break;
            }

            if (possibleCharacters[variations[(currentRow * currentLength) + col]] == password[col]) {
                found = 1;
            } else {
                found = -1;
                break;
            }
        }

        // password found for current id
        if (found == 1) {
            for (int col = 0; col < currentLength; col++) {
                result[col] = possibleCharacters[variations[(currentRow * currentLength) + col]];
                *stop = 1;
            }
        }
    }
}

__kernel void bruteForceWithDictionary(__global char* password, __global char* dictionary, __global char* result, __global int* attempts, int dicSize) {
    int passwordSize = 0;

    while(password[passwordSize] != '\0') {
        passwordSize++;
    }
    int found = 0;
    unsigned long long currentWordIndex = 0;
    for (int idx = 0; idx < dicSize; idx++) {
        int wordSize = 0;
        char word[128];

        if (dictionary[currentWordIndex] == '|') {
            currentWordIndex++;
            *attempts += 1;
        }

        while (dictionary[currentWordIndex] != '|') {
            word[wordSize++] = dictionary[currentWordIndex++];
        }

        for (int i = 0; i < wordSize && found == 0; i++) {
            if (wordSize != passwordSize) {
                found = -1;
            } else if (word[i] != password[i]) {
                found = -1;
            } else {
                result[i] = word[i];
            }
        }

        if (found == -1) {
            found = 0;
        } else {
            found = 1;
            result[wordSize+1] = '\0';
            break;
        }
    }


    if (found != 1) {
        result[0] = '\0';
    }
}
