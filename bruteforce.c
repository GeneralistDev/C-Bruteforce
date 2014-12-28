#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "math.h"
#include "helptext.h"
#include "sys/types.h"
#include "sys/wait.h"

char g_charset[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

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
        printf("%s", filename);
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
    }

    pid_t pID = fork();

    if (pID >= 0) {
        if (pID == 0) {
            printf("I am the child process\n");
            int charsetlength = 3; // k
            int combolength = 3; // n

            /* Print out all permutations */
            int count = 0;
            for (int indexA = 0; indexA < charsetlength; indexA++) {
                for (int indexB = 0; indexB < charsetlength; indexB++) {
                    for (int indexC = 0; indexC < charsetlength; indexC++) {

                        printf("%i: [%c, %c, %c]\n", count, g_charset[indexA], g_charset[indexB], g_charset[indexC]);
                        count++;
                        for (int indexD = 0; indexD < charsetlength; indexD++) {
                            // 	count++;
                            // 	printf("%i: [%c, %c, %c]\n",count , charset[indexA], charset[indexB], charset[indexC]);
                        }
                    }
                }
            }

            char exampleStart[3] = {'1', '1', '0'};
            int totalInSequence = (int) pow((double) charsetlength, (double) combolength);
            int total = totalInSequence;
            int numberInSequence = 0;

            for (int i = 0; i < combolength; i++) {
                total /= combolength;
                if (total == 1) {
                    numberInSequence += (exampleStart[i] - '0');
                } else {
                    numberInSequence += (exampleStart[i] - '0') * total;
                }
            }
            printf("Permutations to end: %i\n", totalInSequence - numberInSequence);
            _exit(0);
        } else {
            printf("I am the parent process\n");
            int childExitStatus;
            wait(&childExitStatus);
            exit(0);
        }
    } else {
        fprintf(stderr, "Could not fork child");
        return 1;
    }
}