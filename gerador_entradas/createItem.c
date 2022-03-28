#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> 


#define ERROR(str){printf("%s\n",str); return -1;}

int numb(int mwi){
    int n = 1;
    while(n%5!=0){
        n=rand() % mwi + 1;
    }
    return n;
}

int stringToInt(char *str){
    int tam = strlen(str);
    int hash = 0;
    for(int i=0; i<tam; i++)
        hash+=(int)str[i];
    return hash;
}

int main(int argc, char **argv){
    if(!argv[1]||!argv[2]||!argv[3]) ERROR("USAGE: generateItems <file> <number_of_items> <max_wight_of_items>")
    
    FILE *file = fopen(argv[1],"w");

    if(!file) ERROR("File couldn't opened")    

    srand((unsigned int)stringToInt("paralela_eh_top"));
    int noi=atoi(argv[2]), mwi=atoi(argv[3]);
    fprintf(file,"%d %d\n",noi,mwi);

    for(int i=0; i<noi; i++){
        fprintf(file,"%d %d\n",numb(mwi),numb(mwi));
    }
    printf("%s %d %d\n",argv[1],noi,mwi);

    fclose(file);

    return 0;

}
