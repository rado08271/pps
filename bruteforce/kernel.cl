#define MAX_BUFFERS 128

__kernel void bruteForce(
          __global char* password,
          __global char* possibleCharacters,
          __global char* result,
          __global long long* attempts,
          //__global char* log,
          int maxLength) {

    size_t localId = get_global_id(0);           // local
    size_t globalId = get_global_id(1);          // global

    //result[0] = possibleCharacters[id];
    //*attempts += globalId;

    int passwordSize = 0;

    while(password[passwordSize] != '\0') {
        passwordSize++;
    }

    int possibleCharactersSize = 0;

    while(possibleCharacters[possibleCharactersSize] != '\0') {
        possibleCharactersSize++;
    }


    long long logLength = 0;
    long long maxAvailableVariations = 1;
    int exp = maxLength;

    while (exp-- > 0) {
        maxAvailableVariations *= possibleCharactersSize;
    }

   //maxAvailableVariations = 5;
    long long currentAttempt = 0;

    int indexes[MAX_BUFFERS];

    int found = 0;
    while (maxAvailableVariations-- > 0 && found != 1) {
        //*attempts++;

        int lengthOfString = possibleCharactersSize - (possibleCharactersSize - maxLength);
        char variation[MAX_BUFFERS];

        for (int i = 0; i < lengthOfString; i++) {
            variation[i] = possibleCharacters[i];
        }

        for (int i = 0; i < passwordSize; i++) {
            if (i > lengthOfString) {
                found = -1;
            } else if (variation[i] != password[i]){
                found = -1;
            } else {
                result[i] = variation[i];
            }
        }

        if (found == -1) {
            found = 0;
        } else {
            found = 1;
        }

        for (int i = 0; i < possibleCharactersSize; i++) {
            if (indexes[i] >= possibleCharactersSize - 1) {
                indexes[i] = 0;
            } else {
                indexes[i]++;
                break;
            }
        }
    }

    if (found != 1) {
        result[0] = '\0';
    }
    //*attempts = found;

}

__kernel void bruteForceWithDictionary(
          __global char* password,
          __global char* dictionary,
          __global char* result,
          __global int* attempts,
          int dicSize) {

    int passwordSize = 0;

    while(password[passwordSize] != '\0') {
        passwordSize++;
    }
    int found = 0;

    unsigned long long currentWordIndex = 0;
    // TODO: Rewrite this loop to globals!
    for (int idx = 0; idx < dicSize; idx++) {
        int wordSize = 0;
        char word[MAX_BUFFERS];

        while (dictionary[currentWordIndex] != '#' && dictionary[currentWordIndex] != '\0') {
            result[wordSize++] = dictionary[currentWordIndex++];
        }
        *attempts += 1;

        if (idx == 5) {
//            for (int i = 0; i < wordSize; i++) {
//                result[i] = dictionary[(currentWordIndex -1) - wordSize + i];
//            }
//            found = 1;
        }
/*
        *attempts += 1;

        for (int i = 0; i < passwordSize; i ++) {
           if (word != passwordSize) {
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
            break;
        }
*/
    }
/*
    if (found != 1) {
        result[0] = '\0';
    }
*/
}
