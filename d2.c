#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "portable_fputs_unlocked.h"

int reps=0;
pthread_mutex_t mutices[4];

int oneRound();

// A broken dining philosophers implementation.
// Thread 3 gets lock 3, then lock 0.
// Still, deadlock is very unlikely, since timing has to be just right to
// demonstrate the problem. It's a race condition.


void *thread_func0(void *vptr_args){

    pthread_mutex_lock(&(mutices[0]));
    pthread_mutex_lock(&(mutices[1]));

	fputs_unlocked("Philosopher 0 is eating.\n", stdout);
    
    pthread_mutex_unlock(&(mutices[0]));
    pthread_mutex_unlock(&(mutices[1]));

    return NULL;
}


void *thread_func1(void *vptr_args){

    pthread_mutex_lock(&(mutices[1]));
    pthread_mutex_lock(&(mutices[2]));

 	fputs_unlocked("Philosopher 1 is eating.\n", stdout);
        
    pthread_mutex_unlock(&(mutices[1]));
    pthread_mutex_unlock(&(mutices[2]));

    return NULL;
}

void *thread_func2(void *vptr_args){

    
    pthread_mutex_lock(&(mutices[2]));
    pthread_mutex_lock(&(mutices[3]));

	fputs_unlocked("Philosopher 2 is eating.\n", stdout);
    
    pthread_mutex_unlock(&(mutices[2]));
    pthread_mutex_unlock(&(mutices[3]));
    
    return NULL;
}

void *thread_func3(void *vptr_args){

    pthread_mutex_lock(&(mutices[3]));
    pthread_mutex_lock(&(mutices[0]));

	fputs_unlocked("Philosopher 3 is eating.\n", stdout);

    pthread_mutex_unlock(&(mutices[3]));
    pthread_mutex_unlock(&(mutices[0]));
    
    return NULL;
}

 
int main( int argc, char *argv[]){

   if(argc != 2){
    
      printf("Usage: ./ex2 reps\n");
      return 1;
    }

	int reps = atoi(argv[1]);
	
	for (int i=0; i<reps; i++){
	
		oneRound();
	}

    return EXIT_SUCCESS;
}


int oneRound(){
    pthread_t thread[4];
    
    for (int i=0; i<4; i++){
        if (pthread_mutex_init(&(mutices[i]), NULL) != 0){
            return EXIT_FAILURE;
        }
    }
    
    
    printf("\nNietzsche, Euclid, Sartre, and Wittgenstein are seated.\n");
    
    if (pthread_create(&(thread[0]), NULL, thread_func0, NULL) != 0){
        return EXIT_FAILURE;
    }
 
    if (pthread_create(&(thread[1]), NULL, thread_func1, NULL) != 0){
        return EXIT_FAILURE;
    }

    if (pthread_create(&(thread[2]), NULL, thread_func2, NULL) != 0){
        return EXIT_FAILURE;
    }
 
    if (pthread_create(&(thread[3]), NULL, thread_func3, NULL) != 0){
        return EXIT_FAILURE;
    }    

    for (int i=0; i<4; i++){
        if (pthread_join(thread[i], NULL) != 0){
            
            return EXIT_FAILURE;
        } else {
            pthread_mutex_destroy(&(mutices[i]));
        }
    }
    
    return EXIT_SUCCESS;
}

