#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <limits.h>

#define STRING_SIZE 80
#define NUM_OF_LEFT_MITTENS 3
#define NUM_OF_RIGHT_MITTENS 3

// Struct prototype
typedef struct Thread_info
	{
	
	pthread_mutex_t lock; 
	pthread_cond_t leftMittens[ NUM_OF_LEFT_MITTENS ]; 
	pthread_cond_t rightMittens[ NUM_OF_RIGHT_MITTENS ]; 
	
	int left_baker_count; 
	int right_baker_count; 
	int cautious_baker_count; 
	
	int leftMittensCount; 
	int rightMittensCount; 
	
	int leftMittenQueues[ NUM_OF_LEFT_MITTENS ];
	int rightMittenQueues[ NUM_OF_RIGHT_MITTENS ]; 

	} Thread_info; 
	
// Enum Prototype
typedef enum BakerType { left, right, cautious } BakerType; 

// Function Prototypes
void* left_baker_work( void* info ); 
void* right_baker_work( void* info ); 
void* cautious_baker_work( void* info ); 

int findBakerID( Thread_info* info, BakerType type ); 

void workWait( int seed ); 
void cookiePrep( int seed, BakerType type, int id ); 
void bakeCookies( int seed, BakerType type, int id );
void printBaker( BakerType type, int id ); 

int grabLeftMitten( BakerType type, int id, Thread_info* mittRack );
int grabRightMitten( BakerType type, int id, Thread_info* mittRack );
int grabMitten( BakerType type, int id, Thread_info* mittRack );
int findShortestQueue( int queue[], int queueLength ); 

/*
* Main Function
*/ 
int main( int argc, char* argv[] )
	{
	
	int index; 
	pthread_mutex_t mutex; 
	pthread_mutex_init( &mutex, NULL ); 
	
	// Check if there are enough command line arguments
	if( argc != 5 )
		{
		
		printf( "Invalid command-line arguments... Aborting\n" ); 
		return 1; 
		}
	// Check if any of the command line arguments are zero or below
	for( index = 1; index < argc; index++ )
		{
		
		
		if( atoi( argv[ index ] ) <= 0 )
			{
	
			printf( "Invalid command-line arguments... Aborting\n" ); 
			return 1; 
			}
		} 
		
	// Save the number of each baker
	int leftBakerCount = atoi( argv[ 1 ] ); 
	int rightBakerCount = atoi( argv[ 2 ] ); 
	int cautiousBakerCount = atoi( argv[ 3 ] );
	int bakerySeed = atoi( argv[ 4 ] ); 
	
	// Initialize baker thread arrays
	pthread_t leftBakers[ leftBakerCount ]; 
	pthread_t rightBakers[ rightBakerCount ]; 
	pthread_t cautiousBakers[ cautiousBakerCount ]; 	
	
	// Initialize the amount of mitts are available to our bakers
	Thread_info thread_info; 
	
	thread_info.lock = mutex; 
	
	// Initialize the mitten condition variables
	for( index = 0; index < NUM_OF_LEFT_MITTENS; index++ )
		{
		
		pthread_cond_init( &thread_info.leftMittens[ index ], NULL ); 
		}
	for( index = 0; index < NUM_OF_RIGHT_MITTENS; index++ )
		{
		
		pthread_cond_init( &thread_info.rightMittens[ index ], NULL ); 
		}
	
	thread_info.leftMittensCount = NUM_OF_LEFT_MITTENS; 
	thread_info.rightMittensCount = NUM_OF_RIGHT_MITTENS; 
	
	thread_info.left_baker_count = 0; 
	thread_info.right_baker_count = 0; 
	thread_info.cautious_baker_count = 0; 
	
	// Initialize left handed baker threads
	for( index = 0; index < leftBakerCount; index++ )
		{
		
		if( pthread_create( &leftBakers[ index ], NULL, left_baker_work, ( void* ) ( &thread_info )))
			{
			
			fprintf( stderr, "Error while creating thread\n" ); 
			}
		}
	
	// Initialize right handed baker threads
	for( index = 0; index < rightBakerCount; index++ )
		{
		
		if( pthread_create( &rightBakers[ index ], NULL, right_baker_work, ( void* ) ( &thread_info )))
			{
			
			fprintf( stderr, "Error while creating thread\n" ); 
			}
		}
	
	// Initialize cautious baker threads
	for( index = 0; index < cautiousBakerCount; index++ )
		{
		
		if( pthread_create( &cautiousBakers[ index ], NULL, cautious_baker_work, ( void* ) ( &thread_info )))
			{
			
			fprintf( stderr, "Error while creating thread\n" ); 
			}
		}
	
	// Join threads
	for( index = 0; index < leftBakerCount; index++ )
		{
		
		pthread_join( leftBakers[ index ], NULL ); 
		}
	for( index = 0; index < rightBakerCount; index++ )
		{
		
		pthread_join( rightBakers[ index ], NULL ); 
		}
	for( index = 0; index < cautiousBakerCount; index++ )
		{
		
		pthread_join( cautiousBakers[ index ], NULL ); 
		}
	}
	
	
/********************************
* Functions
*****************************/ 

void* left_baker_work( void* info )
	{
	
	// Initialize variables
	int baker_id; 
	int batch; 
	BakerType bakerType = left; 
	
	// Cast our thread info back into a struct
	Thread_info* thread_info = ( Thread_info* ) info; 
	
	// Save our lock
	pthread_mutex_t* lock = &(thread_info->lock); 
	
	// Find our Baker's ID
	baker_id = findBakerID( thread_info, bakerType ); 
	
	// Loop through until our baker is done working
	for( batch = 0; batch < 10; batch++ )
		{
		
		cookiePrep( 0, bakerType, baker_id ); 
		grabLeftMitten( bakerType, baker_id, thread_info ); 
		}
	}
	
