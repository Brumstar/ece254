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
    int *bitmap_start;
    int blocks_available;
} mem_struct;

mem_struct best_fit;
mem_struct worst_fit;

/* Functions */
void set_bit(int *bitmap, const unsigned int position);
int test_bit(int *bitmap, const unsigned int position);
void clear_bit(int *bitmap, const unsigned int position);
void print_bitmap(mem_struct mem, int *bitmap);

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
    best_fit.bitmap_start = (int *)(best_fit.mem_chunk + bitmap_size);

    printf("Size of bitmap is %d\n", bitmap_size);

    // available memory starts after bitmap
    best_fit.free_space = best_fit.mem_chunk + 2*bitmap_size;

    printf("Free space starts at %p\n", best_fit.free_space);
    return 0;

}

int worst_fit_memory_init(size_t size)
{
    int bitmap_size;
    // allocate initial mem chunk
    worst_fit.mem_chunk = malloc(size);
    if (!worst_fit.mem_chunk) {
        printf("Can't malloc best fit block.\n");
        return -1;
    }

    printf("Initialised %zd bytes of memory at %p\n", size, worst_fit.mem_chunk);

    // bitmap starts at beginning of memory chunk
    worst_fit.bitmap = (int *)worst_fit.mem_chunk;

    // figure out the total number of blocks available
    worst_fit.blocks_available = (int)(ceil((float)size / (float)BLOCK_SIZE));

    printf("Blocks available %d\n", worst_fit.blocks_available);

    // determine size necessary for bitmap
    bitmap_size = ceil(log2f(worst_fit.blocks_available)) * BLOCK_SIZE;
    worst_fit.bitmap_start = (int *)(worst_fit.mem_chunk + bitmap_size);
    printf("Size of bitmap is %d\n", bitmap_size);

    // available memory starts after bitmap
    worst_fit.free_space = worst_fit.mem_chunk + 2*bitmap_size;

    printf("Free space starts at %p\n", worst_fit.free_space);
    return 0;

}

/* memory allocators */
void *best_fit_alloc(double size)
{
    printf("Trying to alloc %zu bytes\n", size);
    int bitmap_copy[best_fit.blocks_available];
    int num_blocks_required = ceil(size / BLOCK_SIZE);
    printf("Number of blocks required is %d\n", num_blocks_required);
    if (num_blocks_required > best_fit.blocks_available) {
        return NULL;
    }	
    void *best_block;
    int best_size = best_fit.blocks_available;
    int blocks_allocated = 0;
    int changed_best = 0;
    bitmap_copy[0] = best_size;
    for (int i = 0; i < best_fit.blocks_available; i++) {
        if (test_bit(best_fit.bitmap, i)) {
            printf("Found a 1 in position %d\n", i);
            bitmap_copy[blocks_allocated] = i;
            blocks_allocated++;
        }
    }
    int freespace_size[blocks_allocated + 1];
    freespace_size[0] = bitmap_copy[0] - 0;
    if( blocks_allocated ){   
	freespace_size[blocks_allocated] = 255 - bitmap_copy[blocks_allocated-1];
        printf("Freespace size at %d is %d\n", blocks_allocated, freespace_size[blocks_allocated]);
	for (int j = 1; j < blocks_allocated; j++) {
           freespace_size[j] = bitmap_copy[j] - bitmap_copy[j-1] - 1; 
           printf("Freespace size at %d is %d\n", j, freespace_size[j]);
        }
    }
    printf("blocks_allocated is %d\n", blocks_allocated);
    for (int k = 0; k < blocks_allocated + 1; k++) {
        if (freespace_size[k] - num_blocks_required < best_size && (freespace_size[k] - num_blocks_required) >= 0) {
            best_size = freespace_size[k];
	    printf("freespace_size[%d] is %d\n", k, freespace_size[k]);
            changed_best = k;
        }
    }
    if (( changed_best == 0)  && (num_blocks_required > freespace_size[changed_best])) {
        printf("Haven't changed size, returning NULL\n");
        return NULL;
    }else {
        int bitmap_offset = changed_best ?  bitmap_copy[changed_best - 1]+1 : 0 ;
        best_block = best_fit.free_space + (bitmap_offset / BLOCK_SIZE);       

        for (int i = bitmap_offset; i < bitmap_offset + num_blocks_required; i++) {
            set_bit(best_fit.bitmap, i);
        }
	set_bit(best_fit.bitmap_start, bitmap_offset);
        printf("bitmap after allocation");
	print_bitmap(best_fit, best_fit.bitmap);
	printf("bitmap_start after allocation");
        print_bitmap(best_fit, best_fit.bitmap_start);
        printf("Best block is at %p\n", best_block);
        printf("Best size found is %d\n", best_size);
        return best_block;
    }
	return NULL;
}


