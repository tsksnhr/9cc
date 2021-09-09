#include "9cc.h"

int foo(){
	printf("foo!\n");
}

int hoge(int a, int b){
	int ans = a + b;
	printf("%d + %d = %d\n", a, b, ans);

	return ans;
}

int piyo(int a, int b, int c, int d, int e, int f){
	int ans = a + b + c + d + e + f;
	printf("%d + %d + %d + %d + %d + %d = %d\n", a, b, c, d, e, f, ans);

	return ans;
}
