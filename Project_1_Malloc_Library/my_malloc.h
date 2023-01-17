#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct chunk_meta {
  size_t size;               //the size of this chunk
  int free;                  //it is free or not
  struct chunk_meta * next;  //single linkedlist to next chunk
};
typedef struct chunk_meta chunk;

void * allocate_space(size_t size);
void add_chunk(chunk * ptr);
void remove_chunk(chunk * ptr);

//First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void * ptr);

//Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void * ptr);
