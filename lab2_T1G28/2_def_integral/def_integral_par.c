#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

double function(double x) // define f(x)
{
    return x*sin(x);
}

double exact_quadrature(double x)
{
    return sin(x) - x*cos(x); // computes exact value of the inegral
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv); // intialize parallel region
    int rank, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get the rank of current process
    MPI_Comm_size(MPI_COMM_WORLD, &nproc); // get the number of processes

    /// Check parameters
    if(argc != 3) {
        if(rank == 0) { // root process
            fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    int N = (int)strtol(argv[1], NULL, 10); // strtol converts string argv[1] to long integer
    double X_MAX = (double) strtold(argv[2], NULL); //strold converts string argv[2] to double
    double deltaX = X_MAX/(double) N; // compute step size (increment of x)

    clock_t start_time = clock();

    /// Compute integral
    double local_result = 0; // sum for each process
    int i;

    for (i = rank+1; i <= N; i+= nproc) { // iterate over each subinterval from 1 to N
        // after each iteration, increment i by total number of processes
        // so that each process handles disjoint intervals
        double x_middle = (i - 0.5) * deltaX; // compute midpoint of subinterval
        local_result = local_result + function(x_middle) * deltaX; // add value of the function at midpoint to result
    }

    double total_result;
    MPI_Reduce(&local_result, &total_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // perform sum on the root process

    if (rank == 0) {
        // Root computes error and prints results
        double exact = exact_quadrature(X_MAX); // compute exact integral
        double error = fabs(total_result - exact); // abs difference between computed result and exact result
        double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        printf("Parallel result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, total_result, exact, error, run_time);
    }

    MPI_Finalize();
    return 0;
}