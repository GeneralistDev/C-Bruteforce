#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "math.h"

int main(int argc, char* argv[]) {
	int pID = fork();

	if (pID >= 0) {
		if (pID == 0) {
			printf("I am the child process\n");
			char charset[3] = {'0','1','2'};
			int charsetlength = 3; // k
			int combolength = 3; // n

			/* Print out all permutations */
			int count = 0;
			for (int indexA = 0; indexA < charsetlength; indexA++) {
				for (int indexB = 0; indexB < charsetlength; indexB++) {
					for (int indexC = 0; indexC < charsetlength; indexC++){
						
						printf("%i: [%c, %c, %c]\n",count , charset[indexA], charset[indexB], charset[indexC]);
						count++;
						for (int indexD = 0; indexD < charsetlength; indexD++){
						// 	count++;
						// 	printf("%i: [%c, %c, %c]\n",count , charset[indexA], charset[indexB], charset[indexC]);
						}
					}
				}
			}

			char exampleStart[3] = {'1','1','0'};
			int totalInSequence = (int)pow((double)charsetlength, (double)combolength);
			int total = totalInSequence;
			int numberInSequence = 0;
			
			for(int i = 0; i < combolength; i++) {
				total /= combolength;
				if (total == 1) {
					numberInSequence += (exampleStart[i] - '0');
				} else {
					numberInSequence += (exampleStart[i] - '0') * total;
				}
			}
			printf("Permutations to end: %i\n", totalInSequence - numberInSequence);
		} else {
			printf("I am the parent process\n");
		}
	} else {
		fprintf(stderr, "Could not fork child");
		return 1;
	}
	return 0;
}