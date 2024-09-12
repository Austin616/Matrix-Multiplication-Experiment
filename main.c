#include <stdio.h>
#include <stdlib.h>
#include "papi.h"
#include <time.h>


// function to create a matrix of size x size
double **createMatrix(int size)
{   
    // Allocate memory for the matrix
    double **matrix = (double **)malloc(size * sizeof(double *));
    for (int i = 0; i < size; i++)
    {
        // allocate memory for each row
        matrix[i] = (double *)malloc(size * sizeof(double));
    }
    return matrix;
}

// function to populate the matrix with random numbers
void populateMatrix(double **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = (double)rand() / RAND_MAX * 10.0; // Random number between 0 and 10
        }
    }
}

// function to free the matrix and each individual row
void freeMatrix(double **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

// function to initialize the matrix
void initializeMatrix(double **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = 0.0;
        }
    }
}

// function to print the matrix
void printMatrix(double **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%7.2f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// ijk matrix multiplication
void ijk(double **A, double **B, double **C, int size)
{
    initializeMatrix(C, size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
            {
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }
}

// jik matrix multiplication
void jik(double **A, double **B, double **C, int size)
{
    initializeMatrix(C, size);
    for (int j = 0; j < size; j++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int k = 0; k < size; k++)
            {
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }
}

// kij matrix multiplication
void kij(double **A, double **B, double **C, int size)
{
    initializeMatrix(C, size);
    for (int k = 0; k < size; k++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }
}

// ikj matrix multiplication
void ikj(double **A, double **B, double **C, int size)
{
    initializeMatrix(C, size);
    for (int i = 0; i < size; i++)
    {
        for (int k = 0; k < size; k++)
        {
            for (int j = 0; j < size; j++)
            {
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }
}

// jki matrix multiplication
void jki(double **A, double **B, double **C, int size)
{
    initializeMatrix(C, size);
    for (int j = 0; j < size; j++)
    {
        for (int k = 0; k < size; k++)
        {
            for (int i = 0; i < size; i++)
            {
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }
}

// kji matrix multiplication
void kji(double **A, double **B, double **C, int size)
{
    initializeMatrix(C, size);
    for (int k = 0; k < size; k++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int i = 0; i < size; i++)
            {
                C[i][j] = C[i][j] + A[i][k] * B[k][j];
            }
        }
    }
}

void setupPAPI(int *event_set)
{
    // Initialize PAPI
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "PAPI library init error!\n");
        exit(1);
    }

    // Create an event set
    if (PAPI_create_eventset(event_set) != PAPI_OK)
    {
        fprintf(stderr, "PAPI create event set error!\n");
        exit(1);
    }

    // Add events to the event set
    if (PAPI_add_event(*event_set, PAPI_TOT_CYC) != PAPI_OK ||
        PAPI_add_event(*event_set, PAPI_TOT_INS) != PAPI_OK ||
        PAPI_add_event(*event_set, PAPI_LD_INS) != PAPI_OK ||
        PAPI_add_event(*event_set, PAPI_SR_INS) != PAPI_OK ||
        PAPI_add_event(*event_set, PAPI_L1_DCM) != PAPI_OK)
    {
        fprintf(stderr, "PAPI add event error!\n");
        exit(1);
    }
}

// Function to measure cache misses
void measureCacheMisses(void (*multFunc)(double **, double **, double **, int), double **A, double **B, double **C, int size, const char *order)
{
    int event_set = PAPI_NULL;
    long long l1_cache_misses, l2_cache_misses, l1_cache_accesses, l2_cache_accesses;
    long long values[4];

    // Initialize PAPI
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "PAPI library init error!\n");
        exit(1);
    }

    // Create an event set
    if (PAPI_create_eventset(&event_set) != PAPI_OK)
    {
        fprintf(stderr, "PAPI create event set error!\n");
        exit(1);
    }

    // Add cache misses events to the event set
    if (PAPI_add_event(event_set, PAPI_L1_DCM) != PAPI_OK ||
        PAPI_add_event(event_set, PAPI_L2_DCM) != PAPI_OK ||
        PAPI_add_event(event_set, PAPI_L1_DCA) != PAPI_OK ||
        PAPI_add_event(event_set, PAPI_L2_DCA) != PAPI_OK)
    {
        fprintf(stderr, "PAPI cache add event error!\n");
        exit(1);
    }

    // Start counting events
    if (PAPI_start(event_set) != PAPI_OK)
    {
        fprintf(stderr, "PAPI start counting error!\n");
        exit(1);
    }

    // Perform the matrix multiplication
    multFunc(A, B, C, size);

    // Stop counting events
    if (PAPI_stop(event_set, values) != PAPI_OK)
    {
        fprintf(stderr, "PAPI stop counting error!\n");
        exit(1);
    }

    // Read the counts from values array
    l1_cache_misses = values[0];
    l2_cache_misses = values[1];
    l1_cache_accesses = values[2];
    l2_cache_accesses = values[3];

    double l1_miss_rate = (double)l1_cache_misses / l1_cache_accesses;
    double l2_miss_rate = (double)l2_cache_misses / l2_cache_accesses;

    // Print the results
    printf("%s L1 cache misses: %lld\n", order, l1_cache_misses);
    printf("%s L2 cache misses: %lld\n", order, l2_cache_misses);
    printf("%s L1 cache accesses: %lld\n", order, l1_cache_accesses);
    printf("%s L2 cache accesses: %lld\n", order, l2_cache_accesses);
    printf("L1 cache miss rate: %.2f%%\n", l1_miss_rate * 100);
    printf("L2 cache miss rate: %.2f%%\n", l2_miss_rate * 100);
    printf("\n");

    //cache cleanup
    PAPI_cleanup_eventset(event_set);
    PAPI_destroy_eventset(&event_set);

}

