#!/bin/sh

if [ -n "$1" ]; then
    ./TermChess -qt < tests/$1.in > /dev/null
    diff test.out tests/$1.out && echo "OK" || echo "ERR"
    exit 1
fi

for i in tests/*.in; do
    TEST=${i%.in}
    ./TermChess -t < $i > /dev/null
    cmp -s test.out $TEST.out && printf "\033[32m${TEST:6}\n" || printf "\033[31m${TEST:6}\n"
done
