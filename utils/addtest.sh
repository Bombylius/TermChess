#!/bin/sh

LAST=$(ls tests | tail -n1)
[ "$LAST" == "" ] && COUNT=0 || COUNT=$((${LAST:0:1} + 1))
mv test.in tests/$COUNT.in
mv test.out tests/$COUNT.out
