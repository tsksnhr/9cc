#!/bin/bash

assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s

	cc -o tmp tmp.s flist.o
	./tmp

	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

tiny_echo(){
	input="$1"

	./9cc "$input" > tmp.s

	echo "$input"

	cc -o tmp tmp.s flist.o
	./tmp

}

assert 1 "main(){a = 1; return a;}"

assert 5 "main(){a = 1; if (a == 1) return 5; else return 3;}"
assert 3 "main(){a = 0; if (a == 1) return 5; else return 3;}"

assert 4 "main(){return 1 + hoge(1, 2);}"
assert 2 "main(){ return 1 + 1;}"
assert 4 "tinyadd(x, y){return x + y;} main(){ return tinyadd(1, 3); }"
assert 24 "foo(num){ a = 1; for(i = 1; i <= num; i = i + 1;){ a = a * i;} return a;} main(){return foo(4);}"
tiny_echo "fib(num){ if (num <= 2) return 1; else{ a = num - 2; b = num - 1; return fib(a) + fib(b);}} main(){ for (i = 1; i <= 20; i = i + 1){ ans = fib(i); showint(ans);} return 0;}"

assert 9 "plusone(num){return num + 1;} plustwo(num){return num + 2;} main(){num = 2; a = plusone(num); b = plustwo(num); return num + a + b;}"

echo OK
