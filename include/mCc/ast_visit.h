/**
 * @file ast_visit.h
 * @brief Declarations for the AST traversal mechanism.
 *
 * NOTE:
 * To build the symbol table,
 * define MCC_AST_VISIT_SYMTAB_MODE before including this file,
 * and set visitor.userdata to an array of struct #mCc_ast_symtab_scope.
 * It will be used as stack.
 *
 * @author warhawk
 * @date 2018-03-08
 */
#ifndef MCC_AST_VISIT_H
#define MCC_AST_VISIT_H
#define MCC_AST_VISIT_SYMTAB_MODE
#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mCc_ast_visit_traversal {
	MCC_AST_VISIT_DEPTH_FIRST,
	/* TODO: MCC_AST_VISIT_BREADTH_FIRST, */
};

enum mCc_ast_visit_order {
	MCC_AST_VISIT_PRE_ORDER,
	MCC_AST_VISIT_POST_ORDER,
};

enum mCc_ast_visit_mode {
	MCC_AST_VISIT_MODE_NORMAL,
	MCC_AST_VISIT_MODE_SYMTAB_REF
};

/* Callbacks */
typedef void (*mCc_ast_visit_expression_cb)(struct mCc_ast_expression *,
                                            void *);
typedef void (*mCc_ast_visit_literal_cb)(struct mCc_ast_literal *, void *);
typedef void (*mCc_ast_visit_statement_cb)(struct mCc_ast_statement *, void *);
typedef void (*mCc_ast_visit_declaration_cb)(struct mCc_ast_declaration *,
                                             void *);
typedef void (*mCc_ast_visit_arguments_cb)(struct mCc_ast_arguments *, void *);
typedef void (*mCc_ast_visit_parameter_cb)(struct mCc_ast_parameters *, void *);
typedef void (*mCc_ast_visit_identifier_cb)(struct mCc_ast_identifier *,
                                            void *);

typedef void (*mCc_ast_visit_function_def_cb)(struct mCc_ast_function_def *,
                                              void *);
typedef void (*mCc_ast_visit_program_cb)(struct mCc_ast_program *, void *);

struct mCc_ast_visitor {
	enum mCc_ast_visit_traversal traversal;
	enum mCc_ast_visit_order order;
	enum mCc_ast_visit_mode mode;

	void *userdata;

	mCc_ast_visit_statement_cb statement;
	mCc_ast_visit_statement_cb statement_expr;
	mCc_ast_visit_statement_cb statement_if;
	mCc_ast_visit_statement_cb statement_ifelse;
	mCc_ast_visit_statement_cb statement_while;
	mCc_ast_visit_statement_cb statement_return;
	mCc_ast_visit_statement_cb statement_return_void;
	mCc_ast_visit_statement_cb statement_compound;
	mCc_ast_visit_statement_cb statement_assgn;
	mCc_ast_visit_statement_cb statement_decl;
	mCc_ast_visit_declaration_cb declaration;

	mCc_ast_visit_expression_cb expression;
	mCc_ast_visit_expression_cb expression_identifier;
	mCc_ast_visit_expression_cb expression_literal;
	mCc_ast_visit_expression_cb expression_unary_op;
	mCc_ast_visit_expression_cb expression_binary_op;
	mCc_ast_visit_expression_cb expression_parenth;
	mCc_ast_visit_expression_cb expression_call_expr;
	mCc_ast_visit_expression_cb expression_arr_subscr;

	mCc_ast_visit_literal_cb literal;
	mCc_ast_visit_literal_cb literal_int;
	mCc_ast_visit_literal_cb literal_float;
	mCc_ast_visit_literal_cb literal_string;
	mCc_ast_visit_literal_cb literal_bool;

	mCc_ast_visit_identifier_cb identifier;
	mCc_ast_visit_arguments_cb arguments;
	mCc_ast_visit_parameter_cb parameter;
	mCc_ast_visit_function_def_cb function_def;
	mCc_ast_visit_program_cb program;
};

void mCc_ast_visit_statement(struct mCc_ast_statement *statement,
                             struct mCc_ast_visitor *visitor);

void mCc_ast_visit_declaration(struct mCc_ast_declaration *decl,
                               struct mCc_ast_visitor *visitor);

void mCc_ast_visit_expression(struct mCc_ast_expression *expression,
                              struct mCc_ast_visitor *visitor);

void mCc_ast_visit_literal(struct mCc_ast_literal *literal,
                           struct mCc_ast_visitor *visitor);

void mCc_ast_visit_identifier(struct mCc_ast_identifier *identifier,
                              struct mCc_ast_visitor *visitor);

void mCc_ast_visit_arguments(struct mCc_ast_arguments *arguments,
                             struct mCc_ast_visitor *visitor);

void mCc_ast_visit_parameter(struct mCc_ast_parameters *parameter,
                             struct mCc_ast_visitor *visitor);

void mCc_ast_visit_function_def(struct mCc_ast_function_def *func,
                                struct mCc_ast_visitor *visitor);

void mCc_ast_visit_program(struct mCc_ast_program *prog,
                           struct mCc_ast_visitor *visitor);

#ifdef __cplusplus
}
#endif

#endif
