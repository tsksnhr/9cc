#include "9cc.h"

// definition of global varuable
Token *token;		// Token that focused on now
char *user_input;	// input data for error message
Node *code[100];	// stoage for multiple Node (separated by ";")
Lvar *locals;		// head of local varuable
int code_rows;		// number of lines

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
	if ((token->kind != TK_RESERVED
			&& token->kind != TK_RETURN
			&& token->kind != TK_IF
			&& token->kind != TK_ELSE
			&& token->kind != TK_WHILE
			&& token->kind != TK_FOR)
		|| token->len != strlen(op)
		|| memcmp(token->str, op, token->len)){
		return false;
	}
	token = token->next;
	return true;
}

// if Token is identifier, this returns token and increments Token-position
// if not, this returns NULL
Token *consume_ident(){
	Token *tok;

	if (token->kind != TK_IDENT){
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
		error_at(token->str, "Argument is different from '%s'", op);
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

// check character whether it's element of token or not
bool is_token_element(char c){
	bool is_token_elem;
	is_token_elem = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_');
	return is_token_elem;
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
	tok->before = cur;
	return tok;
}

// search local variables
Lvar *find_Lvar(Token *tok){
	for (Lvar *lv = locals; lv; lv = lv->next){
		if (lv->len == tok->len && !memcmp(lv->name, tok->str, lv->len)){
			return lv;
		}
	}
	return NULL;
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
		if (strncmp(p, "if", 2) == 0 && !is_token_element(p[2])){
			cur = new_token(TK_IF, cur, p, 2);
			p += 2;
			continue;
		}
		if (strncmp(p, "else", 4) == 0 && !is_token_element(p[4])){
			cur = new_token(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}
		if (strncmp(p, "while", 5) == 0 && !is_token_element(p[5])){
			cur = new_token(TK_WHILE, cur, p, 5);
			p += 5;
			continue;
		}
		if (strncmp(p, "for", 3) == 0 && !is_token_element(p[3])){
			cur = new_token(TK_FOR, cur, p, 3);
			p += 3;
			continue;
		}
		if (strncmp(p, "return", 6) == 0 && !is_token_element(p[6])){
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}
		if (*p >= 'a' && *p <= 'z'){
			int len = 1;
			char *p_get;
			p_get = p;
			p++;
			while (*p >= 'a' && *p <= 'z'){
				len++;
				p++;
			}
			cur = new_token(TK_IDENT, cur, p_get, len);
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
	int code_rows = 0;
	while (!at_eof()){
		code[code_rows++] = stmt();	// post-fix operator
	}
	code[code_rows] = NULL;
}

// stmt = expr ";"
//	| "return" expr ";"
//	| "if" "(" expr ")" stmt ("else" stmt)?
//	| "while" "(" expr ")" stmt
//	| "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node *stmt(){
	Node *node;

	if (consume("return")){
		node = new_node(ND_RETURN, expr(), NULL);
		expect(";");
	}
	else if (consume("if")){
		expect("(");
		node = calloc(1, sizeof(Node));
		node->lhs = expr();
		expect(")");
		node->rhs = stmt();

		if (consume("else")){
			node->kind = ND_IFELSE;
			token = token->before;		// after checking token, token-position is backed
		}
		else{
			node->kind = ND_IF;
		}
	}
	else if (consume("else")){
		node = calloc(1,sizeof(Node));
		node->kind = ND_ELSE;
		node->rhs = stmt();
	}
	else if (consume("while")){
		expect("(");
		node = calloc(1, sizeof(Node));
		node->kind = ND_WHILE;
		node->lhs = expr();
		expect(")");
		node->rhs = stmt();
	}
	else if (consume("for")){
		expect("(");
		node = calloc(1, sizeof(Node));
		node->kind = ND_FOR;
		node->for_init = NULL;
		node->lhs = NULL;
		node->for_update = NULL;

		int for_node_cnt = 0;
		while (!consume(")")){
			if (consume(";")){
				for_node_cnt++;
				 continue;
			}
			else{
				if (for_node_cnt == 0) node->for_init = expr();
				else if (for_node_cnt == 1) node->lhs = expr();
				else if (for_node_cnt == 2) node->for_update = expr();
				else{
					fprintf(stderr, "Unexpected Token ... for_node_cnt = %d\n", for_node_cnt);
					exit(1);
				}
			}
		}
		node->rhs = stmt();
	}
	else{
		node = expr();
		expect(";");
	}

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

		Lvar *lv = find_Lvar(tok);
		if (lv){
			node->offset = lv->offset;		// same offset memory from locals are reused
		}
		else{
			lv = calloc(1, sizeof(Lvar));
			lv->name = tok->str;
			lv->len = tok->len;
			lv->next = locals;
			lv->offset = locals->offset + 8;
			node->offset = lv->offset;
			locals = lv;
		}
		return node;
	}

	return new_node_num(expect_number());
}