// Function to measure matrix multiplication
void measureMatrixMultiplication(void (*multFunc)(double **, double **, double **, int), double **A, double **B, double **C, int size, const char *order)
{
    int event_set = PAPI_NULL;
    long long cycles, instructions, load_instructions, store_instructions, fp_instructions;
    long long values[5];
    long long startTime, endTime;
    struct timespec start, end;
    struct timespec start2, end2;

    // Set up PAPI
    setupPAPI(&event_set);

    // Start counting events
    if (PAPI_start(event_set) != PAPI_OK)
    {
        fprintf(stderr, "PAPI start counting error!\n");
        exit(1);
    }

    // Start the timer
    startTime = PAPI_get_real_usec();
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    clock_gettime(CLOCK_REALTIME, &start2);

    // Perform the matrix multiplication
    multFunc(A, B, C, size);

    // Stop the timer
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    clock_gettime(CLOCK_REALTIME, &end2);
    endTime = PAPI_get_real_usec();

    // Stop counting events
    if (PAPI_stop(event_set, values) != PAPI_OK)
    {
        fprintf(stderr, "PAPI stop counting error!\n");
        exit(1);
    }

    // Read the counts from values array
    cycles = values[0];
    instructions = values[1];
    load_instructions = values[2];
    store_instructions = values[3];
    fp_instructions = values[4];

    // Print the results
    printf("Result of %s:\n", order);
    printf("%s cycles: %lld\n", order, cycles);
    printf("%s instructions: %lld\n", order, instructions);
    printf("%s load instructions: %lld\n", order, load_instructions);
    printf("%s store instructions: %lld\n", order, store_instructions);
    printf("%s FP instructions: %lld\n", order, fp_instructions);
    printf("%s time in PAPI: %lld.00\n", order, endTime - startTime);
    printf("%s microseconds with CLOCK_THREAD_CPUTIME_ID: %ld.00\n", order, (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000);
    printf("%s microseconds with CLOCK_REALTIME: %ld.00\n", order, (end2.tv_sec - start2.tv_sec) * 1000000 + (end2.tv_nsec - start2.tv_nsec) / 1000);
    measureCacheMisses(multFunc, A, B, C, size, order);
    
    // Cleanup
    PAPI_cleanup_eventset(event_set);
    PAPI_destroy_eventset(&event_set);
}

int main()
{
    int sizes[] = {50, 100, 200, 400, 800, 1200, 1600, 2000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    // Iterate over each matrix size
    for (int i = 0; i < num_sizes; i++)
    {
        int size = sizes[i];

        // Create matrices
        double **matrixA = createMatrix(size);
        double **matrixB = createMatrix(size);
        double **matrixC = createMatrix(size);

        srand(time(NULL)); // Seed the random number generator
        populateMatrix(matrixA, size);
        populateMatrix(matrixB, size);
        printf("Matrix A (%d x %d):\n", size, size);
        printf("Matrix B (%d x %d):\n\n", size, size);

        // Measure and print results for each matrix multiplication order
        measureMatrixMultiplication(ijk, matrixA, matrixB, matrixC, size, "ijk");
        measureMatrixMultiplication(jik, matrixA, matrixB, matrixC, size, "jik");
        measureMatrixMultiplication(kij, matrixA, matrixB, matrixC, size, "kij");
        measureMatrixMultiplication(ikj, matrixA, matrixB, matrixC, size, "ikj");
        measureMatrixMultiplication(jki, matrixA, matrixB, matrixC, size, "jki");
        measureMatrixMultiplication(kji, matrixA, matrixB, matrixC, size, "kji");

        // Free matrices
        freeMatrix(matrixA, size);
        freeMatrix(matrixB, size);
        freeMatrix(matrixC, size);
        printf("Matrix Multiplication for size %d completed\n", size);
    }

    return 0;
}
