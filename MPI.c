#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <mpi.h>

#define WIDTH 1000
#define HEIGHT 1000
#define MAX_ITER 1000

int mandelbrot(double complex c) {
    double complex z = 0;
    int iterations = 0;
    while (cabs(z) <= 2 && iterations < MAX_ITER) {
        z = z * z + c;
        iterations++;
    }
    return iterations;
}
//Main Method
int main(int argc, char* argv[]) {
    //Should have terminal inputs
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = HEIGHT / size;
    int start = rank * chunk_size;
    int end = start + chunk_size;

    int result[chunk_size][WIDTH];

    for (int i = start; i < end; i++) {
        for (int j = 0; j < WIDTH; j++) {
            double real = (j - WIDTH/2) * 4.0/WIDTH;
            double imag = (i - HEIGHT/2) * 4.0/WIDTH;
            double complex c = real + imag * I;
            int iterations = mandelbrot(c);
            result[i-start][j] = iterations;
        }
    }

    int *recv_result;
    if (rank == 0) {
        recv_result = (int*) malloc(HEIGHT * WIDTH * sizeof(int));
    }
    //Try using the gather function to see how slow it is
    MPI_Gather(result, chunk_size * WIDTH, MPI_INT, recv_result, chunk_size * WIDTH, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                printf("%d ", recv_result[i * WIDTH + j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}