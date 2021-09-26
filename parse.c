#include "9cc.h"

// definition of global varuable
Token *token;		// Token that focused on now
char *user_input;	// input data for error message
Node *code[100];	// stoage for multiple Node (separated by ";")
Lvar *locals;		// head of local variable
Lvar *globals;		// head of global variable
int code_row;		// number of lines

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
			&& token->kind != TK_FOR
			&& token->kind != TK_TYPE
			&& token->kind != TK_SIZEOF)
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
	tok->prev = cur;
	return tok;
}

// search local variables
Lvar *find_local_Lvar(Token *tok){
	for (Lvar *lv = locals; lv; lv = lv->next){
		if (lv->len == tok->len && !memcmp(lv->name, tok->str, lv->len)){
			return lv;
		}
	}
	return NULL;
}

Lvar *find_global_Lvar(Token *tok){
	for (Lvar *lv = globals; lv; lv = lv->next){
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
		if (strncmp(p, "int", 3) == 0 && !is_token_element(p[3])){
			cur = new_token(TK_TYPE, cur, p, 3);
			p += 3;
			continue;
		}
		if (strncmp(p, "sizeof", 6) == 0 && !is_token_element(p[6])){
			cur = new_token(TK_SIZEOF, cur, p, 6);
			p += 6;
			continue;
		}
		if (*p >= 'a' && *p <= 'z'){
			int len = 1;
			char *p_get;
			p_get = p;
			p++;
			while ((*p >= 'a' && *p <= 'z') || *p == '_'){
				len++;
				p++;
			}
			cur = new_token(TK_IDENT, cur, p_get, len);
			continue;
		}
		if (!strncmp(p, "==", 2) || !strncmp(p, "!=", 2) || !strncmp(p, "<=", 2) || !strncmp(p, ">=", 2)){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}
		if (!strncmp(p, "+", 1) || !strncmp(p, "-", 1) || !strncmp(p, "*", 1) || !strncmp(p, "/", 1)
			|| !strncmp(p, "(", 1) || !strncmp(p, ")", 1) || !strncmp(p, "{", 1) || !strncmp(p, "}", 1) || !strncmp(p, "[", 1) || !strncmp(p, "]", 1)
			|| !strncmp(p, ";", 1) || !strncmp(p, ",", 1)
			|| !strncmp(p, "<", 1) || !strncmp(p, ">", 1) || !strncmp(p, "=", 1)
			|| !strncmp(p, "&", 1)){
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

	Type *type = calloc(1, sizeof(Type));
	type->type_id = INT;
	node->type = type;

	return node;
}

// set varable type
Type *define_variable_type(){
	Type *type = calloc(1, sizeof(Type));
	Type *base = type;

	while (consume("*")){
		type->type_id = POINTER;

		Type *next = calloc(1, sizeof(Type));
		type->pointer_to = next;
		type = type->pointer_to;
	}
	type->type_id = INT;

	return base;
}

// set variable type of array's factor
Type *define_array(Type *base){
	Type *ret = calloc(1, sizeof(Type));

	ret->pointer_to = base;
	ret->type_id = ARRAY;

	return ret;
}


// Production rules
// program = glv* func "{" stmt* "}"
Node *program(){
	int code_row = 0;
	while (!at_eof()){
		while (true){
			// chech type declare
			if (!consume("int")){
				error_at(token->str, "No type declaration.\n");
			}
			// get ident-name and -type
			Type *type = define_variable_type();
			Token *ident = consume_ident();

			if (consume("(")){
				code[code_row++] = func(type, ident);		// function declare
				break;
			}
			else{
				if (consume("[")){
					type = define_array(type);
					type->array_size = (size_t)(expect_number());
					expect("]");
				}
				code[code_row++] = decllvar(ident, type, ND_GLOBAL_LVAR_DECL);
				expect(";");
			}
		}
		code[code_row++] = stmt();
	}
	code[code_row] = NULL;
}

// func = typename ident "(" argv* ")"
Node *func(Type *type, Token *ident){
	Node *node = calloc(1, sizeof(Node));

	// get return value type of function
	Type *ret_type = type;
	Token *fname = ident;

	if (fname != NULL){
		node->kind = ND_FUNC_DECLARE;
		node->func_name = fname->str;
		node->name_len = fname->len;

		int argv_cnt = 0;
		while (!consume(")")){
			if (argv_cnt != 0) expect(",");
			node->argv_list[argv_cnt++] = unary();
		}
		node->total_argv_num = argv_cnt;
		return node;
	}
	else{
		error_at(fname->str, "Function name is expected, but not.\n");
	}
}

// stmt = expr ";"
//	| "{" stmt* "}"
//	| "return" expr ";"
//	| "if" "(" expr ")" stmt ("else" stmt)?
//	| "while" "(" expr ")" stmt
//	| "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node *stmt(){
	Node *node;

	if (consume("{")){
		node = calloc(1, sizeof(Node));
		node->kind = ND_BLOCK;

		int stmt_num = 0;
		while (!consume("}")){
			node->blk_stmt[stmt_num++] = stmt();
		}
		node->blk_stmt[stmt_num] = NULL;
	}
	else if (consume("return")){
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
			token = token->prev;		// after checking token, token-position is backed
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
            node = new_node(ND_MUL, node, unary());
        }
        else if (consume("/")){
            node = new_node(ND_DIV, node, unary());
        }
        else{
            return node;
        }
    }
}

