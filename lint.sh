#!/bin/bash

sourceFiles="libCombinatorEvolve/*pp"

red="\\\033[0;31m"
green="\\\033[0;32m"
endColor="\\\033[0m"

formatInPlace=0

for arg in "$@"
do
  case $arg in
    -i)
    formatInPlace=1
    shift
    ;;
    *)
    echo "Argument $arg is not recognized."
    echo
    echo "Usage: ./lint.sh [-i]"
    echo "Analyze the C++ code with clang-format and cpplint."
    echo
    echo "Options:"
    echo "  -i  Inplace edit files with clang-format."
    exit 1
    ;;
  esac
done

exitStatus=0

for file in $sourceFiles; do
  diff=$(diff -U0 --label $file $file --label formatted <(clang-format $file))
  if [ $formatInPlace -eq 1 ]; then
    clang-format -i $file
  fi
  if [[ ! -z "$diff" ]]; then
    printf -- "$(echo "$diff\n\n" | sed "s|^-|$red-|g" | sed "s|^+|$green+|g" | sed "s|$|$endColor|g")"
    exitStatus=1
  fi
done

if ! cpplint --quiet --extensions=hpp,cpp $sourceFiles; then
  exitStatus=1
fi

exit $exitStatus
