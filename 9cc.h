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
	TK_TYPE,	// variable's type
	TK_SIZEOF,	// sizeof operator
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
	ND_LOCAL_LVAR,		// local variable
	ND_RETURN,		// return
	ND_IF,			// if
	ND_IFELSE,		// if ... else
	ND_ELSE,		// else
	ND_WHILE,		// while
	ND_FOR,			// for
	ND_BLOCK,		// block
	ND_FUNC_CALL,		// function call
	ND_FUNC_DECLARE,	// function declearation
	ND_ADDRESS,		// for pointer "&"
	ND_DEREF,		// for pointet "*"
	ND_INT,			// variable type of int
	ND_GLOBAL_LVAR,			// global variable
	ND_GLOBAL_LVAR_DECL,	// global variable declaring
} NodeKind;


// structure
typedef struct Token Token;
struct Token {
	TokenKind kind;
	Token *prev;
	Token *next;
	int val;	// number (if kind == TK_NUM)
	char *str;	// Token string
	int len;	// Token string's length
};

typedef struct Type Type;
struct Type {
	enum {
		INT,
		POINTER,
		ARRAY,
	} type_id;
	Type *pointer_to;
	size_t array_size;
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
	int head;		// used if kind == ND_LVAR
	int tail;		// used if kind == ND_LVAR

	char *func_name;	// used if kind == ND_FUNC
	int name_len;		// used if kind == ND_FUNC
	Node *argv_list[6];	// used if kind == ND_FUNC
	int total_argv_num;	// used if kind == ND_FUNC

	Type *type;

	char *glv_name;		// used if kind == ND_GLOBAL
};

typedef struct Lvar Lvar;
struct Lvar {
	Lvar *next;

	char *name;
	int len;
	int head;	// offset from rbp
	int tail;	// used if type == ARRAY

	Type *type;
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
Lvar *find_local_Lvar(Token *tok);
Lvar *find_global_Lvar(Token *tok);
bool is_token_element(char c);
Type *define_variable_type();
Type *define_array(Type *base);

// producttion rules
Node *program();
Node *glv();
Node *func(Type *type, Token *ident);
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *loadlvar(Token *ident);
Node *decllvar(Token *ident, Type *type, int node_type);

// code generator
void gen(Node *node);
void com_gen();
void prologue();
void epilogue();
void pointer_calc_arraignment(Node *node);
void change_array_to_pointer(Node *node);
void gen_global_alloc(Node *node);

// define variable type
Type *define_variable_type();


// Global variable
extern Token *token;
extern char *user_input;
extern Node *code[];
extern Lvar *locals;
extern Lvar *globals;