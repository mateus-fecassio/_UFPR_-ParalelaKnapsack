#!/bin/bash

gcc createItem.c -o createItem

n_obj=(100 200 400 800 1600)
weight_max=(40 80 160 320 640)


for obj in ${n_obj[@]}
do
    for wei in ${weight_max[@]}
    do
        ./createItem ../input/n_Obj-$obj--maxWeight-$wei $obj $wei
    done
done

rm -rf createItem
echo "all files created in ../input"