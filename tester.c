#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <limits.h>
pthread_cond_t pingHit = PTHREAD_COND_INITIALIZER; 
pthread_cond_t pongHit = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

void* ping()
	{
	
	while( true )
		{
		pthread_cond_wait( &pongHit, &lock ); 
		printf( "ping\n" ); 
		usleep( 500000 ); 
		pthread_cond_signal( &pingHit ); 
		}
	}
	
void* pong()
	{
	while( true )
		{
		pthread_cond_wait( &pingHit, &lock ); 
		printf( "pong\n" ); 
		usleep( 500000 ); 
		pthread_cond_signal( &pongHit ); 
		}
	}
	
void* serve()
	{
	
	//usleep( 500000 ); 
	pthread_cond_signal( &pongHit ); 
	}

int main( int argc, char* argv[] )
	{

	pthread_t thread1; 
	pthread_t thread2; 
	pthread_t thread3; 
	
	if( pthread_create( &thread1, NULL, ping, NULL ))
		{
		
		fprintf( stderr, "Error while creating thread\n" ); 
		}
		
	if( pthread_create( &thread2, NULL, pong, NULL ))
		{
		
		fprintf( stderr, "Error while creating thread\n" ); 
		}
		
/*
	if( pthread_create( &thread3, NULL, serve, NULL ))
		{
		
		fprintf( stderr, "Error while creating thread\n" ); 
		} */ 
		
	pthread_join( thread1, NULL ); 
	pthread_join( thread2, NULL ); 
	//pthread_join( thread3, NULL ); 
	
	pthread_cond_signal( &pongHit ); 
	}
	

