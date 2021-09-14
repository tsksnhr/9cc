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

# if "return" doesn't exist, segmentaiton fault will occur
tiny_echo "int fib(int num){ if (num <= 2) return 1; else {int a = num-2; int b = num-1; return fib(a) + fib(b);}} int main(){ for (int i = 1; i <= 10; i = i + 1){ int ans = fib(i); showint(ans);} return 0;}"

assert 40 "int func(int a, int b){int c = a + b; return c;} int main(){int x = 10; for (int i = 0; i < 10; i = i + 1) x = x + 1; int y = 20; return func(x, y);}"
assert 10 "int func(int a){ return a;} int main(){int a = 10; return func(a);}"

assert 3 "int main(){ int a; a = 3; int *b; b = &a; return *b;}"
assert 4 "int main(){ int x; int *y; y = &x; *y = 3 + 1; return x;}"

assert 2 "int main(){ int *p; p = tiny_alloc(1, 2, 4, 8); showptr(p); int *q; q = p + 1; showptr(q); return *q;}"
assert 4 "int main(){ int *p; p = tiny_alloc(1, 2, 4, 8); showptr(p); int *q; q = p + 2; showptr(q); return *q;}"
assert 8 "int main(){ int *p; p = tiny_alloc(1, 2, 4, 8); showptr(p); int *q; q = p + 3; showptr(q); return *q;}"

assert 4 "int main(){ int *p; p = tiny_alloc(1, 2, 4, 8); showptr(p); int *q; q = p + 3; q = q - 1; showptr(q); return *q;}"
assert 2 "int main(){ int *p; p = tiny_alloc(1, 2, 4, 8); showptr(p); int *q; q = p + 3; q = q - 2; showptr(q); return *q;}"
assert 1 "int main(){ int *p; p = tiny_alloc(1, 2, 4, 8); showptr(p); int *q; q = p + 3; q = q - 3; showptr(q); return *q;}"

echo OK
