%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_parser_result *result}

%define parse.trace
%define parse.error verbose
%verbose
%locations

%code requires {
#include "mCc/parser.h"
}

%{
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define UNUSED(id) {(void) (id) ;}

/* Idea to pass start and end location taken from group 16 */
#define loc(ast_node, yylloc_start, yylloc_end) \
        (ast_node)->node.sloc.start_col  = (yylloc_start).first_column; \
        (ast_node)->node.sloc.start_line = (yylloc_start).first_line; \
        (ast_node)->node.sloc.end_col    = (yylloc_end).last_column - 1; \
        (ast_node)->node.sloc.end_line   = (yylloc_end).last_line;

int mCc_parser_lex();
void mCc_parser_error();
%}

%define api.value.type union
%define api.token.prefix {TK_}

%token END 0 "EOF"

%token <long>   INT_LITERAL    "integer literal"
%token <double> FLOAT_LITERAL  "float literal"
%token <char*>  STRING_LITERAL "string literal"
%token <bool>   BOOL_LITERAL   "bool literal"
%token <char*>  IDENTIFIER     "identifier"

%token LPARENTH "("
%token RPARENTH ")"
%token LBRACE "{"
%token RBRACE "}"
%token LBRACK "["
%token RBRACK "]"

%token NOT "!"

%token PLUS  '+'
%token MINUS '-'
%token ASTER '*'
%token SLASH '/'
%token LESS '<'
%token GREATER '>'
%token LESS_EQ "<="
%token GREATER_EQ ">="
%token AND "&&"
%token OR "||"
%token EQUALS "=="
%token NOT_EQUALS "!="
%token ASSGN "="

%token IF "if"
%token ELSE "else"
%token WHILE "while"
%token RETURN "return"
%token VOID "void"
%token SEMICOLON ";"
%token COMMA ","
%token <char*> TYPE "type"

/* TYPES */
%type <enum mCc_ast_unary_op>  unary_op
%type <enum mCc_ast_type> type

%type <struct mCc_ast_expression *> expression single_expr binary_op
%type <struct mCc_ast_literal *> literal
%type <struct mCc_ast_statement *> statement compound_stmt
%type <struct mCc_ast_identifier *> identifier
%type <struct mCc_ast_arguments *> arguments
%type <struct mCc_ast_parameters *> parameters
%type <struct mCc_ast_declaration *> declaration
%type <struct mCc_ast_function_def *> function_def
%type <struct mCc_ast_program *> program

%start toplevel

/* PRECEDENCE RULES (INCREASING) */
%left OR
%left AND
%left LESS_EQ LESS GREATER_EQ GREATER EQUALS NOT_EQUALS
%left PLUS MINUS
%left ASTER SLASH

/* These rules make ELSE bind to the innermost IF like in the spec. */
%precedence "then"
%precedence ELSE

/* DESTRUCTORS */
%destructor { mCc_ast_delete_expression($$); }  expression single_expr binary_op
%destructor { mCc_ast_delete_literal($$); }     literal
%destructor { mCc_ast_delete_statement($$); }   statement compound_stmt
%destructor { mCc_ast_delete_identifier($$); }  identifier
%destructor { mCc_ast_delete_arguments($$); }   arguments
%destructor { mCc_ast_delete_parameters($$); }  parameters
%destructor { mCc_ast_delete_declaration($$); } declaration
%destructor { mCc_ast_delete_func_def($$); }    function_def
%destructor { mCc_ast_delete_program($$); }     program

%%

toplevel : expression { result->expression = $1; }
         | statement  { result->statement  = $1; }
         | %empty     { result->program    = mCc_ast_new_program(NULL); }
         | program    { result->program    = $1; }
         ;

type : TYPE {
        if (!strcmp("bool", $1) )       $$ = MCC_AST_TYPE_BOOL;
        else if (!strcmp("int", $1))    $$ = MCC_AST_TYPE_INT;
        else if (!strcmp("float", $1))  $$ = MCC_AST_TYPE_FLOAT;
        else if (!strcmp("string", $1)) $$ = MCC_AST_TYPE_STRING;
     }
     ;

