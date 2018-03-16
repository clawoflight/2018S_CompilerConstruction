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

/// Size by which to increase compound_stmts when reallocking
const int compound_stmt_alloc_block_size = 10;

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

struct mCc_ast_statement *
mCc_ast_new_statement_while(struct mCc_ast_expression *while_cond,
                            struct mCc_ast_statement *while_stmt)
{
	assert(while_cond);
	assert(while_stmt);

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	if (!stmt)
		return NULL;

	stmt->type = MCC_AST_STATEMENT_TYPE_WHILE;
	stmt->while_cond = while_cond;
	stmt->while_stmt = while_stmt;

	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_return(struct mCc_ast_expression *ret_val)
{

	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	if (!stmt)
		return NULL;
	if (!ret_val) {
		stmt->type = MCC_AST_STATEMENT_TYPE_RET_VOID;
	} else {
		stmt->type = MCC_AST_STATEMENT_TYPE_RET;
	}
	stmt->ret_val = ret_val;
	return stmt;
}

struct mCc_ast_statement *
mCc_ast_new_statement_compound(struct mCc_ast_statement *substatement)
{
	struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
	if (!stmt)
		return NULL;

	stmt->type = MCC_AST_STATEMENT_TYPE_CMPND;
	stmt->compound_stmt_count = 0;
	stmt->compound_stmts = NULL;

	if (substatement &&
	    (stmt->compound_stmts =
	         malloc(compound_stmt_alloc_block_size * sizeof(stmt))) != NULL) {
		stmt->compound_stmt_count = 1;
		stmt->compound_stmt_alloc_size = compound_stmt_alloc_block_size;
		stmt->compound_stmts[0] = substatement;
	}

	return stmt;
}

struct mCc_ast_statement *
mCc_ast_compound_statement_add(struct mCc_ast_statement *self,
                               struct mCc_ast_statement *statement)
{
	assert(self);
	assert(statement);

	if (self->compound_stmt_count < self->compound_stmt_alloc_size) {
		self->compound_stmts[self->compound_stmt_count++] = statement;
		return self;
	}

	// Allocate additional memory if necessary
	struct mCc_ast_statement **tmp;
	self->compound_stmt_alloc_size += compound_stmt_alloc_block_size;
	if ((tmp = realloc(self->compound_stmts, self->compound_stmt_alloc_size *
	                                             sizeof(self))) == NULL) {
		mCc_ast_delete_statement(self);
		return NULL;
	}

	self->compound_stmts = tmp;
	self->compound_stmts[self->compound_stmt_count++] = statement;
	return self;
}

struct mCc_ast_statement *
mCc_ast_new_statement_declaration(enum mCc_ast_declaration_type type,
                         long val,
                         struct mCc_ast_identifier *id)
{
    assert(id);

    struct mCc_ast_statement *stmt = malloc(sizeof(*stmt));
    if (!stmt)
        return NULL;

    stmt->type = MCC_AST_STATEMENT_TYPE_DECL;
    stmt->dec_type = type;
    stmt->dec_id = id;


    if (val) {
        stmt->dec_val = val;
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

	case MCC_AST_STATEMENT_TYPE_WHILE:
		mCc_ast_delete_expression(statement->while_cond);
		mCc_ast_delete_statement(statement->while_stmt);
		break;

	case MCC_AST_STATEMENT_TYPE_CMPND:
		for (unsigned int i = 0; i < statement->compound_stmt_count; ++i)
			mCc_ast_delete_statement(statement->compound_stmts[i]);
		if (statement->compound_stmts)
			free(statement->compound_stmts);
		break;

	case MCC_AST_STATEMENT_TYPE_RET:
		mCc_ast_delete_expression(statement->ret_val);
		break;

    case MCC_AST_STATEMENT_TYPE_DECL:
        mCc_ast_delete_identifier(statement->dec_id);
        if(statement->dec_val) {
            mCc_ast_delete_literal(statement->dec_val);
        }
        break;

	case MCC_AST_STATEMENT_TYPE_RET_VOID: break;
	}

	free(statement);
}