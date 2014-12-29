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
char g_charset[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const int charsetLength = 10;

void nthInSequence(int n, size_t sequenceLength, char* sequenceItem, int calcTotal) {
    for (int i = 0; i < sequenceLength; i++) {
        sequenceItem[i] = '0';
        if (n > 0) {
            calcTotal /= 10;
            int value = n / calcTotal;
            n %= calcTotal;
            sequenceItem[i] = '0' + value;
        }
    }
}

/**
* Calculate where in the sequence of generated permutations for a fixed length sequence the
* given sequenceItem occurs. This is used to find out how many sequences are left to generate.
*/
int numberInSequence(size_t sequenceLength, char *sequenceItem, int calcTotal) {
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
    size_t lowLength = strlen(rangeLow);
    size_t highLength = strlen(rangeHigh);

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
        for (size_t currentLength = lowLength + 1; currentLength < highLength; currentLength++) {
            totalInSequence = (int) pow((double) charsetLength, (double) currentLength);
            permutationsToCalculate += totalInSequence;
        }

        return permutationsToCalculate / threads;
    }
}

int iterateIndividualDigit(char **stringPointers, size_t index) {
    if (*stringPointers[index] == '9') {
        stringPointers[index] = &g_charset[0];
        iterateIndividualDigit(stringPointers, index-1);
    } else {
        stringPointers[index] = stringPointers[index] + sizeof(char*);
    }
}
// Returns 1 if it has exhausted all
int iteratePointers(char **stringPointers, size_t *sequenceLength) {
    char *endMark = NULL;
    char *current = NULL;
    endMark = malloc(sizeof(*sequenceLength));
    current = malloc(sizeof(*sequenceLength));

    strncpy(current, *stringPointers, *sequenceLength);

    for (int i = 0; i < *sequenceLength; i++) {
        endMark[i] = '9';
    }


    // If we're at the end return 1
    if (strcmp(current, endMark)) {
        // Zero all the stringPointers
        for (int i = 0; i < *sequenceLength; i++) {
            stringPointers[i] = &g_charset[0];
        }
        free(current);
        free(endMark);
        return 1;
    }

    iterateIndividualDigit(stringPointers, *sequenceLength);

    free(endMark);
    return 0;
}

// Returns 1 if the password fails or 0 if it passes (-1 if there is a fatal error)
int checkPassword(char **stringPointers, size_t *sequenceLength, char* encryptedFilename) {
    char coreCommand[] = "./fdecrypt ";

    size_t coreCommandLength = strlen(coreCommand);
    size_t filenameLength = strlen(encryptedFilename);
    size_t commandLength = coreCommandLength + filenameLength + *sequenceLength + 1;

    char *command = NULL;
    char *password = NULL;

    command = malloc(sizeof(char) * commandLength);
    password = malloc(sizeof(char) * *sequenceLength);

    for (size_t i = 0; i < *sequenceLength; i++) {
        password[i] = *stringPointers[i];
    }

    strncat(command, coreCommand, commandLength);
    strncat(command, encryptedFilename, commandLength);
    strncat(command, password, commandLength);

    int result = system(command);
    free(command);
    free(password);

    return result;
}

// Returns 1 if rangeHigh is greater than the current stringPointers location
int rangeExceeded(char **stringPointers, size_t *sequenceLength, char* rangeHigh) {
    if (*sequenceLength > strlen(rangeHigh)) {
        return 1;
    }
    for (int i = 0; i < *sequenceLength - 1; i++){
        if (*stringPointers[i] > rangeHigh[i]) {
            return 1;
        }
    }
    return 0;
}

void bruteForceRange(char *rangeLow, char *rangeHigh, char* encryptedFilename) {
    size_t currentSequenceLength = strlen(rangeLow);
    size_t numPointers = strlen(rangeHigh);
    char **stringPointers = NULL;

    // Create the string pointers
    stringPointers = malloc(sizeof(char*) * numPointers + 1);

    // Point them all at the rangeLow values in the charset
    for (int i = 0; i < numPointers; i++) {
        stringPointers[i] = &g_charset[(rangeLow[i] - '0')];
    }

    do {
        checkPassword(stringPointers, &currentSequenceLength, encryptedFilename);
        if (iteratePointers(stringPointers, &currentSequenceLength)) {
            currentSequenceLength++;
        }
    } while (!rangeExceeded(stringPointers, &currentSequenceLength, rangeHigh));

    free(stringPointers);
}

void calculateRange(int thread, int permutationsPerThread, char *rangeLow, char* rangeHigh, size_t maxSequenceLength) {
    char *currentRangeLow = NULL;
    currentRangeLow = malloc(sizeof(char) * maxSequenceLength);
    strcpy(currentRangeLow, rangeLow);

    size_t currentSequenceLength = strlen(rangeLow);

    int countDown;
    // Loop until the countdown has finished
    countDown = permutationsPerThread;
    do {
        int totalForSequence = (int) pow((double) charsetLength, (double) currentSequenceLength);
        if (totalForSequence - numberInSequence(currentSequenceLength, rangeLow, totalForSequence) != totalForSequence) {
            //countDown
        }
        if (totalForSequence - countDown > 0) {
            nthInSequence(countDown, currentSequenceLength, rangeHigh, totalForSequence);
        } else {
            currentSequenceLength++;
        }
        countDown - totalForSequence;
    } while (countDown > 0);
    free(currentRangeLow);
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

    int permutationsPerThread = 0;

    // Arg flags
    int c, errflag = 0, rangeMinFlag = 0, rangeMaxFlag = 0, threadFlag = 0;

    extern char *optarg;
    extern int optind, opterr, optopt;

    int *pipeFD = NULL;

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

        permutationsPerThread = divisionOfWork(threads, rangeLow, rangeHigh);
    } else {
        fprintf(stderr, "ERROR: Must specify beginning and end range values\n");
        exit(3);
    }

    childPIDs = malloc(threads * sizeof(pid_t));
    // All the forks share the same pipe to the parent
    pipeFD = malloc((sizeof(int) * 2));
    pipe(pipeFD);

    for (int i = 0; i < threads; i++){
        char info[2];
        info[0] = i + '0';
        info[1] = '\0';
        write(pipeFD[1], info, 2 );

        if ((pID = fork()) == 0 ) {
            printf("I am the child process. pID: %i\n", pID);
            char buf[2];
            read(pipeFD[0], buf, 2);
            buf[1] = '\0';
            printf("%s\n", buf);
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
    free(pipeFD);

    exit(0);
}