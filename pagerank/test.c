#include <stdio.h>
#include <stdlib.h>

double** createArray(int m, int n)
{
    double* values = calloc(m*n, sizeof(double));
    double** rows = malloc(m*sizeof(double*));
    for (int i=0; i<m; ++i)
    {
        rows[i] = values + i*n;
    }
    return rows;
}

void destroyArray(double** arr)
{
    free(*arr);
    free(arr);
}

int main(void)
{
    double** arr = createArray(6,6);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            arr[i][j] = 42;
        }
    }
    arr[0][0] = 1;
    arr[0][1] = 1;
    arr[1][0] = 2;
    arr[1][1] = 2; 
    destroyArray(arr);
}