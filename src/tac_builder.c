/**
 * @file tac_builder.c
 * @brief Implementation of the linked AST -> TAC converter.
 * @author bennett
 * @date 2018-04-27
 */
//#include "tac_builder.h"

static struct mCc_tac_quad_entry *
mCc_tac_from_expression_binary(struct mCc_tac_program *prog,
                               struct mCc_ast_expression *expr)
{
	struct mCc_tac_quad_entry *result1 =
	    mCc_tac_from_expression(prog, expr->lhs);
	struct mCc_tac_quad_entry *result2 =
	    mCc_tac_from_expression(prog, expr->rhs);

	enum mCc_tac_quad_binary_op operator;
	switch (expr->op) {
	case MCC_AST_BINARY_OP_ADD:
		operator= expr->node.type == FLOAT
		    ? MCC_TAC_EXP_TYPE_BINARY_OP_FLOAT_ADD
		    : MCC_TAC_EXP_TYPE_BINARY_OP_ADD;
		break;
	case MCC_AST_BINARY_OP_SUB:
		operator= expr->node.type == FLOAT
		    ? MCC_TAC_EXP_TYPE_BINARY_OP_FLOAT_SUB
		    : MCC_TAC_EXP_TYPE_BINARY_OP_SUB;
		break;
	case MCC_AST_BINARY_OP_MULT:
		operator= expr->node.type == FLOAT
		    ? MCC_TAC_EXP_TYPE_BINARY_OP_FLOAT_MUL
		    : MCC_TAC_EXP_TYPE_BINARY_OP_MUL;
		break;
	case MCC_AST_BINARY_OP_DIV:
		operator= expr->node.type == FLOAT
		    ? MCC_TAC_EXP_TYPE_BINARY_OP_FLOAT_DIV
		    : MCC_TAC_EXP_TYPE_BINARY_OP_DIV;
	case MCC_AST_BINARY_OP_LT: operator= MCC_TAC_EXP_TYPE_BINARY_OP_LT; break;
	case MCC_AST_BINARY_OP_GT: operator= MCC_TAC_EXP_TYPE_BINARY_OP_GT; break;
	case MCC_AST_BINARY_OP_LEG: operator= MCC_TAC_EXP_TYPE_BINARY_OP_LEQ; break;
	case MCC_AST_BINARY_OP_GEQ: operator= MCC_TAC_EXP_TYPE_BINARY_OP_GEQ; break;
	case MCC_AST_BINARY_OP_AND: operator= MCC_TAC_EXP_TYPE_BINARY_OP_AND; break;
	case MCC_AST_BINARY_OP_OR: operator= MCC_TAC_EXP_TYPE_BINARY_OP_OR; break;
	case MCC_AST_BINARY_OP_EQ: operator= MCC_TAC_EXP_TYPE_BINARY_OP_EQ; break;
	case MCC_AST_BINARY_OP_NEQ: operator= MCC_TAC_EXP_TYPE_BINARY_OP_NEQ; break;
	}

	struct mCc_tac_entry *new_result = TODO_get_new_temporary();

	struct mCc_tac_quad binary_op =
	    mCc_tac_quad_new_op_binary(operator, result1, result2, new_result);
	// TODO error checking
	mCc_tac_program_add_quad(prog, binary_op);
	// TODO error checking
}

static struct mCc_tac_program *
mCc_tac_from_expression_unary(struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for operand
	// create quad [unary_op, result_of_prog]

	// create new program with memory for program + 1 quad
	// copy all into new program
	// (destroy rec. program without destroying it's quads)
}

static struct mCc_tac_program *
mCc_tac_from_expression_arr_subscr(struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for array index
	// create quad [load, result_of_prog]
}

static struct mCc_tac_program *
mCc_tac_from_expression_call(struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for all arguments
	// create quad [param, result_of_prog] for each prog (important: in reverse
	// order!) create quad [jump, label_of_func]

	// create new program with memory for arguments + param quads + jump quad
	// copy all into new program (keep in mind params must be in reverse order)
	// (destroy rec. programs without destroying their quads)
}

// TODO think about if without else - which label to remove
static struct mCc_tac_program *
mCc_tac_from_statement_if(struct mCc_ast_statement *stmt)
{
	// create quad label 1
	// create quad label 2
	// rec. create mCc_tac_program for condition
	// create quad [jumpfalse, condition_var, label 1]
	// rec. create mCc_tac_program for then-branch
	// if no else-branch do not create a label 2 nor a jump 2
	// create quad [jump, label 2]
	// rec. create mCc_tac_program for else-branch

	// create new program with memory for the three programs + 4 quads
	// (jumpfalse, jump, 2 labels) copy condition into new program copy
	// jumpfalse into new program copy then-branch into new program copy jump
	// into new program copy label 1 into new program copy else-branch into new
	// program copy label 2 into new program (destroy rec. programs without
	// destroying their quads)
}

static struct mCc_tac_program *
mCc_tac_from_statement_while(struct mCc_tac_program *prog,
                             struct mCc_ast_statement *stmt)
{
	// TODO error handling everywhere
	struct mCc_tac_quad *label_cond = TODO_get_new_label();
	struct mCc_tac_quad *label_after_while = TODO_get_new_label();

	mCc_tac_program_add_quad(prog, label_cond);
	struct mCC_tac_quad_result *cond =
	    mCc_tac_from_expression(prog, stmt->while_cond);
	struct mCc_tac_quad *jump_after_while =
	    mCc_tac_quad_new_jumpfalse(cond, label_after_while);
	mCc_tac_program_add_quad(prog, jump_after_while);

	mCc_tac_from_statement(prog, stmt->while_stmt);
	struct mCc_tac_quad *jump_to_cond = mCc_tac_quad_new_jump(label_cond);
	mCc_tac_program_add_quad(prog, jump_to_cond);
	mCc_tac_program_add_quad(prog, label_after_while);
}

static struct mCc_tac_program *
mCc_tac_from_statement_return(struct mCc_ast_statement *stmt)
{
	// create mCc_tac_program for return expression
	// TODO will probably need a quad for return?
}