// unary = "+"? primay
//	| "-"? primary
//	| "&" unary
//	| "*" unary
//	| "sizeof" unary
Node *unary(){
	if (consume("+")){
		return new_node(ND_ADD, new_node_num(0), primary());
	}
	else if (consume("-")){
		return new_node(ND_SUB, new_node_num(0), primary());
	}
	else if (consume("&")){
		return new_node(ND_ADDRESS, unary(), NULL);
	}
	else if (consume("*")){
		return new_node(ND_DEREF, unary(), NULL);
	}
	else if (consume("sizeof")){
		// get argument's type
		Node *tmp_node = unary();
		Node *base = tmp_node;

		// left side search
		int deref_cnt = 0;
		while (tmp_node != NULL){
			if (tmp_node->kind == ND_DEREF){
				deref_cnt++;
				if (tmp_node->lhs->kind == ND_DEREF){
					tmp_node = tmp_node->lhs;
					continue;
				}
				else{
					Type *derefed = tmp_node->lhs->type;
					for (int i = 0; i < deref_cnt; i++){
						derefed = derefed->pointer_to;
					}
					if (derefed->type_id == INT){
						return new_node_num(4);
					}
					else if (derefed->type_id == POINTER){
						return new_node_num(8);
					}
					else{
						error_at(token->str, "invalid argument for sizeof.\n");
					}
				}
			}
			if (tmp_node->kind == ND_ADDRESS){
				return new_node_num(8);
			}
			if (tmp_node->kind == ND_LOCAL_LVAR || tmp_node->kind == ND_NUM){
				if (tmp_node->type->type_id == POINTER){
					return new_node_num(8);
				}
				else if (tmp_node->type->type_id == INT){
					return new_node_num(4);
				}
				else if (tmp_node->type->type_id == ARRAY){
					int tmp = tmp_node->type->array_size*4;
					return new_node_num(tmp);
				}
				else{
					error_at(token->str, "invalid argument for sizeof.\n");
				}
			}
			tmp_node = tmp_node->lhs;
		}
	}
	else{
		return primary();
	}
}

// primary = num
//	| ident ("(" argv* ")")?
//	| '(' expr ')'*
Node *primary(){
	if (consume("(")){
		Node *node = expr();
		expect(")");
		return node;
	}

	// declaration of variable
	if (consume("int")){
		Type *var_type = define_variable_type();
		Token *ident = consume_ident();

		if (consume("[")){
			var_type = define_array(var_type);
			var_type->array_size = (size_t)(expect_number());
			expect("]");
		}

		if (ident != NULL){
			return decllvar(ident, var_type, ND_LOCAL_LVAR);
		}
		else{
			error_at(token->str, "A identifier has to be after the typename.\n");
		}
	}

	// function call or get lvalue
	Token *ident = consume_ident();
	if (ident != NULL){
		Node *node = calloc(1, sizeof(Node));

		// function call
		if (consume("(")){
			node->kind = ND_FUNC_CALL;
			node->func_name = ident->str;
			node->name_len = ident->len;

			int argv_cnt = 0;
			while (!consume(")")){
				if (argv_cnt != 0) expect(",");

				// ident has to be declared variable
				Token *argv_ident = consume_ident();
				if(argv_ident != NULL){
					node->argv_list[argv_cnt++] = loadlvar(argv_ident);
				}
				else{
					node->argv_list[argv_cnt++] = expr();
				}
			}
			node->total_argv_num = argv_cnt;
			return node;
		}

		// lvalue (not declaration)
		return loadlvar(ident);

	}
	else{
		return new_node_num(expect_number());		// number (ident == NULL)
	}
}

