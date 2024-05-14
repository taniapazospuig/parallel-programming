#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define X_MAX 1.0
#define Y_MAX 1.0

int main(int argc, char* argv[])
{
    /// TODO Init MPI
    int rank, size;
    int provided;

    /// TODO
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    printf("Integral 2D: Rank %d of %d\n", rank, size);

    /// Check parameters
    if(argc != 4) {
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s filename Nx Ny\n", argv[0]);
        MPI_Finalize(); 
        return -1;
    }
    char* filename = argv[1];
    int Nx = (int)strtol(argv[2], NULL, 10);
    int Ny = (int)strtol(argv[3], NULL, 10);

    const double deltaX = 2.0*X_MAX/(double) Nx;
    const double deltaY = 2.0*Y_MAX/(double) Ny;

    /// TODO compute rank ranges

    int rows_per_rank = Nx / size; //how many rows each process will handle
    int remainder = Nx %size; //distribute if any leftover row
    int extra_rows; 
    if(rank < remainder) extra_rows = rank; 
    else extra_rows = remainder; 
    int start_row = rank * rows_per_rank + extra_rows; 
    int end_row = start_row + rows_per_rank - 1; 
    if(rank < remainder) end_row ++; 

    /// TODO

    /// TODO Read data
    int num_rows = end_row - start_row + 1; 
    double* data = (double*)malloc(num_rows * Ny * sizeof(double));

    MPI_File fh; 
    MPI_Offset offset = sizeof(double) * start_row * Ny; 
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh); 
    MPI_File_read_at_all(fh, offset, data, num_rows * Ny, MPI_DOUBLE, MPI_STATUS_IGNORE); 
    MPI_File_close(&fh); 
    
    /// TODO
    double result = 0.0; 
    double start_time = omp_get_wtime();
    /// TODO Compute integral

    #pragma omp parallel
    {
        double local_result = 0.0; 

        #pragma omp for collapse(2) nowait // both loops parallelized
        for(int i=0; i<num_rows; i++) {
            for(int j=0; j<Ny; j++) {
                int global_i = start_row + i; // compute the global index
                double x = -X_MAX + (global_i + 0.5)*deltaX;
                double y = -Y_MAX + (j + 0.5)*deltaY;
                result += data[i*Ny + j]*deltaX*deltaY;
            }
        }

        #pragma omp atomic
        result += local_result; 
    }

    
    /// TODO

    double run_time = omp_get_wtime() - start_time;
    double global_result = 0.0;
    MPI_Reduce(&result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    ///  Print results
    if(rank == 0)
    {
        printf("Result with N=%d M=%d size: %d threads: %d is %.12lf in %lf seconds\n", Nx, Ny,
            size, omp_get_max_threads(), result, run_time);
    }

    free(data);
    MPI_Finalize();
    return 0;
}
