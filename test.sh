#!/bin/bash

assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s

	cc -o tmp tmp.s testf.o
	./tmp

	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

call(){
	input="$1"

	./9cc "$input" > tmp.s

	echo "$input was called."

	cc -o tmp tmp.s testf.o
	./tmp

}


assert 5 " 3 + 2;"
assert 255 "z = 255;"
assert 5 "a = 3; b = 2; c = a + b;"
assert 6 "a = 3; b = 3; c = 2; d = 5; e = 3; ((a + b) * (d - c))/e;"

assert 11 "foo = 3; bar = 2; foo + bar * 4;"
assert 1 "hoge = 2; piyo = 5; hoge * 5 == piyo * 2;"
assert 1 "hoge = 3; piyo = 2; (hoge-2)*3 == (piyo != 0) + piyo;"

assert 5 "return a = 5;"
assert 10 "hoge = 2; piyo = 5; return hoge*piyo;"
assert 2 "hoge = 4; piyo = 2; foo = hoge/piyo; return foo;"

assert 3 "hoge = 1; piyo = 2; if ((hoge + piyo)*10 == 30) return 3; else return 4;"
assert 4 "hoge = 1; piyo = 2; if ((hoge + piyo)*10 != 30) return 3; else return 4;"

assert 10 "test = 1; while (test < 10) test = test + 1; return test;"
assert 50 "test = 1; while (test*2 < 100) test = test+ 1;"

assert 110 "foo = 0; for (i = 0; i <= 10; i = i + 1) foo = foo + 10; foo;"
assert 16 "hoge = 0; piyo = 5; for( ; hoge <= 10; ) hoge = hoge + 1; return hoge + piyo;"
assert 20 "hoge = 5; piyo = 5; for(foo = 0; foo < 10; ) foo = foo + 1; return hoge + piyo + foo;"

assert 3 "{a = 1; b = 2; return a + b;}"
assert 30 "hoge = 0; piyo = 0; for (i = 0; i < 10; i = i + 1){hoge = hoge + 1; piyo = piyo + 2;} return hoge + piyo;"
assert 30 "hoge = 0; piyo = 0; while(hoge < 10){hoge = hoge + 1; piyo = piyo + 2;} return hoge + piyo;"
assert 3 "hoge = 0; piyo = 0; if (hoge == 0){hoge = hoge + 1; piyo = piyo + 2;} else{hoge = 10; piyo = 10;} return hoge + piyo;"
assert 20 "hoge = 0; piyo = 0; if (hoge != 0){hoge = hoge + 1; piyo = piyo + 2;} else{hoge = 10; piyo = 10;} return hoge + piyo;"

call "for (i = 0; i < 10; i = i + 1) foo();"
call "a = 1; b = 2; hoge(3, 4, 5);"
#assert 12  "hoge(3, 4, 5);"
call "piyo(1, 2, 3, 4, 5, 6);"

echo OK
