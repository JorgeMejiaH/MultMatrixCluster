#include <stdio.h>
#include <stdlib.h>

void printMatriz(int **matriz, int length)
{
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

void fillMatrix(int **matrix, int length, int max)
{
    for(int i = 0; i < length; i++){
        for(int j = 0; j < length; j++){
            matrix[i][j] = rand() % max;
        }
    }
}

int** generateMatrix(int length)
{
    int **matrix;

    matrix = (int **)malloc(length * sizeof(int *));

    for(int i = 0; i < length; i++){
        matrix[i] = (int *)malloc(length * sizeof(int *));
    }

    return matrix;
}

void freeMatrix(int **matrix, int length)
{
    for(int i = 0; i < length; i++){
        free(matrix[i]);
    }

    free(matrix);
}

int main(int argc, char *argv[]){
    int length = atoi(argv[1]);
    int max_number = 100;
    int **matrix;

    matrix = generateMatrix(length);
    fillMatrix(matrix, length, max_number);
    printMatriz(matrix, length);
    freeMatrix(matrix, length);
    return 0;
}