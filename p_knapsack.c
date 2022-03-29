#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

//---------------------------------------VARIÁVEIS GLOBAIS
int N_THREADS;
int CACHE_SIZE;
int CHUNCK_SIZE;



//---------------------------------------FUNÇÕES UTILITÁRIAS
//FUNÇÃO PARA O CÁCULO DO TEMPO
double timestamp(void){
	struct timeval tp;
	gettimeofday(&tp,NULL);
	return ((double)(tp.tv_sec + (double)tp.tv_usec/1000000)); //em segundos
}



// FUNÇÃO UTILITÁRIA, que vai retornar o máximo entre dois inteiros
int max(int a, int b){
	return (a > b) ? a : b;
}


//---------------------------------------FUNÇÃO knapSack
int knapSack(int W, int wt[], int val[], int n){
    //aloca o vetor com uma posição a mais que o peso da mochila e inicializa com zeros
	int *dp = (int*) calloc(W+1, sizeof(int));
	int *aux = (int*) calloc(W+1, sizeof(int)); //utilizado para guardar os itens que devem ser comparados na próxima iteração
	int j, weight, calc, value, max_v = 0;

	//define o chunk_size. Como estamos trabalhando com três vetores de inteiros(dp,wt,val), então 3 * 4 bytes
	CHUNCK_SIZE = CACHE_SIZE / N_THREADS*8;


	int i = 1;
	int w = W;
	
	for (i = 1; i <= n; i++){
		weight = wt[i];
		value = val[i];
		
	#pragma omp parallel num_threads(N_THREADS) shared(dp, weight, value, W) private(w) firstprivate(i) proc_bind(close)
	{
	#pragma omp for simd schedule(guided,CHUNCK_SIZE)
		for (w = 1; w <= W; w++){
			if (weight <= w)
			//encontra o maior valor
			aux[w] = max(dp[w],
						 dp[w - weight] + value);
		}
		
    }
	memmove(dp, aux, (W+1) * sizeof(int) );
	//memset(aux,0,sizeof(int));
	// for (j = 0; j < W+1; j++)
	// 	printf("%d   ", aux[j]);
	// printf("\n");
	// for (j = 0; j < W+1; j++)
	// 	printf("%d   ", dp[j]);
	// break;
	// printf( "THREADS CRIADAS = %d\n", omp_get_num_threads() );
	}
    return dp[W]; //retorna o maior valor encontrado
}


//---------------------------------------MAIN
int main(int argc, char **argv){
	if(argc != 3){
		printf("argc = %d\n", argc);
		// printf("argv[0] = %s\n", argv[0]);
		// printf("argv[1] = %s\n", argv[1]);
		// printf("argv[2] = %s\n", argv[2]);
		// printf("argv[3] = %s\n", argv[3]);
		// printf("argv[4] = %s\n", argv[4]);

		printf("USAGE: p_knapsack <THREADS> <CACHE_SIZE> <( < file_of_items )> \n");
		return -1;
	}

	//---------LEITURA DA ENTRADA
	int n, W;

	N_THREADS = atoi(argv[1]);
	// printf("N_THREADS = %d\n", N_THREADS);

    CACHE_SIZE = atoi(argv[2]);
    CACHE_SIZE *= 1024;
	// printf("CACHE_SIZE = %d\n", CACHE_SIZE);


	scanf("%d %d", &n, &W);
	int *val = (int*) calloc(n, sizeof(int)); //VALOR(profit)
	int *wt = (int*) calloc(n, sizeof(int)); //PESOS(weight)

	int i;
	for (i = 0; i < n; ++i){
		scanf("%d %d", &(val[i]), &(wt[i])); 
	}
	//--------------------------

	

	//---------ALGORITMO
	double timeBegin = timestamp();
    int max_value = knapSack(W, wt, val, n);
    double timeEnd = timestamp();

	// printf("%d       ", sizeof(int));


	// printf("MAXIMUM VALUE = %d, with %g ms\n", max_value, timeEnd-timeBegin);
	printf("%d,%d,%d,%d,%g\n", N_THREADS,n,W,max_value,timeEnd-timeBegin); //PRINT FINAL
	//--------------------------

	free(val);
	free(wt);
	return 0;
}
