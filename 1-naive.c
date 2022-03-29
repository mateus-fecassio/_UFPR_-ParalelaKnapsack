#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//---------------------------------------VARIÁVEIS GLOBAIS


//---------------------------------------FUNÇÕES UTILITÁRIAS
// FUNÇÃO UTILITÁRIA, que vai retornar o máximo entre dois inteiros
int max(int a, int b){
	return (a > b) ? a : b;
}


//FUNÇÃO PARA O CÁCULO DO TEMPO
double timestamp(void){
	struct timeval tp;
	gettimeofday(&tp,NULL);
	// return((double)(tp.tv_sec*1000.0 + (double)tp.tv_usec/1000.0)); //microsseconds
	return ((double)(tp.tv_sec + (double)tp.tv_usec/1000000)); //seconds
}

//---------------------------------------FUNÇÃO knapSack (recursiva)
// Returns the maximum value that can be
// put in a knapsack of capacity W
int knapSack(int W, int wt[], int val[], int n)
{
	// Base Case
	if (n == 0 || W == 0)
		return 0;

	// If weight of the nth item is more than
	// Knapsack capacity W, then this item cannot
	// be included in the optimal solution
	if (wt[n - 1] > W)
		return knapSack(W, wt, val, n - 1);

	// Return the maximum of two cases:
	// (1) nth item included
	// (2) not included
	else
		return max(
			val[n - 1]
				+ knapSack(W - wt[n - 1],
						wt, val, n - 1),
			knapSack(W, wt, val, n - 1));
}



//---------------------------------------MAIN
int main()
{
	//---------LEITURA DA ENTRADA
	int n, W;

	scanf("%d %d", &n, &W);
	int *val = (int*) calloc(n, sizeof(int));
	int *wt = (int*) calloc(n, sizeof(int));

	int i;
	for (i = 0; i < n; ++i) {
		scanf("%d %d", &(val[i]), &(wt[i])); 
	}
	//--------------------------

	//---------ALGORITMO
	double s_init = timestamp();					//T1
    int max_value = knapSack(W, wt, val, n);
	double s_end = timestamp();						//T2
	//--------------------------

	printf("%d;%d;%d;%g\n", n,W,max_value,s_end-s_init); //PRINT FINAL

	free(val);
	free(wt);
	return 0;
}