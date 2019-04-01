#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "barrier.h"

// Another simple barrier example. This one puts the "round" loop inside the thread.

// 
int gResult=0;

// We'll make the counter global, since we're not using arguments with the barrier function.
int gCounter=0;

typedef struct {

    int tid; // thread id
    int numRounds;
     
    barrier_t * barrierPtr;
    int * counterPtr; // Not truly necessary, since gCounter is global now.
    pthread_mutex_t * counterLockPtr;
} thread_args;


int runRounds(  pthread_t *threads, int nthreads, int numRounds, 
                barrier_t * barrierPtr, int * counterPtr, pthread_mutex_t * counterLockPtr);
                
void * thread_func(void *a);
void * barrier_func(void * a);




int main( int argc, char *argv[]){

   if(argc != 3){
    
      printf("Usage: ./d7 nthreads nreps\n");
      return 1;
    }

	int nthreads = atoi(argv[1]);
	int numRounds = atoi(argv[2]);

	pthread_t threads[nthreads]; 
	
	barrier_t barrier;
	barrier_init(&barrier, nthreads+1, barrier_func); // nthreads+1 to account for the main thread.
	
	//int counter=0;  // now global
	
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
	
	runRounds(threads, nthreads, numRounds, &barrier, &gCounter, &lock);
	

    // We're done, so clean up.
    if (barrier_destroy(&barrier) != 0){
    
        return EXIT_FAILURE;
    } 
    
    if (pthread_mutex_destroy(&lock) != 0){
    
        return EXIT_FAILURE;
    } 

    return EXIT_SUCCESS;
}


int runRounds(pthread_t *threads, int nthreads, int numRounds, barrier_t * barrierPtr, int * counterPtr, pthread_mutex_t * counterLockPtr){
    
    for(int i=0; i<nthreads; i++){

        thread_args * args = malloc(sizeof(thread_args));
        
        args->tid = i;
        args->numRounds = numRounds;
        args->barrierPtr = barrierPtr;
        args->counterPtr = counterPtr;  // actually points to gCounter
        args->counterLockPtr = counterLockPtr;
        
        if (pthread_create(&(threads[i]), NULL, thread_func, args) != 0){
            return EXIT_FAILURE;
        }    
    }
    
    for(int round = 0; round < numRounds; round++){

  	    barrier_wait(barrierPtr, NULL);  
  	    
  	    printf("gResult = %d\n", gResult);
  	}
  	
  	
  	for (int i=0; i<nthreads; i++){
	
        if (pthread_join(threads[i], NULL) != 0){
            return EXIT_FAILURE;
        } 
    }


    return EXIT_SUCCESS;
}


void * thread_func(void *a){

    thread_args * args = (thread_args *) a;
    
    int tid = args->tid;
    int numRounds = args->numRounds;
    pthread_mutex_t * counterLockPtr = args->counterLockPtr;
    int * counterPtr = args->counterPtr;
    barrier_t *barrierPtr = args->barrierPtr;
    
    free(a); a=NULL; args=NULL;
    
    for(int round = 0; round<numRounds; round++){

        pthread_mutex_lock(counterLockPtr);
        (*counterPtr)++;
        pthread_mutex_unlock(counterLockPtr);
        
        barrier_wait(barrierPtr, NULL);
        
        // This printf is unsafe. Nothing prevents a race condition in which a "fast" thread
        // prints a value, then goes back to the top of the loop, where it increments the
        // counter. Some other, slower, thread will now print a value that is not the result
        // of a full "round" of increments, which is not what we intended.
        //printf("thread %d: %d\n", tid, *counterPtr);
    }
    
    return NULL;
}

void * barrier_func(void * a){
   
    printf("\n");
    
    // Let's pretend we have some significant job to do between rounds.  For demo purposes,
    // we'll just copy gCounter to gResult. In practice, we could do much more heavyweight things,
    // keeping in mind that this is strictly serial code.

    // This copy only gets made once per round, meaning that the main thread can print gResult
    // without causing a race condition with the other threads. 
    gResult = gCounter; 
    
    return NULL;
}


