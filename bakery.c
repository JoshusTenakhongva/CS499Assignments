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
	pthread_mutex_t line; 
	pthread_mutex_t leftMittens[ NUM_OF_LEFT_MITTENS ];
	//pthread_cond_t leftMittens[ NUM_OF_LEFT_MITTENS ]; 
	pthread_mutex_t rightMittens[ NUM_OF_RIGHT_MITTENS ];
	//pthread_cond_t rightMittens[ NUM_OF_RIGHT_MITTENS ]; 
	
	int left_baker_count; 
	int right_baker_count; 
	int cautious_baker_count; 
	
	int leftMittensCount; 
	int rightMittensCount; 
	
	int leftMittenQueues[ NUM_OF_LEFT_MITTENS ];
	int rightMittenQueues[ NUM_OF_RIGHT_MITTENS ]; 
	
	int leftQueueIndex; 
	int rightQueueIndex; 
	
	int randomizerSeed; 

	} Thread_info; 
	
// Enum Prototype
typedef enum BakerType { left, right, cautious } BakerType; 

// Function Prototypes
void* left_baker_work( void* info ); 
void* right_baker_work( void* info ); 
void* cautious_baker_work( void* info ); 
void* openMittenRack( void* info );

int findBakerID( Thread_info* info, BakerType type ); 

void workWait( int seed ); 
void cookiePrep( int seed, BakerType type, int id ); 
void removeCookies( BakerType type, int id ); 
void bakeCookies( int seed, BakerType type, int id );
void printBaker( BakerType type, int id ); 

int grabLeftMitten( BakerType type, int id, Thread_info* mittRack );
int grabRightMitten( BakerType type, int id, Thread_info* mittRack );
int grabMitten( BakerType type, int id, Thread_info* mittRack );
int chooseRandomQueue( int queueLength ); 

void returnLeftMitten( Thread_info* mittRack, int mittNumber, BakerType type, int id  );
void returnRightMitten( Thread_info* mittRack, int mittNumber, BakerType type, int id  );

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
	pthread_t rackOpener; 	
	
	// Initialize the amount of mitts are available to our bakers
	Thread_info thread_info; 
	
	thread_info.lock = mutex; 
	pthread_mutex_init( &( thread_info.line ), NULL );
	
	// Initialize the mitten condition variables
	for( index = 0; index < NUM_OF_LEFT_MITTENS; index++ )
		{
		
		pthread_mutex_init( &thread_info.leftMittens[ index ], NULL ); 
		//pthread_cond_init( &thread_info.leftMittens[ index ], NULL ); 
		}
	for( index = 0; index < NUM_OF_RIGHT_MITTENS; index++ )
		{
		
		pthread_mutex_init( &thread_info.rightMittens[ index ], NULL ); 
		//pthread_cond_init( &thread_info.rightMittens[ index ], NULL ); 
		}
	
	thread_info.leftMittensCount = NUM_OF_LEFT_MITTENS; 
	thread_info.rightMittensCount = NUM_OF_RIGHT_MITTENS; 
	
	thread_info.left_baker_count = 0; 
	thread_info.right_baker_count = 0; 
	thread_info.cautious_baker_count = 0; 
	
	thread_info.leftQueueIndex = 0; 
	thread_info.rightQueueIndex = 0; 
	
	thread_info.randomizerSeed = bakerySeed; 
	
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
	
	/*
	if( pthread_create( &rackOpener, NULL, openMittenRack, ( void* ) ( &thread_info )))
		{
			
		fprintf( stderr, "Error while creating thread\n" ); 
		} 
		*/ 
	
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
	//pthread_join( rackOpener, NULL );	
	
	}
	
	
/********************************
* Functions
*****************************/ 

void* left_baker_work( void* info )
	{
	
	// Cast our thread info back into a struct
	Thread_info* thread_info = ( Thread_info* ) info; 
	
	// Initialize variables
	int baker_id; 
	int batch; 
	int mitten; 
	int randomSeed = thread_info->randomizerSeed;
	BakerType bakerType = left; 

	// Save our lock
	pthread_mutex_t* lock = &(thread_info->lock); 
	
	// Find our Baker's ID
	baker_id = findBakerID( thread_info, bakerType ); 
	
	// Loop through until our baker is done working
	for( batch = 0; batch < 10; batch++ )
		{

		cookiePrep( randomSeed, bakerType, baker_id ); 
		mitten = grabLeftMitten( bakerType, baker_id, thread_info );  
		bakeCookies( randomSeed, bakerType, baker_id ); 
		removeCookies( bakerType, baker_id );
		returnLeftMitten( thread_info, mitten, bakerType, baker_id ); 
		}
	}
	
void* right_baker_work( void* info ) 
	{
	
	// Cast our thread info back into a struct
	Thread_info* thread_info = ( Thread_info* ) info; 
	
	// Initialize variables
	int baker_id; 
	int batch; 
	int mitten; 
	int randomSeed = thread_info->randomizerSeed; 
	BakerType bakerType = right; 

	
	// Save our lock
	pthread_mutex_t* lock = &(thread_info->lock); 
	
	// Find our baker's ID
	baker_id = findBakerID( thread_info, bakerType ); 
	
	// Loop through until our baker is done working
	for( batch = 0; batch < 10; batch++ )
		{
		
		cookiePrep( randomSeed, bakerType, baker_id ); 
		mitten = grabRightMitten( bakerType, baker_id, thread_info );  
		bakeCookies( randomSeed, bakerType, baker_id ); 
		removeCookies( bakerType, baker_id );
		returnRightMitten( thread_info, mitten, bakerType, baker_id ); 
		}
	}
	
	
