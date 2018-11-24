/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author: 
 * @date: 2015-11-20
 */

/* includes */
#include <stdio.h> 
#include <stdlib.h> 
#include "mem.h"

#define BITS_IN_BYTE 8
#define BYTE_ALIGNMENT 4
#define PTR_WIDTH 32
#define BLOCK_SIZE sizeof(int)/sizeof(char)

/* defines */

/* global variables */
typedef struct mem_struct {
    void *mem_chunk;
    void *free_space;
    int *bitmap;
    int blocks_available;
} mem_struct;

mem_struct best_fit;
mem_struct worst_fit;

/* Functions */
void set_bit(int *bitmap, const unsigned int position);
int test_bit(int *bitmap, const unsigned int position);
void clear_bit(int *bitmap, const unsigned int position);
void print_bitmap(int *bitmap);

/* memory initializer */
int best_fit_memory_init(size_t size)
{
    int bitmap_size;
    // allocate initial mem chunk    
    best_fit.mem_chunk = malloc(size);
    if (!best_fit.mem_chunk) {
        printf("Can't malloc best fit block.\n");
        return -1;
    }

    printf("Initialised %zd bytes of memory at %p\n", size, best_fit.mem_chunk);
    
    // bitmap starts at beginning of memory chunk
    best_fit.bitmap = (int *)best_fit.mem_chunk;

    // figure out the total number of blocks available
    best_fit.blocks_available = (int)(ceil((float)size / (float)BLOCK_SIZE));

    printf("Blocks available %d\n", best_fit.blocks_available);
    
    // determine size necessary for bitmap
    bitmap_size = ceil(log2f(best_fit.blocks_available)) * BLOCK_SIZE;

    printf("Size of bitmap is %d", bitmap_size);

    // available memory starts after bitmap
    best_fit.free_space = best_fit.mem_chunk + bitmap_size;

    printf("Free space starts at %p\n", best_fit.free_space);

    print_bitmap(best_fit.bitmap);

    set_bit(best_fit.bitmap, 0);
    set_bit(best_fit.bitmap, 2);
    print_bitmap(best_fit.bitmap);

    clear_bit(best_fit.bitmap, 0);
    print_bitmap(best_fit.bitmap);
    return 0;

}

int worst_fit_memory_init(size_t size)
{
        /*worst_fit_block = malloc(size);
        if (!worst_fit_block) {
            printf("Can't malloc worst fit block.\n");
            return -1;
        }*/
	// To be completed by students
	// You call malloc once here to obtain the memory to be managed.
	return 0;

}

/* memory allocators */
void *best_fit_alloc(size_t size)
{
	// To be completed by students
	return NULL;
}


void *worst_fit_alloc(size_t size)
{
	// To be completed by students
	return NULL;
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{
	free(ptr);
	// To be completed by students
	return;
}

void worst_fit_dealloc(void *ptr) 
{
	free(ptr);
	// To be completed by students
	return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size)
{
	// To be completed by students
	return 0;
}

int worst_fit_count_extfrag(size_t size)
{
	// To be completed by students
	return 0;
}

void set_bit(int *bitmap, const unsigned int position) {
    bitmap[position / (sizeof(int) * BITS_IN_BYTE)] |= 1 << (position % (sizeof(int) * BITS_IN_BYTE));
}

int test_bit(int *bitmap, const unsigned int position) {
    return (bitmap[position / (sizeof(int) * BITS_IN_BYTE)] & (1 << (position % (sizeof(int) * BITS_IN_BYTE)))) != 0;
}

void clear_bit(int *bitmap, const unsigned int position) {
    bitmap[position / (sizeof(int) * BITS_IN_BYTE)] &= ~(1 << (position % (sizeof(int) * BITS_IN_BYTE)));
}

void print_bitmap(int *bitmap) { 
    int range = (best_fit.free_space - best_fit.mem_chunk) * BITS_IN_BYTE;
    printf("Range is %d\n", range);
    for (int i = 0; i < range; i++) {
        if (i % 32 == 0) {
            printf("\n");
        } else if (i % 16 == 0) {
            printf("\t\t");
        } else if (i % 8 == 0) {
	    printf("\t");
        }
        printf("%d", test_bit(bitmap, i));
    }
    printf("\n");
}
