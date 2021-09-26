// header
#include "9cc.h"

// get declare code of global variable
void gen_global_alloc(Node *node){
	if (node->kind == ND_GLOBAL_LVAR_DECL){
		printf(".comm	");
		for (int i = 0; i < node->name_len; i++) printf("%c", (node->glv_name)[i]);		// get variable's name
		if (node->type->type_id == ARRAY){
			printf(",	%d,	%d\n", node->tail, node->tail);
		}
		else if (node->type->type_id == INT){
			printf(",	%d,	%d\n", 8, 8);
		}
		else if (node->type->type_id == POINTER){
			printf(",	%d,	%d\n", 8, 8);
		}/*
		else if (node->type->type_id == INT){
			printf(",	%d,	%d\n", node->head, node->head);
		}*/
		else if (node->type->type_id == CHAR){
			printf(",	%d,	%d\n", 1, 1);
		}
		else{
			fprintf(stderr, "invalid type.\n");
			exit(1);
		}
	}
	else{
		fprintf(stderr, "invalid call.\n");
		exit(1);
	}

	return;
}

// get variable's offset from rbp
void gen_lval(Node *node){

	if (node->kind == ND_LOCAL_LVAR){
		printf("	mov rax, rbp\n");
		if (node->type->type_id == ARRAY){
			printf("	sub rax, %d\n", node->tail);
		}
		else{
			printf("	sub rax, %d\n", node->head);
		}
	}
	printf("	push rax\n");

	return;
}

// get value from already assigned global variable
void gen_global_as_rval(Node *node){

	// return pointer that have head addres of array
	if (node->type->type_id == ARRAY){
		// copy head address of array to rax 
		printf("	lea rax, ");
		for (int i = 0; i < node->name_len; i++) printf("%c", node->glv_name[i]);	// variable name
		printf("[rip+8]\n");

		// copy head address of array to pointer
		printf("	mov ");
		for (int i = 0; i < node->name_len; i++) printf("%c", node->glv_name[i]);	// variable name
		printf("[rip], rax\n");

		// copy head address of array to rax
		printf("	mov rax, QWORD PTR ");
		for (int i = 0; i < node->name_len; i++) printf("%c", node->glv_name[i]);
		printf("[rip]\n");

		printf("	push rax\n");
	}
	// return global pointer
	else if (node->type->type_id == POINTER){
		// copy address of global pointer 
		printf("	lea rax, ");
		for (int i = 0; i < node->name_len; i++) printf("%c", node->glv_name[i]);	// variable name
		printf("[rip]\n");

		printf("	push rax\n");
	}
	// return global variable's right-value
	else if (node->type->type_id == INT){
		printf("	mov eax, DWORD PTR ");
		for (int i = 0; i < node->name_len; i++) printf("%c", node->glv_name[i]);
		printf("[rip]\n");

		printf("	push rax\n");
	}
	// return global variable's right-value
	else if (node->type->type_id == CHAR){
		printf("	movsx rax, BYTE PTR ");
		for (int i = 0; i < node->name_len; i++) printf("%c", node->glv_name[i]);
		printf("[rip]\n");

		printf("	push rax\n");
	}

	return;
}

// get global variable's address and push it to stack
void gen_global_address(Node *node){

	printf("	lea rax, ");
	for (int i = 0; i < node->name_len; i++) printf("%c", node->glv_name[i]);
	printf("[rip]\n");
	printf("	push rax\n");

	return;
}


void gen(Node *node){
	int stmt_num = 0;
	switch (node->kind){
		case ND_BLOCK:
			while (node->blk_stmt[stmt_num] != NULL){
				gen(node->blk_stmt[stmt_num++]);
				printf("	pop rax\n");
			}
			return;

		case ND_RETURN:
			gen(node->lhs);
			printf("	pop rax\n");
			epilogue();
			return;

		case ND_IF:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .Lend000\n");

			gen(node->rhs);
			printf(".Lend000:\n");
			return;

		case ND_IFELSE:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("je .Lelse101\n");

			gen(node->rhs);
			printf("jmp .Lend102\n");
			return;

		case ND_ELSE:
			printf(".Lelse101:\n");

			gen(node->rhs);
			printf(".Lend102:\n");
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
			return;

		case ND_NUM:
			printf("	push %d\n", node->val);
			return ;

		// load value from variable
		case ND_LOCAL_LVAR:
			// get offset value from rbp, it has been pushed
			gen_lval(node);

			// return pointer that have head of array, and push it to rax
			if (node->type->type_id == ARRAY){
				printf("	pop rdi\n");
				printf("	mov rax, rdi\n");
				printf("	add rdi, 8\n");
				printf("	mov [rax], rdi\n");	// copy head address of array to pointer
				printf("	push rdi\n");
				return;
			}

			printf("	pop rax\n");
			if (node->type->type_id == CHAR){
				printf("	movsx rax, BYTE PTR [rax]\n");
				printf("	push rax\n");
				return;
			}
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
			return;

		case ND_GLOBAL_LVAR:
			gen_global_as_rval(node);
			return;

		case ND_GLOBAL_LVAR_DECL:
			gen_global_alloc(node);
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
			return;

		case ND_ASSIGN:
			// "*" is used for left hand side of "=", get variable's address directly
			// if through ND_DEREF, left hand side of "=" will be right value
			if (node->lhs->kind == ND_DEREF){
				gen(node->lhs->lhs);	// push variable's address
			}
			else if (node->lhs->kind == ND_GLOBAL_LVAR){
				// skip
			}
			else{
				gen_lval(node->lhs);	// push offset value from rbp
			}

			gen(node->rhs);		// right value
			printf("	pop rdi\n");

			// process for global variable
			if (node->lhs->kind == ND_GLOBAL_LVAR){
				if (node->lhs->type->type_id == INT){
					printf("	mov DWORD PTR ");
					for (int i = 0; i < node->lhs->name_len; i++) printf("%c", node->lhs->glv_name[i]);
					printf("[rip], edi\n");
				}
				if (node->lhs->type->type_id == CHAR){
					printf("	mov BYTE PTR ");
					for (int i = 0; i < node->lhs->name_len; i++) printf("%c", node->lhs->glv_name[i]);
					printf("[rip], dil\n");
				}

				// get global variable's value and push it to stack
				gen_global_as_rval(node->lhs);
				return;
			}

			// process for local "char"-type variable
			if (node->lhs->type != NULL && node->lhs->type->type_id == CHAR){
				printf("	mov [rax], dil\n");		// dil is 8-bit rdi
				printf("	push rdi\n");
				return;
			}

			// process for local "int"-type variable
			printf("	pop rax\n");
			printf("	mov [rax], rdi\n");
			printf("	push rdi\n");
			return;

		case ND_ADDRESS:
			if (node->lhs->kind == ND_LOCAL_LVAR){
				gen_lval(node->lhs);	// get variable's offset from rbp, and pushed
			}
			else if (node->lhs->kind == ND_GLOBAL_LVAR){
				gen_global_address(node->lhs);
			}
			else{
				fprintf(stderr, "It's invalid.\n");
				exit(1);
			}
			return;

		case ND_DEREF:
			gen(node->lhs);		// load variable's value (if node is pointer, value is address)
			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
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

