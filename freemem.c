/* freemem.c
   implements freemem (free) for memory system
   CSE 374 HW6
   Yuhan Zhang
   12/16/2022
*/

#include <assert.h>
#include <stdio.h>
#include "mem.h"
#include "mem_impl.h"

void combine_two_nodes(freeNode* before);

// Free the memory at the given pointer address.
void freemem(void* p) {
  // check everything before start
  check_heap();
  if (p == NULL)
    return;

  // initialize variables
  freeNode* node = (freeNode*)((uintptr_t)p - NODESIZE);
  totalfree = totalfree + node->size + NODESIZE;
  freeNode* node_end = (freeNode*)((uintptr_t)(node) + node->size +
				   NODESIZE);
  freeNode* curr = freelist;
  freeNode* prev = NULL;
  
  // if freelist has no node in it
  if (curr == NULL) {
    blockfree++;
    freelist = node;
    check_heap();
    return;
  }

  // check if the node should be before the first node
  if (node_end == curr) {
    node->next = freelist->next;
    node->size = node->size + freelist->size + NODESIZE;
    freelist = node;
    check_heap();
    return;
  }

  // normal case: node needs to be inserted between two blocks
  while (curr->next != NULL) {
    // check if node should be inserted between curr and next
    if (curr->next == node_end ||
	((uintptr_t)curr + curr->size + NODESIZE) == (uintptr_t)(node)) {
      blockfree++;
      // combine node with the after node
      if (curr->next == node_end) {
	combine_two_nodes(node);
	blockfree--;
	totalfree += NODESIZE;
	check_heap();
      }
      // combine node with the before node
      if (((uintptr_t)curr + curr->size + NODESIZE) == (uintptr_t)(node)){
	combine_two_nodes(curr);
	blockfree--;
	totalfree += NODESIZE;
	check_heap();
      }
      return;
    }
    // nothing found, continue
    curr = curr->next;
  }

  // default case: node needs to place after the whole list
  
  //curr->next = node;
  //blockfree++;
  // check if the node should be combined with the last node in the list
  if (((uintptr_t)curr + curr->size + NODESIZE) == (uintptr_t)(node)) {
    curr->size = curr->size + node->size + NODESIZE;
    curr->next = NULL;
    blockfree--;
  }
}

// Combine current node with the next node together.
void combine_two_nodes(freeNode* before) {
  freeNode* temp = before->next;
  before->next = before->next->next;
  before->size = before->size + temp->size + NODESIZE;
}
