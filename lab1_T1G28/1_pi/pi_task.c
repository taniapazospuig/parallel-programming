#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void computePi(int start, int end, int N, double* pi){ //N is the number of steps we need
    double sum = 0.0; //to store total area of the rectangles
    double dx = 1.0/N;  //width of each rectangle ∆x (1 divided by N (number of rectangles))
    double x; 
    
    for(int i = start; i<end; i++){ //iterate through all N rectangles
        x = (i+0.5)*dx; //midpoint of the current interval 
        double height = 4.0 / (1.0 + x*x); //computes the height of the rectangle
        sum += height; 
    }
    #pragma omp atomic
        *pi += sum*dx; 
}

void tasksPi(int start, int end, int M, int N, double* pi){
    if (end - start <= M) 
    { // if the number of steps is smaller than M we compute Pi
        computePi(start, end, N, pi);
    } 
    else 
    { //if it is not, we divide into smaller steps
        int middle = (start + end) / 2;
        double left_sum, right_sum;
#pragma omp task  // left tasks
        { 
            tasksPi(start, middle, M, N, pi);
        }
#pragma omp task // right tasks
        {
            tasksPi(middle, end, M, N, pi);
        }
#pragma omp taskwait // wait for all the tasks to finish
    }
}

int main(int argc, char *argv[]){ //will receive as argument only N (number of rectangles) --> argc=2
    if(argc != 3){
        printf("Incorrect number of arguments\n"); 
        return 1; 
    }

    int N = atoi(argv[1]); //We convert to string the input argument N 
    int M = atoi(argv[2]); //same for argument M
     
    clock_t start = clock(); //we start the timer before computing pi

    double* pi = (double*)malloc(sizeof(double)); 
    *pi = 0.0;
    #pragma omp parallel // to parallelize the computation of pi in the tasksPi function
    { 
        #pragma omp single // to make sure only one thread calls the function
        { 
            tasksPi(0, N, M, N, pi); 
        }
    }
    clock_t end = clock(); 
    double runtime = (double)(end-start)/CLOCKS_PER_SEC; //convert from clock ticks to seconds

    printf("\nPi with %d steps is %.15lf in %lf seconds \n ", N, *pi, runtime); 

    return 0; 
}
