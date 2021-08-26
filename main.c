#include "9cc.h"

int main(int argc, char **argv){

	if (argc != 2){
		fprintf(stderr, "The number of argument is not correct.\n");
		return 1;
	}

	// after tokenize, token is 2nd factor of linked-list
	user_input = argv[1];
	token = tokenize(argv[1]);

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	Node *node = expr();
	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
