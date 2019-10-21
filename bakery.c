#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <limits.h>

#define STRING_SIZE 80

// Struct prototype
typedef struct Thread_info
	{
	
	pthread_mutex_t lock; 
	
	int left_baker_count; 
	int right_baker_count; 
	int cautious_baker_count; 
	
	int leftMitts; 
	int rightMitts; 
	
	
	} Thread_info; 
	
// Enum Prototype
typedef enum BakerType { left, right, cautious } BakerType; 

// Function Prototypes
void* left_baker_work( void* info ); 
void* right_baker_work( void* info ); 
void* cautious_baker_work( void* info ); 
int findBakerID( Thread_info* info, BakerType type ); 
void workWait( int seed ); 

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
	
	thread_info.leftMitts = 3; 
	thread_info.rightMitts = 3; 
	
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
		
		workWait( 0 ); 
		printf( "left %d batch: %d\n", baker_id, batch );
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
		
		workWait( 0 ); 
		printf( "right %d batch: %d\n", baker_id, batch );
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
		
		workWait( 0 ); 
		}
	}
	
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
		printf( "left_baker %d\n", baker_id ); 
		info->left_baker_count++; 
		}
		
	else if( type == right )
		{
		
		// Save our baker ID
		baker_id = info->right_baker_count; 
		printf( "right_baker %d\n", baker_id ); 
		info->right_baker_count++; 
		}
		
	else
		{
		
		baker_id = info->cautious_baker_count; 
		printf( "cautious_baker %d\n", baker_id ); 
		info->cautious_baker_count++; 
		}

	// Unlock our lock 
	pthread_mutex_unlock( lock ); 
	
	return baker_id; 
	}
	
void workWait( int seed )
	{
	
	int workTime = 3.0 * (double) rand()/ (double)RAND_MAX; 
	workTime = ( 2 + workTime ) * 100000; 
	
	usleep( workTime ); 
	}
	
void retrieveMitts( BakerType type, Thread_info* mittRack )
	{
	
	// Check if the baker needs a left mitt
	
		// 
	
	// Check if the baker needs a right mitt
	
		// 
		
	// Check if the baker needs both
	
		// 
	}
	
	
	
	
//
