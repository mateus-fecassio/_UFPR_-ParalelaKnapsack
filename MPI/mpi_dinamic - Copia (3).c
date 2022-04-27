#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>


// A utility function that returns
// maximum of two integers
int max(int a, int b) {return (a > b) ? a : b;}


int** get_matrix(int rows, int columns) {   
    int **mat;
    int i;
    
    // for each line
    mat = (int**) calloc(rows, sizeof (int*));
    
    mat[0] = (int*) calloc(rows * columns, sizeof (int));

    // set up pointers to rows
    for (i = 1; i < rows; i++)
        mat[i] = mat[0] + i * columns;

    return mat;
}

void free_matrix(int** mat) {
    free(mat[0]);
    free(mat);
}


int knapsack(int MAXIMUM_CAPACITY, int wt[], int val[], int n, int rank, int numproc, int map[]){
    // Matrix-based solution
    int** V = get_matrix(n + 1, MAXIMUM_CAPACITY + 1);

    // V Stores, for each (1 + i, j), the best profit for a knapscak
    // of capacity `j` considering every item k such that (0 <= k < i)
    int i, j, k, previous_value, replace_items, value, tag_s, tag_r;
    MPI_Status status;
	MPI_Request request;

    // evaluate item `i`
    for(i = 0; i <= n; i++) {
        for(j = rank; j <= MAXIMUM_CAPACITY; j+=numproc) {
            if (i == 0 || j == 0) {
                V[i][j] = 0;
            } else if (wt[i] <= j) { // could put item in knapsack
                value = V[i][j - wt[i]];
                // MPI_Recv(&value, 1, MPI_INT, map[j-wt[i]], i-1, MPI_COMM_WORLD, &status);
                previous_value = V[i][j];
                replace_items = val[i] + value;

                // is it better to keep what we already got,
                // or is it better to swap whatever we have in the bag that weights up to `j`
                // and put item `i`?
                V[1 + i][j] = max(previous_value, replace_items);
            } else {
                // can't put item `i`
                V[1 + i][j] = V[i][j];
			}
            tag_s = ((1+i)*n)+j;
            MPI_Isend(&V[i+1][j], 1, MPI_INT, map[j], tag_s, MPI_COMM_WORLD, &request);

            MPI_Recv(&V[i+1][j], 1, MPI_INT, map[j], tag_s, MPI_COMM_WORLD, &status);
        }
    }

    int retval = V[n][MAXIMUM_CAPACITY];
    printf("teste=%d\n",retval);
    
    free_matrix(V);
    
    return retval;
}


void print_knapsack (int n, int W, int *val, int *wt, int rank, int *map) {
    printf("RANK = %d, n = %d, W = %d\n", rank,n,W);
    // printf("RANK = %d\n", rank);

    for (int i = 0; i < n; ++i) {
        printf(" %d %d ", val[i], wt[i]);
    }
    // for (int i = 0; i < W; ++i) {
    //     printf("%d ", map[i]);
    // }
    printf("\n\n");

}

// Driver program to test above function
int main(int argc,char *argv[]){
	int n, W;
    int *val, *wt;

    int numproc, rank;

    MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numproc);

    if (rank == 0) {
        printf("\nnumproc = %d\n", numproc);
        scanf("%d %d", &n, &W);
        val = (int*) calloc(n, sizeof(int));
        wt = (int*) calloc(n, sizeof(int));

        for (int i = 0; i < n; ++i) {
            scanf("%d %d", &(val[i]), &(wt[i])); 
        }

        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&W, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Bcast(&val[0], n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&wt[0], n, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&W, 1, MPI_INT, 0, MPI_COMM_WORLD);

        val = (int*) calloc(n, sizeof(int));
        wt = (int*) calloc(n, sizeof(int));

        MPI_Bcast(&val[0], n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&wt[0], n, MPI_INT, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);


    int *map = (int*) calloc(W, sizeof(int));
    for (int k = 0; k < W; ++k) {
		map[k] = k%numproc;
	}


    print_knapsack(n,W,val,wt,rank,map);

    printf("%d\n", knapsack(W, wt, val, n, rank, numproc, map));
    MPI_Finalize();

    
    
    return 0;
}