// load number from declared variable
Node *loadlvar(Token *ident){
	Node *node = calloc(1, sizeof(Node));

	if (ident != NULL){
		Lvar *lv = find_local_Lvar(ident);
		if (lv){
			node->kind = ND_LOCAL_LVAR;

			// same offset from rbp is copied
			node->head = lv->head;
			node->tail = lv->tail;
			// same type is copied, but it's pointer
			node->type = lv->type;

			// replace array with pointer
			if (consume("[")){
				// allocate memory
				Node *inside = calloc(1, sizeof(Node));
				Node *whole = calloc(1, sizeof(Node));
				Node *ret = calloc(1, sizeof(Node));

				inside = primary();		// make new node for inside of []
				expect("]");

				whole = new_node(ND_ADD, inside, node);			// make new node for " (array + num) "
				return ret = new_node(ND_DEREF, whole, NULL);	// make new node for " *(array + num) "
			}
			return node;
		}
		else{
			lv = find_global_Lvar(ident);
			if (lv){
				node->kind = ND_GLOBAL_LVAR;

				// same offset from rbp is copied
				node->head = lv->head;
				node->tail = lv->tail;
				// same type is copied, but it's pointer
				node->type = lv->type;
				// get variable's name
				node->glv_name = lv->name;
				node->name_len = lv->len;

				// replace array with pointer
				if (consume("[")){
					// allocate memory
					Node *inside = calloc(1, sizeof(Node));
					Node *whole = calloc(1, sizeof(Node));
					Node *ret = calloc(1, sizeof(Node));

					inside = primary();		// make new node for inside of []
					expect("]");

					whole = new_node(ND_ADD, inside, node);			// make new node for " (array + num) "
					return ret = new_node(ND_DEREF, whole, NULL);	// make new node for " *(array + num) "
				}
				return node;
			}
			else{
				error_at(ident->str, "Not declared yet.\n");
			}
		}
	}
}

// declare variable
Node *decllvar(Token *ident, Type *type, int node_kind){
	Node *node = calloc(1, sizeof(Node));

	if (ident != NULL){
		node->kind = node_kind;
//		Lvar *lv = find_local_Lvar(ident);

		if (node_kind == ND_LOCAL_LVAR){
			Lvar *lv = calloc(1, sizeof(Lvar));
			lv->name = ident->str;
			lv->len = ident->len;
			lv->next = locals;
			lv->head = locals->tail + 8;
			if (type->type_id == ARRAY){
				lv->tail = lv->head + 8*(int)(type->array_size);
			}
			else{
				lv->tail = lv->head;
			}
			lv->type = type;

			node->head = lv->head;
			node->tail = lv->tail;
			node->type = lv->type;
			locals = lv;

			return node;
		}
		else if (node_kind == ND_GLOBAL_LVAR_DECL){
//			lv = find_global_Lvar(ident);
//			if (lv){
				Lvar *lv = calloc(1, sizeof(Lvar));
				lv->name = ident->str;
				lv->len = ident->len;
				lv->next = globals;
				lv->head = globals->tail + 8;
				if (type->type_id == ARRAY){
					lv->tail = lv->head + 8*(int)(type->array_size);
				}
				else{
					lv->tail = lv->head;
				}
				lv->type = type;

				node->head = lv->head;
				node->tail = lv->tail;
				node->glv_name = lv->name;
				node->name_len = lv->len;
//				strncpy(node->glv_name, ident->str, ident->len + 1);
				node->type = lv->type;
				globals = lv;

				return node;
//			}
		}
	}
	else {
		error_at(ident->str, "identifier does not exist.\n");
	}
}
