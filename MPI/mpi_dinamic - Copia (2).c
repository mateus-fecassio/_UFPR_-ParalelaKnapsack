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


int knapsack(int MAXIMUM_CAPACITY, int wt[], int val[], int n){
    // Matrix-based solution
    int** V = get_matrix(n + 1, MAXIMUM_CAPACITY + 1);

    // V Stores, for each (1 + i, j), the best profit for a knapscak
    // of capacity `j` considering every item k such that (0 <= k < i)
    int i, j, k, previous_value, replace_items, value, tag_s, tag_r;
    int numproc, rank;
    MPI_Status status;
	MPI_Request request;



    MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numproc);
    if(rank==0) printf("\nnumproc = %d\n", numproc);

    // evaluate item `i`
    for(i = 0; i < n; i++) {
        for(j = rank; j <= MAXIMUM_CAPACITY; j+=numproc) {
            if(wt[i] <= j) { // could put item in knapsack
                
                // value = V[i][j - wt[i]];
                tag_r = ((i)*n)+(j - wt[i]);
                MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, tag_r, MPI_COMM_WORLD, &status);
                previous_value = V[i][j];
                replace_items = val[i] + value;

                // is it better to keep what we already got,
                // or is it better to swap whatever we have in the bag that weights up to `j`
                // and put item `i`?
                V[1 + i][j]= max(previous_value, replace_items);
                
                tag_s = ((1+i)*n)+j;
                for(k = 0; k < numproc; ++k) {
                   MPI_Isend(&V[1 + i][j], 1, MPI_INT, k, tag_s, MPI_COMM_WORLD, &request); 
                }

                
                
                // MPI_Isend(&V[1 + i][j], 1, MPI_INT, 0, tag_s, MPI_COMM_WORLD, &request);
                // MPI_Bcast(&V[1 + i][j], 1, MPI_INT, rank, MPI_COMM_WORLD);
            }
            else {
                // can't put item `i`
                V[1 + i][j] = V[i][j];
                tag_s = ((1+i)*n)+j;
                for(k = 0; k < numproc; ++k) {
                   MPI_Isend(&V[1 + i][j], 1, MPI_INT, k, tag_s, MPI_COMM_WORLD, &request); 
                }
                // MPI_Bcast(&V[1 + i][j], 1, MPI_INT, rank, MPI_COMM_WORLD);
                // tag_s = ((1+i)*n)+j;
                // MPI_Isend(&V[1 + i][j], 1, MPI_INT, 0, tag_s, MPI_COMM_WORLD, &request);
			}
        }
    }

    MPI_Finalize();

    int retval = V[n][MAXIMUM_CAPACITY]; 
    
    free_matrix(V);
    
    return retval;
}

// Driver program to test above function
int main(int argc,char *argv[])
{
	int n, W;

	scanf("%d %d", &n, &W);
	int *val = (int*) calloc(n, sizeof(int));
	int *wt = (int*) calloc(n, sizeof(int));

	int i;
	for (i = 0; i < n; ++i) {
		scanf("%d %d", &(val[i]), &(wt[i])); 
	}
    
    printf("%d\n", knapsack(W, wt, val, n, argc, argv));
    
    return 0;
}
