#!/bin/sh

if [ -n "$1" ]; then
    ./chess -qt < tests/$1.in > /dev/null
    cmp test.out tests/$1.out > /dev/null && echo "OK" || echo "ERR"
    exit 1
fi

for i in tests/*.in; do
    TEST=${i%.in}
    ./chess -qt < $i > /dev/null
    cmp test.out $TEST.out > /dev/null && printf "\033[32m${TEST:6}\n" || printf "\033[31m${TEST:6}\n"
done
