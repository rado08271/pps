#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <regex.h>

typedef char* string;

string checkPassword(string password, string possibleCharacters, int maxLength, long* attempts) {
    unsigned long possibleCharactersAmount = strlen(possibleCharacters);

    for (int actualLength = 1; actualLength < maxLength; actualLength++ ) {
        int* indexes = (int*) calloc(actualLength, sizeof (int));

        const double maxAvailable = pow((double) possibleCharactersAmount, (double) actualLength);
        double max = maxAvailable;

        while (max-- > 0) {
            *attempts += 1;
            string variation = (string) calloc(actualLength, sizeof (char));
            for (int i = 0; i < possibleCharactersAmount - (possibleCharactersAmount - actualLength); i++) {
                variation[i] = possibleCharacters[indexes[i]];
            }

            if (strcmp(password, variation) == 0) {
                return variation;
            }

            for (int i = 0; i < possibleCharactersAmount; i++) {
                if (indexes[i] >= possibleCharactersAmount - 1) {
                    indexes[i] = 0;
                } else {
                    indexes[i]++;
                    break;
                }
            }

            free(variation);
        }

        free(indexes);
    }

    return NULL;
}

double getPossibleInvokes(string possibleCharacters, int maxLength) {
    unsigned long possibleCharactersAmount = strlen(possibleCharacters);

    double allSolutions = 0;

    for (int actualLength = 1; actualLength < maxLength; actualLength++ ) {
        allSolutions += pow(possibleCharactersAmount, actualLength);
    }

    return allSolutions;
}

string checkDictionary(string password, string* dictionary, long dicSize, long* attempts) {
    for (int i = 0; i < dicSize; i++) {
        string currentVariation = dictionary[i];

        if (strcmp(currentVariation, password) == 0) {
            *attempts = i;
            return currentVariation;
        }

        free(currentVariation);
    }

    return NULL;
}

string* readDictionary(string path, long* entries) {
    FILE *file = fopen(path, "r");

    size_t bufferLength = 1024;
    char buffer[bufferLength];
    ssize_t lineLength = 0;

    long currentIdx = 0;

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

int main() {
//    printf("Please provide a password use [a-z][A-Z][0-9] at max length 10\n");
    string password = "123456";

    string possibleCharacters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int maxLength = 10;

    bool shouldCheck = true;

    if (shouldCheck) {
        long attempts = 0;
        long dicSize = 0;

        string* dictionary = readDictionary("../dir.lst", &dicSize);

        printf("\nThere are %ld entries in dictionary, that I will check against", dicSize);

        string dicGuessed = checkDictionary(password, dictionary, dicSize, &attempts);

        if (dicGuessed != NULL) {
            printf("\nPassword %s guessed with dictionary in %d ns, with %ld attempts", dicGuessed, 0, (long) attempts);
        } else {
            printf("\nThere are %ld possible solutions for password at max length of %d", (long ) getPossibleInvokes(possibleCharacters, maxLength), maxLength);

            string guessed = checkPassword(password, possibleCharacters, maxLength, &attempts);

            if (guessed != NULL) {
                printf("\nPassword %s guessed with inc in %d ns, with %ld attempts", guessed, 0, (long) attempts + dicSize);
            } else {
                printf("\nPassword could not be found");
            }

        }
    }

    return 0;
}
