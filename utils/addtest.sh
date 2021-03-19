#!/bin/sh

[ -z "$1" ] && exit 1
mv -i test.in tests/$1.in
mv -i test.out tests/$1.out
