#!/bin/bash

source ./assert.sh

assert 42 '42;'
assert 21 '5+20-4 ;'
assert 41 ' 12 + 34 - 5 ;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 1 '5 == 5;'
assert 0 '5 == 6;'
assert 1 '5 != 6;'
assert 1 '5 < 145;'
assert 1 '5 <= 145;'
assert 0 '0 >= 1;'
assert 1 '0 >= 0;'
assert 1 '0 > -1;'
assert 1 '-5 < -1;'

# local variables 
assert 14 'foo = 5; foo = foo + 9;'
assert 34 'foo = 2; bar = 33; foo = bar + 1;'
assert 37 'foo = 2; bar = 33; foo = bar + 1; foo = foo + 3;'

# return statement
assert 1 'return 1; foo = 2; bar = 33; foo = bar + 1; foo = foo + 3; return foo;'
assert 37 'foo = 2; bar = 33; foo = bar + 1; foo = foo + 3; return foo;'

