#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long int n_obj[5]={20000, 40000, 80000, 160000, 320000};
long int weight_max[5]={50000, 100000, 200000, 400000, 800000};

int main(){
    int count_files = 0;
    char cmd[100];
    long int weight,obj;

    long int size_obj = sizeof(n_obj)/sizeof(long int);
    long int size_weight = sizeof(weight_max)/sizeof(long int);

    for(obj=0; obj < size_obj; obj++)
        for(weight=0; weight < size_weight; weight++){
            count_files++;
            sprintf(cmd,"./createItem input/n_Obj-%ld--maxWeight-%ld %ld %ld", n_obj[obj], weight_max[weight], n_obj[obj], weight_max[weight]);
            if(system(cmd));
        }
    // printf("\nfiles type: n_Obj-maxWeight\nto use all %d files, run ./test.sh\n",count_files);
    printf("\nfiles type: n_Obj-maxWeight\nto use all %d files, run ./run_tests.sh\n", count_files);
}
