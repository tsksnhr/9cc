#!/bin/bash

assert(){

	expected="$1"
	input="$2"

	/home/9cc/9cc "$input" > tmp.s
	cc -o tmp tmp.s
	/home/9cc/tmp
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

assert 21 "5+20-4"
assert 46 " 5 + 40 - 2 + 3"

echo OK
