#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define HEAVY 100000
#define SHORT 1
#define LONG 10
#define TAG 0
#define N 20

double heavy(int x, int y);
void masterJob(int myid, int numprocs);
void slaveJob(int myid, int numprocs);


int main(int argc, char *argv[]) {
	int myid, numprocs;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (myid == 0)
		masterJob(myid, numprocs);
	else
		slaveJob(myid, numprocs);
	
	MPI_Finalize();
}

void masterJob(int myid, int numprocs){
	MPI_Status status;
    double start = MPI_Wtime();
	int i;
	double result = 0, tempResult = 0;
    for (i = 0; i < numprocs - 1; i++)
        MPI_Send(&tempResult, 1, MPI_DOUBLE, i + 1, TAG, MPI_COMM_WORLD);
    for (i = 0; i < numprocs - 1; i++)
    {
        MPI_Recv(&tempResult, 1, MPI_DOUBLE, i + 1, TAG, MPI_COMM_WORLD, &status);
        result += tempResult;
    }
       
	double end = MPI_Wtime();
	printf("Final answer is  = %lf\n", result);
	double time = end - start;
	printf("time it took:  %lf\n", time);
}

void slaveJob(int myid, int numprocs){
	MPI_Status status;
    double tempResult;
	MPI_Recv(&tempResult, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);
	int amountOfJob = N / (numprocs - 1);
	int initial = (myid - 1) * amountOfJob; 
	int final = initial + amountOfJob;
	int x, y;
	for (x = initial; x < final; x++)
		for (y = 0; y < N; y++)
			tempResult += heavy(x, y);
	MPI_Send(&tempResult, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
}

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