void* cautious_baker_work( void* info ) 
	{
	
	// Cast our thread info back into a struct
	Thread_info* thread_info = ( Thread_info* ) info; 
	
	// Initialize variables
	int baker_id; 
	int batch; 
	int leftMitten;
	int rightMitten; 
	int randomSeed = thread_info->randomizerSeed; 
	BakerType bakerType = cautious; 
	
	// Save our lock
	pthread_mutex_t* lock = &(thread_info->lock); 
	
	// Find our baker's ID
	baker_id = findBakerID( thread_info, bakerType ); 
	
	// Loop through until our baker is done working
	for( batch = 0; batch < 10; batch++ )
		{
		
		cookiePrep( randomSeed, bakerType, baker_id ); 
		
		leftMitten = grabLeftMitten( bakerType, baker_id, thread_info );  
		rightMitten = grabRightMitten( bakerType, baker_id, thread_info );  
		
		bakeCookies( randomSeed, bakerType, baker_id ); 
		removeCookies( bakerType, baker_id );
		returnLeftMitten( thread_info, leftMitten, bakerType, baker_id ); 
		returnRightMitten( thread_info, rightMitten, bakerType, baker_id ); 
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
	
	// seed our randomizer
	srand( seed ); 
	
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
	
void removeCookies( BakerType type, int id )
	{
	
	printBaker( type, id ); 
	printf( "has taken cookies out of the oven...\n" ); 
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
	int chosenQueue; 
	int* queuePosition;
	pthread_mutex_t* lock = &( mittRack->lock ); 
	pthread_mutex_t* upNext = &( mittRack->line ); 
	int* queue = mittRack->leftMittenQueues;
	int queueLength = mittRack->leftMittensCount; 
	
	// Print that our baker wants a left mitten 
	printBaker( type, id ); 
	printf( "wants a left-handed mitt...\n" ); 
	
	/* Figure out which queue we want to get into */ 
	
	// Wait in the queue
	pthread_mutex_lock( upNext ); 
	
	// Choose a random queue
	queuePosition = &( mittRack->leftQueueIndex ); 
	chosenQueue = *queuePosition; 
	*queuePosition = *queuePosition + 1; 
	
	if( *queuePosition == queueLength )
		{
		
		*queuePosition = 0; 
		}
	
	// Exit the queue
	pthread_mutex_unlock( upNext ); 
	
	/* Get into the front of the line */ 

	// Wait for our chosen mitten 
	pthread_mutex_lock( &(mittRack->leftMittens[ chosenQueue ]) ); 
	//pthread_cond_wait( &(mittRack->leftMittens[ chosenQueue ]), lock ); 
	
	// Print that our baker has got a left mitten 
	printBaker( type, id ); 
	printf( "has got a left-handed mitt...\n" ); 
	
	// Return our mitten number 
	return chosenQueue;  
	}	
	
int grabRightMitten( BakerType type, int id, Thread_info* mittRack )
	{
	
	// Initialize variables
	int chosenQueue; 
	int* queuePosition;
	pthread_mutex_t* lock = &( mittRack->lock ); 
	pthread_mutex_t* upNext = &( mittRack->line ); 
	int* queue = mittRack->rightMittenQueues;
	int queueLength = mittRack->rightMittensCount; 
	
	// Print that our baker wants a left mitten 
	printBaker( type, id ); 
	printf( "wants a right-handed mitt...\n" ); 
	
	/* Figure out which queue we want to get into */ 
	
	// Wait in the queue
	pthread_mutex_lock( upNext ); 
	
	// Choose a random queue
	queuePosition = &( mittRack->rightQueueIndex ); 
	chosenQueue = *queuePosition; 
	*queuePosition = *queuePosition + 1; 
	
	if( *queuePosition == queueLength )
		{
		
		*queuePosition = 0; 
		}
	
	// Exit the queue
	pthread_mutex_unlock( upNext ); 
	
	/* Get into the front of the line */ 

	// Wait for our chosen mitten 
	pthread_mutex_lock( &(mittRack->rightMittens[ chosenQueue ]) ); 
	//pthread_cond_wait( &(mittRack->leftMittens[ chosenQueue ]), lock ); 
	
	// Print that our baker has got a left mitten 
	printBaker( type, id ); 
	printf( "has got a right-handed mitt...\n" ); 
	
	// Return our mitten number 
	return chosenQueue;  
	}
	
void returnLeftMitten( Thread_info* mittRack, int mittNumber, BakerType type, int id  )
	{

	printBaker( type, id ); 
	printf( "has put back a left-handed mitt...\n" ); 
	pthread_mutex_unlock( &(mittRack->leftMittens[ mittNumber ]) ); 
	//pthread_cond_signal( &(mittRack->leftMittens[ mittNumber ]) ); 
	}
	
void returnRightMitten( Thread_info* mittRack, int mittNumber, BakerType type, int id )
	{
	
	printBaker( type, id ); 
	printf( "has put back a right-handed mitt...\n" ); 
	pthread_mutex_unlock( &(mittRack->rightMittens[ mittNumber ] ) ); 
	//pthread_cond_signal( &(mittRack->rightMittens[ mittNumber ]) ); 
	}
	
	/*
void* openMittenRack( void* info )
	{
	
	Thread_info* mittRack = ( Thread_info* ) info; 
	int index; 
	int leftMittRackSize = mittRack->leftMittensCount; 
	int rightMittRackSize = mittRack->rightMittensCount; 
	usleep( 500000 );
	
	for( index = 0; index < leftMittRackSize; index++ )
		{
		
		pthread_cond_signal( &(mittRack->leftMittens[ index ]) );
		}
		
	for( index = 0; index < rightMittRackSize; index++ )
		{
		
		pthread_cond_signal( &(mittRack->rightMittens[ index ]) ); 
		}	
	}*/ 
	
//
