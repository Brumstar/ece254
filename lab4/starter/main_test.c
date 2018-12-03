/*E254 Linux Dynamic Memory Management Lab
  * @file: main_test.c
  * @brief: The main file to write tests of memory allocation algorithms
  */ 

/* includes */
/* system provided header files. You may add more */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* non-system provided header files. 
   Do not include more user-defined header files here
 */
#include "mem.h"



int main(int argc, char *argv[])
{
	char model = *argv[1];
	int algo = atoi(argv[2]); 
	void *p, *q;
	int num = 0;

	if( model == 'b' ){
		best_fit_memory_init(1024);
		switch(algo){
			case 0:
				//Allocate full free space region
				p = best_fit_alloc(1024);
				printf("best fit: p=%p\n", p);
				if ( p != NULL ) {
				best_fit_dealloc(p);	
				}
				num = best_fit_count_extfrag(4);
				break;
			case 1:
				//Allocate greater than initialised memory
				p = best_fit_alloc(1025);
				printf("best fit: p=%p\n", p);
				if ( p != NULL ) {
				best_fit_dealloc(p);	
				}
				num = best_fit_count_extfrag(4);
				break;
			case 2:
				//Repeatedly allocate and deallocate same block while checking external fragmentation
				while(1){
					p = best_fit_alloc(256);
					printf("best fit: p=%p\n", p);
					if ( p != NULL ) {
					best_fit_dealloc(p);	
					}
					num = best_fit_count_extfrag(4);
					printf("num = %d\n", num);
				} 
				break;
			case 3:
				//Repeated allocations of a fixed size(small)
				;int i = 12;
				while(i>0){
					p = best_fit_alloc(8);
					printf("best fit: p=%p\n", p);
					i--;
				}
				break;
			case 4:
				//Repeated allocations of a fixed size(large)
				; i = 4;
				while(i>0){
					p = best_fit_alloc(256);
					printf("best fit: p=%p\n", p);
					i--;
				}
				break;
			case 5:
				//Increasing size allocations
				for(int i = 1; i < 512; i = 2*i ){
					p = best_fit_alloc(i);
					printf("best fit: p=%p\n", p);
				}
				break;
			case 6:
				//Decreasing size allocations
				for(int i = 512; i > 1; i = i/2 ){
					p = best_fit_alloc(i);
					printf("best fit: p=%p\n", p);
				}
				break;
			case 7:
				//Dealloc invalid pointers
				p = NULL;
				best_fit_dealloc(p);
				break;	
		}
		

	}else if( model == 'w' ){
		worst_fit_memory_init(1024);
		switch(algo){
			case 0:
				//Allocate full free space region
				q = worst_fit_alloc(1024);
				printf("worst fit: q=%p\n", q);
				if ( q != NULL ) {
					worst_fit_dealloc(q);
				}
				num = worst_fit_count_extfrag(4);
				break;
			case 1:
				//Allocate greater than initialised memory
				q = worst_fit_alloc(1025);
				printf("worst fit: q=%p\n", q);
				if ( q != NULL ) {
					worst_fit_dealloc(q);
				}
				num = worst_fit_count_extfrag(4);
				break;
			case 3:
				//Repeated allocations of a fixed size(small)
				while(1){
					p = worst_fit_alloc(8);
					printf("worst fit: p=%p\n", p);
				}
				break;
			case 4:
				//Repeated allocations of a fixed size(large)
				while(1){
					p = worst_fit_alloc(256);
					printf("worst fit: p=%p\n", p);
				}
				break;
			case 5:
				//Increasing size allocations
				for(int i = 1; i < 512; i = 2*i ){
					p = worst_fit_alloc(i);
					printf("worst fit: p=%p\n", p);
				}
				break;
			case 6:
				//Decreasing size allocations
				for(int i = 512; i > 1; i = i/2 ){
					p = worst_fit_alloc(i);
					printf("worst fit: p=%p\n", p);
				}
				break;
			case 7:
				//Dealloc invalid pointers
				p = NULL;
				worst_fit_dealloc(p);
				break;	
		}
		

	}else{
		fprintf(stderr, "Should not reach here!\n");
		exit(1);
	}

	printf("num = %d\n", num);

	return 0;
}

