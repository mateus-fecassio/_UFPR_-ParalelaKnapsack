#!/bin/bash

gcc createItem.c -o createItem

n_obj=(10000 20000)
weight_max=(1000)


for obj in ${n_obj[@]}
do
    for wei in ${weight_max[@]}
    do
        ./createItem ../input/n_Obj-$obj--maxWeight-$wei $obj $wei
    done
done

rm -rf createItem
echo "all files created in ../input"