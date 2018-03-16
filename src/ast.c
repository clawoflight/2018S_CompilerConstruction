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
mCc_ast_new_expression_identifier(struct mCc_ast_identifier *identifier)
{
	assert(identifier);
	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_IDENTIFIER;
	expr->identifier = identifier;
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

struct mCc_ast_expression *
mCc_ast_new_expression_call_expr(struct mCc_ast_identifier *identifier,
                                 struct mCc_ast_arguments *arguments)
{
	assert(identifier);

	struct mCc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr)
		return NULL;

	expr->type = MCC_AST_EXPRESSION_TYPE_CALL_EXPR;
	expr->f_name = identifier;
	expr->arguments = arguments;
	return expr;
}

void mCc_ast_delete_expression(struct mCc_ast_expression *expression)
{
	assert(expression);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		mCc_ast_delete_literal(expression->literal);
		break;

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		mCc_ast_delete_identifier(expression->identifier);
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		mCc_ast_delete_expression(expression->unary_expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		mCc_ast_delete_expression(expression->lhs);
		mCc_ast_delete_expression(expression->rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
		mCc_ast_delete_identifier(expression->f_name);
		mCc_ast_delete_arguments(expression->arguments);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		mCc_ast_delete_expression(expression->expression);
		break;
	}

	free(expression);
}

/* ------------------------------------------------------------- Identifier */

struct mCc_ast_identifier *mCc_ast_new_identifier(char *value)
{
	struct mCc_ast_identifier *id = malloc(sizeof(*id));
	if (!id) {
		return NULL;
	}

	char *str = malloc((strlen(value) + 1) * sizeof(char));
	strcpy(str, value);

	id->id_value = str;
	return id;
}

void mCc_ast_delete_identifier(struct mCc_ast_identifier *identifier)
{
	assert(identifier);
	free(identifier->id_value);
	free(identifier);
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
	if (literal->type == MCC_AST_LITERAL_TYPE_STRING)
		free(literal->s_value);
	free(literal);
}

/*--------------------------------------------------------------- Arguments */

/// Size by which to increase arguments when reallocating
const int arguments_alloc_block_size = 10;

struct mCc_ast_arguments *
mCc_ast_new_arguments(struct mCc_ast_expression *expression)
{
	assert(expression);

	struct mCc_ast_arguments *args = malloc(sizeof(*args));
	if (!args)
		return NULL;

	args->expression_count = 0;
	args->expressions = NULL;

	if (expression && (args->expressions = malloc(arguments_alloc_block_size *
	                                              sizeof(args))) != NULL) {
		args->expression_count = 1;
		args->arguments_alloc_block_size = arguments_alloc_block_size;
		args->expressions[0] = expression;
	}

	return args;
}

struct mCc_ast_arguments *
mCc_ast_arguments_add(struct mCc_ast_arguments *self,
                      struct mCc_ast_expression *expression)
{
	assert(self);
	assert(expression);

	if (self->expression_count < self->arguments_alloc_block_size) {
		self->expressions[self->expression_count++] = expression;
		return self;
	}

	struct mCc_ast_expression **tmp;
	if ((tmp = realloc(self->expressions,
	                   arguments_alloc_block_size * sizeof(self))) == NULL) {
		mCc_ast_delete_arguments(self);
		return NULL;
	}

	self->arguments_alloc_block_size += arguments_alloc_block_size;
	self->expressions = tmp;
	self->expressions[self->expression_count++] = expression;
	return self;
}

void mCc_ast_delete_arguments(struct mCc_ast_arguments *arguments)
{
	if (arguments) {

		for (unsigned int i = 0; i < arguments->expression_count; ++i)
			mCc_ast_delete_expression(arguments->expressions[i]);
		if (arguments->expressions)
			free(arguments->expressions);

		free(arguments);
	}
}
