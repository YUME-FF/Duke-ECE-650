#include "my_malloc.h"

#define META_SIZE sizeof(struct chunk)

chunk * first = NULL;
chunk * free_region = NULL;
chunk * free_region_before = NULL;
size_t __SIZE__ = 0;



/*
When there is no free block, 
space should be allocated from the OS using sbrk 
and add new block to the end of the struct chunk.
*/
void * allocate_space(size_t size){
  chunk * new_block = sbrk(size + META_SIZE);
  __SIZE__ += size + META_SIZE; // record the size of data
  if (first == NULL) {
    first = new_block;
  }
  new_block->size = size;
  new_block->free = 0;
  new_block->next = NULL;
  new_block->prev = NULL;
  return (char *)new_block + META_SIZE;
}

/*
When find ptr not at free_region, 
then move ptr to it.
*/
void add_chunk(chunk * ptr){
  if (free_region == NULL)|| (ptr < free_region)) {
    ptr->prev = NULL;
    ptr->next = free_region;
    if (ptr->next != NULL) {
      ptr->next->prev = ptr;
    }
    else {
      free_region_before = ptr;
    }
    free_region = ptr;
  }
  else {
    Metadata * curr = free_region;
    while ((curr->next != NULL) && (ptr > curr->next)) {
      curr = curr->next;
    }
    ptr->prev = curr;
    ptr->next = curr->next;
    curr->next = ptr;
    if (ptr->next != NULL) {
      ptr->next->prev = ptr;
    }
    else {
      free_region_before = ptr;
    }
  }
}
}

/*
Function: split the chunk.
One of First fit's short is it may make a small size to
be set in a big chunk, thus when the remain chunk is large
enough, the chunk should be splitted.
*/
chunk * split_chunk(size_t size, chunk * chk){
  chunk * splitChunk;
  splitChunk = chk + META_SIZE + size;
  splitChunk->size = chk->size - size - META_SIZE;
  splitChunk->free = 1;
  splitChunk->next = NULL;
  splitChunk->prev = NULL;
  return splitChunk;
}
