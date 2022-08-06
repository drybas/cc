#!/bin/bash

assert() {
 expected="$1"
 input="$2"
 echo "checking $input => $expected"
 ./cc "$input" > tmp.S
 cc -o tmp tmp.S
 ./tmp
 actual="$?"

 if [ "$actual" = "$expected" ]; then
 echo "$input => $actual"
 else
 echo "$input => $expected expected, but got $actual"
 exit 1
 fi
}