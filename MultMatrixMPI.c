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
        matrix[i] = (int *)malloc(length * sizeof(int *));
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

void ReadMatrixFromFile(const char *filename, int **matrix, int length) 
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                fprintf(stderr, "Error reading from file");
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);

    double time_spent = 0.0;
    clock_t begin = clock();
    int length = atoi(argv[1]);
    int verbose = 0;
    int rank, size;
    int **matrixA;
    int **matrixB;
    int **matrixResult;
    const char *filename;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    if (argc > 2 && strcmp(argv[2], "-v") == 0) {
        verbose = 1;
    }
    
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

    matrixA = GenerateMatrix(length);
    matrixB = GenerateMatrix(length);
    matrixResult = GenerateMatrix(length);

    ReadMatrixFromFile(filename, matrixA, length);
    ReadMatrixFromFile(filename, matrixB, length);

    int rowsPerProcess = length / size;
    int startRow = rank * rowsPerProcess;
    int endRow = (rank == size - 1) ? length : startRow + rowsPerProcess;

    int **localResult = GenerateMatrix(length);


    MultBlock(matrixA, matrixB, localResult, startRow, endRow, length);

    MPI_Gather(&(localResult[startRow][0]), (endRow - startRow) * length, MPI_INT,
               &(matrixResult[0][0]), (endRow - startRow) * length, MPI_INT, 0, MPI_COMM_WORLD);


    if(verbose){
        PrintMatriz(matrixResult, length);
    }

    FreeMatrix(matrixA, length);
    FreeMatrix(matrixB, length);
    FreeMatrix(localResult, length);
    FreeMatrix(matrixResult, length);

    clock_t end = clock();

    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    printf("%f \n", time_spent);

    MPI_Finalize();

    return 0;
}