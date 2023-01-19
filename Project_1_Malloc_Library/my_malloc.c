#include "my_malloc.h"

#define META_SIZE sizeof(struct chunk)

chunk * first = NULL;




/*
When there is no free block, 
space should be allocated from the OS using sbrk 
and add new block to the end of the struct chunk.
*/
void * allocate_space(size_t size){
  chunk * new_block = sbrk(size + META_SIZE);
  if (first == NULL) {
    first = new_block;
  }
  new_block->size = size;
  new_block->free = 0;
  new_block->next = NULL;
  return (char *)new_block + META_SIZE;
}
