#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>  // for EBUSY

#include "portable_fputs_unlocked.h"

int reps=0;

pthread_mutex_t theBowl;

int oneRound();

// A correct dining philosophers implementation.
// All four threads get locks/chopsticks in numerical order.
// But let's force the issue, and see if we can make a deadlock	


// Nietzsche sits at the top position.  	(thread 0)
// Euclid sits at the right side.			(thread 1)
// Sartre sits at the bottom position.		(thread 2)
// Wittgenstein sits at the left side.		(thread 3)

// Chopsticks are numbered 0-3.  Chopstick 0 is between Wittgenstein and Nietzsche, and 
// the rest are numbered in clockwise fashion.

// To demonstrate (the absence of) deadlock, we'll make sure that all available chopsticks 
// have been picked up before anyone tries to pick up a second chopstick. We'll introduce 
// a new rule:  Before picking up a second chopstick, diners must wait for the bowl of rice 
// to be placed on the table.
//
// In practice, we'll implement this new rule by creating a mutex that is owned by the 
// main thread.  We'll do this before calling pthread_create(). Threads must wait for (and
// release) this mutex before picking up the second chopstick.  


//We'll put the bowl down when three (not four) chopsticks are picked up. (this is all
// that are available, since two diners are contesting possession of one chopstick.


// Order of execution is still not deterministic. There's still a race condition, even
// though deadlock will not occur.  Who gets chopstick 0 first? Or chopstick 3 ...

typedef struct {

	pthread_mutex_t lock;
//	int held;  // no longer needed, since sticksHeld() now uses trylock()
} chopstick;


chopstick sticks[4];



void pickupStick(int s){

	pthread_mutex_lock(&(sticks[s].lock));
	//sticks[s].held = 1;
}


void putDownStick(int s){

	//sticks[s].held = 0;
	pthread_mutex_unlock(&(sticks[s].lock));
}


// return the number of sticks held by a philosopher. 
// Uses trylock() to determine if a lock is owned by another thread.
int sticksHeld(){
    int count = 0;
    for(int i=0; i<4; i++){
    
        if(pthread_mutex_trylock(&(sticks[i].lock))==EBUSY)
            count ++;
        else 
            pthread_mutex_unlock(&(sticks[i].lock));
    }
    
    return count;
}




void *thread_func0(void *vptr_args){    // Nietzsche 

	pickupStick(0);                     // pick up stick 0
	
	pthread_mutex_lock(&theBowl);		// wait until the bowl mutex is available. This
										// can't happen until main() releases the bowl 
										// mutex. main() won't do that until three 
										// chopsticks have been picked up.
										
										// One philosopher is waiting for another to 
										// finish with a chopstick, so only three (not 
										// four) will be picked up.
										
	pthread_mutex_unlock(&theBowl);		// immediately release the bowl. Another philosopher
										// can grab it now.
 
 	pickupStick(1);						// get the second stick

	fputs_unlocked("Philosopher 0 is eating.\n", stdout);  // eat
	fputs("0", stderr); // for checking eating order with the check script.

	putDownStick(0);   // Nietzsche is done eating.
    putDownStick(1);

    return NULL;
}


void *thread_func1(void *vptr_args){

    pickupStick(1);
    
    pthread_mutex_lock(&theBowl);
	pthread_mutex_unlock(&theBowl);
	
	pickupStick(2);
	
	fputs_unlocked("Philosopher 1 is eating.\n", stdout);
	fputs("1", stderr);
	
    putDownStick(1);
    putDownStick(2);

    return NULL;
}

void *thread_func2(void *vptr_args){

    pickupStick(2);

    pthread_mutex_lock(&theBowl);
	pthread_mutex_unlock(&theBowl);
	
	pickupStick(3);
	
	fputs_unlocked("Philosopher 2 is eating.\n", stdout);
	fputs("2", stderr);
	
    putDownStick(2);
    putDownStick(3);
    
    return NULL;
}

void *thread_func3(void *vptr_args){

    pickupStick(0);

    pthread_mutex_lock(&theBowl);
	pthread_mutex_unlock(&theBowl);
	
	pickupStick(3);

	fputs_unlocked("Philosopher 3 is eating.\n", stdout);
	fputs("3", stderr);
	
    putDownStick(3);
    putDownStick(0);
    
    return NULL;
}

 
int main( int argc, char *argv[]){

   if(argc != 2){
    
      printf("Usage: ./ex3 reps\n");
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
        if (pthread_mutex_init(&(sticks[i].lock), NULL) != 0){
            return EXIT_FAILURE;
        }
    }

	if (pthread_mutex_init(&theBowl, NULL) != 0){
		return EXIT_FAILURE;
	}

	pthread_mutex_lock(&theBowl);

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

	// threads could be:
	//      not even running yet
	// 		just starting up
	// 		waiting for a chopstick
	// 		waiting for the bowl
	
	// Let's put the system into a known state: All available sticks are picked up.
	
	// Because this is the non-deadlock example, one chopstick will remain on the table, 
	// since one philosopher is waiting for a stick used by another.
	
	// When three chopsticks are being held by the philosophers, put down the bowl. 
	while(sticksHeld() != 3); 
	
	printf("Putting down the bowl! Let's eat.\n");
	pthread_mutex_unlock(&theBowl);
	

    for (int i=0; i<4; i++){
        if (pthread_join(thread[i], NULL) != 0){
            
            return EXIT_FAILURE;
        } 
    }
 
    for (int i=0; i<4; i++){
 
        pthread_mutex_destroy(&(sticks[i].lock));
    }

    fputs("\n", stderr);
    
    pthread_mutex_destroy(&theBowl);
    
    return EXIT_SUCCESS;
}

