// header
#include "9cc.h"

void gen_lval(Node *node){
	if (node->kind != ND_LVAR){
		fprintf(stderr, "Not lvalue.\n");
	}

	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", node->offset);
	printf("	push rax\n");

	return;
}

void gen(Node *node){
	switch (node->kind){
		case ND_NUM:
			printf("	push %d\n", node->val);
			return ;
		case ND_LVAR:
			gen_lval(node);

			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
			return;
		case ND_ASSIGN:
			gen_lval(node->lhs);	// address
			gen(node->rhs);		// value

			printf("	pop rdi\n");
			printf("	pop rax\n");
			printf("	mov [rax], rdi\n");
			printf("	push rdi\n");
			return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch (node->kind){
		case ND_ADD:
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
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
	printf(".global main\n");
	printf("main:\n");
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

