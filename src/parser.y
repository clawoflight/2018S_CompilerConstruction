%define api.prefix {mCc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mCc_ast_expression **result} {struct mCc_ast_statement **stmt_result} {struct mCc_ast_program **program_result}

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

#define loc(ast_node, ast_sloc) \
	(ast_node)->node.sloc.start_col = (ast_sloc).first_column;

%}

%define api.value.type union
%define api.token.prefix {TK_}

%locations

%token END 0 "EOF"

%token <long>   INT_LITERAL   "integer literal"
%token <double> FLOAT_LITERAL "float literal"
%token <char*>  STRING_LITERAL "string literal"
%token <bool>   BOOL_LITERAL  "bool literal"
%token <char*>  IDENTIFIER    "identifier"

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
%type <enum mCc_ast_declaration_type> type

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
%left PLUS MINUS
%left ASTER SLASH
%left AND OR
%left LESS_EQ LESS GREATER_EQ GREATER EQUALS NOT_EQUALS

/* These rules make ELSE bind to the innermost IF like in the spec. */
%precedence "then"
%precedence ELSE

/* DESTRUCTORS */
%destructor { mCc_ast_delete_expression($$); } expression single_expr binary_op
%destructor { mCc_ast_delete_literal($$); } literal
%destructor { mCc_ast_delete_statement($$); } statement compound_stmt
%destructor { mCc_ast_delete_identifier($$); } identifier
%destructor { mCc_ast_delete_arguments($$); } arguments
%destructor { mCc_ast_delete_parameters($$); } parameters
%destructor { mCc_ast_delete_declaration($$); } declaration
%destructor { mCc_ast_delete_func_def($$); } function_def
%destructor { mCc_ast_delete_program($$); } program

%%

toplevel : expression { *result = $1; }
         | statement  { *stmt_result = $1; }
         | %empty     { *program_result = mCc_ast_new_program(NULL); }
         | program    { *program_result = $1; }
         ;

type : TYPE {
        if (!strcmp("bool", $1) )       $$ = MCC_AST_TYPE_BOOL;
        else if (!strcmp("int", $1))    $$ = MCC_AST_TYPE_INT;
        else if (!strcmp("float", $1))  $$ = MCC_AST_TYPE_FLOAT;
        else if (!strcmp("string", $1)) $$ = MCC_AST_TYPE_STRING;
     }
     ;

literal : INT_LITERAL    { $$ = mCc_ast_new_literal_int($1); }
        | FLOAT_LITERAL  { $$ = mCc_ast_new_literal_float($1); }
        | STRING_LITERAL { $$ = mCc_ast_new_literal_string($1); }
        | BOOL_LITERAL   { $$ = mCc_ast_new_literal_bool($1); }
        ;

identifier : IDENTIFIER { $$ = mCc_ast_new_identifier($1); }
           ;

unary_op  : NOT   { $$ = MCC_AST_UNARY_OP_NOT; }
          | MINUS { $$ = MCC_AST_UNARY_OP_NEG; }
          ;

binary_op : expression PLUS expression       { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ADD, $1, $3); }
          | expression MINUS expression      { $$ = mCc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SUB, $1, $3); }
          | expression ASTER expression      { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_MUL, $1, $3); }
          | expression SLASH expression      { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_DIV, $1, $3); }
          | expression LESS expression       { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_LT, $1, $3); }
          | expression GREATER expression    { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_GT, $1, $3); }
          | expression LESS_EQ expression    { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_LEQ, $1, $3); }
          | expression GREATER_EQ expression { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_GEQ, $1, $3); }
          | expression AND expression        { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_AND, $1, $3); }
          | expression OR expression         { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_OR, $1, $3); }
          | expression EQUALS expression     { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_EQ, $1, $3); }
          | expression NOT_EQUALS expression { $$ = mCc_ast_new_expression_binary_op( MCC_AST_BINARY_OP_NEQ, $1, $3); }
          ;

single_expr : literal                                   { $$ = mCc_ast_new_expression_literal($1); }
            | identifier                                { $$ = mCc_ast_new_expression_identifier($1); }
            | identifier LBRACK expression RBRACK       { $$ = mCc_ast_new_expression_arr_subscr($1, $3); }
            | unary_op single_expr                      { $$ = mCc_ast_new_expression_unary_op($1, $2); }
            | LPARENTH expression RPARENTH              { $$ = mCc_ast_new_expression_parenth($2); }
            | identifier LPARENTH RPARENTH              { $$ = mCc_ast_new_expression_call_expr($1, NULL); }
            | identifier LPARENTH arguments RPARENTH    { $$ = mCc_ast_new_expression_call_expr($1, $3); }
            ;

