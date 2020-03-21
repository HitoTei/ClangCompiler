#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
EOF

try() {
  expected="$1"
  input="$2"

  ./Ccompiler "$input" > tmp.s
  gcc -static -o tmp tmp.s tmp2.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}


try 55 "i=0;j = 0;while(i<=10){j = j+i;i=i+1;} return j;"

try 47 '5+6*7;'
try 15 '5*(9-6);'

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
try 1 '0<=1;'
try 1 '1<=1;'

try 1 '1>0;'
try 1 '1>=0;'

try 10 'a = 100;b = 90;a - b;'
try 10 'abc = 100;def = 90;return abc - def;'

try 1 'if(1)return 1;'
try 2 'if(0)return 1-11; else return 1+1;'

try 10 'i=0; while(i<10) i=i+1; return i;'

try 55 'i=0; j=0; for (i=0; i<=10; i=i+1) j=i+j; return j;'
try 3 'for (;;) return 3; return 5;'

try 3 '{1; {2;} return 3;}'

try 3 'return ret3();'
try 5 'return ret5();'

try 10 'ret5 = ret5(); return ret5 + 5;'

echo OK