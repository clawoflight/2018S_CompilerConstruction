/**
 * @file ast_statements.c
 * @brief Implementation of the AST statement node constructors and destructors.
 * @author bennett
 * @date 2018-03-09
 */
#include "mCc/ast.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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
