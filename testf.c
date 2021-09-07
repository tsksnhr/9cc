#include "9cc.h"

int foo(){
	printf("function \"foo\" was called!\n");
}

int hoge(int a, int b, int c){
	int ans = a + b + c;
	printf("%d\n", ans);

//	return a + b;
}

int piyo(int a, int b, int c, int d, int e, int f){
	int ans = a + b + c + d + e + f;
	printf("%d\n", ans);

//	return ans;
}
