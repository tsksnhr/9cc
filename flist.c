#include "9cc.h"

int hoge(int a, int b){
	int ans = a + b;
//	printf("%d + %d = %d\n", a, b, ans);

	return ans;
}

int piyo(int a, int b, int c, int d, int e, int f){
	int ans = a + b + c + d + e + f;
//	printf("%d + %d + %d + %d + %d + %d = %d\n", a, b, c, d, e, f, ans);

	return ans;
}

int showint(int a){
	printf("%d\n", a);
	return 0;
}

int showptr(int *a){
	printf("%p\n", a);
	return 0;
}

int *tiny_alloc(int w, int x, int y, int z){

	int *p;
	p = (int *)malloc(sizeof(int)*4);

	if (p == NULL) fprintf(stderr, "failed to allocate.\n");

	*(p + 0) = w;
//	fprintf(stdout, "%d\n", *(p+0));
	*(p + 1) = x;
//	fprintf(stdout, "%d\n", *(p+1));
	*(p + 2) = y;
//	fprintf(stdout, "%d\n", *(p+2));
	*(p + 3) = z;
//	fprintf(stdout, "%d\n", *(p+3));

//	fprintf(stdout, "%p\n", p);
	return p;
}

int insert_ten(int x){

	x = 10;
	return 0;
}

int push_ten(int *x){

	*x = 10;
	return 0;
}
