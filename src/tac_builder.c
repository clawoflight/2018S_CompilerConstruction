/**
 * @file tac_builder.c
 * @brief Implementation of the linked AST -> TAC converter.
 * @author bennett
 * @date 2018-04-27
 */
#include "mCc/tac.h"
#include "mCc/tac_builder.h"

static struct mCc_tac_quad_entry *
mCc_tac_from_expression_binary(struct mCc_tac_program *prog,
                               struct mCc_ast_expression *expr)
{
	struct mCc_tac_quad_entry *result1 =
	    mCc_tac_from_expression(prog, expr->lhs);
	struct mCc_tac_quad_entry *result2 =
	    mCc_tac_from_expression(prog, expr->rhs);

	enum mCc_tac_quad_binary_op op;
	switch (expr->op) {
	case MCC_AST_BINARY_OP_ADD:
		op = expr->node.type == FLOAT ? MCC_TAC_OP_BINARY_FLOAT_ADD
		                              : MCC_TAC_OP_BINARY_ADD;
		break;
	case MCC_AST_BINARY_OP_SUB:
		op = expr->node.type == FLOAT ? MCC_TAC_OP_BINARY_FLOAT_SUB
		                              : MCC_TAC_OP_BINARY_SUB;
		break;
	case MCC_AST_BINARY_OP_MULT:
		op = expr->node.type == FLOAT ? MCC_TAC_OP_BINARY_FLOAT_MUL
		                              : MCC_TAC_OP_BINARY_MUL;
		break;
	case MCC_AST_BINARY_OP_DIV:
		op = expr->node.type == FLOAT ? MCC_TAC_OP_BINARY_FLOAT_DIV
		                              : MCC_TAC_OP_BINARY_DIV;
	case MCC_AST_BINARY_OP_LT: op = MCC_TAC_OP_BINARY_LT; break;
	case MCC_AST_BINARY_OP_GT: op = MCC_TAC_OP_BINARY_GT; break;
	case MCC_AST_BINARY_OP_LEQ: op = MCC_TAC_OP_BINARY_LEQ; break;
	case MCC_AST_BINARY_OP_GEQ: op = MCC_TAC_OP_BINARY_GEQ; break;
	case MCC_AST_BINARY_OP_AND: op = MCC_TAC_OP_BINARY_AND; break;
	case MCC_AST_BINARY_OP_OR: op = MCC_TAC_OP_BINARY_OR; break;
	case MCC_AST_BINARY_OP_EQ: op = MCC_TAC_OP_BINARY_EQ; break;
	case MCC_AST_BINARY_OP_NEQ: op = MCC_TAC_OP_BINARY_NEQ; break;
	}

	struct mCc_tac_quad_entry *new_result = TODO_get_new_temporary();

	struct mCc_tac_quad *binary_op =
	    mCc_tac_quad_new_op_binary(op, result1, result2, new_result);
	// TODO error checking
	mCc_tac_program_add_quad(prog, binary_op);
	// TODO error checking
}

static struct mCc_tac_quad_entry *
mCc_tac_from_expression_unary(struct mCc_tac_program *prog,
                              struct mCc_ast_expression *expr)
{
	enum mCc_tac_quad_unary_op op;
	switch (expr->unary_op) {
	case MCC_AST_UNARY_OP_NEG: op = MCC_TAC_OP_UNARY_NEG; break;
	case MCC_AST_UNARY_OP_NOT: op = MCC_TAC_OP_UNARY_NOT; break;
	}

	struct mCc_tac_quad_entry *result =
	    mCc_tac_from_expression(prog, expr->unary_expression);

	result = mCc_tac_quad_new_op_unary(op, result, result);
	mCc_tac_program_add_quad(prog, result);
	// TODO error checking
}

static struct mCc_tac_quad_entry *
mCc_tac_from_expression_arr_subscr(struct mCc_tac_program *prog,struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for array index
	// create quad [load, result_of_prog]
	struct mCc_tac_quad_entry *result = TODO_get_new_tmp();
	//struct mCC_tac_entry *result1 =	TODO_get_lable_from_identifier(expr->array_id);// TODO Think about adding label or entry
	struct mCC_tac_entry *result2 = mCc_tac_from_expression(prog, expr->subscript_expr);//array subscript
	struct mCc_tac_quad *array_subscr = mCc_tac_quad_new_load(result, result2, result);
	mCc_tac_program_add_quad(prog, array_subscr);

}

