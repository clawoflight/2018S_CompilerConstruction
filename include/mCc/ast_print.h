/**
 * @file ast_print.h
 * @brief Declarations for the ast printing mechanism.
 * @author warhawk
 * @date 2018-03-08
 */
#ifndef MCC_AST_PRINT_H
#define MCC_AST_PRINT_H

#include <stdio.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get a human-readable string from a unary operation.
 *
 * @param op the type of the operation
 *
 * @return A human-readable string
 */
const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op);

/**
 * Get a human-readable string from a binary operation.
 *
 * @param op the type of the operation
 *
 * @return A human-readable string
 */
const char *mCc_ast_print_binary_op(enum mCc_ast_binary_op op);

/* ------------------------------------------------------------- DOT Printer */

void mCc_ast_print_dot_statement(FILE *out,
                                 struct mCc_ast_statement *statement);

void mCc_ast_print_dot_arguments(FILE *out,
                                 struct mCc_ast_arguments *arguments);

void mCc_ast_print_dot_parameter(FILE *out,
                                 struct mCc_ast_parameter *parameter);

void mCc_ast_print_dot_declaration(FILE *out, struct mCc_ast_declaration *decl);

void mCc_ast_print_dot_expression(FILE *out,
                                  struct mCc_ast_expression *expression);

void mCc_ast_print_dot_literal(FILE *out, struct mCc_ast_literal *literal);
void mCc_ast_print_dot_identifier(FILE *out,
                                  struct mCc_ast_identifier *identifier);
void mCc_ast_print_dot_func_def(FILE *out,
                                 struct mCc_ast_function_def *func);

#ifdef __cplusplus
}
#endif

#endif