void* right_baker_work( void* info ) 
	{
	
	// Initialize variables
	int baker_id; 
	int batch; 
	BakerType bakerType = right; 
	
	// Cast our thread info back into a struct
	Thread_info* thread_info = ( Thread_info* ) info; 
	
	// Save our lock
	pthread_mutex_t* lock = &(thread_info->lock); 
	
	// Find our baker's ID
	baker_id = findBakerID( thread_info, bakerType ); 
	
	// Loop through until our baker is done working
	for( batch = 0; batch < 10; batch++ )
		{
		
		cookiePrep( 0, bakerType, baker_id ); 
		}
	}
	
	
void* cautious_baker_work( void* info ) 
	{
	
	// Initialize variables
	int baker_id; 
	int batch; 
	BakerType bakerType = cautious; 
	
	// Cast our thread info back into a struct
	Thread_info* thread_info = ( Thread_info* ) info; 
	
	// Save our lock
	pthread_mutex_t* lock = &(thread_info->lock); 
	
	// Find our baker's ID
	baker_id = findBakerID( thread_info, bakerType ); 
	
	// Loop through until our baker is done working
	for( batch = 0; batch < 10; batch++ )
		{
		
		cookiePrep( 0, bakerType, baker_id ); 
		}
	}
	
/*
* During initialization, finds the baker's number and returns it
*/ 
int findBakerID( Thread_info* info, BakerType type )
	{
	
	int baker_id; 
	
	// Save our lock
	pthread_mutex_t* lock = &( info->lock ); 
	
	// Lock the lock while we assign baker IDs
	pthread_mutex_lock( lock ); 
	
	// Save our baker ID based on what bakertype we have
	if( type == left )
		{
		
		// Save our baker ID
		baker_id = info->left_baker_count; 
		//printf( "left_baker %d\n", baker_id ); 
		info->left_baker_count++; 
		}
		
	else if( type == right )
		{
		
		// Save our baker ID
		baker_id = info->right_baker_count; 
		//printf( "right_baker %d\n", baker_id ); 
		info->right_baker_count++; 
		}
		
	else
		{
		
		baker_id = info->cautious_baker_count; 
		//printf( "cautious_baker %d\n", baker_id ); 
		info->cautious_baker_count++; 
		}

	// Unlock our lock 
	pthread_mutex_unlock( lock ); 
	
	return baker_id; 
	}
	
/*
* Has the baker sleep while they do prework or wait for the cookies to bake
*/ 
void workWait( int seed )
	{
	
	// Determine the random section of our sleep time 
	int workTime = 3.0 * (double) rand()/ (double)RAND_MAX; 
	
	// Add the random time section to our minimum amount of time to sleep
	workTime = ( 2 + workTime ) * 100000; 
	
	// Sleep for our calculated amount of time 
	usleep( workTime ); 
	}

/*
* Prints that the baker is preparing the cookies to be baked and sleeps
*/ 
void cookiePrep( int seed, BakerType type, int id )
	{
	
	printBaker( type, id ); 
	printf( "is working...\n" ); 
	workWait( seed ); 
	}
	
/*
* Prints that the baker has put the cookies in the oven and sleeps
*/ 
void bakeCookies( int seed, BakerType type, int id )
	{
	
	printBaker( type, id ); 
	printf( "has put cookies in the oven and is waiting...\n" ); 
	workWait( seed ); 
	}
	
/* 
* Prints the prefix for every line with what baker is doing the action
*/ 
void printBaker( BakerType type, int id )
	{
	
	switch( type )
		{
		
		case left:
			printf( "[Left-handed baker %d] ", id ); 
			break; 
		
		case right:
			printf( "[Right-handed baker %d] ", id ); 
			break; 
		
		case cautious:
			printf( "[Cautious baker %d] ", id ); 
			break; 
		
		default: 
			printf( "=====================\n    There was an issue\n============\n" ); 
		}
	}
	
int grabLeftMitten( BakerType type, int id, Thread_info* mittRack )
	{
	
	// Initialize variables
	int shortestQueueIndex; 
	pthread_mutex_t* upNext = &( mittRack->lock ); 
	int* queue = mittRack->leftMittenQueues;
	int queueLength = mittRack->leftMittensCount; 
	
	// Print that our baker wants a left mitten 
	printBaker( type, id ); 
	printf( "wants a left-handed mitt...\n" ); 
	
	// Lock our queue 
	pthread_mutex_lock( upNext ); 
	
	// Find the shortest mitten queue 
	shortestQueueIndex = findShortestQueue( queue, queueLength ); 
	
	// 
	
	// Unlock our queue
	pthread_mutex_unlock( upNext ); 
	
	// Print that our baker has got a left mitten 
	printBaker( type, id ); 
	printf( "has got a left-handed mitt...\n" ); 
	
	// Return our mitten number 
	return shortestQueueIndex; 
	}	
	
int grabRightMitten( BakerType type, int id, Thread_info* mittRack )
	{
	
	
	// Print that our baker wants a right mitten 
	
	// Print that our baker has got a right mitten 
	}
	
int grabMitten( BakerType type, int id, Thread_info* mittRack )
	{
	
	
	}
	
int findShortestQueue( int queue[], int queueLength )
	{
	
	int index; 
	int shortestQueueIndex; 
	
	for( index = 0; index < queueLength; index++ )
		{
		
		if( index != 0 )
			{
			
			if( queue[ index ] < shortestQueueIndex )
				{
				
				shortestQueueIndex = index; 
				}
			}
			
		else
			{
			
			shortestQueueIndex = queue[ index ]; 
			}
		}
		
	return shortestQueueIndex; 
	}
	
//
