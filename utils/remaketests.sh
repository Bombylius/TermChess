#!/bin/sh

for i in tests/*.in; do
    ./TermChess -t < $i > /dev/null
    mv test.out ${i%.in}.out
done
