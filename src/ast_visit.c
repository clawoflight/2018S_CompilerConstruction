/**
 * @file ast_visit.c
 * @brief Implementation of the AST traversal mechanism.
 * @author warhawk
 * @date 2018-03-08
 */
#include "mCc/ast_visit.h"

#include <assert.h>

#define visit(node, callback, visitor) \
	do { \
		if (callback) { \
			(callback)(node, (visitor)->userdata); \
		} \
	} while (0)

#define visit_if(cond, node, callback, visitor) \
	do { \
		if (cond) { \
			visit(node, callback, visitor); \
		} \
	} while (0)

#define visit_if_pre_order(node, callback, visitor) \
	visit_if((visitor)->order == MCC_AST_VISIT_PRE_ORDER, node, callback, \
	         visitor)

#define visit_if_post_order(node, callback, visitor) \
	visit_if((visitor)->order == MCC_AST_VISIT_POST_ORDER, node, callback, \
	         visitor)

void mCc_ast_visit_statement(struct mCc_ast_statement *statement,
                             struct mCc_ast_visitor *visitor)
{
	assert(statement);
	assert(visitor);

	visit_if_pre_order(statement, visitor->statement, visitor);

	switch (statement->type) {
	case MCC_AST_STATEMENT_TYPE_EXPR:
		visit_if_pre_order(statement, visitor->statement_expr, visitor);
		mCc_ast_visit_expression(statement->expression, visitor);
		visit_if_post_order(statement, visitor->statement_expr, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_IFELSE:
		visit_if_pre_order(statement, visitor->statement_ifelse, visitor);
		mCc_ast_visit_expression(statement->if_expr, visitor);
		mCc_ast_visit_statement(statement->if_stmt, visitor);
		mCc_ast_visit_statement(statement->else_stmt, visitor);
		visit_if_post_order(statement, visitor->statement_ifelse, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_IF:
		visit_if_pre_order(statement, visitor->statement_if, visitor);
		mCc_ast_visit_expression(statement->if_expr, visitor);
		mCc_ast_visit_statement(statement->if_stmt, visitor);
		visit_if_post_order(statement, visitor->statement_if, visitor);
		break;
	}

	visit_if_post_order(statement, visitor->statement, visitor);
}

void mCc_ast_visit_expression(struct mCc_ast_expression *expression,
                              struct mCc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit_if_pre_order(expression, visitor->expression, visitor);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		visit_if_pre_order(expression, visitor->expression_literal, visitor);
		mCc_ast_visit_literal(expression->literal, visitor);
		visit_if_post_order(expression, visitor->expression_literal, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		visit_if_pre_order(expression, visitor->expression_unary_op, visitor);
		mCc_ast_visit_expression(expression->unary_expression, visitor);
		visit_if_post_order(expression, visitor->expression_unary_op, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		visit_if_pre_order(expression, visitor->expression_binary_op, visitor);
		mCc_ast_visit_expression(expression->lhs, visitor);
		mCc_ast_visit_expression(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_binary_op, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		visit_if_pre_order(expression, visitor->expression_parenth, visitor);
		mCc_ast_visit_expression(expression->expression, visitor);
		visit_if_post_order(expression, visitor->expression_parenth, visitor);
		break;
	}

	visit_if_post_order(expression, visitor->expression, visitor);
}

void mCc_ast_visit_literal(struct mCc_ast_literal *literal,
                           struct mCc_ast_visitor *visitor)
{
	assert(literal);
	assert(visitor);

	visit_if_pre_order(literal, visitor->literal, visitor);

	switch (literal->type) {
	case MCC_AST_LITERAL_TYPE_INT:
		visit(literal, visitor->literal_int, visitor);
		break;

	case MCC_AST_LITERAL_TYPE_FLOAT:
		visit(literal, visitor->literal_float, visitor);
		break;

	case MCC_AST_LITERAL_TYPE_STRING:
		visit(literal, visitor->literal_string, visitor);
		break;

	case MCC_AST_LITERAL_TYPE_BOOL:
		visit(literal, visitor->literal_bool, visitor);
		break;
	}

	visit_if_post_order(literal, visitor->literal, visitor);
}
