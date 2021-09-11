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
	TK_IDENT,	// identifier
	TK_NUM,		// number
	TK_EOF,		// End Of File
	TK_RETURN,	// return
	TK_IF,		// if
	TK_ELSE,	// else
	TK_WHILE,	// while
	TK_FOR,		// for
} TokenKind;

typedef enum {
   	ND_ADD,			// +
	ND_SUB,			// -
	ND_MUL,			// *
	ND_DIV,			// /
	ND_EQUAL,		// ==
	ND_NOT_EQUAL,		// !=
	ND_RELATIONAL,		// <, >
	ND_EQ_RELATIONAL,	// <=, >=
	ND_NUM,			// number
	ND_ASSIGN,		// =
	ND_LVAR,		// local variable
	ND_RETURN,		// return
	ND_IF,			// if
	ND_IFELSE,		// if ... else
	ND_ELSE,		// else
	ND_WHILE,		// while
	ND_FOR,			// for
	ND_BLOCK,		// block
	ND_FUNC_CALL,		// function call
	ND_FUNC_DECLEAR,	// function declearation
	ND_ADDRESS,		// for pointer "&"
	ND_DEREF,		// for pointet "*"
} NodeKind;


// structure
typedef struct Token Token;
struct Token {
	TokenKind kind;
	Token *before;
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

	Node *for_init;		// used if kind == ND_FOR
	Node *for_update;	// used if kind == ND_FOR

	Node *blk_stmt[100];	// used if kind == ND_BLOCK

	int val;		// used if kind == ND_NUM
	int offset;		// used if kind == ND_LVAR

	char *func_name;	// used if kind == ND_FUNC
	int name_len;		// used if kind == ND_FUNC
	Node *argv_list[6];	// used if kind == ND_FUNC
	int total_argv_num;	// used if kind == ND_FUNC
};

typedef struct Lvar Lvar;
struct Lvar {
	Lvar *next;
	char *name;
	int len;
	int offset;
};

// prototype declaraiton
// parse
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);
Node *new_node(NodeKind kind, Node *lhs, Node *rsh);
Node *new_node_num(int val);
Lvar *find_Lvar(Token *tok);
bool is_token_element(char c);

// producttion rules
Node *program();
Node *func();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *argv(Token *ident);
Node *lvar(Token *ident);

// code generator
void gen(Node *node);
void com_gen();
void prologue();
void epilogue();

// Test func
int foo();


// Global variable
extern Token *token;
extern char *user_input;
extern Node *code[];
extern Lvar *locals;
