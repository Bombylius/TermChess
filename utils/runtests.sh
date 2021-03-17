#!/bin/bash

[ "$1" == "" ] || (./chess -qt < tests/$1.in > /dev/null && cmp test.out tests/$1.out > /dev/null && echo "OK" || echo "ERR")
[ "$1" == "" ] || exit 1

LAST=$(ls tests | tail -n1)
COUNT=${LAST:0:1}
for ((i=0;i<=$COUNT;i+=1)); do
    ./chess -qt < tests/$i.in > /dev/null
    cmp test.out tests/$i.out > /dev/null && echo "$i OK" || echo "$i ERR"
done