static struct mCc_tac_quad_entry *
mCc_tac_from_expression_call(struct mCc_tac_program *prog,
                             struct mCc_ast_expression *expr)
{
	// Compute all params in reverse order and push them
	// TODO: I fear that we will need to first compute all params, then push
	// them. That would make this far more annoying - use variable-length array
	// to store results maybe?
	for (unsigned int i = expr->arguments->expression_count - 1; i >= 0; --i) {
		struct mCc_tac_quad_entry *param_temporary =
		    mCc_tac_from_expression(prog, expr->arguments->expressions[i]);
		struct mCc_tac_quad *param = mCc_tac_quad_new_param(param_temporary);
		mCc_tac_program_add_quad(prog, param);
	}

	struct mCc_tac_label label_fun = TODO_get_label_for_function(expr->f_name);
	struct mCc_tac_quad *jump_to_fun = mCc_tac_quad_new_jump(label_fun);
	mCc_tac_program_add_quad(prog, jump_to_fun);
}

// TODO think about if without else - which label to remove
static struct mCc_tac_quad_entry *
mCc_tac_from_statement_if(struct mCc_tac_program *prog,
                          struct mCc_ast_statement *stmt)
{
	// TODO error handling everywhere
	struct mCc_tac_label label_else = TODO_get_new_label();
	struct mCc_tac_label label_after_if = TODO_get_new_label();

	struct mCc_tac_quad_entry *cond =
	    mCc_tac_from_expression(prog, stmt->if_cond);
	struct mCc_tac_quad *jump_to_else =
	    mCc_tac_quad_new_jumpfalse(cond, label_else);
	mCc_tac_program_add_quad(prog, jump_to_else);
	mCc_tac_from_stmt(prog, stmt->if_stmt);

	struct mCc_tac_quad *jump_after_if = mCc_tac_quad_new_jump(label_after_if);
	mCc_tac_program_add_quad(prog, jump_after_if);

	mCc_tac_from_stmt(prog, stmt->else_stmt);
	struct mCc_tac_quad *label_after_if_quad = mCc_Tac_quad_new_label(label_after_if);
	mCc_tac_program_add_quad(prog, label_after_if_quad);
}

static int mCc_tac_from_statement_while(struct mCc_tac_program *prog,
                                        struct mCc_ast_statement *stmt)
{
	// TODO error handling everywhere
	struct mCc_tac_label label_cond = TODO_get_new_label();
	struct mCc_tac_label label_after_while = TODO_get_new_label();
	struct mCc_tac_quad *label_cond_quad = mCc_tac_quad_new_label(label_cond);
	struct mCc_tac_quad *label_after_while_quad = mCc_tac_quad_new_label(label_after_while);

	mCc_tac_program_add_quad(prog, label_cond_quad);
	struct mCc_tac_quad_entry *cond =
	    mCc_tac_from_expression(prog, stmt->while_cond);
	struct mCc_tac_quad *jump_after_while =
	    mCc_tac_quad_new_jumpfalse(cond, label_after_while);
	mCc_tac_program_add_quad(prog, jump_after_while);

	mCc_tac_from_statement(prog, stmt->while_stmt);
	struct mCc_tac_quad *jump_to_cond = mCc_tac_quad_new_jump(label_cond);
	mCc_tac_program_add_quad(prog, jump_to_cond);
	mCc_tac_program_add_quad(prog, label_after_while_quad);
}

static int mCc_tac_from_statement_return(struct mCc_tac_program *prog,
                                         struct mCc_ast_statement *stmt)
{
	// create mCc_tac_program for return expression
	// TODO will probably need a quad for return?
}

static int mCc_tac_from_function_def(struct mCc_tac_program *prog, struct mCc_ast_function_def *fun_def)
{
	struct mCc_tac_label label_fun = TODO_get_label_from_fun_name(fun_def->identifier);

	struct mCc_tac_quad *label_fun_quad = mCc_tac_quad_new_label(label_fun);
	mCc_tac_program_add_quad(prog, label_fun_quad);
	// TODO error checking

	mCc_tac_from_statement(prog, fun_def->body);
	// TODO error checking
}
