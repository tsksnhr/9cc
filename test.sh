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

assert 5 " 3 + 2;"
assert 255 "z = 255;"
assert 5 "a = 3; b = 2; a + b;"
assert 6 "a = 3; b = 3; c = 2; d = 5; e = 3; ((a + b) * (d - c))/e;"

echo OK
