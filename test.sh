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

assert 11 "foo = 3; bar = 2; foo + bar * 4;"
assert 1 "hoge = 2; piyo = 5; hoge * 5 == piyo * 2;"
assert 1 "hoge = 3; piyo = 2; (hoge-2)*3 == (piyo != 0) + piyo;"

assert 5 "return a = 5;"
assert 10 "hoge = 2; piyo = 5; return hoge*piyo;"
assert 2 "hoge = 4; piyo = 2; foo = hoge/piyo; return foo;"

assert 3 "hoge = 1; piyo = 2; if ((hoge + piyo)*10 == 30) return 3; else return 4;"
assert 4 "hoge = 1; piyo = 2; if ((hoge + piyo)*10 != 30) return 3; else return 4;"

echo OK
