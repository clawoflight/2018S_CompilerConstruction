/**
 * @file ast.c
 * @brief Implementation of the AST.
 * @author warhawk
 * @date 2018-03-08
 */
#include "mCc/ast.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------- Expressions */

struct mCc_ast_expression *
mCc_ast_new_expression_literal(struct mCc_ast_literal *literal)
{
	assert(literal);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_LITERAL;
	expr->literal = literal;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_unary_op(enum mCc_ast_unary_op op,
                                struct mCc_ast_expression *subexpression)
{
	assert(subexpression);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr)
		return NULL;

	expr->type = MCC_AST_EXPRESSION_TYPE_UNARY_OP;
	expr->unary_op = op;
	expr->unary_expression = subexpression;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
                                 struct mCc_ast_expression *lhs,
                                 struct mCc_ast_expression *rhs)
{
	assert(lhs);
	assert(rhs);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_BINARY_OP;
	expr->op = op;
	expr->lhs = lhs;
	expr->rhs = rhs;
	return expr;
}

struct mCc_ast_expression *
mCc_ast_new_expression_parenth(struct mCc_ast_expression *expression)
{
	assert(expression);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_PARENTH;
	expr->expression = expression;
	return expr;
}

void mCc_ast_delete_expression(struct mCc_ast_expression *expression)
{
	assert(expression);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		mCc_ast_delete_literal(expression->literal);
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		mCc_ast_delete_expression(expression->unary_expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		mCc_ast_delete_expression(expression->lhs);
		mCc_ast_delete_expression(expression->rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		mCc_ast_delete_expression(expression->expression);
		break;
	}

	free(expression);
}

/*--------------------------------------------------------------- Statements */

struct mCc_ast_statement *
mCc_ast_new_statement_expression(struct mCc_ast_expression *expression)
{
	assert(expression);

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	if (!stmt)
		return NULL;

	stmt->type = MCC_AST_STATEMENT_TYPE_EXPR;
	stmt->expression = expression;
	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_if(struct mCc_ast_expression *if_cond,
                         struct mCc_ast_statement *if_stmt,
                         struct mCc_ast_statement *else_stmt)
{
	assert(if_cond);
	assert(if_stmt);

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	if (!stmt)
		return NULL;

	stmt->type = MCC_AST_STATEMENT_TYPE_IF;
	stmt->if_cond = if_cond;
	stmt->if_stmt = if_stmt;

	if (else_stmt) {
		stmt->type = MCC_AST_STATEMENT_TYPE_IFELSE;
		stmt->else_stmt = else_stmt;
	}
	return stmt;
}

void mCc_ast_delete_statement(struct mCc_ast_statement *statement)
{
	assert(statement);

	switch (statement->type) {
	case MCC_AST_STATEMENT_TYPE_EXPR:
		mCc_ast_delete_expression(statement->expression);
		break;

	case MCC_AST_STATEMENT_TYPE_IFELSE:
		mCc_ast_delete_statement(statement->else_stmt);
		// Fallthrough

	case MCC_AST_STATEMENT_TYPE_IF:
		mCc_ast_delete_expression(statement->if_cond);
		mCc_ast_delete_statement(statement->if_stmt);
		break;
	}
}

/* ---------------------------------------------------------------- Literals */

struct mCc_ast_literal *mCc_ast_new_literal_int(long value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_float(double value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_string(char *value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	char *str = malloc((strlen(value) + 1) * sizeof(char));
	strcpy(str, value);

	lit->type = MCC_AST_LITERAL_TYPE_STRING;
	lit->s_value = str;
	return lit;
}

struct mCc_ast_literal *mCc_ast_new_literal_bool(bool value)
{
	struct mCc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_BOOL;
	lit->b_value = value;
	return lit;
}

void mCc_ast_delete_literal(struct mCc_ast_literal *literal)
{
	assert(literal);
	free(literal);
}
