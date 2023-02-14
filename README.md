# mem_hog2

memory hogging application

## description
The application measures performance based on memory access pattern (sequential or random) for a given memory size (byte argument).

Random function's overhead is eliminated by generating random indexes in advance using gen_rand.py script.

To use the application, execute mem_acc with arguments indicating the total memory size, number of memory access iterations, infinite memory access (optional), and ratio of sequential and random access.

Note that the random index file and mem_acc binary should be in the same folder.

## examples
1. Generate 10,000,000 random indexes that don't exceed 100MB/sizeof(int) using the gen_rand.py script:
``` ./gen_rand.py 100mb 10000000 ```

2. Measure the performance of memory access with a total memory size of 100MB, 1000 iterations, no infinite access, and a ratio of 0.5 (equal ratio of sequential and random access):
``` ./mem_acc 104857600 1000 0 0.5 ```

This will output the running time for each memory access pattern (sequential and random).
