#include "helptext.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "unistd.h"

/**
* @Author       Daniel Parker
* @Version      1.0
* @Date         29-12-14
*/

// The set of characters that make up the password (global variable)
const char g_charset[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const int charsetLength = 10;

/**
* Calculate where in the sequence of generated permutations for a fixed length sequence the
* given sequenceItem occurs. This is used to find out how many sequences are left to generate.
*/
int numberInSequence(int sequenceLength, char *sequenceItem, int calcTotal) {
    int numberInSequence = 0;

    for (int i = 0; i < sequenceLength; i++) {
        calcTotal /= charsetLength;
        if (calcTotal == 1) {
            numberInSequence += (sequenceItem[i] - '0');
        } else {
            numberInSequence += (sequenceItem[i] - '0') * calcTotal;
        }
    }
    return numberInSequence;
}

/**
* Returns the number of sequences that EACH thread should calculate to
* have an even load over the number of threads requested.
*/
int divisionOfWork(int threads, char *rangeLow, char *rangeHigh) {
    int lowLength = strlen(rangeLow);
    int highLength = strlen(rangeHigh);

    int totalInSequence;
    int permutationsToCalculate = 0;

    if (lowLength == highLength) {
        // For a range within the same sequence length

        printf("low and high same\n");

        // The total number of permutations in the sequence
        totalInSequence = (int) pow((double) charsetLength, (double) lowLength);

        int rangeLowIndex = numberInSequence(lowLength, rangeLow, totalInSequence);
        int rangeHighIndex = numberInSequence(highLength, rangeHigh, totalInSequence);

        permutationsToCalculate = rangeHighIndex + 1 - rangeLowIndex;

        return permutationsToCalculate / threads;
    } else {
        // For a range that spans different sequence lengths

        printf("low and high different\n");

        // First calculate how many in the lowest sequence block
        totalInSequence = (int) pow((double) charsetLength, (double) lowLength);
        int rangeLowIndex = numberInSequence(lowLength, rangeLow, totalInSequence);

        permutationsToCalculate += totalInSequence - rangeLowIndex;

        // then calculate how many are in the topmost sequence block
        totalInSequence = (int) pow((double) charsetLength, (double) highLength);
        int rangeHighIndex = numberInSequence(highLength, rangeHigh, totalInSequence);

        permutationsToCalculate += rangeHighIndex + 1;

        // then add the remaining sequences in between if there are any
        for (int currentLength = lowLength + 1; currentLength < highLength; currentLength++) {
            totalInSequence = (int) pow((double) charsetLength, (double) currentLength);
            permutationsToCalculate += totalInSequence;
        }

        return permutationsToCalculate / threads;
    }
}

void openFile(char *filename) {

}

/**
*   Argument parsing and bruteforce bootstrapping
*/
int main(int argc, char *argv[]) {
    // Program variables
    int threads = 1;
    char *rangeLow;
    char *rangeHigh;
    char *filename;

    // Arg flags
    int c, errflag = 0, rangeMinFlag = 0, rangeMaxFlag = 0, threadFlag = 0;

    extern char *optarg;
    extern int optind, opterr, optopt;

    pid_t *childPIDs = NULL;
    pid_t pID;

    // Parse command line arguments
    while ((c = getopt(argc, argv, ":m:M:t:h")) != -1) {
        switch (c) {
            case 'h':
                printf(HELP_TEXT);
                return 0;

            case 't':
                if (threadFlag) {
                    errflag++;
                } else {
                    threadFlag++;
                    threads = atoi(optarg);
                }
                break;

            case 'm':
                rangeMinFlag++;
                rangeLow = optarg;
                break;

            case 'M':
                rangeMaxFlag++;
                rangeHigh = optarg;
                break;

            case ':':
                fprintf(stderr, "Option -%c requires an argument\n", optopt);
                errflag++;
                break;

            case '?':
                fprintf(stderr, "Unrecognised option: -%c\n", optopt);
                errflag++;
                break;

            default:
                fprintf(stderr, HELP_TEXT);
                return 1;
        }
        if (errflag) {
            fprintf(stderr, HELP_TEXT);
            exit(2);
        }
    }

    if (optind < argc) {
        filename = argv[optind++];
        printf("%s\n", filename);
    } else {
        fprintf(stderr, "ERROR: Filename operand missing\n");
        fprintf(stderr, HELP_TEXT);
        exit(3);
    }

    if (threadFlag) {
        printf("Threads: %i\n", threads);
    }

    if (rangeMinFlag && rangeMaxFlag) {
        printf("Range high: %s\n", rangeHigh);
        printf("Range low: %s\n", rangeLow);

        printf("Suggested permutations per thread: %i\n", divisionOfWork(threads, rangeLow, rangeHigh));
    }

    childPIDs = malloc(threads * sizeof(pid_t));

    for (int i = 0; i < threads; i++){
        if ((pID = fork()) == 0 ) {
            printf("I am the child process. pID: %i\n", pID);
            exit(0);
        } if (pID > 0) {
            childPIDs[i] = pID;
            // Do parent work.
        } else {
            fprintf(stderr, "Could not fork child");
            return 1;
        }
    }

    printf("I am the parent process. pID: %i\n", pID);

    int stillWaiting;

    do {
        stillWaiting = 0;
        for (int i = 0; i < threads; ++i) {
            if (childPIDs[i] > 0) {
                if (waitpid(childPIDs[i], NULL, WNOHANG) != 0) {
                    childPIDs[i] = 0;
                }
                else {
                    stillWaiting = 1;
                }
            }
            sleep(0);
        }
    } while (stillWaiting);

    free(childPIDs);
    exit(0);
}