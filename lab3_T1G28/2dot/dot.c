#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>

double dot_product_cpu(int n, double* x, double* y)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += x[i] * y[i];
    }
    return sum;
}

double dot_product_gpu(int n, double* x, double* y)
{
    double sum = 0.0;
    #pragma acc parallel loop reduction(+:sum) present(x[0:n], y[0:n])
    for (int i = 0; i < n; i++) {
        sum += x[i] * y[i];
    }
    return sum;
}

int main(int argc, char **argv)
{
    int     vec_size = atoi(argv[1]);
    double  dot_cpu, dot_gpu;
    double  time_start, time_end, time_cpu, time_gpu;

    double* x = (double*) malloc (vec_size*sizeof(double));
    double* y = (double*) malloc (vec_size*sizeof(double));

    // fill vectors with sinusoidals for testing the code
    for(int i = 0; i < vec_size; i++)
    {
        x[i] = sin(i*0.01);
        y[i] = cos(i*0.01);
    }

    time_start = omp_get_wtime();

    for(int i = 0; i < 100; i++)
        dot_cpu = dot_product_cpu(vec_size, x, y);

    time_end = omp_get_wtime();
    time_cpu = time_end - time_start;
    
    #pragma acc enter data copyin(x[0:vec_size], y[0:vec_size]) 
    time_start = omp_get_wtime();

    for(int i = 0; i < 100; i++)
        dot_gpu = dot_product_gpu(vec_size, x, y);

    time_end = omp_get_wtime();
    time_gpu = time_end - time_start;
    #pragma acc exit data delete(x[0:vec_size], y[0:vec_size]) 

    printf("dot product comparison cpu vs gpu %e, size %d\n",
           dot_cpu - dot_gpu, vec_size);

    double speed_up = time_cpu / time_gpu; // TODO
    printf("CPU Time: %lf - GPU Time: %lf - Speed up: %lf \n", time_cpu, time_gpu, speed_up);

    // free allocated memory
    free(x);
    free(y);
}
