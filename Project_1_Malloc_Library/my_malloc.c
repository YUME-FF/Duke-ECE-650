#include "my_malloc.h"

#include <assert.h>

#define META_SIZE sizeof(chunk)

chunk * free_region = NULL;         //end
chunk * free_region_before = NULL;  //start

/*
Function: Request for space with size of 'size'.

When there is no free block, 
space should be allocated from the OS (request for space) 
using sbrk and add new block to the end of the struct chunk.
*/
void * allocate_space(chunk * first, size_t size) {
  chunk * new;
  new = sbrk(0);
  void * request = sbrk(size + META_SIZE);
  assert((void *)new == request);
  if (request == (void *)-1) {
    return NULL;  // sbrk failed.
  }
  if (first == NULL) {
    first = new;
  }
  new->size = size;
  new->free = 0;
  new->next = NULL;
  new->prev = NULL;
  return (char *)new + META_SIZE;
}

/*
Function:checking Find a free chunk and return it is straightforward. 
Iterate through linked list to see if there's a large enough free chunk.
If large enough, split it
*/
chunk * find_free_chunk(chunk ** ptr, size_t size) {
  chunk * curr = free_region;
  while (curr) {
    if (curr->size >= size + META_SIZE) {  ////find space that large enough
      chunk * split = split_chunk(size, curr);
      extend_chunk(split);
      curr->size = size;
      curr->free = 0;
      curr->next = NULL;
      curr->prev = NULL;
      *ptr = curr;
    }
    curr = curr->next;
  }
  return curr;
}

/*
Function: extend free memory region in chunk
*/
void extend_chunk(chunk * ptr) {
  if (!free_region || (ptr < free_region)) {
    ptr->prev = NULL;
    ptr->next = free_region;
    if (ptr->next) {
      ptr->next->prev = ptr;
    }
    else {
      free_region_before = ptr;
    }
    free_region = ptr;
  }
  else {
    chunk * curr = free_region;
    //find until ptr < curr->next
    while (curr->next && (ptr > curr->next)) {
      curr = curr->next;
    }

    //curr -> ptr -> curr->next
    ptr->prev = curr;
    ptr->next = curr->next;
    curr->next = ptr;

    if (ptr->next) {  //ptr is the last
      ptr->next->prev = ptr;
    }
    else {
      free_region_before = ptr;
    }
  }
}

/*
Function: split the chunk.
One of First fit's short is that it may make a small size to
be set in a big chunk, thus when the remain chunk is large
enough, the chunk should be splitted.
*/
chunk * split_chunk(size_t size, chunk * chk) {
  chunk * splitChunk;
  splitChunk = (chunk *)((char *)chk + META_SIZE + size);
  splitChunk->size = chk->size - size - META_SIZE;
  splitChunk->free = 1;
  splitChunk->next = NULL;
  splitChunk->prev = NULL;
  return splitChunk;
}

//First Fit malloc/free
void * ff_malloc(size_t size) {
  chunk * _chunk;
  if (size <= 0) {
    return NULL;
  }
  if (!free_region) {
    _chunk = allocate_space(NULL, size);
    if (!_chunk) {
      return NULL;
    }
  }
  else {
    chunk * ptr = free_region;
    _chunk = find_free_chunk(&ptr, size);
    if (!_chunk) {  //there is no space large enough
      _chunk = allocate_space(free_region, size);
      if (!_chunk) {
        return NULL;
      }
    }
    else {
      _chunk = ptr + META_SIZE;
    }
  }
  return _chunk;
}

void ff_free(void * ptr) {
  if (!ptr) {  //call free with a NULL ptr
    return;
  }
  chunk * pointer;
  pointer = (chunk *)((char *)ptr - META_SIZE);
  assert(pointer->free == 0);
  pointer->free = 1;
  extend_chunk(pointer);

  if (pointer->next &&
      ((char *)pointer + pointer->size + META_SIZE == (char *)pointer->next)) {
    pointer->size += META_SIZE + pointer->next->size;
    pointer->next->next = NULL;
    pointer->next->prev = NULL;
  }
  if (pointer->prev &&
      ((char *)pointer->prev + pointer->prev->size + META_SIZE == (char *)pointer)) {
    pointer->prev->size += META_SIZE + pointer->size;
    pointer->next = NULL;
    pointer->prev = NULL;
  }
}
