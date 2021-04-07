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

    string possibleCharacters = "0123456789abcdef";
    // FIXME: It failes for more than 16 on MSI!!!

    int maxLength = 10;
    uul attempts = 0;

    if (INCLUDE_UI == 1) {
        string password = generateUIForPasswordInput();

        // TODO: Check password is OK
        if (password != NULL) {
            printf("\nThere are %llu possible solutions for password at max length of %d\n", (uul) getPossibleInvokes(possibleCharacters, maxLength), maxLength);
            int answer = generateUiForBruteAttackProceedings();

            if (answer == 1) {
                int answer = generateUiForBruteAttack();

                if (answer == 1) {
                    answer = generateUIForDeviceChoosing();

                    if (answer == 1) {
                        computeMatrixIntWithKernel(password, possibleCharacters, maxLength, chooseDevice());
                    } else if (answer == 2) {
                        computeMatrixIntWithKernel(password, possibleCharacters, maxLength, chooseBestDevice());
                    }
                } else if (answer == 2) {
                    startTimer();
                    string guessed = checkPassword(password, possibleCharacters, maxLength, &attempts);
                    stopTimer();

                    if (guessed != NULL) {
                        printf("\nPassword %s guessed with inc in %ld ns, with %llu attempts", guessed, getTimerResult(), attempts);
                    } else {
                        printf("\nPassword could not be found");
                    }
                }
            }
        }

    } else {

//    printf("Please provide a password use [a-z][A-Z][0-9] at max length 10\n");
        string password = "12345";
        bool shouldCheck = true;

        if (shouldCheck) {
//            uul dicSize = 0;
//
//            string* dictionary = readDictionary("../dir.lst", &dicSize);
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
            string guessed = checkPassword(password, possibleCharacters, maxLength, &attempts);
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
