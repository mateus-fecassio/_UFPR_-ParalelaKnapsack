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


int knapsack(int MAXIMUM_CAPACITY, int wt[], int val[], int n, int rank, int numproc, int map[]) {
    // Matrix-based solution
    int** V = get_matrix(n + 1, MAXIMUM_CAPACITY + 1);

    // V Stores, for each (1 + i, j), the best profit for a knapscak
    // of capacity `j` considering every item k such that (0 <= k < i)
    int i, j, k, previous_value, replace_items, value, tag_s, tag_r;
    MPI_Status status;
	MPI_Request request;

    int chunk = (MAXIMUM_CAPACITY+1)/numproc;
    int init, end;
    // evaluate item `i`
    for(i = 0; i <= n; i++) {
        init = (rank*chunk);
        end = init+chunk;
        for(j = init; j < end; j++) {
            if (i == 0 || j == 0) {
                V[i][j] = 0;
            } else if (wt[i-1] <= j) { // could put item in knapsack
                value = V[i-1][j - wt[i-1]];
                previous_value = V[i-1][j];
                replace_items = val[i-1] + value;

                // is it better to keep what we already got,
                // or is it better to swap whatever we have in the bag that weights up to `j`
                // and put item `i`?
                V[i][j] = max(previous_value, replace_items);
            } else {
                // can't put item `i`
                V[i][j] = V[i-1][j];
			}
        }
      
        MPI_Allgather(&V[i][init], chunk,MPI_INT,&V[i][0],chunk,MPI_INT,MPI_COMM_WORLD);

        if ((MAXIMUM_CAPACITY+1)%numproc != 0) {
            if (rank == 0) {
                init = ((numproc-1)*chunk);
                end = MAXIMUM_CAPACITY+1;
                
                for(j = init; j < end; j++) {
                    if (i == 0 || j == 0) {
                        V[i][j] = 0;
                    } else if (wt[i-1] <= j) { // could put item in knapsack
                        value = V[i-1][j - wt[i-1]];
                        previous_value = V[i-1][j];
                        replace_items = val[i-1] + value;

                        // is it better to keep what we already got,
                        // or is it better to swap whatever we have in the bag that weights up to `j`
                        // and put item `i`?
                        V[i][j] = max(previous_value, replace_items);
                    } else {
                        // can't put item `i`
                        V[i][j] = V[i-1][j];
                    }
                }
            
            //broadcast
            MPI_Bcast(&V[i][init], end-init, MPI_INT, 0, MPI_COMM_WORLD);
            }
        }
       
    }

    int retval = V[n][MAXIMUM_CAPACITY];
    
    free_matrix(V);
    
    return retval;
}


void print_knapsack (int n, int W, int *val, int *wt, int rank, int *map) {
    printf("RANK = %d, n = %d, W = %d\n", rank,n,W);
    // printf("RANK = %d\n", rank);

    // for (int i = 0; i < n; ++i) {
    //     printf(" %d %d ", val[i], wt[i]);
    // }
    // for (int i = 0; i < W; ++i) {
    //     printf("%d ", map[i]);
    // }
    printf("\n\n");

}

// Driver program to test above function
int main(int argc,char *argv[]) {
	int n, W;
    int *val, *wt;

    int numproc, rank;

    MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numproc);

    if (rank == 0) {
        printf("\nnumproc = %d\n", numproc);
        scanf("%d %d", &n, &W);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); //ENVIA E RECEBE
    MPI_Bcast(&W, 1, MPI_INT, 0, MPI_COMM_WORLD);

    val = (int*) calloc(n, sizeof(int));
    wt = (int*) calloc(n, sizeof(int));

    if (rank == 0) {
        for (int i = 0; i < n; ++i) {
            scanf("%d %d", &(val[i]), &(wt[i])); 
        }
    }
    MPI_Bcast(&val[0], n, MPI_INT, 0, MPI_COMM_WORLD); //ENVIA E RECEBE
    MPI_Bcast(&wt[0], n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);


    int *map = (int*) calloc(W, sizeof(int));
    for (int k = 0; k < W; ++k) {
		map[k] = k%numproc;
	}

    print_knapsack(n,W,val,wt,rank,map);
    int result = knapsack(W, wt, val, n, rank, numproc, map);

    if (rank==0) printf("%d\n", result);
    MPI_Finalize();

    return 0;
}
