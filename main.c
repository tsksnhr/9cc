#include "9cc.h"

int main(int argc, char **argv){

	if (argc != 2){
		fprintf(stderr, "The number of argument is not correct.\n");
		return 1;
	}

	// after tokenize, token is 2nd factor of linked-list
	user_input = argv[1];
	token = tokenize(argv[1]);

	// get memory for head of local and global variables
	locals = calloc(1, sizeof(Lvar));
	globals = calloc(1, sizeof(Lvar));

	// make data-tree (each tree saved in code[100])
	program();

	// former part of assenmly
	com_gen();

	for (int i = 0; code[i]; i++){
		gen(code[i]);

		// pop last value of each foluma
//		printf("	pop rax\n");
	}

	return 0;
}
