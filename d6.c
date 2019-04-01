#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "barrier.h"

// A simple barrier example. All threads will print the same value after each round.
// The \n meant to separate the rounds prints in the right place, because we now 
// use the barrier function.





typedef struct {

    int tid; // thread id
    barrier_t * barrierPtr;
    int * counterPtr;
    pthread_mutex_t * counterLockPtr;
} arguments;

int oneRound(pthread_t *threads, int nthreads, barrier_t * barrierPtr, int * counterPtr, pthread_mutex_t * counterLockPtr);
void * thread_func(void *a);
void * barrier_func(void * a);

int main( int argc, char *argv[]){

   if(argc != 3){
    
      printf("Usage: ./d6 nthreads nreps\n");
      return 1;
    }

	int nthreads = atoi(argv[1]);
	int reps = atoi(argv[2]);

	pthread_t threads[nthreads]; 
	
	barrier_t barrier;
	barrier_init(&barrier, nthreads, barrier_func); // no "nthreads + 1" here, since
	                                                // the main thread is not using the
	                                                // barrier.
	
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
 *  they "see". This function does not sync on the barrier, since it doesn't try to do anything
 *  other than create and join threads.
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
       	
	for (int i=0; i<nthreads; i++){
        if (pthread_join(threads[i], NULL) != 0){
            
            return EXIT_FAILURE;
        } 
    }


    return EXIT_SUCCESS;
}

/** Increment a counter, print the resulting value, and exit.
 */
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
    // Looking at the output, you'll see that the barrier function is executed by the "waking"
    // thread before the printf immediately below is executed by _all_ threads, including
    // the waking thread.

    printf("thread %d: %d\n", tid, *counterPtr);
    
    return NULL;
}

/** This function is executed by the last thread to enter the barrier. It is executed under
 *  the protection of the barrier mutex, which guarantees that it runs before the other 
 *  threads have started running.
 */
void * barrier_func(void * a){

    printf("\n");
    
    return NULL;
}