expression : binary_op   { $$ = $1; }
           | single_expr { $$ = $1; }
           ;

statement : expression SEMICOLON                                           { $$ = mCc_ast_new_statement_expression($1); }
          | WHILE LPARENTH expression RPARENTH statement                   { $$ = mCc_ast_new_statement_while($3, $5); }
          | IF LPARENTH expression RPARENTH statement                      { $$ = mCc_ast_new_statement_if($3, $5, NULL); } %prec "then" /* give this statement the precedence named "then" */
          | IF LPARENTH expression RPARENTH statement ELSE statement       { $$ = mCc_ast_new_statement_if($3, $5, $7); }
          | LBRACE RBRACE                                                  { $$ = mCc_ast_new_statement_compound(NULL); }
          | LBRACE compound_stmt RBRACE                                    { $$ = $2; }
          | RETURN expression SEMICOLON                                    { $$ = mCc_ast_new_statement_return($2); }
          | RETURN SEMICOLON                                               { $$ = mCc_ast_new_statement_return(NULL); }
          | declaration SEMICOLON                                          { $$ = mCc_ast_new_statement_declaration($1);}
          | identifier ASSGN expression SEMICOLON                          { $$ = mCc_ast_new_statement_assgn($1, NULL, $3); }
          | identifier LBRACK expression RBRACK ASSGN expression SEMICOLON { $$ = mCc_ast_new_statement_assgn($1, $3, $6); }
          ;

compound_stmt : statement               { $$ = mCc_ast_new_statement_compound($1); }
              | compound_stmt statement { $$ = mCc_ast_compound_statement_add($1, $2); }
              ;

declaration : type identifier                       {$$ = mCc_ast_new_declaration($1, NULL, $2);}
            | type LBRACK literal RBRACK identifier {$$ = mCc_ast_new_declaration($1, $3 , $5);}
            ;

arguments : expression                 { $$ = mCc_ast_new_arguments($1);     }
          | arguments COMMA expression { $$ = mCc_ast_arguments_add($1, $3); }
          ;

parameters : declaration                  { $$ = mCc_ast_new_parameters($1);     }
           | parameters COMMA declaration { $$ = mCc_ast_parameters_add($1, $3); }
           ;

function_def : VOID identifier LPARENTH RPARENTH LBRACE RBRACE                          { $$ = mCc_ast_new_function_def_void($2, NULL, NULL); }
             | VOID identifier LPARENTH RPARENTH LBRACE compound_stmt RBRACE            { $$ = mCc_ast_new_function_def_void($2, NULL, $6); }
             | VOID identifier LPARENTH parameters RPARENTH LBRACE RBRACE               { $$ = mCc_ast_new_function_def_void($2, $4, NULL); }
             | VOID identifier LPARENTH parameters RPARENTH LBRACE compound_stmt RBRACE { $$ = mCc_ast_new_function_def_void($2, $4, $7); }
             | type identifier LPARENTH RPARENTH LBRACE RBRACE                          { $$ = mCc_ast_new_function_def_type($1, $2, NULL, NULL); }
             | type identifier LPARENTH RPARENTH LBRACE compound_stmt RBRACE            { $$ = mCc_ast_new_function_def_type($1, $2, NULL, $6); }
             | type identifier LPARENTH parameters RPARENTH LBRACE RBRACE               { $$ = mCc_ast_new_function_def_type($1, $2, $4, NULL); }
             | type identifier LPARENTH parameters RPARENTH LBRACE compound_stmt RBRACE { $$ = mCc_ast_new_function_def_type($1, $2, $4, $7); }
             ;

program : function_def         { $$ = mCc_ast_new_program($1); }
        | program function_def { $$ = mCc_ast_program_add($1, $2); }
        ;
%%

#include <assert.h>

#include "scanner.h"

void mCc_parser_error(struct MCC_PARSER_LTYPE *yylloc, yyscan_t *scanner,
                      const char *msg)
{
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

	if (yyparse(scanner, &result.expression, &result.statement, &result.program) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mCc_parser_lex_destroy(scanner);

	return result;
}
