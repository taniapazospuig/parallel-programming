#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double function(double x) // define f(x)
{
    return x*sin(x);
}

double exact_quadrature(double x)
{
    return sin(x) - x*cos(x); // computes exact value of the integral
}

int main(int argc, char* argv[])
{
    /// Check parameters
    if(argc != 3) {
        // arg 1: N points in which we divide x domain
        // arg 2: value of L (computational domain XMAX)
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        return -1;
    }
    int N = (int)strtol(argv[1], NULL, 10); // strtol converts string argv[1] to long integer
    double X_MAX = (double) strtold(argv[2], NULL); //strold converts string argv[2] to double
    double deltaX = X_MAX/(double) N; // compute step size (increment of x)


    clock_t start_time = clock();

    /// Compute integral

    int i;
    double result = 0;
    for (i = 1; i <= N; i++) { // iterate over each subinterval
        double x_middle = (i - 0.5) * deltaX; // compute midpoint of subinterval
        result = result + function(x_middle) * deltaX; // add value of the function at midpoint to result
    }

    /// Compute error and print results
    double exact = exact_quadrature(X_MAX); // compute exact integral
    double error = fabs(result - exact); // abs difference between computed result and exact result
    double run_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("Sequential result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, result, exact, error, run_time);

    return 0;
}