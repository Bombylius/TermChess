#!/bin/sh

for i in tests/*.in; do
    ./chess -qt < $i > /dev/null
    mv test.out ${i%.in}.out
done