literal : INT_LITERAL    { $$ = mCc_ast_new_literal_int($1);    loc($$, @1, @1); }
        | FLOAT_LITERAL  { $$ = mCc_ast_new_literal_float($1);  loc($$, @1, @1); }
        | STRING_LITERAL { $$ = mCc_ast_new_literal_string($1); loc($$, @1, @1); }
        | BOOL_LITERAL   { $$ = mCc_ast_new_literal_bool($1);   loc($$, @1, @1); }
        ;

identifier : IDENTIFIER { $$ = mCc_ast_new_identifier($1); loc($$, @1, @1); }
           ;

unary_op  : NOT   { $$ = MCC_AST_UNARY_OP_NOT; }
          | MINUS { $$ = MCC_AST_UNARY_OP_NEG; }
          ;

binary_op : expression PLUS expression       { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ADD, $1, $3); loc($$, @1, @3); }
          | expression MINUS expression      { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SUB, $1, $3); loc($$, @1, @3); }
          | expression ASTER expression      { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_MUL, $1, $3); loc($$, @1, @3); }
          | expression SLASH expression      { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_DIV, $1, $3); loc($$, @1, @3); }
          | expression LESS expression       { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LT,  $1, $3); loc($$, @1, @3); }
          | expression GREATER expression    { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GT,  $1, $3); loc($$, @1, @3); }
          | expression LESS_EQ expression    { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LEQ, $1, $3); loc($$, @1, @3); }
          | expression GREATER_EQ expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GEQ, $1, $3); loc($$, @1, @3); }
          | expression AND expression        { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_AND, $1, $3); loc($$, @1, @3); }
          | expression OR expression         { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_OR,  $1, $3); loc($$, @1, @3); }
          | expression EQUALS expression     { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_EQ,  $1, $3); loc($$, @1, @3); }
          | expression NOT_EQUALS expression { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_NEQ, $1, $3); loc($$, @1, @3); }
          ;

single_expr : literal                                   { $$ = mCc_ast_new_expression_literal($1);         loc($$, @1, @1); }
            | identifier                                { $$ = mCc_ast_new_expression_identifier($1);      loc($$, @1, @1); }
            | identifier LBRACK expression RBRACK       { $$ = mCc_ast_new_expression_arr_subscr($1, $3);  loc($$, @1, @4); }
            | unary_op single_expr                      { $$ = mCc_ast_new_expression_unary_op($1, $2);    loc($$, @1, @2); }
            | LPARENTH expression RPARENTH              { $$ = mCc_ast_new_expression_parenth($2);         loc($$, @1, @3); }
            | identifier LPARENTH RPARENTH              { $$ = mCc_ast_new_expression_call_expr($1, NULL); loc($$, @1, @3); }
            | identifier LPARENTH arguments RPARENTH    { $$ = mCc_ast_new_expression_call_expr($1, $3);   loc($$, @1, @4); }
            ;

expression : binary_op   { $$ = $1; }
           | single_expr { $$ = $1; }
           ;

statement : expression SEMICOLON                                           { $$ = mCc_ast_new_statement_expression($1);      loc($$, @1, @2); }
          | WHILE LPARENTH expression RPARENTH statement                   { $$ = mCc_ast_new_statement_while($3, $5);       loc($$, @1, @5); }
          | IF LPARENTH expression RPARENTH statement                      { $$ = mCc_ast_new_statement_if($3, $5, NULL);    loc($$, @1, @5); } %prec "then" /* give this statement the precedence named "then" */
          | IF LPARENTH expression RPARENTH statement ELSE statement       { $$ = mCc_ast_new_statement_if($3, $5, $7);      loc($$, @1, @7); }
          | LBRACE RBRACE                                                  { $$ = mCc_ast_new_statement_compound(NULL);      loc($$, @1, @2); }
          | LBRACE compound_stmt RBRACE                                    { $$ = $2; loc($$, @1, @3); }
          | RETURN expression SEMICOLON                                    { $$ = mCc_ast_new_statement_return($2);          loc($$, @1, @1); }
          | RETURN SEMICOLON                                               { $$ = mCc_ast_new_statement_return(NULL);        loc($$, @1, @2); }
          | declaration SEMICOLON                                          { $$ = mCc_ast_new_statement_declaration($1);     loc($$, @1, @2); }
          | identifier ASSGN expression SEMICOLON                          { $$ = mCc_ast_new_statement_assgn($1, NULL, $3); loc($$, @1, @4); }
          | identifier LBRACK expression RBRACK ASSGN expression SEMICOLON { $$ = mCc_ast_new_statement_assgn($1, $3, $6);   loc($$, @1, @7); }
          ;

