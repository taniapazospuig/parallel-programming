#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


double computePi(int N){ //N is the number of steps we need
    double pi = 0.0; //to store area computed by each thread
    double dx = 1.0/N;  //width of each rectangle ∆x (1 divided by N (number of rectangles))
    double *sum_threads = (double*)malloc(sizeof(double)*omp_get_max_threads()); 
    for(int i = 0; i < omp_get_max_threads(); i++){
        sum_threads[i] = 0.0; 
    }

    
    
    #pragma omp parallel
    {
        double x; 
        double sum = 0.0; //this is a variable for each thread
        int id = omp_get_thread_num(); 
        int nthreads = omp_get_num_threads(); 

        int first = N/nthreads*id; 
        int last = N/nthreads*(id+1); 

        if(id == nthreads-1) last = N; 
       
        for(int i = first; i<last; i++){ //iterate through all N rectangles
            x = (i+0.5)*dx; //midpoint of the current interval 
            double height = 4.0 / (1.0 + x*x); //computes the height of the rectangle
            sum += height; 
        }
        
        sum_threads[id] = sum*dx;
        
    }

    for (int i = 0; i < omp_get_max_threads(); i++) {
        pi += sum_threads[i];
    }

    free(sum_threads);
    return pi;
}

int main(int argc, char *argv[]){ //will receive as argument only N (number of rectangles) --> argc=2
    if(argc != 2){
        printf("Incorrect number of arguments\n"); 
        return 1; 
    }
    
    int N = atoi(argv[1]); //We convert to string the input argument N
    clock_t start = clock(); //we start the timer before computing pi
    double pi = computePi(N); 
    clock_t end = clock(); 
    double runtime = (double)(end-start)/CLOCKS_PER_SEC; //convert from clock ticks to seconds

    printf("\nPi with %d steps is %.15lf in %lf seconds \n ", N, pi, runtime); 

    return 0; 
}
