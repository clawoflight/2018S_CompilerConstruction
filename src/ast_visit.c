/**
 * @file ast_visit.c
 * @brief Implementation of the AST traversal mechanism.
 * @author warhawk
 * @date 2018-03-08
 */
#include "mCc/ast_visit.h"
#include "mCc/symtab.h"
#include "stack.h"

#include <assert.h>
#include <stdio.h>

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
		mCc_ast_visit_expression(statement->if_cond, visitor);
		mCc_ast_visit_statement(statement->if_stmt, visitor);
		mCc_ast_visit_statement(statement->else_stmt, visitor);
		visit_if_post_order(statement, visitor->statement_ifelse, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_IF:
		visit_if_pre_order(statement, visitor->statement_if, visitor);
		mCc_ast_visit_expression(statement->if_cond, visitor);
		mCc_ast_visit_statement(statement->if_stmt, visitor);
		visit_if_post_order(statement, visitor->statement_if, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_WHILE:
		visit_if_pre_order(statement, visitor->statement_while, visitor);
		mCc_ast_visit_expression(statement->while_cond, visitor);
		mCc_ast_visit_statement(statement->while_stmt, visitor);
		visit_if_post_order(statement, visitor->statement_while, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_RET:
		visit_if_pre_order(statement, visitor->statement_return, visitor);
		mCc_ast_visit_expression(statement->ret_val, visitor);
		visit_if_post_order(statement, visitor->statement_return, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_RET_VOID:
		visit_if_pre_order(statement, visitor->statement_return_void, visitor);
		visit_if_post_order(statement, visitor->statement_return_void, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_CMPND:
		// Only execute when linking symbol table
		// TODO: try out if I can move the scope creation to the callback
		// (assuming pre-order). That would fix the double scope for every
		// function issue. (DO THAT AFTER HAVING RUNNING TESTS!)
		if (visitor->mode == MCC_AST_VISIT_MODE_SYMTAB_REF) {
			struct mCc_symtab_scope *new_scope =
			    mCc_symtab_new_scope_in(visitor->userdata, "anon");
			visitor->userdata = new_scope;
		}

		visit_if_pre_order(statement, visitor->statement_compound, visitor);
		for (unsigned int i = 0; i < statement->compound_stmt_count; ++i)
			mCc_ast_visit_statement(statement->compound_stmts[i], visitor);
		visit_if_post_order(statement, visitor->statement_compound, visitor);

		// Only execute when linking symbol table
		if (visitor->mode == MCC_AST_VISIT_MODE_SYMTAB_REF) {
			struct mCc_symtab_scope *tmp = visitor->userdata;
			visitor->userdata = tmp->parent;
		}
		break;

	case MCC_AST_STATEMENT_TYPE_ASSGN:
		visit_if_pre_order(statement, visitor->statement_assgn, visitor);
		mCc_ast_visit_identifier(statement->id_assgn, visitor);
		if (statement->lhs_assgn)
			mCc_ast_visit_expression(statement->lhs_assgn, visitor);
		mCc_ast_visit_expression(statement->rhs_assgn, visitor);

		visit_if_post_order(statement, visitor->statement_assgn, visitor);
		break;

	case MCC_AST_STATEMENT_TYPE_DECL:
		visit_if_pre_order(statement, visitor->statement_decl, visitor);
		mCc_ast_visit_declaration(statement->declaration, visitor);
		visit_if_post_order(statement, visitor->statement_decl, visitor);
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

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		visit_if_pre_order(expression, visitor->expression_identifier, visitor);
		mCc_ast_visit_identifier(expression->identifier, visitor);
		visit_if_post_order(expression, visitor->expression_identifier,
		                    visitor);
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

	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
		visit_if_pre_order(expression, visitor->expression_call_expr, visitor);
		mCc_ast_visit_identifier(expression->f_name, visitor);
		mCc_ast_visit_arguments(expression->arguments, visitor);
		visit_if_post_order(expression, visitor->expression_call_expr, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR:
		visit_if_pre_order(expression, visitor->expression_arr_subscr, visitor);
		mCc_ast_visit_identifier(expression->array_id, visitor);
		mCc_ast_visit_expression(expression->subscript_expr, visitor);
		visit_if_post_order(expression, visitor->expression_arr_subscr,
		                    visitor);
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

void mCc_ast_visit_identifier(struct mCc_ast_identifier *identifier,
                              struct mCc_ast_visitor *visitor)
{
	assert(identifier);
	assert(visitor);

	visit_if_pre_order(identifier, visitor->identifier, visitor);

	visit_if_post_order(identifier, visitor->identifier, visitor);
}

void mCc_ast_visit_arguments(struct mCc_ast_arguments *arguments,
                             struct mCc_ast_visitor *visitor)
{
	assert(visitor);

	if (arguments) {

		visit_if_pre_order(arguments, visitor->arguments, visitor);

		for (unsigned int i = 0; i < arguments->expression_count; ++i)
			mCc_ast_visit_expression(arguments->expressions[i], visitor);
		visit_if_post_order(arguments, visitor->arguments, visitor);
	}
}

void mCc_ast_visit_parameter(struct mCc_ast_parameters *parameter,
                             struct mCc_ast_visitor *visitor)
{
	assert(visitor);

	if (parameter) {

		visit_if_pre_order(parameter, visitor->parameter, visitor);

		for (unsigned int i = 0; i < parameter->decl_count; ++i)
			mCc_ast_visit_declaration(parameter->decl[i], visitor);
		visit_if_post_order(parameter, visitor->parameter, visitor);
	}
}

void mCc_ast_visit_declaration(struct mCc_ast_declaration *decl,
                               struct mCc_ast_visitor *visitor)
{
	assert(decl);
	assert(visitor);

	visit_if_pre_order(decl, visitor->declaration, visitor);
	if (decl->decl_array_size) {
		mCc_ast_visit_literal(decl->decl_array_size, visitor);
	}
	mCc_ast_visit_identifier(decl->decl_id, visitor);
	visit_if_post_order(decl, visitor->declaration, visitor);
}

void mCc_ast_visit_function_def(struct mCc_ast_function_def *func,
                                struct mCc_ast_visitor *visitor)
{
	assert(func);
	assert(visitor);

	// Only execute when linking symbol table
	if (visitor->mode == MCC_AST_VISIT_MODE_SYMTAB_REF) {
		struct mCc_symtab_scope *new_scope = mCc_symtab_new_scope_in(
		    visitor->userdata, func->identifier->id_value);
		visitor->userdata = new_scope;
	}

	visit_if_pre_order(func, visitor->function_def, visitor);
	mCc_ast_visit_identifier(func->identifier, visitor);
	if (func->body) {
		mCc_ast_visit_statement(func->body, visitor);
	}
	if (func->para) {
		mCc_ast_visit_parameter(func->para, visitor);
	}
	visit_if_post_order(func, visitor->function_def, visitor);

	// Only execute when linking symbol table
	if (visitor->mode == MCC_AST_VISIT_MODE_SYMTAB_REF) {
		struct mCc_symtab_scope *tmp = visitor->userdata;
		visitor->userdata = tmp->parent;
	}
}

void mCc_ast_visit_program(struct mCc_ast_program *prog,
                           struct mCc_ast_visitor *visitor)
{
	assert(prog);
	assert(visitor);

	visit_if_pre_order(prog, visitor->program, visitor);
	for (unsigned int i = 0; i < prog->func_def_count; ++i)
		mCc_ast_visit_function_def(prog->func_defs[i], visitor);
	visit_if_post_order(prog, visitor->program, visitor);
}
