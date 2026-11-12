#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_SIZE 100
#define NUM_THREADS 10

int matA[MAX_SIZE][MAX_SIZE]; 
int matB[MAX_SIZE][MAX_SIZE]; 

int matSumResult[MAX_SIZE][MAX_SIZE];
int matDiffResult[MAX_SIZE][MAX_SIZE]; 
int matProductResult[MAX_SIZE][MAX_SIZE]; 

int MAX;

void fillMatrix(int matrix[MAX_SIZE][MAX_SIZE]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            matrix[i][j] = rand()%10+1;
        }
    }
}

void printMatrix(int matrix[MAX_SIZE][MAX_SIZE]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int firstRow(int threadNum) {
  return threadNum * MAX / NUM_THREADS;
}

int endRow(int threadNum) {
  return (threadNum + 1) * MAX / NUM_THREADS;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) { // pass in the number of the ith thread
  int threadNum = *(int*)args;
  free(args);

  for(int i = firstRow(threadNum); i < endRow(threadNum); i++) {
    for(int j = 0; j < MAX; j++) {
      matSumResult[i][j] = matA[i][j] + matB[i][j];
    }
  }
  return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) { // pass in the number of the ith thread
  int threadNum = *(int*)args;
  free(args);

  for(int i = firstRow(threadNum); i < endRow(threadNum); i++) {
    for(int j = 0; j < MAX; j++) {
      matDiffResult[i][j] = matA[i][j] - matB[i][j];
    }
  }
  return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) { // pass in the number of the ith thread
  int threadNum = *(int*)args;
  free(args);

  for(int i = firstRow(threadNum); i < endRow(threadNum); i++) {
    for(int j = 0; j < MAX; j++) {
      int total = 0;
      for(int k = 0; k < MAX; k++) {
        total += matA[i][k] * matB[k][j];
      }
      matProductResult[i][j] = total;
    }
  }
  return NULL;
}

void runThreads(void* (*work)(void*)) {
  pthread_t threads[NUM_THREADS];

  for(int i = 0; i < NUM_THREADS; i++) {
    int* threadNum = malloc(sizeof(int));
    if(threadNum == NULL) {
      fprintf(stderr, "malloc failed\n");
      exit(1);
    }
    *threadNum = i;

    if(pthread_create(&threads[i], NULL, work, threadNum) != 0) {
      fprintf(stderr, "pthread_create failed\n");
      exit(1);
    }
  }

  for(int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main(int argc, char* argv[]) {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // 0. Get the matrix size from the command line and assign it to MAX
    MAX = 20;
    if(argc > 1) {
      MAX = atoi(argv[1]);
    }
    if(MAX < 1 || MAX > MAX_SIZE) {
      fprintf(stderr, "Matrix size must be between 1 and %d\n", MAX_SIZE);
      return 1;
    }
    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our threads.
    runThreads(computeSum);
    runThreads(computeDiff);
    runThreads(computeProduct);
    // 4. Create a thread for each cell of each matrix operation.
    // 
    // You'll need to pass in the coordinates of the cell you want the thread
    // to compute.
    // 
    // One way to do this is to malloc memory for the thread number i, populate the coordinates
    // into that space, and pass that address to the thread. The thread will use that number to calcuate 
    // its portion of the matrix. The thread will then have to free that space when it's done with what's in that memory.
    
    // 5. Wait for all threads to finish.
    
    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    return 0;
  
}