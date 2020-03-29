#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define HEAVY 100000
#define SHORT 1
#define LONG 10
#define TAG 0
#define N 20
#define BUFFER_SIZE 100

double masterBody(int *x, int *y);
void masterInit(int numprocs, int *x, int *y);
int pack(char *buffer, int *x, int* y);
void unpack(char *buffer, int *x, int* y);
double heavy(int x, int y);
void masterJob(int myid, int numprocs);
void slaveJob(int myid, int numprocs);
void updateXandY(int* x, int* y);

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
    double start = MPI_Wtime();
	double result = 0;
    int x = N - 1, y = N - 1;
    
    masterInit(numprocs, &x, &y);
   
    result = masterBody(&x, &y);
    
	double end = MPI_Wtime();
	printf("Final answer is  = %e\n", result);
	double time = end - start;
	printf("time it took:  %lf\n", time);
}

void masterInit(int numprocs, int *x, int *y){
    int position, i;
    char buffer[BUFFER_SIZE];

	for (i = 0; i < numprocs - 1; i++){
        position = pack(buffer, x, y);
        MPI_Send(buffer, position, MPI_PACKED, i + 1, TAG, MPI_COMM_WORLD);
        updateXandY(x,y);
    }
    (*y)++;// otherwise it will start from --y
}

double masterBody(int *x, int *y){
    MPI_Status status;
    int i, position;
    double tempResult, result = 0;
    char buffer[BUFFER_SIZE];
	for (i = 0; i < N * N - 1; i++)
	{
		MPI_Recv(&tempResult, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
        result += tempResult;
        updateXandY(x,y);
        position = pack(buffer, x, y);
        MPI_Send(buffer, position, MPI_PACKED, status.MPI_SOURCE, TAG, MPI_COMM_WORLD);
	}
    result += tempResult; // the slaves skip (x=0, y=0) iteration so I add it one last time
    return result;
}

void updateXandY(int* x, int* y){
    *y -= 1;
        if(*y == -1){
            (*x)--;
            *y += N;
        }
        if(*x < 0){
            *x = 0;
            *y = 0;
        }
}

void slaveJob(int myid, int numprocs){
	MPI_Status status;
    int x, y;
    char buffer[BUFFER_SIZE];
    double tempResult;
    MPI_Recv(buffer, BUFFER_SIZE, MPI_PACKED, 0, 0, MPI_COMM_WORLD, &status);
    unpack(buffer, &x, &y);
    while(1){
        tempResult = heavy(x, y);
        if (x == 0 && y == 0)
            break;
        MPI_Send(&tempResult, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
        MPI_Recv(buffer, BUFFER_SIZE, MPI_PACKED, 0, TAG, MPI_COMM_WORLD, &status);
        unpack(buffer, &x, &y);
    }
}

int pack(char *buffer, int *x, int* y){
    int position = 0;
    MPI_Pack(x, 1, MPI_INT, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    MPI_Pack(y, 1, MPI_INT, buffer, BUFFER_SIZE, &position, MPI_COMM_WORLD);
    return position;
}

void unpack(char *buffer, int *x, int* y){
    int position = 0;
    MPI_Unpack(buffer, BUFFER_SIZE, &position, x, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, BUFFER_SIZE, &position, y, 1, MPI_INT, MPI_COMM_WORLD);
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
