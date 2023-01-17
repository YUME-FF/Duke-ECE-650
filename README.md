# Duke-ECE-650

This is Zhe Fan's ECE650 Project Repository.

# Table of Contents

1. [Malloc Library](#Malloc-Library)
2. [My second project](#my-second-project)
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
1. First Fit: Examine the free space tracker (e.g. free list), and allocate an address from
the first free region with enough space to fit the requested allocation size.
2. Best Fit: Examine all of the free space information, and allocate an address from the
free region which has the smallest number of bytes greater than or equal to the
requested allocation size.

Details can be found in [Project1.pdf](Project_1_Malloc_Library/Project1.pdf)

## My second project
