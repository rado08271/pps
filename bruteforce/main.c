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

#define INCLUDE_UI 1

int main() {
    uul dicSize = 0;
    size_t memory = 0;

    string* dictionary = NULL;

    const string possibleCharacters = "0123456789";
    string password;
    const int maxLength = 5;
    uul attempts = 0;

    string guessed = NULL;

    if (INCLUDE_UI == 1) {
        password = generateUIForPasswordInput(maxLength, possibleCharacters);
        password = checkPassword(password, possibleCharacters, maxLength);

        if (password != NULL) {
            int answer = generateUIForDictionary();

            if (answer == 1) {
//                dictionary = readDictionary("../simple.lst", &dicSize, &memory);
                dictionary = readDictionary("../easy.lst", &dicSize, &memory);
            } else if (answer == 2) {
                dictionary = readDictionary("../advanced.lst", &dicSize, &memory);
            }

            printf("\nThere are %llu possible solutions for password at max length of %d\n", (uul) getPossibleInvokes(possibleCharacters, maxLength) + dicSize, maxLength);
            answer = generateUiForBruteAttackProceedings();

            if (answer == 1) {
                answer = generateUiForBruteAttack();

                if (answer == 1) {
                    answer = generateUIForDeviceChoosing();

                    cl_device_id device;

                    if (answer == 1) {
                        device = chooseDevice();
                        if (dictionary != NULL) {
                            guessed = bruteForcePasswordGuesserDictionary(password, strlen(password), dictionary, dicSize, memory, &attempts, device);
                        }

                        if (guessed == NULL) {
                            int* variations = getAllVariations(possibleCharacters, strlen(password), device);
                            guessed = bruteForcePasswordGuesser(password, possibleCharacters, strlen(password), &attempts, variations, device);
                        }
                    } else if (answer == 2) {
                        device = chooseBestDevice();
                        if (dictionary != NULL) {
                            guessed = bruteForcePasswordGuesserDictionary(password, strlen(password), dictionary, dicSize, memory, &attempts, device);
                        }

                        if (guessed == NULL) {
//                            int* variations = getAllVariations(possibleCharacters, maxLength, device);
                            int* variations = getAllVariationsSeq(possibleCharacters, strlen(password));
                            guessed = bruteForcePasswordGuesser(password, possibleCharacters, strlen(password), &attempts, variations, device);
                        }
                    }

                    clReleaseDevice(device);
                } else if (answer == 2) {
                    startTimer();
                    guessed = bruteForcePasswordGuesserSeq(password, possibleCharacters, maxLength, dictionary, dicSize, &attempts);
                    stopTimer();
                }
            }
        } else {
            printf("Password is not in correct format! Please check your password\n");
        }

    } else {
        password = "13516";
        startTimer();
        guessed = bruteForcePasswordGuesserSeq(password, possibleCharacters, maxLength, dictionary, dicSize, &attempts);
        stopTimer();
    }

    if (guessed != NULL) {
        printf("\n\n\n=================================================================", guessed, getTimerResult(), attempts);
        printf("\nPassword %s guessed with inc in %ld [ns], with %llu attempts", guessed, getTimerResult(), attempts);
        if (attempts > 1000000000) {
            printf("\nYour password could not be easily found");
        } else {
            printf("\nYour password is very weak");
        }
    } else {
        printf("\nPassword could not be found");
    }

    return 0;
}
