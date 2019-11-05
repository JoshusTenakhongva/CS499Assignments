#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#define SIZE 50
#define ITERATION_NUM 10
#define NUM_OF_THREADS 2

int i = -1; 
int iter;
double mat[SIZE][SIZE];
pthread_mutex_t lock; 
pthread_mutex_init( &lock, NULL ); 
int thread_1_iterations = 0; 
int thread_2_iterations = 0; 
pthread_t thread1;
pthread_t thread2; 

double do_crazy_computation(int i,int j);

int main(int argc, char **argv) 
	{
  // Create threads 
  double tstart_1; 
  double tstart_2; 
  double tend_1; 
  double tend_2; 
  
	double tstart; 
  for( iter = 0; iter < ITERATION_NUM; iter++ )
  	{
  	
  	if( pthread_create( &thread1, NULL, do_work, NULL))
			{
			
			fprintf( stderr, "Error while creating thread\n" ); 
			}
		
		if( pthread_create( &thread2, NULL, do_work, NULL))
			{
			
			fprintf( stderr, "Error while creating thread\n" ); 
			}
			
		pthread_join( thread1, NULL ); 
		pthread_join( thread2, NULL ); 
  	}
  double tend; 

  exit(0);
	}

//Crazy computation
double do_crazy_computation(int x,int y) {
   int iter;
   double value=0.0;

   for (iter = 0; iter < 5*x*x*x+1 + y*y*y+1; iter++) {
     value +=  (cos(x*value) + sin(y*value));
   }
  return value;
}

void* do_work()
	{
	
	double tstart=omp_get_wtime();
	double j; 
	
	pthread_mutex_lock( &lock ); 
	i++; 
	pthread_mutex_unlock( &lock ); 
	while (i<SIZE) 
		{ 
	  for (j=0;j<SIZE;j++) 
	  	{
	    mat[i][j] = do_crazy_computation(i,j);
	    fprintf(stderr,".");
	    }
	    
	  pthread_mutex_lock( &lock ); 
		i++; 
		pthread_mutex_unlock( &lock ); 
		}

  double tend=omp_get_wtime();
  double elapsed=tend - tstart;
  printf("Elapsed time: %f seconds\n",elapsed);
	}