void *worst_fit_alloc(double size)
{
    print_bitmap(worst_fit, worst_fit.bitmap);
    printf("Trying to alloc %zu bytes\n", size);
    char bitmap_copy[worst_fit.blocks_available];
    int num_blocks_required = ceil(size / BLOCK_SIZE);
    printf("Number of blocks required is %d\n", num_blocks_required);
    if (num_blocks_required > worst_fit.blocks_available) {
        return NULL;
    }
    void *best_block;
    int best_size = 0;
    int blocks_allocated = 0;
    int changed_best = 0;

    for (int i = 0; i < worst_fit.blocks_available; i++) {
        if (test_bit(worst_fit.bitmap, i)) {
            printf("Found a 1 in position %d\n", i);
            bitmap_copy[blocks_allocated] = i;
            blocks_allocated++;
        }
    }
    int freespace_size[blocks_allocated + 1];
    freespace_size[0] = bitmap_copy[0] - 0;
    freespace_size[blocks_allocated] =  ((worst_fit.free_space - worst_fit.mem_chunk) * BITS_IN_BYTE) - bitmap_copy[blocks_allocated-1];
    for (int j = 1; j < blocks_allocated; j++) {
       freespace_size[j] = bitmap_copy[j] - bitmap_copy[j-1] - 1;
       printf("Freespace size at %d is %d\n", j, freespace_size[j]);
    }
    printf("Blocks available is %d\n", worst_fit.blocks_available);
    for (int k = 0; k < blocks_allocated + 1; k++) {

        printf("Freespace math %d\n", freespace_size[k] - num_blocks_required);
        if (freespace_size[k] > best_size) {
            best_size = freespace_size[k];
            changed_best = k;
        }
    }

    if (!changed_best) {
        printf("Haven't changed size, returning NULL\n");
        return NULL;
    } else {
        printf("Bitmap address is %p\n", worst_fit.bitmap);
        int bitmap_offset = bitmap_copy[changed_best - 1] + 1;
        printf("Bitmap offset %d\n", bitmap_offset);
        printf("Free space is at address %p\n", worst_fit.free_space);
        best_block = worst_fit.free_space + (bitmap_offset / BLOCK_SIZE);

        for (int i = bitmap_offset; i < bitmap_offset + num_blocks_required; i++) {
            set_bit(worst_fit.bitmap, i);
        }
        set_bit(worst_fit.bitmap_start, bitmap_offset);
        print_bitmap(worst_fit, worst_fit.bitmap);
        print_bitmap(worst_fit, worst_fit.bitmap_start);
        printf("Best block is at %p\n", best_block);
        printf("Best size found is %d\n", best_size);
        return best_block;
    }

    return NULL;
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{
    if (ptr < best_fit.mem_chunk || ptr > best_fit.mem_chunk + (best_fit.blocks_available * BLOCK_SIZE)) {
        printf("Best fit dealloc pointer at address %p invalid\n", ptr);
        return;
    }

    int bitmap_index = (ptr - best_fit.free_space) * BLOCK_SIZE;
    printf("Bitmap dealloc index %d\n", bitmap_index);
    if (!test_bit(best_fit.bitmap_start, bitmap_index)) {
        printf("Best fit dealloc incorrect starting block\n");
        return;
    }

    clear_bit(best_fit.bitmap_start, bitmap_index);
    while (test_bit(best_fit.bitmap, bitmap_index)) {
        clear_bit(best_fit.bitmap, bitmap_index);
        bitmap_index++;
    }
    printf("bitmap after deallocation");
    print_bitmap(best_fit, best_fit.bitmap);
    printf("bitmap_start after deallocation");
    print_bitmap(best_fit, best_fit.bitmap_start);
    return;
}

void worst_fit_dealloc(void *ptr) 
{
    if (ptr < worst_fit.mem_chunk || ptr > worst_fit.mem_chunk + (worst_fit.blocks_available * BLOCK_SIZE)) {
        printf("Best fit dealloc pointer at address %p invalid\n", ptr);
        return;
    }

    int bitmap_index = (ptr - worst_fit.free_space) * BLOCK_SIZE;
    printf("Bitmap dealloc index %d\n", bitmap_index);
    print_bitmap(worst_fit, worst_fit.bitmap);

    if (!test_bit(worst_fit.bitmap_start, bitmap_index)) {
        printf("Best fit dealloc incorrect starting block\n");
        return;
    }

    clear_bit(worst_fit.bitmap_start, bitmap_index);
    while (test_bit(worst_fit.bitmap, bitmap_index)) {
        clear_bit(worst_fit.bitmap, bitmap_index);
        bitmap_index++;
    }
    print_bitmap(worst_fit, worst_fit.bitmap);
    return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size)
{
    int frag_blocks = 0;
    int free_spaces = 0;
    for(int i = 0; i < best_fit.blocks_available; i++){
	 if(!test_bit(best_fit.bitmap, i)){
	    free_spaces++;
	}else{ 
	    if(free_spaces){
		if(free_spaces < size){
		    frag_blocks++;
		    free_spaces = 0;
                }
	    }
	}
    }
    printf("%d frag blocks\n",frag_blocks);	
    return frag_blocks;
}

int worst_fit_count_extfrag(size_t size)
{
    int frag_blocks = 0;
    int free_spaces = 0;
    for(int i = 0; i < worst_fit.blocks_available; i++){
         if(!test_bit(worst_fit.bitmap, i)){
            free_spaces++;
        }else{
            if(free_spaces){
                if(free_spaces < size){
                    frag_blocks++;
                    free_spaces = 0;
                }
            }
        }
    }
    printf("%d frag blocks\n",frag_blocks);
    return frag_blocks;
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

void print_bitmap(mem_struct mem, int *bitmap) { 
    for (int i = 0; i < mem.blocks_available; i++) {
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
