#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include "sort.h"

int sort(int *array, int n) {
    int i, j, tmp;

    for (i=1;i<n;i++) {  
        tmp = array[i];  
        for(j=i-1;j >= 0 && array[j] > tmp;j--) {  
            array[j+1] = array[j];  
        }  
        array[j+1] = tmp;  
    }
}

int sort_openmp(int *array, int n) {
    //TODO
    int chunk_size = n/_NUM_THREADS; 
    int remainder = n % _NUM_THREADS; // handle the case where the remainder is not 0

    int *tmp_array = (int*)malloc(sizeof(int)*n); //we create tmp array to store the sorted values computed by the threads
    

    #pragma omp parallel num_threads(_NUM_THREADS) 
    {
        int thread_id = omp_get_thread_num(); 
        // compute the start and end of the chunk of each thread
        int start = thread_id * chunk_size; 
        int end = start + chunk_size; // end will be last index for that thread + 1

        if(thread_id == _NUM_THREADS-1){ // chunk of the last thread will be added the remainder  
            end += remainder; 
        }

        // each thread reads chunk in array and sorts in tmp_array
        for (int i=start; i<end; i++) {  
            int tmp = array[i];  //we take the variable from the array 
            int j;
            // perform insertion sort in the chunk of array
            for(j=i-1; j >= start && tmp < array[j]; j--) {  
                array[j+1] = array[j];  
            }  
            array[j+1] = tmp;  
        }

        // each thread saves chunk into tmp_array
        for(int i=start; i<end;i++) {
            tmp_array[i] = array[i];
        }
    }
   
    // sequential phase
    // merge sorted chunks from tmp_array into array
    int head_i[_NUM_THREADS]; 
    for(int i = 0; i<_NUM_THREADS; i++){
        head_i[i] = i*chunk_size; 
    }

    int i = 0; // starting index to fill array
    while (i<n) {
        int min_val = INT32_MAX; // store minimum value among heads of each thread
        int min_thread = -1; // thread with minimum value

        // find the minimum value among heads
        for(int j=0; j<_NUM_THREADS; j++){ // iterate through each head
            // check if head in correct thread chunk range
            if(head_i[j] < (j+1)*chunk_size && tmp_array[head_i[j]] < min_val) {
                min_val = tmp_array[head_i[j]];
                min_thread = j;
            }
        }


        // save minimum value in array and move the head that contained it
        array[i] = min_val;
        i++;
        head_i[min_thread]++;
    }

    free(tmp_array);
}

void fill_array(int *array, int n) {
    int i;
    
    srand(time(NULL));
    for(i=0; i < n; i++) {
        array[i] = rand()%n;
    }
}


