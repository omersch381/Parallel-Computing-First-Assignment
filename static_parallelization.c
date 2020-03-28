#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define HEAVY 100000
#define SHORT 1
#define LONG 10


// This function performs heavy computations, 
// its run time depends on x and y values
 double heavy(int x, int y) {
       	int i, loop = SHORT;
       	double sum = 0;
       
// Super heavy tasks
	 if (x < 3 || y < 3)
		loop = LONG;
	 // Heavy calculations
	 for(i = 0;  i < loop*HEAVY;  i++)
          sum += cos(exp(sin((double)i/HEAVY)));

     return sum;             
}
 

int main(int argc, char *argv[]) {
       	int myid, numprocs;
		double result = -1, tempResult = -1; // -1 because I want to make sure that it works
		int tag = 0;
       	int N = 20;
      	double answer = 0;

		MPI_Status status;
  
		MPI_Init(&argc,&argv);
		MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
		MPI_Comm_rank(MPI_COMM_WORLD, &myid);

		
		if (myid == 0){
			double start = MPI_Wtime();
			int i;
			result = 0;
			tempResult = 0; // I could have avoided using mpi_send from master to slaves but parallelization time includes also the sending time.
			for (i = 0; i < numprocs - 1; i++)
				MPI_Send(&tempResult, 1, MPI_DOUBLE, i + 1, tag, MPI_COMM_WORLD);
			for (i = 0; i < numprocs - 1; i++)
			{
				MPI_Recv(&tempResult, 1, MPI_DOUBLE, i + 1, tag, MPI_COMM_WORLD, &status);
				result += tempResult;
			}
			double end = MPI_Wtime();
			printf("Final answer is  = %lf\n", result);
			double time = end - start;
			printf("time it took:  %lf\n", time);
		}
		else{
			MPI_Recv(&tempResult, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
			int amountOfJob = N / (numprocs - 1);
			int initial = (myid - 1) * amountOfJob; 
			int final = initial + amountOfJob - 1;
			int x, y;
			for (x = initial; x < final; x++)
				for (y = 0; y < N; y++){
					answer += heavy(x, y);
					tempResult += answer;
				}
			MPI_Send(&tempResult, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
		}
		
	   MPI_Finalize();
}

