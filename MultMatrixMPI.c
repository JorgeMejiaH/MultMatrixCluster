#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>

void MultBlock(int **matrixA, int **matrixB, int **resultado, int startRow, int endRow, int length) 
{
    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < length; j++) {
            resultado[i][j] = 0;
            for (int k = 0; k < length; k++) {
                resultado[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
}

void mostrarMatriz(int *matrix, int length) {
    int j;
    for (int i = 0; i < length; i++) {
        for (j = 0; j < length; j++) {
            printf("%4d |", matrix[i * length + j]);
        }
        printf("\n");
        for (j = 0; j < length - 1; j++) {
            printf("------+");
        }
        printf("\n");
    }
}

void PrintMatriz(int **matriz, int length) 
{
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

int** GenerateMatrix(int length)
{
    int **matrix;

    matrix = (int **)malloc(length * sizeof(int *));

    for(int i = 0; i < length; i++){
        matrix[i] = (int *)malloc(length * sizeof(int));
    }

    return matrix;
}

void FreeMatrix(int **matrix, int length)
{
    for(int i = 0; i < length; i++){
        free(matrix[i]);
    }

    free(matrix);
}

void ReadMatrixFromFile(const char *filename, int *matrix, int length) 
{
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fwrite(matrix, sizeof(int), length * length, file);
        fclose(file);
    } else {
        fprintf(stderr, "Error opening file for writing: %s\n", filename);
    }
}

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);

    struct timespec start, end;
    double elapsed_time;
    int length = atoi(argv[1]);
    int verbose = 0;
    int rank, size;
    const char *filename;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    if (argc > 2 && strcmp(argv[2], "-v") == 0) {
        verbose = 1;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    switch(length){
        case 32:
            filename = 
            "/home/cluser/Documents/HPC/Matrix/Matrix32.txt";
            break;
        case 64:
            filename = 
            "/home/cluser/Documents/HPC/Matrix/Matrix64.txt";
            break;
        case 128:
            filename = 
            "/home/cluser/Documents/HPC/Matrix/Matrix128.txt";
            break;
        case 256:
            filename = 
            "/home/cluser/Documents/HPC/Matrix/Matrix256.txt";
            break;
        case 512:
            filename = 
            "/home/cluser/Documents/HPC/Matrix/Matrix512.txt";
            break;
        case 1024:
            filename = 
            "/home/cluser/Documents/HPC/Matrix/Matrix1024.txt";
            break;
        case 2048:
            filename = 
            "/home/cluser/Documents/HPC/Matrix/Matrix2048.txt";
            break;
        default:
            filename = 
            "/home/cluser/Documents/HPC/Matrix/Matrix32.txt";
            break;
    }

    if (rank == 0){
        clock_gettime(CLOCK_MONOTONIC, &start);
    }

    int *matrixA = (int *)malloc(length * length * sizeof(int));
    int *matrixB = (int *)malloc(length * length * sizeof(int));
    int *result = (int *)malloc(length * length * sizeof(int));
    int *gathered_result = NULL;

    ReadMatrixFromFile(filename, matrixA, length);
    ReadMatrixFromFile(filename, matrixB, length);

    int rows_per_process = length / size;
    int extra_rows = length % size;
    int start_row = rank * rows_per_process;
    int end_row = start_row + rows_per_process + (rank == size - 1 ? extra_rows : 0);

    int **localResult = GenerateMatrix(length);


    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < length; j++) {
            result[i * length + j] = 0;
            for (int k = 0; k < length; k++) {
                result[i * length + j] += matrixA[i * length + k] * matrixB[k * length + j];
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        gathered_result = (int *)malloc(length * length * sizeof(int));
    }

    int local_rows = (rank == size - 1) ? rows_per_process + extra_rows : rows_per_process;


    MPI_Gather(
        result + start_row * length,   // Send buffer: the local portion of the result array for the current process
        local_rows * length,           // Number of elements to send from the send buffer
        MPI_INT,                       // Data type of each element in the send buffer (integer in this case)
        gathered_result,               // Receive buffer: the buffer where the root process will gather data
        local_rows * length,           // Number of elements to receive from each process
        MPI_INT,                       // Data type of each element in the receive buffer (integer in this case)
        0,                             // Root process (the process that will receive the gathered data)
        MPI_COMM_WORLD                 // Communicator (in this case, the world communicator)
    );

    if (rank == 0){
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("%f\n", elapsed_time);
        if(verbose){
        mostrarMatriz(gathered_result, length);
        }
    }
    

    free(matrixA);
    free(matrixB);
    free(result);
    free(gathered_result);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    return 0;
}