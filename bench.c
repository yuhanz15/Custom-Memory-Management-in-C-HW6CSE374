/* bench.c is the benchmarking /test program for mem memory management
   bench.c is implemented as part of HW6, CSE374 22WI
   Yuhan Zhang
   12/16/2022
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "mem.h"

/* print_stats is a helper function that prints performance
   information about the memory system.  Requires a clock_t start
   time, and prints elapsed time plus results from get_mem_stats()
*/
void print_stats(clock_t start);

/* fill_mem is a helper function that takes in an address and the
   size of the memory allocation at that address.  It should fill
   the first 16 bytes (or up to size) with hexedecimal values.*/
void fill_mem(void* ptr, uintptr_t size);

/* Synopsis:   bench (main)
   [ntrials] (10000) getmem + freemem calls
   [pctget] (50) % of calls that are get mem
   [pctlarge] (10) % of calls requesting more memory than lower limit
   [small_limit] (200) largest size in bytes of small block
   [large_limit] (20000) largest size in byes of large block
   [random_seed] (time) initial seed for randn
*/
int main(int argc, char** argv ) {
  // Initialize the parameters
  int NTRIALS;
  int PCTGET;
  int PCTLARGE;
  int SMALL_L;
  int LARGE_L;

  (argc > 1) ? (NTRIALS = atoi(argv[1])) : (NTRIALS = 10000);
  (argc > 2) ? (PCTGET = atoi(argv[2])) : (PCTGET = 50);
  (argc > 3) ? (PCTLARGE = atoi(argv[3])) : (PCTLARGE = 10);
  (argc > 4) ? (SMALL_L = atoi(argv[4])) : (SMALL_L = 200);
  (argc > 5) ? (LARGE_L = atoi(argv[5])) : (LARGE_L = 20000);

  // initialize random number gen.
  (argc > 6) ? srand(atoi(argv[6])) : srand(time(NULL));

  printf("Running bench for %d trials, %d%% getmem calls.\n", NTRIALS, PCTGET);
  
  void* blocks[NTRIALS + 1];  // upperbound block storage
  int len = 0;
  clock_t start = clock();
  for (int i = 1; i <= NTRIALS; i++) {
    //decide get or free
    if (rand() % 100 < PCTGET) {
      // get case
      int get_size;
      if (rand() % 100 < PCTLARGE)
        get_size = SMALL_L + rand() % (LARGE_L - SMALL_L);
      else
        get_size = (rand() % SMALL_L) + 1;
      blocks[len] = getmem(get_size);
      len++;
      //fill_mem(blocks[len], get_size); // would fail if use in program
    } else {
      // free case
      if (len != 0) {
	int index = rand() % len;
	freemem(blocks[index]);
	blocks[index] = blocks[len - 1];
	len--;
      }
    }
    // print out stats regularly
    // if the NTRIALS is greater than or equal to 10
    // avoid floating pointer error
    if (NTRIALS >= 10) {
      if (i % (NTRIALS / 10) == 0) {
       print_stats(start);
      }
    } else {
      // NTRIALS < 10: print out stats every time
      print_stats(start);
    }
  }

  return EXIT_SUCCESS;
}

// Fill the get memory with hex number 0xFE in the first 16 bytes
// If the memory is smaller than 16 bytes, it fills all the memory
// with 0xFE
void fill_mem(void* ptr, uintptr_t size) {
  if (size > 16)
    size = 16;
  unsigned char placer = 0xFE;
  for (int i = 0; i < size; i++) {
    unsigned char* current = (unsigned char*)((uintptr_t)ptr + i);
    *current = placer;
  }
}

// Print the stats about total malloc memory, total free memory,
// and total numebr of free blocks.
void print_stats(clock_t start) {
  uintptr_t total_malloc = 0;
  uintptr_t total_free = 0;
  uintptr_t free_blocks = 0;
  get_mem_stats(&total_malloc, &total_free, &free_blocks);
  clock_t curr = clock();
  double duration = 1000.0 * (curr - start) / (double) CLOCKS_PER_SEC; // in ms
  printf("CPU runtime: %.2f ms.\n", duration);
  printf("Total malloc memory: %lu, ", total_malloc);
  printf("Total free memory: %lu, ", total_free);
  printf("Total free blocks: %lu\n", free_blocks);
}
