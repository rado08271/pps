#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "main.h"
#include "brute_force.h"
#include "file_reader.h"
#include "simple_timer.h"
#include "ui.h"
#include "opencl_utils.h"

#define INCLUDE_UI 2

int main() {
    uul dicSize = 0;
    size_t memory = 0;

    string* dictionary = NULL;

    const string possibleCharacters = "abcdef";
    string password = "abc123";
    const int maxLength = strlen(password);
    uul attempts = 0;

    if (INCLUDE_UI == 1) {
        password = generateUIForPasswordInput(maxLength, possibleCharacters);
        password = checkPassword(password, possibleCharacters, maxLength);

        // TODO: Check password is OK
        if (password != NULL) {
            int answer = generateUIForDictionary();

            if (answer == 1) {
                dictionary = readDictionary("../simple.lst", &dicSize, &memory);
            } else if (answer == 2) {
                dictionary = readDictionary("../advanced.lst", &dicSize, &memory);
            }

            printf("\nThere are %llu possible solutions for password at max length of %d\n", (uul) getPossibleInvokes(possibleCharacters, maxLength) + dicSize, maxLength);
            answer = generateUiForBruteAttackProceedings();

            if (answer == 1) {
                answer = generateUiForBruteAttack();

                if (answer == 1) {
                    answer = generateUIForDeviceChoosing();

                    if (answer == 1) {
                        if (dictionary != NULL) {
                            bruteForcePasswordGuesserDictionary(password, maxLength, dictionary, dicSize, memory, &attempts, chooseDevice());
                        } else {
                            bruteForcePasswordGuesser(password, possibleCharacters, maxLength, &attempts, chooseDevice());
                        }
                    } else if (answer == 2) {
                        if (dictionary != NULL) {
                            bruteForcePasswordGuesserDictionary(password, maxLength, dictionary, dicSize, memory, &attempts, chooseBestDevice());
                        } else {
                            bruteForcePasswordGuesser(password, possibleCharacters, maxLength, &attempts, chooseBestDevice());
                        }
                    }
                } else if (answer == 2) {
                    startTimer();
                    string guessed = bruteForcePasswordGuesserSeq(password, possibleCharacters, maxLength, dictionary, dicSize, &attempts);
                    stopTimer();

                    if (guessed != NULL) {
                        printf("\nPassword %s guessed with inc in %ld ns, with %llu attempts", guessed, getTimerResult(), attempts);
                    } else {
                        printf("\nPassword could not be found");
                    }
                }
            }
        } else {
            printf("Password is not correct! Please check your password\n");
        }

    } else if (INCLUDE_UI == 3){
        printf("\npow %.2f", pow(strlen(possibleCharacters), maxLength));
        printf("\nnomrla %llu", strlen(possibleCharacters)^maxLength);

        unsigned long long maxAvailableCharacters = 1;
        int exp = maxLength;

        while (exp > 0) {
            maxAvailableCharacters *= strlen(possibleCharacters);
            printf("\n%llu\t\t%llu", maxAvailableCharacters, strlen(possibleCharacters));
            exp--;
        }
        attempts = maxAvailableCharacters;
        printf("\n\nwith %llu\n\n", attempts);
    } else if (INCLUDE_UI == 2){
        dictionary = readDictionary("../easy.lst", &dicSize, &memory);
        printf("\nmemory: %zu\n", memory);

//        string result = bruteForcePasswordGuesser(password, possibleCharacters, maxLength, dictionary, dicSize, &attempts, chooseBestDevice());
        string result = bruteForcePasswordGuesserDictionary(password, maxLength, dictionary, dicSize, memory, &attempts, chooseBestDevice());
//        string result = calloc(maxLength, sizeof(char));
//        test(password, possibleCharacters, result, &attempts, maxLength);

        printf("\n\nwith %llu and String is %s\n\n", attempts, result);
    } else {

//    printf("Please provide a password use [a-z][A-Z][0-9] at max length 10\n");
        string password = "12345";
        bool shouldCheck = true;

        if (shouldCheck) {
//            uul dicSize = 0;
//
//            string* dictionary = readDictionary("../simple.lst", &dicSize);
//
//            printf("\nThere are %llu entries in dictionary, that I will check against", dicSize);
//
//            string dicGuessed = checkDictionary(password, dictionary, dicSize, &attempts);
//
//            if (dicGuessed != NULL) {
//                printf("\nPassword %s guessed with dictionary in %d ns, with %ld attempts", dicGuessed, 0, (long) attempts);
//            } else {
            printf("\nThere are %llu possible solutions for password at max length of %d\n", (uul) getPossibleInvokes(possibleCharacters, maxLength), maxLength);

            startTimer();
            string guessed = bruteForcePasswordGuesserSeq(password, possibleCharacters, maxLength, dictionary, dicSize, &attempts);
            stopTimer();

            if (guessed != NULL) {
                printf("\nPassword %s guessed with inc in %ld ns, with %llu attempts", guessed, getTimerResult(), attempts);
            } else {
                printf("\nPassword could not be found");
            }

//            }
        }
    }

    return 0;
}
