C-Bruteforce
============
# Dependencies
- gcc
- cmake

# Build instructions
1. Clone this git repository
2. Navigate to the root of the cloned repository
3. Create a new build directory ```mkdir build```
4. Change to the build directory ```cd build```
5. Run cmake ```cmake ..```
6. Build ```make```

# Running the program
```
Usage:	./bruteforce [OPTIONS] <encrypted filename>

OPTIONS:
	-m	Brute force starting integer (eg. 0000)
	-M	Brute force ending integer (eg. 9999)
	-t	Number of CPU threads to use for brute forcing
	-h	Show this help text
```