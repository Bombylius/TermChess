#!/bin/bash

LAST=$(ls tests | tail -n1)
COUNT=${LAST:0:1}
for ((i=0;i<=$COUNT;i+=1)); do
    ./chess -qt < tests/$i.in > /dev/null
    mv test.out tests/$i.out
done
