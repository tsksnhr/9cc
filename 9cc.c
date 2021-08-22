#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
	TK_RESERVED,	// operand
	TK_NUM,		// number
	TK_EOF,		// End Of File
} TokenKind;

typedef struct Token Token;

struct Token {
	TokenKind kind;
	Token *next;
	int val;
	char *str;	// Token-type string
};

// Token that be focused on
Token *token;

// get input for error message
char *user_input;

// output error message and error position to stderr
void error_at(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;		// user_input = input's the head address, loc = each token's the head address
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	fprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// if Token's kind is expected operand, this returns true and increments Token-position
// if not, this returns false
bool consume(char op){
	if (token->kind != TK_RESERVED || token->str[0] != op){
		return false;
	}
	token = token->next;
	return true;
}

// if Token'd kind is expected operand, this increments Token-position
// if not, calls error-func
void expect(char op){
	if (token->kind != TK_RESERVED || token->str[0] != op){
		error_at(token->str, "Argument is different from '%c'", op);
	}
	token = token->next;
}

// if Token is interger, this returns interger and increments Token-position
// if not, calls error-func
int expect_number(){
	if (token->kind != TK_NUM){
		error_at(token->str ,"Argument is not a number.\n");
	}
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

// get new memory for Token
// connect node from current Token to next
Token *new_token(TokenKind kind, Token *cur, char *str){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// make linked-list of Token
// return 2nd-Token (next Token of head)
Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p){
		if (isspace(*p)){
			p++;
			continue;
		}

		if (*p == '+' || *p == '-'){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if (isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p, "Cannot tokenize.\n");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}


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

	printf("	mov rax, %d\n", expect_number());

	while (!at_eof()){
		if (consume('+')){
			printf("	add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("	sub rax, %d\n", expect_number());
	}

	printf("	ret\n");
	return 0;
}
