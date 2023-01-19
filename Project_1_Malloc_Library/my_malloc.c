#include "my_malloc.h"

#define META_SIZE sizeof(chunk)

chunk * first = NULL;
chunk * free_region = NULL;
chunk * free_region_before = NULL;
size_t __SIZE__ = 0;

/*
When there is no free block, 
space should be allocated from the OS using sbrk 
and add new block to the end of the struct chunk.
*/
void * allocate_space(size_t size) {
  chunk * new_block = sbrk(size + META_SIZE);
  __SIZE__ += size + META_SIZE;  // record the size of data
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
Function: extend free memory region in chunk
*/
void extend_chunk(chunk * ptr) {
  if ((free_region == NULL) || (ptr < free_region)) {
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
    chunk * curr = free_region;
    //find until ptr < curr->next
    while ((curr->next != NULL) && (ptr > curr->next)) {
      curr = curr->next;
    }

    //curr -> ptr -> curr->next
    ptr->prev = curr;
    ptr->next = curr->next;
    curr->next = ptr;

    if (ptr->next != NULL) {  //ptr is the last
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
One of First fit's short is it may make a small size to
be set in a big chunk, thus when the remain chunk is large
enough, the chunk should be splitted.
*/
chunk * split_chunk(size_t size, chunk * chk) {
  chunk * splitChunk;
  splitChunk = chk + META_SIZE + size;
  splitChunk->size = chk->size - size - META_SIZE;
  splitChunk->free = 1;
  splitChunk->next = NULL;
  splitChunk->prev = NULL;
  return splitChunk;
}

//First Fit malloc/free
void * ff_malloc(size_t size) {
  if (free_region != NULL) {
    chunk * ptr = free_region;
    while (ptr != NULL) {
      if (ptr->size >= size) {  //Start first fit
        if (ptr->size >= size + META_SIZE) {
          chunk * split = split_chunk(size, ptr);
          remove_chunk(ptr);
          extend_chunk(split);
          ptr->size = size;
          //free_size -= (size + sizeof(Metadata));
        }
        else {
          remove_chunk(ptr);
          //free_size -= (p->size + sizeof(Metadata));
        }
        ptr->free = 0;
        ptr->next = NULL;
        ptr->prev = NULL;
        return (char *)ptr + META_SIZE;
      }
      ptr = ptr->next;
    }
  }
  return allocate_space(size);
}

void ff_free(void * ptr) {
  chunk * pointer;
  pointer = (chunk *)((char *)ptr - META_SIZE);
  pointer->free = 1;
  //free_size += p->size + sizeof(Metadata);
  extend_chunk(pointer);

  if ((pointer->next != NULL) &&
      ((char *)pointer + pointer->size + META_SIZE == (char *)pointer->next)) {
    pointer->size += META_SIZE + pointer->next->size;
    //remove_block(p->next);
    pointer->next->next = NULL;
    pointer->next->prev = NULL;
  }
  if ((pointer->prev != NULL) &&
      ((char *)pointer->prev + pointer->prev->size + META_SIZE == (char *)pointer)) {
    pointer->prev->size += META_SIZE + pointer->size;
    //remove_block(p);
    pointer->next = NULL;
    pointer->prev = NULL;
  }
}
