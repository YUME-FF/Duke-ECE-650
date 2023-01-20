#include "my_malloc.h"

#include <assert.h>

#define META_SIZE sizeof(chunk)

chunk * free_region = NULL;         //end
chunk * free_region_before = NULL;  //start
chunk * chk = NULL;
size_t data_size = 0;
size_t free_size = 0;

/*
Function: Request for space with size of 'size'.

When there is no free block, 
space should be allocated from the OS (request for space) 
using sbrk and add new block to the end of the struct chunk.
*/
void * allocate_space(size_t size) {
  data_size += size + META_SIZE;
  chunk * new = sbrk(size + META_SIZE);
  /*
  chunk * new;
  new = sbrk(0);
  void * request = sbrk(size + META_SIZE);
  assert((void *)new == request);
  if (request == (void *)-1) {
    return NULL;  // sbrk failed.
    }*/
  if (!chk) {
    chk = new;
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
void * find_free_chunk(size_t size) {
  chunk * ptr = free_region;
  while (ptr) {
    if (ptr->size >= size) {
      if (ptr->size > size + META_SIZE) {  ////find space that large enough
        chunk * split = split_chunk(size, ptr);
        /*chunk * splitChunk;
        splitChunk = (chunk *)((char *)ptr + META_SIZE + size);
        splitChunk->size = ptr->size - size - META_SIZE;
        splitChunk->free = 1;
        splitChunk->next = NULL;
        splitChunk->prev = NULL;*/
        remove_chunk(ptr);
        extend_chunk(split);
        ptr->size = size;
        free_size -= size + META_SIZE;
      }
      else {
        remove_chunk(ptr);
        free_size -= ptr->size + META_SIZE;
      }
      ptr->free = 0;
      ptr->next = NULL;
      ptr->prev = NULL;
      return (char *)ptr + META_SIZE;
    }
    ptr = ptr->next;
  }
  return NULL;
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
Function: remove targeted chunk
*/

void remove_chunk(chunk * ptr) {
  if (free_region_before == ptr) {
    if (free_region == ptr) {
      free_region = NULL;
      free_region_before = NULL;
    }
    else {
      free_region_before = ptr->prev;
      free_region_before->next = NULL;
    }
  }
  else if (free_region == ptr) {
    free_region = ptr->next;
    free_region->prev = NULL;
  }
  else {
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;
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
    _chunk = allocate_space(size);
    if (!_chunk) {
      return NULL;
    }
  }
  else {
    _chunk = find_free_chunk(size);
    if (!_chunk) {  //there is no space large enough
      _chunk = allocate_space(size);
      if (!_chunk) {
        return NULL;
      }
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
  free_size += pointer->size + META_SIZE;
  extend_chunk(pointer);

  //merge chunk
  if (pointer->next &&
      ((char *)pointer + pointer->size + META_SIZE == (char *)pointer->next)) {
    pointer->size += META_SIZE + pointer->next->size;
    //pointer->next->next = NULL;
    //pointer->next->prev = NULL;
    remove_chunk(pointer->next);
  }
  if (pointer->prev &&
      ((char *)pointer->prev + pointer->prev->size + META_SIZE == (char *)pointer)) {
    pointer->prev->size += META_SIZE + pointer->size;
    //pointer->next = NULL;
    //pointer->prev = NULL;
    remove_chunk(pointer);
  }
}

unsigned long get_data_segment_size() {
  return data_size;
}

unsigned long get_data_segment_free_space_size() {
  return free_size;
}
