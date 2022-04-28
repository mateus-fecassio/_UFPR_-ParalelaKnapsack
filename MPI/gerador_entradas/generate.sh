#!/bin/bash

gcc createItem.c -o createItem

n_obj=(2000 4000 8000 12000 16000 18000 20000)
weight_max=(100000)


for obj in ${n_obj[@]}
do
    for wei in ${weight_max[@]}
    do
        ./createItem ../input/n_Obj-$obj--maxWeight-$wei $obj $wei
    done
done

rm -rf createItem
echo "all files created in ../input"