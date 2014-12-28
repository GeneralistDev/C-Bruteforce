all:
	gcc -std=gnu11 -Wall -pedantic main.c -lm -o main

debug:
	gcc -std=gnu11 -ggdb -Wall -pedantic main.c -lm -o main

