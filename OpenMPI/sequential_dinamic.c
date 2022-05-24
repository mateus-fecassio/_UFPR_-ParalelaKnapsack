#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

//-------------------------ESCOPO DAS FUNÇÕES
double timestamp(void);
int max(int a, int b);
int** get_matrix(int rows, int columns);
void free_matrix(int** mat);
int knapsack(int MAXIMUM_CAPACITY, int wt[], int val[], int n);
//-------------------------


//---------------------------------------VARIÁVEIS GLOBAIS





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
//---------------------------------------

//---------------------------------------FUNÇÃO knapSack
int knapsack(int MAXIMUM_CAPACITY, int wt[], int val[], int n)
{
    // Matrix-based solution
    int** V = get_matrix(n + 1, MAXIMUM_CAPACITY + 1);

    // V Stores, for each (1 + i, j), the best profit for a knapscak
    // of capacity `j` considering every item k such that (0 <= k < i)
    int i, j;

    // evaluate item `i`
    for(i = 0; i < n; i++) {
        for(j = 1; j <= MAXIMUM_CAPACITY; j++) {
            if(wt[i] <= j) { // could put item in knapsack
                int previous_value = V[1 + i - 1][j];
                int replace_items = val[i] + V[1 + i - 1][j - wt[i]];

                // is it better to keep what we already got,
                // or is it better to swap whatever we have in the bag that weights up to `j`
                // and put item `i`?
                V[1 + i][j] = max(previous_value, replace_items);
            }
            else {
                // can't put item `i`
                V[1 + i][j] = V[1 + i - 1][j];
            }
        }
    }

    int retval = V[1 + n - 1][MAXIMUM_CAPACITY]; 
    
    free_matrix(V);
    
    return retval;
}
//---------------------------------------



//---------------------------------------MAIN
int main(){
    double t_init = timestamp();                    //T1

    int n, W;

    scanf("%d %d", &n, &W);
    int *val = (int*) calloc(n, sizeof(int));
    int *wt = (int*) calloc(n, sizeof(int));

    int i;
    for (i = 0; i < n; ++i) {
        scanf("%d %d", &(val[i]), &(wt[i])); 
    }

    //---------ALGORITMO
    double s_init = timestamp();
    int max_value = knapsack(W, wt, val, n);
    double s_end = timestamp();
    //--------------------------
    
    double t_end = timestamp();                     //T2
    //n_obj;max_weight;max_value;parallelizable_time;total_time
    printf("%d;%d;%d;%g;%g\n", n,W,max_value,s_end-s_init,t_end-t_init); //PRINT FINAL

    free(val);
    free(wt);
    return 0;
}