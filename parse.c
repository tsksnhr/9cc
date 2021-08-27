#include "9cc.h"

// Token that focused on now
Token *token;

// input data for error message
char *user_input;

// stoage for multiple Node (separated by ";")
Node *code[100];

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
bool consume(char *op){
	if (token->kind != TK_RESERVED
		|| token->len != strlen(op)
		|| memcmp(token->str, op, token->len)){
		return false;
	}
	token = token->next;
	return true;
}

Token *consume_ident(){
	Token *tok;

	if (token->kind != TK_IDENT
//		|| token->len != strlen(op)
//		|| memcmp(token->str, op, token->len)
	){
		return NULL;
	}
	tok = token;
	token = token->next;
	return tok;
}

// if Token's kind is expected operand, this increments Token-position
// if not, calls error-func
void expect(char *op){
	if (token->kind != TK_RESERVED
		|| token->len != strlen(op)
		|| memcmp(token->str, op, token->len)){
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
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;

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
		if (*p >= 'a' && *p <= 'z'){
			cur = new_token(TK_IDENT, cur, p++, 1);
			continue;
		}
		if (!strncmp(p, "==", 2) || !strncmp(p, "!=", 2) || !strncmp(p, "<=", 2) || !strncmp(p, ">=", 2)){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p = p + 2;
			continue;
		}
		if (!strncmp(p, "+", 1) || !strncmp(p, "-", 1) || !strncmp(p, "*", 1) || !strncmp(p, "/", 1) || !strncmp(p, "(", 1)
			|| !strncmp(p, ")", 1) || !strncmp(p, "<", 1) || !strncmp(p, ">", 1) || !strncmp(p, "=", 1) || !strncmp(p, ";", 1)){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}
		if (isdigit(*p)){
			cur = new_token(TK_NUM, cur, p, 3);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p, "This was cannot be tokenized.\n");
	}

	new_token(TK_EOF, cur, p, 1);
	return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

// Production rules
// program = stmt*
Node *program(){
	int i = 0;
	while (!at_eof()){
		code[i++] = stmt();
	}
	code[i] = NULL;
}

// stmt = expr ";"
Node *stmt(){
	Node *node = expr();

	expect(";");
	return node;
}

// expr = assign
Node *expr(){
	return assign();
}

// assign = equality ("=" assign)?
Node *assign(){
	Node *node = equality();

	if (consume("=")){
		node = new_node(ND_ASSIGN, node, assign());
	}
	return node;
}

// equality = relational ("=="relational | "!="relational)*
Node *equality(){
	Node *node = relational();

	for (;;){
		if (consume("==")){
			node = new_node(ND_EQUAL, add(), node);
		}
		else if (consume("!=")){
			node = new_node(ND_NOT_EQUAL, add(), node);
		}
		else{
			return node;
		}
	}
}

// relational = add ("<="add | "<"add)*
Node *relational(){
	Node *node = add();

	for (;;){
		if (consume("<=")){
			node = new_node(ND_EQ_RELATIONAL, node, add());
		}
		else if (consume(">=")){
			node = new_node(ND_EQ_RELATIONAL, add(), node);		// arguments are reversed to use same code-generator
		}
		else if (consume("<")){
			node = new_node(ND_RELATIONAL, node, add());
		}
		else if (consume(">")){
			node = new_node(ND_RELATIONAL, add(), node);		// arguments are reversed to use same code-generator
		}
		else{
			return node;
		}
	}
}

// add = mul ("+"mul | "-"mul)*
Node *add(){
    Node *node = mul();

    for(;;){
        if (consume("+")){
            node = new_node(ND_ADD, node, mul());
        }
        else if (consume("-")){
            node = new_node(ND_SUB, node, mul());
        }
        else{
            return node;
        }
    }
}

// mul = unary ('*'unary | '-'unary)*
Node *mul(){
    Node *node = unary();

    for(;;){
        if (consume("*")){
            node = new_node(ND_MUL, node, primary());
        }
        else if (consume("/")){
            node = new_node(ND_DIV, node, primary());
        }
        else{
            return node;
        }
    }
}

// unary = ('+' | '-')? primay
Node *unary(){
	if (consume("+")){
		return new_node(ND_ADD, new_node_num(0), primary());
	}
	if (consume("-")){
		return new_node(ND_SUB, new_node_num(0), primary());
	}
	return primary();
}

// primary = num | ident | '(' expr ')'*
Node *primary(){
	if (consume("(")){
		Node *node = expr();
		expect(")");
		return node;
	}

	Token *tok = consume_ident();
	if (tok){
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;
		node->offset = (tok->str[0] - 'a' + 1) * 8;
		return node;
	}


	return new_node_num(expect_number());
}
