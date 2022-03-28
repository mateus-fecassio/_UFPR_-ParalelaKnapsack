#!/bin/bash

PAR=./p_knap
DATE="$(date +"%H%M")"

#verificar o tamanho da cache L2 #
CACHEL2=$(cat /sys/devices/system/cpu/cpu0/cache/index2/size | tr "K" " ")
if [ -z "$CACHEL2" ]
then
      echo "\$CACHEL2 is NULL, assigning 256KB"
      CACHEL2=256
fi

#extrair a topologia da máquina
# lstopo --output-format png -v --whole-system --no-collapse --gridsize 20 --horiz > cpu_info.png


export OMP_WAIT_POLICY=PASSIVE

threads=(2 4 8 12)

OUTPUT=./output_csv
INPUT=./input/
ITEMS=$(find $INPUT -maxdepth 1 -type f | xargs ls -1t | tac)

# SEQUENCIAL
# gcc -O3 s_knapsack.c -o s_knapsack
# SEQ=./s_knapsack
# for entry in ${ITEMS[@]}
# do
#     size=$(echo $entry | awk -F"/" '{print $3}')
    
#     out_s=$OUTPUT"/sequencial/"$size
#     out_s=$out_s.csv
#     echo "i,n_obj,max_weight,max_value,time" > "$out_s"
#     for i in {1..20}
#     do
#         printf "$((i))," >> "$out_s"
#         # nice -n -20 $SEQ $i < $entry >> "$out_s"
#         $SEQ < $entry >> "$out_s"
#     done
# done



# PARALELO
gcc -fopenmp -O3 p_knapsack.c -o p_knapsack
PAR=./p_knapsack
for entry in ${ITEMS[@]}
do
    size=$(echo $entry | awk -F"/" '{print $3}')

    for thread in ${threads[@]}
    do
        out_p=$OUTPUT"/paralelo/"$thread-$size
        out_p=$out_p.csv
        echo "i,N_THREADS,n_obj,max_weight,max_value,time" > "$out_p"

        for i in {1..20}
        do
            printf "$((i))," >> "$out_p"
            # nice -n -20 $PAR $thread $CACHEL2 < $entry >> "$out_p"
            $PAR $thread $CACHEL2 < $entry >> "$out_p"
        done
    done
done
