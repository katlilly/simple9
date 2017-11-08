#!/bin/bash

rm -f nums.txt;
for i in `seq 10000`;
do  echo $RANDOM;
done > nums.txt;



