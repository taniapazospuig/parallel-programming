#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>

#define NUM_TEST 50
#define WAITING_TIME 1e6

void test_synchronous(int rank, int n);
void test_asynchronous(int rank, int n);

int main(int argc, char** argv)
{
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(size <= 2) {
        if (rank == 0)
            printf("Error, this program needs to be executed with more than 2 processors.\n");
        return 1;
    }

    /// FIRST TEST - SYNCHRONOUS COMMUNICATION
    test_synchronous(rank, size);

    /// FIRST TEST - ASYNCHRONOUS COMMUNICATION
    test_asynchronous(rank, size);

    MPI_Finalize();
    return 0;
}

void test_synchronous(int rank, int size)
{
    double start_time, run_time, average_time, max_time, min_time;
    int sbuf[NUM_TEST];
    int rbuf[NUM_TEST];
    MPI_Status status;

    /// TODO compute send/receive cores
    int send_core = (rank+1)%size;// rank of the processor to send data (%size for circular manner)
    int recv_core = (rank + size -1)%size; //rank of the processor to receive data from (add the size to ensure rank -1 positive)
    /// TODO

    start_time = MPI_Wtime(); // we start timer
    for(int i = 0; i < NUM_TEST; i++) { //each iter represents a communication round
        sbuf[i] = rank;

        /// TODO send
        MPI_Send(&sbuf[i], 1, MPI_INT, send_core, 0, MPI_COMM_WORLD); 
        //we send the rank of the current process stored in sbuf[i] to the next processor 

        if (i % size == rank) //each process enters usleep in a different iteration to simulate some delay
            usleep(WAITING_TIME);


        /// TODO recv
        MPI_Recv(&rbuf[i], 1, MPI_INT, recv_core, 0, MPI_COMM_WORLD, &status); 
        //we receive the rank of the previous processor

        assert(rbuf[i] == recv_core); //see if value received macthes expected value
    }
    run_time = MPI_Wtime() - start_time;

    /// TODO compute average, max and min times
    // use collective communication routines: result is placed in all tasks in the group
    MPI_Allreduce(&run_time, &average_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    average_time /= size; // we receive the sum of all the running times: divide to compute the average
    MPI_Allreduce(&run_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&run_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    
    if (rank == 0) { // only the first processor will print
        printf("Synchronous send/receive test with %d processes and %d repetitions.\n", size, NUM_TEST);
        printf("    average: %.2lf s\n", average_time);
        printf("    min:     %.2lf s\n", min_time);
        printf("    max:     %.2lf s\n", max_time);
    }

    MPI_Barrier(MPI_COMM_WORLD);

}

void test_asynchronous(int rank, int size)
{
    double start_time, run_time, average_time, max_time, min_time;
    int sbuf[NUM_TEST];
    int rbuf[NUM_TEST];

    /// TODO compute send/receive cores
    MPI_Request send_requests[NUM_TEST], recv_requests[NUM_TEST]; // array of send and receive requests
    MPI_Status recv_statuses[NUM_TEST]; // array of receive statuses

    int send_core = (rank+1)%size;
    int recv_core = (rank + size -1)%size;
    /// TODO

    start_time = MPI_Wtime();

    for(int i = 0; i < NUM_TEST; i++) {
        sbuf[i] = rank;

        /// TODO async send
        /// TODO
        MPI_Isend(&sbuf[i], 1, MPI_INT, send_core, 0, MPI_COMM_WORLD, &send_requests[i]);

        if (rank == i)
            usleep(WAITING_TIME);

        /// TODO async receive
        /// TODO
        MPI_Irecv(&rbuf[i], 1, MPI_INT, recv_core, 0, MPI_COMM_WORLD, &recv_requests[i]);
    }

    /// TODO wait for async send/receive
    /// TODO
    MPI_Waitall(NUM_TEST, send_requests, MPI_STATUSES_IGNORE);
    MPI_Waitall(NUM_TEST, recv_requests, recv_statuses);

    for(int i = 0; i < NUM_TEST; i++) {
        assert(rbuf[i] == recv_core);
    }

    run_time = MPI_Wtime() - start_time;

    /// TODO compute average, max and min times
    /// TODO
    MPI_Allreduce(&run_time, &average_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    average_time /= size; 
    MPI_Allreduce(&run_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&run_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("Asynchronous send/receive test with %d processes and %d repetitions.\n", size, NUM_TEST);
        printf("    average: %.2lf s\n", average_time);
        printf("    min:     %.2lf s\n", min_time);
        printf("    max:     %.2lf s\n", max_time);
    }

}
