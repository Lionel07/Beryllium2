/*
 * malloc.c
 *   Dynamic memory allocation routines:
 *   void* malloc(size_t size);
 *   void  free(void* ptr);
 *   void* calloc(size_t num_of_elts, size_t elt_size);
 *   void* realloc(void* pointer, size_t size);
 *
 *   Nathan Bossart
 *   Nov 3, 2013
 */

#include <string.h>
#include <beryllium/memory.h>
#define PAGE_SIZE 0x1000
// our node struct, which is conveniently 32 bytes!
typedef struct Meta Meta;
struct Meta {
  size_t size;
  char   free;
  Meta*  prev;
  Meta*  next;
};

// init the doubly-linked list
static Meta tail;
static Meta head = {0, 0, NULL, &tail};
static Meta tail = {0, 0, &head, NULL};

size_t alloc_end = 0;  // track amount of the heap we have used

void* malloc(size_t size) {
  // long word align
  if (size%sizeof(size_t) != 0) {
    size += sizeof(size_t) - (size%sizeof(size_t));
  }

  // first fit search
  Meta* block = &head;
  while (block->next!=&tail) {
    block = block->next;
    if (block->free && block->size >= size) {
      block->free = 0;
      if (block->size - size >= sizeof(size_t)+sizeof(Meta)) {  // we can split the block

	// make a new block
	Meta* newBlock = (void*) block + size + sizeof(Meta);

	// adjust sizes
	newBlock->size = block->size - size - sizeof(Meta);
	block->size   -= (newBlock->size + sizeof(Meta));

	// set pointers
	block->next->prev = newBlock;
	newBlock->prev    = block;
	newBlock->next    = block->next;
	block->next       = newBlock;

	// set new block as free
	newBlock->free = 1;

	// if the new free block is adjacent to another free block, combine
	if (newBlock->next->free) {
	  newBlock->size += newBlock->next->size + sizeof(Meta);
	  newBlock->next->next->prev = newBlock;
	  newBlock->next = newBlock->next->next;
	}
      }
      return ((Meta*) block) + 1;   
    }
  }
  // if we made it here, must add a new node!
 
  // on first run, find initial head addr
  if (alloc_end == 0) {
    alloc_end = (size_t) sbrk(0);
  }

  // enlarge the heap as much as needed
  while ((size_t) sbrk(0) - alloc_end < size + sizeof(Meta)) {
    if ((void*)sbrk(PAGE_SIZE) == (void*) -1) {
      return NULL;
    }
  }

  // init new block
  Meta* newBlock       = (Meta*) alloc_end;
  newBlock->next       = &tail;
  newBlock->prev       = tail.prev;
  newBlock->prev->next = newBlock;
  tail.prev            = newBlock;
  newBlock->free       = 0;
  newBlock->size       = size;
  alloc_end           += size + sizeof(Meta);
  return ((Meta*) newBlock) + 1;

}


void free(void* ptr) {
  if (ptr!=NULL) {
    Meta* block = ((Meta*) ptr) - 1;
    block->free = 1;
    
    // combine with previous block if both free
    if (block->prev->free) {
      block->next->prev  = block->prev;
      block->prev->next  = block->next;
      block->prev->size += block->size + sizeof(Meta);
      block              = block->prev;
    }
    
    // combine with next block if both free
    if (block->next->free) {
      block->next->next->prev = block;
      block->size       += block->next->size + sizeof(Meta);
      block->next        = block->next->next;
    }
  }
}


void* calloc(size_t num_of_elts, size_t elt_size) {
  size_t size = num_of_elts * elt_size;
  void* ptr = malloc(size);
  if (ptr!=NULL) {
    memset(ptr, 0, size);
  }
  return ptr;
}


void* realloc(void* pointer, size_t size) {
  void* new_block = malloc(size);
  if (pointer!=NULL && size==0) {
    free(pointer);
  }
  if (pointer!=NULL && new_block!=NULL) {
    Meta* old_block = (Meta*) pointer - 1;
    size_t min = ((size < old_block->size) ? size : old_block->size);
    memcpy(new_block, pointer, min);
    free(pointer);
  }
  return new_block;
}
