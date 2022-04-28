#!/bin/bash
# echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
DATE="$(date +"%H%M")"

procs=(1 2 3 4 5 6)

OUTPUT=./output_csv
INPUT=./input/
ITEMS=$(find $INPUT -maxdepth 1 -type f | xargs ls -1t | tac)

make all

# SEQUENCIAL (dynamic programming approach)
SEQ=./sequential
for entry in ${ITEMS[@]}
do
    size=$(echo $entry | awk -F"/" '{print $3}')
    echo "SEQUENCIAL: $entry"
    
    out_s=$OUTPUT"/sequencial/"$size
    out_s=$out_s.csv
    echo "i;n_obj;max_weight;max_value;parallelizable_time;total_time" > "$out_s"
    for i in {1..20}
    do
        printf "$((i));" >> "$out_s"
        # nice -n -20 $SEQ < $entry >> "$out_s"
        $SEQ < $entry | tr "." "," >> "$out_s"
    done
done



# PARALELO
PAR=./parallel
for entry in ${ITEMS[@]}
do
    size=$(echo $entry | awk -F"/" '{print $3}')

    for proc in ${procs[@]}
    do
        echo "PARALELO($proc): $entry"
        out_p=$OUTPUT"/paralelo/"$proc-$size
        out_p=$out_p.csv
        echo "i;procs;n_obj;max_weight;max_value;total_time" > "$out_p"

        for i in {1..20}
        do
            printf "$((i));" >> "$out_p"
            printf "$((proc));" >> "$out_p"
            # nice -n -20 $PAR $proc $CACHEL2 < $entry >> "$out_p"
            mpirun -np $proc $PAR < $entry | tr "." "," >> "$out_p"
        done
    done
done


make clean