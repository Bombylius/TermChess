#!/bin/bash

[ "$1" == "c" ] && make
LAST=$(ls tests | tail -n1)
COUNT=${LAST:0:1}
for ((i=0;i<=$COUNT;i+=1)); do
    ./chess test < tests/$i.in > /dev/null
    cmp test.out tests/$i.out > /dev/null && echo "$i OK" || echo "$i ERR"
done
