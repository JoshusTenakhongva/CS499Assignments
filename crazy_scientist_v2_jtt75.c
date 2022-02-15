#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

#define SIZE 50
#define ITERATION_NUM 10

double do_crazy_computation(int i,int j);

int main(int argc, char **argv) {
  double mat[SIZE][SIZE];
  int i,j;
  int index; 
  double totalElapsedTime = 0.0; 
  double totalThread_1 = 0.0; 
  double totalThread_2 = 0.0; 
  double averageElapsedTime; 
  double averageThread_1; 
  double averageThread_2; 

	/*
	* Code to time 
	*/ 
	
	for( index = 0; index < ITERATION_NUM; index++ )
	{

  double tstart=omp_get_wtime();
  double elapsed_1; 
  double elapsed_2; 
  double tstart_1; 
  double tstart_2; 
  
  #pragma omp parallel shared( mat ) private( i, j )
  	{ // Pragma brackets 
  	
  	#pragma omp sections 
  		{
		
			#pragma omp section 
				{
				
				tstart_1=omp_get_wtime();
				
				for (i=0;i<SIZE/2;i++) /* loop over the rows */
					{ 
					for (j=0;j<SIZE;j++) /* loop over the columns */
						{  
						mat[i][j] = do_crazy_computation(i,j);
						fprintf(stderr,".");
						}
					}
					
				double tend_1=omp_get_wtime();
        elapsed_1=tend_1 - tstart;
				}
			#pragma omp section 
				{
				
				tstart_2=omp_get_wtime();
				
				for (i=SIZE/2;i<SIZE;i++) /* loop over the rows */
					{ 
					for (j=0;j<SIZE;j++) /* loop over the columns */
						{  
						mat[i][j] = do_crazy_computation(i,j);
						fprintf(stderr,".");
						}
					}
					
				double tend_2=omp_get_wtime();
        elapsed_2=tend_2 - tstart;
				}
			}
		}

  double tend=omp_get_wtime();
  double elapsed=tend - tstart;
  totalElapsedTime += elapsed; 
  totalThread_1 += elapsed_1; 
  totalThread_2 += elapsed_2; 
  double loadImbalance; 
  
  if( totalThread_1 > totalThread_2 )
    {
    
    loadImbalance = elapsed_1 - elapsed_2; 
    }
  else 
    {
    
    loadImbalance = elapsed_2 - elapsed_1; 
    }
  printf( "\nTotal Time (sanity check): %f seconds\n",elapsed );
  printf( "Time Thread1: %f\n", elapsed_1 ); 
  printf( "Time Thread2: %f\n", elapsed_2 ); 
  printf( "Load Imbalance: %f\n", loadImbalance ); 
  
  
  }
  /*
  * End code to time 
  */ 
  averageElapsedTime = totalElapsedTime / ITERATION_NUM;
  averageThread_1 = totalThread_1 / ITERATION_NUM; 
  averageThread_2 = totalThread_2 / ITERATION_NUM; 
  printf( "Average Elapsed Time: %f seconds\n", averageElapsedTime );
  printf( "Average Thread1 Time: %f seconds\n", averageThread_1 ); 
  printf( "Average Thread2 Time: %f seconds\n", averageThread_2 ); 

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

