#!/bin/sh

[ -z "$1" ] && exit 1
if [ -f test.in -a -f test.out ]; then
    mv -i test.in tests/$1.in
    mv -i test.out tests/$1.out
else
    echo "No test available."
    exit 1
fi
