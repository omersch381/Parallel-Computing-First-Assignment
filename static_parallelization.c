#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define HEAVY 100000
#define SHORT 1
#define LONG 10

//////////////////////////////////////?///////////////////?///////////////////?///////////////////
TODO: change the type of result and tempResult, also in mpiReceive and mpiSend
///////////////////?///////////////////?///////////////////?///////////////////?///////////////////?


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
       	int myid, numprocs
		int result = 0, tempResult = 0; // Master will recieve a temp from each process and sum all results in result
		int tag = 0;
		int x, y;
       	int N = 20;
      	double answer = 0;

		MPI_Status status;
  
		MPI_Init(&argc,&argv);
		MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
		MPI_Comm_rank(MPI_COMM_WORLD, &myid);

		if (myid == 0){
			int i;
			for (i = 0; i < numprocs - 1; i++)
			{
				MPI_Recv(tempResult, 1, MPI_INT, i + 1, tag, MPI_COMM_WORLD, &status);
				result += tempResult;
				printf("answer = %e\n", answer);
			}
		}
		else{
			int amountOfJob = N / (numprocs - 1);
			int initial = (myid - 1) * amountOfJob; 
			int final = initial + amountOfJob - 1;
			for (x = initial; x < final; x++)
				for (y = 0; y < N; y++){
					answer += heavy(x, y);
					tempResult += answer;
				}
			MPI_Send(tempResult, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		}
		
	   MPI_Finalize();
}

