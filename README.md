# Duke-ECE-650

This is Zhe Fan's ECE650 Project Repository.

# Table of Contents

1. [Malloc Library](#Malloc-Library)
2. [Implementation Description](#Implementation-Description)
3. [Malloc](#Malloc)
4. [Free](#Free)
5. [Performance Result Presentation and Analysis](#Performance-Result-Presentation-and-Analysis)
6. [My second project](#my-second-project)
## Malloc Library

For this assignment, I will implement my own version of several memory allocation functions from the C standard library. This implementation is to be done in C code. The C standard library includes 4 malloc reallrelated library functions: malloc(), free(), calloc(), and oc(). In this assignment, I implement versions of malloc() and free(): 
```
void *malloc(size_t size); 
void free(void *ptr); 
```
Essentially, malloc() takes in a size (number of bytes) for a memory allocation, locates an address in the program’s data region where there is enough space to fit the specified number of bytes, and returns this address for use by the calling program. 
The free() function takes an address (that was returned by a previous malloc operation) and marks that data region as available again for use.

The task is to implement 2 versions of malloc and free, each based on a different strategy for
determining the memory region to allocate. The two strategies are:
1. **First Fit**: Examine the free space tracker (e.g. free list), and allocate an address from
the first free region with enough space to fit the requested allocation size.
2. **Best Fit**: Examine all of the free space information, and allocate an address from the
free region which has the smallest number of bytes greater than or equal to the
requested allocation size.

Details of requirement can be found in [Project1.pdf](Project_1_Malloc_Library/Project1.pdf)

### 1.1 Implementation Description

#### 1.1.1 Malloc

  For each space chunk, I define a struct using **Double LinkedList** to know the **size** of the chunk, whether or not it's **free**, and what the **next** and **previous** chunk.
  
```
struct chunk_meta {
  size_t size;               //the size of this chunk
  int free;                  //it is free or not
  struct chunk_meta * next;
  struct chunk_meta * prev;
};
typedef struct chunk_meta chunk
```
  
  I also define two variable to record the head and tail of free list:
  
```
chunk * free_region_Start = NULL;
chunk * free_region_End = NULL;
```
  
  For malloc, space should only be allocated when existing space is not enough. Thus given that we have this linked list structure, checking if we have a free chunk and return it. When we get a request of some size, we iterate through our linked list to see if there's a free chunk that's large enough.

 In details, when the chunk is large enough, I also need to reuse free space efficiently if possible. To implementaion that, given that I have LinkedList, we can using splitting and merging.
 
If we don't find a free block, we'll have to request space using sbrk to allocate space.

#### 1.1.2 Free

For free(), first I upadate the size of the chunk and make the **free** = 1, then check the previous and next chunk if free or not, if free, then merge them.

### 1.2 Performance Result Presentation and Analysis



## My second project
