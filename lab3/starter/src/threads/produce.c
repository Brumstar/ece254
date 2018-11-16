#define _XOPEN_SOURCE 500 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>


/* Please don't change anything between this line and 
 *    the start of the global variables */

typedef struct {
    int id;
    int value;
} task;

typedef struct node {
    task* task;
    struct node* next;
} node;

node* list_head;

/* Global Variables -- Add more if you need! */
int total_tasks;
int active_tasks;
int remaining_tasks = 0;
int task_id_counter = 0;
pthread_mutex_t mutex;
sem_t empty_list;
sem_t done;

struct timeval tv;
double g_time[2];


/* Function Prototypes for pthreads */
void* producer( void* );
void* consumer( void * );

/* Don't change the function prototypes below;
 *    they are your API for how the house elves do work */

/* Removes a task from the list of things to do
 *    To be run by: house elf threads
 *       Takes no arguments
 *          NOT thread safe!
 *             return: a pointer to a task for an elf to do
 *             */
task* take_task();

/* Perform the provided task
 *    To be run by: house elf threads
 *       argument: pointer to task to be done
 *          IS thread safe
 *             return: nothing
 *             */  
void check_root( task* todo, int consumer_id );

/* Put tasks in the list for elves to do
 *    To be run by: Dobby
 *       argument: how many tasks to put in the list
 *          NOT thread safe
 *             return: nothing
 *             */
void post_tasks( int producer_id, int value );

/* Used to unlock a mutex, if necessary, if a thread
 *    is cancelled when blocked on a semaphore
 *    */
void consumer_cleanup( void* arg);

/* Complete the implementation of main() */

node* list_head;
int total_tasks;
int buffer_size;
int producer_num;
int consumer_num;

int main( int argc, char** argv ) {
    gettimeofday(&tv, NULL);
    g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;


    if ( argc != 5 ) {
        printf( "Wrong arguments\n");
        return -1;
    }
    /* Init global variables here */
    list_head = NULL;
    total_tasks = atoi( argv[1] );
    buffer_size = atoi( argv[2] );
    producer_num = atoi( argv[3] );
    consumer_num = atoi( argv[4] );

    pthread_t P[producer_num];
    pthread_t C[consumer_num];
    
    active_tasks = 0;
    remaining_tasks = total_tasks;
    pthread_mutex_init( &mutex, NULL );
    sem_init( &done, 0, 0 );
    sem_init( &empty_list, 0, 0 );
  
  
    /* Launch threads here */
    for( int i = 0; i < producer_num; i++ ){
    	int *prod_id = malloc(sizeof(int));
	*prod_id = i;
        pthread_create(&P[i], NULL, producer, prod_id);
    }

    for( int i = 0; i < consumer_num; i++ ) {
        int *cons_id = malloc(sizeof(int));
        *cons_id = i;
        pthread_create(&C[i], NULL, consumer, cons_id);
    }
   
    /* Wait for Dobby to be done */
    printf("Before join\n");
    for( int i = 0; i < producer_num; i++) {
	pthread_join(P[i], NULL);
    }
    printf("Before done\n");
    sem_wait(&done);
    printf("After done\n");
    for( int i = 0; i < consumer_num; i++) {
       pthread_cancel(C[i]);
    } 

    /* Cleanup Global Variables here */
    sem_destroy( &empty_list );
    sem_destroy( &done);
    pthread_mutex_destroy( &mutex );

    gettimeofday(&tv, NULL);
    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    printf("System execution time: %.6lf seconds\n", \
        g_time[1] - g_time[0]);
    return 0;
}

/* Write the implementation of dobby() */

void* producer( void * arg ) {

    int *producer_id = (int *)arg;
    while(1){
	for (int j = *producer_id; j < total_tasks; j += producer_num){
	    pthread_mutex_lock(&mutex);
	    if (active_tasks <= buffer_size) {
		post_tasks(*producer_id, j);
	    } else {
		pthread_mutex_unlock(&mutex);
		sem_wait(&empty_list);
		pthread_mutex_lock(&mutex);
		post_tasks(*producer_id, j);
	    }
	    pthread_mutex_unlock(&mutex);
	}
	free(arg);
	pthread_exit(0);
    }
}


void* consumer( void * ignore ) {
    int consumer_id = *((int *)ignore);
    task* todo;
    pthread_cleanup_push( consumer_cleanup, ignore );
    while( 1 ) {
        pthread_testcancel();
        //printf("Consumer prelock\n");
        pthread_mutex_lock( &mutex );
        //printf("Consumer post lock\n");
	if (active_tasks > 0) {
	    todo = take_task();
	    check_root(todo, consumer_id);
            printf("rem %d\n", remaining_tasks);
	    if (remaining_tasks == 0) {
	        sem_post(&done);
            }
            active_tasks--;
	}
        //printf("Consumer pre unlock\n");
        pthread_mutex_unlock( &mutex );
        //printf("Consumer post unlock\n");
	if (active_tasks == buffer_size - 1) {
	    sem_post(&empty_list);
	}
    }
    pthread_cleanup_pop( 0 ) ;
}

void consumer_cleanup( void* arg ) {
    printf("Consumer cancel id %d\n", *((int *)arg));
    free(arg);
}


void post_tasks( int producer_id, int value ) {
    task* t = malloc( sizeof( task ));
    t->id = ++task_id_counter;
    t->value = value;
    node* n = malloc( sizeof( node ));
    n->task = t;
    n->next = list_head;
    list_head = n;
    active_tasks++;
}

void check_root( task* todo, int consumer_id ) {
     int received = todo -> value; 
     int root = sqrt(todo -> value);
     if ((root * root) == received) {
         printf("%d %d %d\n", consumer_id, received, root);
     }
     free( todo );
}

task* take_task( ) {
    if (list_head == NULL) {
        exit( -1 );
    }
    node* head = list_head;
    task* t = head->task;
    list_head = list_head->next;
    free( head );
    remaining_tasks--;
    return t;
}


