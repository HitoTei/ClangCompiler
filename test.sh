#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./Ccompiler "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

try 47 '5+6*7'
try 15 '5*(9-6)'
try 4 '(3+5)/2'
try 0 "0 * (219 + 32 - 32 / 32 + 13 * 3)"
echo OK