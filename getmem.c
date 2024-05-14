/* getmem.c
   implements getmem (malloc) for memory system
   CSE 374 HW6
   Yuhan Zhang
   12/16/2022
*/

#include <assert.h>
#include "mem.h"
#include "mem_impl.h"

// initialize global variables
freeNode* freelist = NULL;
uintptr_t totalmalloc = 0;
uintptr_t totalfree = 0;
uintptr_t blockfree = 0;

freeNode* add_new_mem(uintptr_t size);
void split_block(freeNode* n, uintptr_t size);

// Get memory and return a block with the certain size
void* getmem(uintptr_t size) {
  // check everything before start
  if (size <= 0)
    return NULL;
  if (size <= MINCHUNK)
    size = MINCHUNK;

  // initialize freelist, just do once
  if (!freelist){
    freelist = add_new_mem(BIGCHUNK);
    freelist->next = NULL;
    freelist->size = BIGCHUNK - NODESIZE;
  }
  // check freelist - it shouldn't be null at this point
  check_heap();
  assert(freelist != NULL);

  size += NODESIZE; // save enough place for offset
  if (size % MINCHUNK != 0)
    size = size - size % MINCHUNK + MINCHUNK;

  freeNode* result;
  // check if the first node has enough space.
  if (freelist->size >= size) {
    // if too large
    if (freelist->size >= size + MINCHUNK)
      split_block(freelist, size);
    result = freelist;
    freelist = freelist->next;
    check_heap();
    blockfree -= 1;
    totalfree -= size;
    return (void*)((uintptr_t)result + NODESIZE);
  }

  // normal case: travel the whole list and find a suitable block
  freeNode* current = freelist;
  while (current->next != NULL) {
    if (current->next->size >= size) {
      if (current->next->size >= size + MINCHUNK)
	split_block(current->next, size);
      result = current->next;
      current->next = current->next->next;
      totalfree -= size;
      blockfree -= 1;
      check_heap();
      return (void*)((uintptr_t)result + NODESIZE);
    } else {
      // nothing found, continue
      current = current->next;
    }
  }

  // default: no block with large enough memory found
  // request new memory block
  current->next = add_new_mem(size);
  if (current->next->size >= size + MINCHUNK)
    split_block(current->next, size);
  result = current->next;
  current->next = current->next->next;
  blockfree -= 1;
  totalfree -= size;

  check_heap();
  return (void*)((uintptr_t)result + NODESIZE);
}

// Request a new block of memory and return it with a freeNode pointer
freeNode* add_new_mem(uintptr_t size) {
  freeNode* result;
  if (size <= BIGCHUNK) {
    result = malloc(BIGCHUNK + NODESIZE);
    result->size = BIGCHUNK;
    totalmalloc = totalmalloc + BIGCHUNK + NODESIZE;
    totalfree = totalfree + BIGCHUNK + NODESIZE;
  } else {
    result = malloc(size + NODESIZE);
    result->size = size;
    totalmalloc = totalmalloc + size + NODESIZE;
    totalfree = totalfree + size + NODESIZE;
  }
  blockfree += 1;
  result->next = NULL;
  return result;
}

// Split a large block into two block, and the first one with suitable size
void split_block(freeNode* curr, uintptr_t size) {
  freeNode* newNode = (freeNode*)((uintptr_t)(curr) + size + NODESIZE);
  newNode->size = curr->size - size - NODESIZE;
  newNode->next = curr->next;
  curr->size = size;
  blockfree += 1;
}