compound_stmt : statement                           { $$ = mCc_ast_new_statement_compound($1);     loc($$, @1, @1); }
              | compound_stmt statement             { $$ = mCc_ast_compound_statement_add($1, $2); loc($$, @1, @2); }
              ;

declaration : type identifier                       { $$ = mCc_ast_new_declaration($1, NULL, $2); loc($$, @1, @2); }
            | type LBRACK literal RBRACK identifier { $$ = mCc_ast_new_declaration($1, $3 , $5);  loc($$, @1, @5); }
            ;

arguments : expression                              { $$ = mCc_ast_new_arguments($1);     loc($$, @1, @1); }
          | arguments COMMA expression              { $$ = mCc_ast_arguments_add($1, $3); loc($$, @1, @3); }
          ;

parameters : declaration                            { $$ = mCc_ast_new_parameters($1);     loc($$, @1, @1); }
           | parameters COMMA declaration           { $$ = mCc_ast_parameters_add($1, $3); loc($$, @1, @3); }
           ;

function_def : VOID identifier LPARENTH RPARENTH LBRACE RBRACE                          { $$ = mCc_ast_new_function_def_void($2, NULL, NULL); loc($$, @1, @6); }
             | VOID identifier LPARENTH RPARENTH LBRACE compound_stmt RBRACE            { $$ = mCc_ast_new_function_def_void($2, NULL, $6); loc($$, @1, @7); }
             | VOID identifier LPARENTH parameters RPARENTH LBRACE RBRACE               { $$ = mCc_ast_new_function_def_void($2, $4, NULL); loc($$, @1, @7); }
             | VOID identifier LPARENTH parameters RPARENTH LBRACE compound_stmt RBRACE { $$ = mCc_ast_new_function_def_void($2, $4, $7); loc($$, @1, @8); }
             | type identifier LPARENTH RPARENTH LBRACE RBRACE                          { $$ = mCc_ast_new_function_def_type($1, $2, NULL, NULL); loc($$, @1, @6); }
             | type identifier LPARENTH RPARENTH LBRACE compound_stmt RBRACE            { $$ = mCc_ast_new_function_def_type($1, $2, NULL, $6); loc($$, @1, @7); }
             | type identifier LPARENTH parameters RPARENTH LBRACE RBRACE               { $$ = mCc_ast_new_function_def_type($1, $2, $4, NULL); loc($$, @1, @7); }
             | type identifier LPARENTH parameters RPARENTH LBRACE compound_stmt RBRACE { $$ = mCc_ast_new_function_def_type($1, $2, $4, $7); loc($$, @1, @8); }
             ;

program : function_def         { $$ = mCc_ast_new_program($1);     loc($$, @1, @1); }
        | program function_def { $$ = mCc_ast_program_add($1, $2); loc($$, @1, @2); }
        ;
%%

#include <assert.h>

#include "scanner.h"

void mCc_parser_error(struct MCC_PARSER_LTYPE *yylloc, yyscan_t *scanner,
                      void *result, const char *msg)
{
	UNUSED(scanner);
	struct mCc_parser_result *r = result;

	r->status = MCC_PARSER_STATUS_PARSE_ERROR;
	r->err_msg = strdup(msg);
	r->err_text = strdup(mCc_parser_get_text(scanner));

	// Copy error location to result
	r->err_loc.start_line = yylloc->first_line;
	r->err_loc.end_line   = yylloc->last_line;
	r->err_loc.start_col  = yylloc->first_column;
	r->err_loc.end_col    = yylloc->last_column - 1;
}

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

	if (yyparse(scanner, &result) != 0 && result.status == MCC_PARSER_STATUS_OK) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mCc_parser_lex_destroy(scanner);

	return result;
}
