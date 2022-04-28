#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <mpi.h>
//-------------------------ESCOPO DAS FUNÇÕES
double timestamp(void);
int max(int a, int b);
int** get_matrix(int rows, int columns);
void free_matrix(int** mat);
void print_knapsack (int n, int W, int *val, int *wt, int rank);
int knapsack(int MAXIMUM_CAPACITY, int wt[], int val[], int n, int rank, int numproc);
//-------------------------


//---------------------------------------VARIÁVEIS GLOBAIS
double seq_time = 0;
double par_time = 0;




//---------------------------------------FUNÇÕES UTILITÁRIAS
// FUNÇÃO PARA O CÁCULO DO TEMPO
double timestamp(void){
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return ((double)(tp.tv_sec + (double)tp.tv_usec/1000000)); //em segundos
}

// FUNÇÃO UTILITÁRIA que vai retornar o máximo entre dois inteiros
int max(int a, int b) {return (a > b) ? a : b;}


// FUNÇÃO UTILITÁRIA que vai alocar uma matriz auxiliar
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

// FUNÇÃO UTILITÁRIA que vai desalocar uma matriz
void free_matrix(int** mat) {
    free(mat[0]);
    free(mat);
}

// FUNÇÃO UTILITÁRIA, utilizada para verificar se todos os nós contém o mesmo conteúdo
void print_knapsack (int n, int W, int *val, int *wt, int rank) {
    printf("RANK = %d, n = %d, W = %d\n", rank,n,W);
    // printf("RANK = %d\n", rank);

    for (int i = 0; i < n; ++i) {
        printf(" %d %d ", val[i], wt[i]);
    }
    printf("\n\n");

}
//---------------------------------------

//---------------------------------------FUNÇÃO knapSack
int knapsack(int MAXIMUM_CAPACITY, int wt[], int val[], int n, int rank, int numproc) {
    // Matrix-based solution
    int** V = get_matrix(n + 1, MAXIMUM_CAPACITY + 1);

    // V Stores, for each (1 + i, j), the best profit for a knapscak
    // of capacity `j` considering every item k such that (0 <= k < i)
    int i, j, k, previous_value, replace_items, value, tag_s, tag_r;
    MPI_Status status;
    MPI_Request request;

    //cálculo do chunk_size, ou seja, o pedaço com que cada processador vai ficar responsável
    int chunk = (MAXIMUM_CAPACITY+1)/numproc;
    int init, end;
    // evaluate item `i`
    for(i = 0; i <= n; i++) {
        init = rank*chunk;
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

        //realiza uma operação de all_gather para juntar todas as frações de chunks que foram calculadas por outros processadores
        MPI_Allgather(&V[i][init], chunk,MPI_INT,&V[i][0],chunk,MPI_INT,MPI_COMM_WORLD);


        //verifica e calcula se existe algum resto de matriz a ser calculada. se existir, isso ficará a cargo do root
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
            
            //realiza o broadcast da quantidade de itens (end-init) que foram calculados como resto da operação para todos os processadores
            MPI_Bcast(&V[i][init], end-init, MPI_INT, 0, MPI_COMM_WORLD);
            }
        }
       
    }

    int retval = V[n][MAXIMUM_CAPACITY];
    
    free_matrix(V);
    
    return retval;
}
//---------------------------------------



//---------------------------------------MAIN
int main(int argc,char *argv[]) {
    int n, W;
    int *val, *wt;

    int numproc, rank;

    double t_init, t_end;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&numproc);

    //leitura do número de itens e peso máximo da mochila
    if (rank == 0) {
        t_init = timestamp();                    //T1

        // printf("\nnumproc = %d\n", numproc);
        scanf("%d %d", &n, &W);
    }

    //Broadcast (envio e recebimento) dos dois valores lidos para todos os nós alocarem vetores necessários
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&W, 1, MPI_INT, 0, MPI_COMM_WORLD);

    val = (int*) calloc(n, sizeof(int));
    wt = (int*) calloc(n, sizeof(int));

    //leitura dos valores de entrada para a mochila
    if (rank == 0) {
        for (int i = 0; i < n; ++i) {
            scanf("%d %d", &(val[i]), &(wt[i])); 
        }
    }

    //Broadcast (envio e recebimento) dos valores lidos para a mochila
    MPI_Bcast(&val[0], n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&wt[0], n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // print_knapsack(n,W,val,wt,rank);
    //---------ALGORITMO
    int max_value = knapsack(W, wt, val, n, rank, numproc);
    //--------------------------

    if (rank==0) {
        t_end = timestamp();                     //T2
        //n_obj;max_weight;max_value;total_time
        printf("%d;%d;%d;%g\n" ,n,W,max_value,t_end-t_init); //PRINT FINAL
    }

    MPI_Finalize();
    return 0;
}