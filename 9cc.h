// header
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>


// constant
typedef enum {
	TK_RESERVED,	// operand
	TK_NUM,		// number
	TK_EOF,		// End Of File
} TokenKind;

typedef enum {
   	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQUAL,
	ND_NOT_EQUAL,
	ND_RELATIONAL,
	ND_EQ_RELATIONAL,
	ND_NUM,
} NodeKind;


// structure
typedef struct Token Token;
struct Token {
	TokenKind kind;
	Token *next;
	int val;	// number (if kind == TK_NUM)
	char *str;	// Token string
	int len;	// Token string's length
};

typedef struct Node Node;
struct Node {
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};


// prototype declaraiton
// parse
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);
Node *new_node(NodeKind kind, Node *lhs, Node *rsh);
Node *new_node_num(int val);

// producttion rules
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// code generator
void gen(Node *node);


// Global variable
extern Token *token;
extern char *user_input;
