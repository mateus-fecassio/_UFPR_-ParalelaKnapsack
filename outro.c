#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#define ERROR(str){printf("%s\n",str); return -1;}
int N_THREADS;
int CACHE_SIZE;
    
double timestamp(void){
	struct timeval tp;
	gettimeofday(&tp,NULL);
	return ((double)(tp.tv_sec + (double)tp.tv_usec/1000000));
}


int knapsack(int *value, int *weight, int n_obj, int weight_max, size_t **V){
    CACHE_SIZE/=N_THREADS*12; //3 arrays of int load per thread
    int w,                                             //peso iterativo 
        i,
	chunck_size = CACHE_SIZE; 
    size_t *tmp,   //line one, two and tmp
            max_value = 0;

        for(i=1; i <= n_obj; i++){                    //percorre apartir do primeiro item até o ultimo (i=0==NULL)
	#pragma omp parallel num_threads(N_THREADS)
	{
	#pragma omp for schedule(dynamic,chunck_size)
            for(w = 1; w <= weight_max; w++){               //percorre desde o peso 1 até o peso maximo da mochila
                if( (weight[i] <= w) && ((max_value = value[i]+V[0][w-weight[i]]) > V[0][w])){    
                    //se o item i caber no peso w E o valor do item i + 
                    //o valor da linha de cima no peso que sobra da mochila com o item i
                    //for maior que o valor do item de cima com o peso w
                    V[1][w]= max_value;   //coloca essa soma
                }else{
                    V[1][w] = V[0][w];                      //senão coloca o valor de cima 
                }
            }
	}
            tmp=V[0];                //tmp recebe linha zero
            V[0]=V[1];                //linha 0 recebe linha 1
            V[1]=tmp;                //linha 1 recebe tmp
            memset(V[1],0,sizeof(size_t)); //zera linha 1
        }
    return V[0][weight_max];
}

void read_file(FILE *input, int *value, int *weight, int i){
    char line[32];                          
    while(fgets(line,32,input)){                    
        value[i] = atoi(strtok(line," "));
        weight[i] = atoi(strtok(NULL," "));
        i++;
    };
}                                                                                                     

int main(int argc, char **argv){
    if(argc!=4) ERROR("USAGE: knapsack <file_of_items> <THREADS> <CACHE_SIZE>")
    
    FILE *file = fopen(argv[1],"r");

    if(!file) ERROR("File couldn't opened")

    N_THREADS = atoi(argv[2]);
    CACHE_SIZE = atoi(argv[3]);
    CACHE_SIZE*=1024;

    int max_weight, n_obj, *values, *weights;

    if(fscanf(file,"%d %d\n",&n_obj,&max_weight));
 
    values = malloc((n_obj+1)*sizeof(int));
    weights = malloc((n_obj+1)*sizeof(int));

    read_file(file,values,weights,1); //le o arquivo e coloca apartir da posição 1, para deixar i igual o item 
    fclose(file);

    size_t **bottom = malloc(2*sizeof(size_t *));  //it's needly only two rows
                                            
    for(int i=0; i<2; i++){
    	bottom[i]= malloc((max_weight+1) * sizeof(size_t));
        memset(bottom[i],0,(max_weight+1) * sizeof(size_t));
    }

    double timeBegin = timestamp();
    size_t max_value = knapsack(values,weights,n_obj,max_weight,bottom);
    double timeEnd = timestamp();

    printf("%d,%d,%d,%d,%zd,%f\n",N_THREADS,CACHE_SIZE,n_obj,max_weight,max_value,timeEnd-timeBegin);
	
    free(values);	
    free(weights);
    free(bottom[0]);
    free(bottom[1]);
    free(bottom);
    return 0;
}
