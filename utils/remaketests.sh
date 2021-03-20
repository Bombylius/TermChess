#!/bin/sh

for i in tests/*.in; do
    ./chess -t < $i > /dev/null
    mv test.out ${i%.in}.out
done
