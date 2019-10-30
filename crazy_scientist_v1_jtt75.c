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
  double averageElapsedTime; 

	/*
	* Code to time 
	*/ 
	
	for( index = 0; index < ITERATION_NUM; index++ )
	{
  double tstart=omp_get_wtime();

  #pragma omp parallel shared( mat ) private( i, j )
  	{ // Pragma brackets 
  	
  	#pragma omp sections 
  		{
		
			#pragma omp section 
				{
				
				for (i=0;i<SIZE/2;i++) /* loop over the rows */
					{ 
					for (j=0;j<SIZE/2;j++) /* loop over the columns */
						{  
						mat[i][j] = do_crazy_computation(i,j);
						fprintf(stderr,".");
						}
					}
				}
			#pragma omp section 
				{
				
				for (i=SIZE/2;i<SIZE;i++) /* loop over the rows */
					{ 
					for (j=SIZE/2;j<SIZE;j++) /* loop over the columns */
						{  
						mat[i][j] = do_crazy_computation(i,j);
						fprintf(stderr,".");
						}
					}
				}
			}
		}

  double tend=omp_get_wtime();
  double elapsed=tend - tstart;
  totalElapsedTime += elapsed; 
  printf("Elapsed time: %f seconds\n",elapsed);
  
  }
  /*
  * End code to time 
  */ 
  averageElapsedTime = totalElapsedTime / ITERATION_NUM; 
  printf( "Average Elapsed Time: %f seconds\n", averageElapsedTime ); 

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

