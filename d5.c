#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "barrier.h"

// A simple barrier example. All threads will print the same value after each round.
// However, order of output is not guaranteed, and the \n meant to separate the rounds
// prints in the wrong place.



typedef struct {

    int tid; // thread id
    barrier_t * barrierPtr;
    int * counterPtr;
    pthread_mutex_t * counterLockPtr;
} arguments;

int oneRound(pthread_t *threads, int nthreads, barrier_t * barrierPtr, int * counterPtr, pthread_mutex_t * counterLockPtr);
void * thread_func(void *a);

int main( int argc, char *argv[]){

   if(argc != 3){
    
      printf("Usage: ./d5 nthreads nreps\n");
      return 1;
    }

	int nthreads = atoi(argv[1]);
	int reps = atoi(argv[2]);

	pthread_t threads[nthreads]; 
	
	barrier_t barrier;
	barrier_init(&barrier, nthreads+1, NULL);
	
	int counter=0;
	
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
	
	for (int i=0; i<reps; i++){
	
		oneRound(threads, nthreads, &barrier, &counter, &lock);
	}
	
    // We're done, so clean up.
    if (barrier_destroy(&barrier) != 0){
    
        return EXIT_FAILURE;
    } 
    
    if (pthread_mutex_destroy(&lock) != 0){
    
        return EXIT_FAILURE;
    } 

    return EXIT_SUCCESS;
}

/** Run one "round" of a very simple parallel computation, in which each thread increments a
 *  a counter. During each round, the specified number of threads are created, executed, and
 *  then terminated. After passing through a barrier, threads will print the counter value that
 *  they "see". The main thread will sync on this same barrier, and print a "\n", which is 
 *  _intended_ to separate the output from each round. 
*/
int oneRound(pthread_t *threads, int nthreads, barrier_t * barrierPtr, int * counterPtr, pthread_mutex_t * counterLockPtr){
    
    for(int i=0; i<nthreads; i++){

        arguments * args = malloc(sizeof(arguments));
        
        args->tid = i;
        args->barrierPtr = barrierPtr;
        args->counterPtr = counterPtr;
        args->counterLockPtr = counterLockPtr;
        
        if (pthread_create(&(threads[i]), NULL, thread_func, args) != 0){
            return EXIT_FAILURE;
        }    
    }
     
    // The hope here is that this barrier sync will make the following printf execute only after
    // all the other threads have printed. It won't work. The barrier _does_ guarantee that after 
    // the main thread syncs, the counter value reflects the increments from all the other threads.
    // It _does_not_ guarantee that the main thread would only print after the other threads have
    // printed their value. 
  	barrier_wait(barrierPtr, NULL);
  	
  	printf("\n");   // Problem: This may print at the wrong time. There is no guarantee that
  	                // that it will print after the other threads have printed. We need to 
  	                // use the barrier function for that.
  	                
   	// Try out the line below, and you'll get correct values.               
  	//printf("Main thread: %d\n", *counterPtr);
 	  

    // We wait for all the threads to terminate. 
	for (int i=0; i<nthreads; i++){
        if (pthread_join(threads[i], NULL) != 0){
            
            return EXIT_FAILURE;
        } 
    }

    return EXIT_SUCCESS;
}


void * thread_func(void *a){

    arguments * args = (arguments *) a;
    
    int tid = args->tid;
    pthread_mutex_t * counterLockPtr = args->counterLockPtr;
    int * counterPtr = args->counterPtr;
    barrier_t *barrierPtr = args->barrierPtr;
    
    free(a); a=NULL; args=NULL;
    
    pthread_mutex_lock(counterLockPtr);
    (*counterPtr)++;
    pthread_mutex_unlock(counterLockPtr);
    
    barrier_wait(barrierPtr, NULL);
    
    // This next line prints the counter value after all threads have incremented
    // it. After doing this, the threads all terminate, so there is no danger of
    // any thread incrementing more than once. 
    printf("thread %d: %d\n", tid, *counterPtr);
    
    
    
    return NULL;
}


