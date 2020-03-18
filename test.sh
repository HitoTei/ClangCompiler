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

try 47 '5+6*7;'
try 15 '5*(9-6);'
try 4 '(3+5)/2;'
try 0 "0 * (219 + 32 - 32 / 32 + 13 * 3);"

try 0 "1 + -1;"
try 2 "1 + +1;"

try 1 "1 == 1;"

try 10 '- - +10;'


try 0 '0==1;'
try 1 '42==42;'
try 1 '0!=1;'
try 0 '42!=42;'

try 1 '0<1;'
try 0 '1<1;'
try 0 '2<1;'
try 1 '0<=1;'
try 1 '1<=1;'
try 0 '2<=1;'

try 1 '1>0;'
try 0 '1>(1 == 1);'
try 0 '1>2;'
try 1 '1>=0;'
try 1 '1>=1;'
try 0 '1>=2;'

try 10 'a = 100;b = 90;a - b;'
try 10 'abc = 100;def = 90;abc - def;'
echo OK