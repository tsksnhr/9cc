#!/bin/bash

assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s

	cc -o tmp tmp.s
	./tmp

	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

assert 0 0
assert 42 42

assert 21 '5+20-4'
assert 46 ' 5 + 40 - 2 + 3'

assert 26 '2 * 3 + 4 * 5'
assert 44 '4 * ( 6 + 5 )'
assert 3 '36 / ( 4 + 8 )'

assert 2 '-3+5'
assert 155 '+ 5 * ( + 6 + 5 ) -10 + 110'

assert 1 " 2 == 2 "
assert 0 " 2 == 1"

assert 1 " 1 != 2 "
assert 0 " 1 != 1 "

assert 1 " 1 <= 1 "
assert 1 " 1 <= 2 "
assert 1 " 1 < 2 "
assert 0 " 1 > 2 "
assert 0 " 1 >= 2 "

assert 1 " 1 >= 1 "
assert 1 " 2 >= 1"
assert 1 " 3 > 2 "
assert 0 " 2 > 3 "
assert 0 " 2 >= 3 "

assert 1 " (1 + 3) == (2 * 2) "
assert 2 "(1 == 1) + (2 < 3) + (3 < 2) "
assert 1 " (1 != (1 == 2)) "

echo OK
