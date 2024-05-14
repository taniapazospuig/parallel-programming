#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

double function(double x)
{
    return x*sin(x);
}

double exact_integral(double x)
{
    return sin(x) - x*cos(x);
}

int main(int argc, char* argv[]) //it takes command line arguments N and XMAX
{ // N is the number of intervals for numerical intergration --> defines the number of iterations 
  // XMAX is the upper limit of the integration range (defines intergration domain)

    MPI_Init(&argc, &argv); //we initialize MPI execution environment

    //we get the rank of the current process and the total number of processes
    int rank, size; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    /// Check parameters
    if(argc != 3) {
        if(rank == 0){ // we only need one of the processes to check this, for convenience the root process will do it
            fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        }
        MPI_Finalize(); //if the command line arguments is incorrect we finalize the MPI environment
        return -1;
    }

    int N = (int)strtol(argv[1], NULL, 10); // convert to int
    double X_MAX = (double) strtold(argv[2], NULL); // convert to double
    double deltaX = X_MAX/(double) N; // compute deltaX (width of each integration interval)

    
    clock_t start_time = clock();

    double local_integral = 0.0; // local_integral is a local variable allocated by each of the MPI processes
    int i; 
    for (i = rank + 1; i <= N; i+= size) {
        double x = (i - 0.5) * deltaX;
        local_integral += deltaX * function(x);
    }

    double global_integral = 0.0; //global_integral will be the total real value of the resulting integral 
    MPI_Reduce(&local_integral, &global_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    
    if(rank == 0){ //we only need one process to complete the final integral and the root has all of the results 
        double def_result = global_integral; //definitive result over all the integral
        double exact = exact_integral(X_MAX); //the exact result of the integral 
        double error = fabs(def_result - exact); //error in absolute value 
        double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        
        printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, def_result, exact, error, run_time);
    
        /// Print integral data
        char filename_dat[100];
        sprintf(filename_dat, "indef_integral_par.dat"); //print integral data in binary file
        FILE* file_dat = fopen(filename_dat, "wb");
        if (file_dat != NULL) {
            fwrite(&def_result, sizeof(def_result), 1, file_dat);
            fclose(file_dat);
        }
    
    
        /// Print integral info
        char filename_info[100];
        sprintf(filename_info, "indef_integral_par.info");
        FILE* file_info = fopen(filename_info, "w");
        if (file_dat != NULL) {
            fprintf(file_info, "# %s\n", "Indefinite integral version parallel");
            fprintf(file_info, "data %s\n", filename_dat);
            fprintf(file_info, "N %d\n", N);
            fprintf(file_info, "XMAX %lf\n", X_MAX);
            fprintf(file_info, "size %d\n", size); //only one integral 
            fclose(file_info);
        }
    }

    MPI_Finalize(); 
    return 0;
}
