#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

//---------------------------------------VARIÁVEIS GLOBAIS


//---------------------------------------FUNÇÕES UTILITÁRIAS
//FUNÇÃO PARA O CÁCULO DO TEMPO
double timestamp(void){
	struct timeval tp;
	gettimeofday(&tp,NULL);
	// return((double)(tp.tv_sec*1000.0 + (double)tp.tv_usec/1000.0)); //microsseconds
	return ((double)(tp.tv_sec + (double)tp.tv_usec/1000000)); //seconds
}


// FUNÇÃO UTILITÁRIA, que vai retornar o máximo entre dois inteiros
int max(int a, int b){
	return (a > b) ? a : b;
}



//---------------------------------------FUNÇÃO knapSack
int knapSack(int W, int wt[], int val[], int n){
    //aloca o vetor com uma posição a mais que o peso da mochila e inicializa com zeros
	int *dp = (int*) calloc(W+1, sizeof(int));
  
    for (int i = 1; i < n + 1; i++){
        for (int w = W; w >= 0; w--){
            if (wt[i - 1] <= w)
                //encontra o maior valor
                dp[w] = max(dp[w],
                            dp[w - wt[i - 1]] + val[i - 1]);
        }
    }
    return dp[W]; //retorna o maior valor encontrado
}



//---------------------------------------MAIN
int main(){
	//---------LEITURA DA ENTRADA
	int n, W;

	scanf("%d %d", &n, &W);
	int *val = (int*) calloc(n, sizeof(int)); //VALOR(profit)
	int *wt = (int*) calloc(n, sizeof(int)); //PESOS(weight)

	int i;
	for (i = 0; i < n; ++i){
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