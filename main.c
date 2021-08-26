#include "9cc.h"

int main(int argc, char **argv){

	if (argc != 2){
		fprintf(stderr, "The number of argument is not correct.\n");
		return 1;
	}

	// after tokenize, token is 2nd factor of linked-list
	user_input = argv[1];
	token = tokenize(argv[1]);

	// make data-tree, following by production rules
	Node *node = expr();

	code_generator(node);

	return 0;
}
