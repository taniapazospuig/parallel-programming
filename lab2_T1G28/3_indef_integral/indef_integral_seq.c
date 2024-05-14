#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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

    /// Check parameters
    if(argc != 3) {
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        return -1;
    }

    int N = (int)strtol(argv[1], NULL, 10); // convert to int
    double X_MAX = (double) strtold(argv[2], NULL); // convert to double
    double deltaX = X_MAX/(double) N; // compute deltaX (width of each integration interval)
    double* integral = (double *) calloc(N+1, sizeof(double)); // array to store integral values at each interval
    // it is N+1 to store the integral value at the upper limit of integration


    clock_t start_time = clock();
    int i;

    for (i = 1; i <= N; i++){ //calculates integral at each interval
        double x = (i-0.5)*deltaX; 
        integral[i] = integral[i-1] + deltaX*function(x); //store it int array
    }

    double def_result = integral[N]; //definitive result over all the integral
    double exact = exact_integral(X_MAX); //the exact result of the integral 
    double error = fabs(def_result - exact); //error in absolute value 
    double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, def_result, exact, error, run_time);

    /// Print integral data
    char filename_dat[100];
    sprintf(filename_dat, "indef_integral_seq.dat"); //print integral data in binary file
    FILE* file_dat = fopen(filename_dat, "wb");
    if (file_dat != NULL) {
        fwrite(integral, sizeof(integral), N+1, file_dat);
        fclose(file_dat);
    }
    
    /// Print integral info
    char filename_info[100];
    sprintf(filename_info, "indef_integral_seq.info");
    FILE* file_info = fopen(filename_info, "w");
    if (file_dat != NULL) {
        fprintf(file_info, "# %s\n", "Indefinite integral version sequential");
        fprintf(file_info, "data %s\n", filename_dat);
        fprintf(file_info, "N %d\n", N + 1);
        fprintf(file_info, "XMAX %lf\n", X_MAX);
        fprintf(file_info, "size %d\n", 1); //only one integral 
        fclose(file_info);
    }

    free(integral);
    return 0;
}
