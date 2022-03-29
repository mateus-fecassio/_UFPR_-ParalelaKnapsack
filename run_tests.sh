#!/bin/bash
DATE="$(date +"%H%M")"

# VERIFICAR O TAMANHO DA CACHE L2
CACHEL2=$(cat /sys/devices/system/cpu/cpu0/cache/index2/size | tr "K" " ")
if [ -z "$CACHEL2" ]
then
      echo "\$CACHEL2 is NULL, assigning 256KB"
      CACHEL2=256
fi

# EXTRAIR A TOPOLOGIA DA MÁQUINA ONDE SERÁ EXECUTADO
# lstopo --output-format png -v --whole-system --no-collapse --gridsize 20 --horiz > cpu_info.png


# export OMP_WAIT_POLICY=PASSIVE
export OMP_WAIT_POLICY=ACTIVE

threads=(2 4 8 12)

OUTPUT=./output_csv
INPUT=./input/
ITEMS=$(find $INPUT -maxdepth 1 -type f | xargs ls -1t | tac)



# SEQUENCIAL (recursivo) -- IMPRATICÁVEL! DEMORA DEMAIS (entradas acima de 400 objetos)
# gcc -O3 1-naive.c -o naive
# REC=./naive
# for entry in ${ITEMS[@]}
# do
    
#     size=$(echo $entry | awk -F"/" '{print $3}')
#     echo "RECURSIVO: $entry"
    
#     out_s=$OUTPUT"/recursivo/"$size
#     out_s=$out_s.csv
#     echo "i;n_obj;max_weight;max_value;seq_time" > "$out_s"
#     for i in {1..1}
#     do
#         printf "$((i));" >> "$out_s"
#         # nice -n -20 $REC < $entry >> "$out_s"
#         $REC < $entry | tr "." "," >> "$out_s"
#     done
# done




# SEQUENCIAL (dynamic programming approach)
gcc -O3 2-s_knapsack.c -o s_knapsack
SEQ=./s_knapsack
for entry in ${ITEMS[@]}
do
    size=$(echo $entry | awk -F"/" '{print $3}')
    echo "SEQUENCIAL: $entry"
    
    out_s=$OUTPUT"/sequencial/"$size
    out_s=$out_s.csv
    echo "i;n_obj;max_weight;max_value;seq_time" > "$out_s"
    for i in {1..20}
    do
        printf "$((i));" >> "$out_s"
        # nice -n -20 $SEQ < $entry >> "$out_s"
        $SEQ < $entry | tr "." "," >> "$out_s"
    done
done



# PARALELO
gcc -fopenmp -O3 3-p_knapsack.c -o p_knapsack
PAR=./p_knapsack
for entry in ${ITEMS[@]}
do
    size=$(echo $entry | awk -F"/" '{print $3}')

    for thread in ${threads[@]}
    do
        echo "PARALELO($thread): $entry"
        out_p=$OUTPUT"/paralelo/"$thread-$size
        out_p=$out_p.csv
        echo "i;N_THREADS;n_obj;max_weight;max_value;time;seq_time;par_time" > "$out_p"

        for i in {1..20}
        do
            printf "$((i));" >> "$out_p"
            # nice -n -20 $PAR $thread $CACHEL2 < $entry >> "$out_p"
            $PAR $thread $CACHEL2 < $entry | tr "." "," >> "$out_p"
        done
    done
done


rm -rf $REC
rm -rf $SEQ
rm -rf $PAR