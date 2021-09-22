// header
#include "9cc.h"

// get variable's offset from rbp
void gen_lval(Node *node){
	if (node->kind != ND_LVAR){
		fprintf(stderr, "Not left value.\n");
		exit(1);
	}

	printf("	mov rax, rbp\n");
	if (node->type->type_id == ARRAY){
		printf("	sub rax, %d\n", node->tail);
	}
	else{
		printf("	sub rax, %d\n", node->head);
	}
	printf("	push rax\n");
	printf("\n");

	return;
}


void gen(Node *node){
	int stmt_num = 0;
	switch (node->kind){
		case ND_BLOCK:
			while (node->blk_stmt[stmt_num] != NULL){
				if (stmt_num != 0){
					printf("	pop rax\n");
				}
				gen(node->blk_stmt[stmt_num]);
				stmt_num++;
				printf("\n");
			}
			return;

		case ND_RETURN:
			gen(node->lhs);
			printf("	pop rax\n");
			epilogue();
			printf("\n");
			return;

		case ND_IF:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .Lend000\n");

			gen(node->rhs);
			printf(".Lend000:\n");
			printf("\n");
			return;

		case ND_IFELSE:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("je .Lelse101\n");

			gen(node->rhs);
			printf("jmp .Lend102\n");
			printf("\n");
			return;

		case ND_ELSE:
			printf(".Lelse101:\n");

			gen(node->rhs);
			printf(".Lend102:\n");
			printf("\n");
			return;

		case ND_WHILE:
			printf(".Lbegin203:\n");

			gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .Lend204\n");

			gen(node->rhs);
			printf("        jmp .Lbegin203\n");
			printf(".Lend204:\n");
			printf("\n");
			return;

		case ND_FOR:
			if (node->for_init != NULL) gen(node->for_init);
			printf(".Lbegin305:\n");

			if (node->lhs != NULL) gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .Lend306\n");

			gen(node->rhs);
			if (node->for_update != NULL) gen(node->for_update);
			printf("	jmp .Lbegin305\n");
			printf(".Lend306:\n");
			printf("\n");
			return;

		case ND_NUM:
			printf("	push %d\n", node->val);
			printf("\n");
			return ;

		// load value from variable
		case ND_LVAR:
			gen_lval(node);		// get offset value from rbp, it has been pushed

			// copy pointer to head of array to rax
			if (node->type->type_id == ARRAY){
				printf("	pop rdi\n");
				printf("	mov rax, rdi\n");
				printf("	add rdi, 8\n");
				printf("	mov [rax], rdi\n");	// copy head address of array to pointer
				printf("	push rdi\n");

				printf("\n");
				return;
			}

			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
			printf("\n");
			return;

		case ND_FUNC_CALL:
			// evaluate arguments
			for (int i = 0; i < node->total_argv_num; i++){
				gen(node->argv_list[i]);
				printf("	pop rax\n");

				if (i == 0){
					printf("	mov rdi, rax\n");
				}
				if (i == 1){
					printf("	mov rsi, rax\n");
				}
				if (i == 2){
					printf("	mov rdx, rax\n");
				}
				if (i == 3){
					printf("	mov rcx, rax\n");
				}
				if (i == 4){
					printf("	mov r8, rax\n");
				}
				if (i == 5){
					printf("	mov r9, rax\n");
				}
			}

			// jump to the address of function
			printf("	call ");
			for (int i = 0; i < node->name_len; i++) printf("%c", (node->func_name)[i]);
			printf("\n");

			printf("	push rax\n");
			printf("\n");
			return;

		case ND_FUNC_DECLARE:
			// label
			for (int i = 0; i < node->name_len; i++) printf("%c", (node->func_name)[i]);
			printf(":\n");

			prologue();

			// evaluate arguments
			// make or acsess the address of loval variable
			for (int i = 0; i < node->total_argv_num; i++){
				gen_lval(node->argv_list[i]);

				printf("	pop rax\n");
				if (i == 0){
					printf("	mov [rax], rdi\n");
				}
				if (i == 1){
					printf("	mov [rax], rsi\n");
				}
				if (i == 2){
					printf("	mov [rax], rdx\n");
				}
				if (i == 3){
					printf("	mov [rax], rcx\n");
				}
				if (i == 4){
					printf("	mov [rax], r8\n");
				}
				if (i == 5){
					printf("	mov [rax], r9\n");
				}
			}
			printf("	push rax\n");
			printf("\n");
			return;

		case ND_ASSIGN:
			// "*" is used for left hand side of "=", get variable's address directly
			// if through ND_DEREF, left hand side of "=" will be right value
			if (node->lhs->kind == ND_DEREF){
				gen(node->lhs->lhs);	// push variable's address
			}
			else{
				gen_lval(node->lhs);	// push offset value from rbp
			}

			gen(node->rhs);		// value

			printf("	pop rdi\n");
			printf("	pop rax\n");
			printf("	mov [rax], rdi\n");
			printf("	push rdi\n");
			printf("\n");
			return;

		case ND_ADDRESS:
			gen_lval(node->lhs);	// get variable's offset from rbp, and pushed
			printf("\n");
			return;

		case ND_DEREF:
			gen(node->lhs);		// load variable's value (if node is pointer, value is address)
			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
			printf("\n");
			return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");	// node->rhs is copied to rdi
	printf("	pop rax\n");	// node->lhs is copied to rax

	switch (node->kind){
		case ND_ADD:
			pointer_calc_arraignment(node);
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
			pointer_calc_arraignment(node);
			printf("	sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("	imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
		case ND_EQUAL:
			printf("	cmp rax, rdi\n");
			printf("	sete al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_NOT_EQUAL:
			printf("	cmp rax, rdi\n");
			printf("	setne al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_RELATIONAL:
			printf("	cmp rax, rdi\n");
			printf("	setl al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_EQ_RELATIONAL:
			printf("	cmp rax, rdi\n");
			printf("	setle al\n");
			printf("	movzb rax, al\n");
			break;
	}
	printf("	push rax\n");
	printf("\n");
	return;
}

// first part of assembly
void com_gen(){
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	return;
}

// called when function is called
void prologue(){
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, 208\n");
	return;
}

// called when returned from function
void epilogue(){
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
	return;
}

// pointer calculation
// only used for int or pointer
void pointer_calc_arraignment(Node *node){
	if (node->lhs->type != NULL){
		if ((node->lhs->type->type_id == POINTER || node->lhs->type->type_id == ARRAY) && node->lhs->type->pointer_to->type_id == INT){
			printf("	push 4\n");
			printf("	pop rsi\n");
			printf("	imul rdi, rsi\n");
		}
		if ((node->lhs->type->type_id == POINTER || node->lhs->type->type_id == ARRAY) && node->lhs->type->pointer_to->type_id == POINTER){
			printf("	push 8\n");
			printf("	pop rsi\n");
			printf("	imul rdi, rsi\n");
		}
	}
	if (node->rhs->type != NULL){
		if ((node->rhs->type->type_id == POINTER || node->rhs->type->type_id == ARRAY) && node->rhs->type->pointer_to->type_id == INT){
			printf("	push 4\n");
			printf("	pop rsi\n");
			printf("	imul rax, rsi\n");
		}
		if ((node->rhs->type->type_id == POINTER || node->rhs->type->type_id == ARRAY) && node->rhs->type->pointer_to->type_id == POINTER){
			printf("	push 8\n");
			printf("	pop rsi\n");
			printf("	imul rax, rsi\n");
		}
	}
	return;
}

