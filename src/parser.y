%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_ast_expression **result} {struct mCc_ast_statement **stmt_result}

%define parse.trace
%define parse.error verbose

%code requires {
#include "mCc/parser.h"
}

%{
#include <string.h>
#include <stdbool.h>

int mCc_parser_lex();
void mCc_parser_error();
%}

%define api.value.type union
%define api.token.prefix {TK_}

%token END 0 "EOF"

%token <long>   INT_LITERAL   "integer literal"
%token <double> FLOAT_LITERAL "float literal"
%token <char*>  STRING_LITERAL "string literal"
%token <bool>   BOOL_LITERAL  "bool literal"
%token <char*>  IDENTIFIER    "identifier"

%token LPARENTH "("
%token RPARENTH ")"

%token NOT "!"

%token PLUS  "+"
%token MINUS "-"
%token ASTER "*"
%token SLASH "/"
%token LESS "<"
%token GREATER ">"
%token LESS_EQ "<="
%token GREATER_EQ ">="
%token AND "&&"
%token OR "||"
%token EQUALS "=="
%token NOT_EQUALS "!="

%token IF "if"
%token ELSE "else"
%token WHILE "while"
%token SEMICOLON ";"

/* TYPES */

%type <enum mCc_ast_binary_op> binary_op
%type <enum mCc_ast_unary_op>  unary_op

%type <struct mCc_ast_expression *> expression single_expr
%type <struct mCc_ast_literal *> literal
%type <struct mCc_ast_statement *> statement
%type <struct mCc_ast_identifier *> identifier

%start toplevel

/* PRECEDENCE RULES (INCREASING) */

/* These rules make ELSE bind to the innermost IF like in the spec. */
%precedence "then"
%precedence ELSE

/* DESTRUCTORS */
%destructor { mCc_ast_delete_expression($$); } expression single_expr
%destructor { mCc_ast_delete_literal($$); } literal
%destructor { mCc_ast_delete_statement($$); } statement
%destructor { mCc_ast_delete_identifier($$); } identifier

%%

toplevel : expression { *result = $1; }
         | statement { *stmt_result = $1; }
         ;

unary_op  : NOT   { $$ = MCC_AST_UNARY_OP_NOT; }
          | MINUS { $$ = MCC_AST_UNARY_OP_NEG; }
          ;

binary_op : PLUS  { $$ = MCC_AST_BINARY_OP_ADD; }
          | MINUS { $$ = MCC_AST_BINARY_OP_SUB; }
          | ASTER { $$ = MCC_AST_BINARY_OP_MUL; }
          | SLASH { $$ = MCC_AST_BINARY_OP_DIV; }
          | LESS  { $$ = MCC_AST_BINARY_OP_LT; }
          | GREATER { $$ = MCC_AST_BINARY_OP_GT; }
          | LESS_EQ { $$ = MCC_AST_BINARY_OP_LEQ; }
          | GREATER_EQ { $$ = MCC_AST_BINARY_OP_GEQ; }
          | AND    { $$ = MCC_AST_BINARY_OP_AND; }
          | OR     { $$ = MCC_AST_BINARY_OP_OR; }
          | EQUALS { $$ = MCC_AST_BINARY_OP_EQ; }
          | NOT_EQUALS { $$ = MCC_AST_BINARY_OP_NEQ; }
          ;

single_expr : literal                         { $$ = mCc_ast_new_expression_literal($1); }
            | identifier                      { $$ = mCc_ast_new_expression_identifier($1); }
            | unary_op expression             { $$ = mCc_ast_new_expression_unary_op($1, $2); }
            | LPARENTH expression RPARENTH    { $$ = mCc_ast_new_expression_parenth($2); }
            ;

expression : single_expr                      { $$ = $1;                                           }
           | single_expr binary_op expression { $$ = mCc_ast_new_expression_binary_op($2, $1, $3); }
           ;

literal : INT_LITERAL   { $$ = mCc_ast_new_literal_int($1);   }
        | FLOAT_LITERAL { $$ = mCc_ast_new_literal_float($1); }
        | STRING_LITERAL { $$ = mCc_ast_new_literal_string($1); }
        | BOOL_LITERAL  { $$ = mCc_ast_new_literal_bool($1);  }
        ;

identifier : IDENTIFIER { $$ = mCc_ast_new_identifier($1); }
           ;

statement : expression SEMICOLON { $$ = mCc_ast_new_statement_expression($1); }
          | WHILE LPARENTH expression RPARENTH statement { $$ = mCc_ast_new_statement_while($3, $5); }
          | IF LPARENTH expression RPARENTH statement { $$ = mCc_ast_new_statement_if($3, $5, NULL); } %prec "then" /* give this statement the precedence named "then" */
          | IF LPARENTH expression RPARENTH statement ELSE statement { $$ = mCc_ast_new_statement_if($3, $5, $7); }
          ;

%%

#include <assert.h>

#include "scanner.h"

void yyerror(yyscan_t *scanner, const char *msg) {}

struct mCc_parser_result mCc_parser_parse_string(const char *input)
{
	assert(input);

	struct mCc_parser_result result = { 0 };

	FILE *in = fmemopen((void *)input, strlen(input), "r");
	if (!in) {
		result.status = MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM;
		return result;
	}

	result = mCc_parser_parse_file(in);

	fclose(in);

	return result;
}

struct mCc_parser_result mCc_parser_parse_file(FILE *input)
{
	assert(input);

	yyscan_t scanner;
	mCc_parser_lex_init(&scanner);
	mCc_parser_set_in(input, scanner);

	struct mCc_parser_result result = {
		.status = MCC_PARSER_STATUS_OK,
	};

	if (yyparse(scanner, &result.expression, &result.statement) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mCc_parser_lex_destroy(scanner);

	return result;
}
