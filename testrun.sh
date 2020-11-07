#!/bin/bash

shopt -s extglob

TESTS=$(ls tests/ | grep -v --exclude-dir={output} '\.')

mkdir -p "tests/output"


for test in $TESTS; do
  echo $test
  ./etapa4 < tests/$test > tests/output/${test}_output.txt
done


