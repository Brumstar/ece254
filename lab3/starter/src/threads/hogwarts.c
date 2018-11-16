#define _XOPEN_SOURCE 500 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>


/* Please don't change anything between this line and 
 *    the start of the global variables */

int task_id_counter = 0;

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
pthread_mutex_t mutex;
sem_t empty_list;
sem_t full_list;


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
void consumer_cleanup( void * );

/* Complete the implementation of main() */

node* list_head;
int total_tasks;
int buffer_size;
int producer_num;
int consumer_num;

int main( int argc, char** argv ) {

    if ( argc != 5 ) {
        printf( "Wrong arguments\n");
        return -1;
    }
    /* Init global variables here */
    list_head = NULL;
    total_tasks = atoi( argv[1] );
    buffer_size = atoi( argv[2] );
    producer_num = atoi( argv[3] );
    producer_num = 1; //REMOVE
    consumer_num = atoi( argv[4] );

    pthread_t P[producer_num];
    pthread_t C[consumer_num];
    
    active_tasks = 0;
    pthread_mutex_init( &mutex, NULL );
    sem_init( &full_list, 0, 0 );
    sem_init( &empty_list, 0, 0 );
  
    printf("There are %d tasks to do today.\n", total_tasks);
  
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
    
    pthread_join(P[0], NULL);

    /* Cleanup Global Variables here */
    sem_destroy( &empty_list );
    sem_destroy( &full_list );
    pthread_mutex_destroy( &mutex );

    return 0;
}

/* Write the implementation of dobby() */

void* producer( void * arg ) {

    int *producer_id = (int *)arg;
    while(1){
        printf("Starting producer\n");
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
    }
}

/* Complete the implementation of house_elf() */

void* consumer( void * ignore ) {
  /* The following function call registers the cleanup
   *      handler to make sure that pthread_mutex_t locks 
   *           are unlocked if the thread is cancelled. It must
   *                be bracketed with a call to pthread_cleanup_pop 
   *                     so don't change this line or the other one */
    int consumer_id = *((int *)ignore);
    // TODO: Free ignore
    printf("Starting consumer with id %d\n", consumer_id);
    pthread_cleanup_push( consumer_cleanup, NULL); 
    while( 1 ) {
        pthread_mutex_lock( &mutex );
        pthread_testcancel();
        active_tasks--;
	printf("Taking a task\n");
        task* todo = take_task();
        pthread_mutex_unlock( &mutex );
	if (active_tasks == buffer_size - 1) {
	    sem_post(&empty_list);
	}
        check_root( todo, consumer_id );
    }
    /* This cleans up the registration of the cleanup handler */
    pthread_cleanup_pop( 0 ) ;
}

/* Implement unlocking of any pthread_mutex_t mutex types
 *    that are locked in the house_elf thread, if any 
 *    */
void consumer_cleanup( void* arg ) {
    pthread_mutex_unlock( &mutex );
}

/****** Do not change anything below this line -- Internal Functionality Only ********/ 

void post_tasks( int producer_id, int value ) {
    task* t = malloc( sizeof( task ));
    t->id = ++task_id_counter;
    printf("Task id is: %d\n", t->id);
    t->value = value;
    node* n = malloc( sizeof( node ));
    n->task = t;
    n->next = list_head;
    list_head = n;
    active_tasks++;
}

void check_root( task* todo, int consumer_id ) {
     printf("Task %d was completed!\n", todo->id);
     int received = todo -> value; 
     int root = sqrt(todo -> value);
     if ((root * root) == received) {
         printf("%d %d %d\n", consumer_id, received, root);
         // What is this supposed to be
	 /*if (received == num - 1) {
             return;
         }*/
     }
     free( todo );
}

task* take_task( ) {
    if (list_head == NULL) {
        /* Something has gone wrong */
        printf("Trying to take from an empty list!\n");
        exit( -1 );
    }
    node* head = list_head;
    task* t = head->task;
    list_head = list_head->next;
    free( head );
    return t;
}


