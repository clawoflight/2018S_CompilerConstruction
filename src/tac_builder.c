/**
 * @file tac_builder.c
 * @brief Implementation of the linked AST -> TAC converter.
 * @author bennett
 * @date 2018-04-27
 */
#include "tac_builder.h"

static struct mCc_tac_program *mCc_tac_from_expression_binary(struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for both operands
	// create quad [binary_op, result_of_prog_1, result_of_prog_2]

	// create new program with memory for both programs + 1 quad
	// copy all into new program
	// (destroy rec. programs without destroying their quads)
}

static struct mCc_tac_program *mCc_tac_from_expression_unary(struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for operand
	// create quad [unary_op, result_of_prog]

	// create new program with memory for program + 1 quad
	// copy all into new program
	// (destroy rec. program without destroying it's quads)
}

static struct mCc_tac_program *mCc_tac_from_expression_arr_subscr(struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for array index
	// create quad [load, result_of_prog]
}

static struct mCc_tac_program *mCc_tac_from_expression_call(struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for all arguments
	// create quad [param, result_of_prog] for each prog (important: in reverse order!)
	// create quad [jump, label_of_func]

	// create new program with memory for arguments + param quads + jump quad
	// copy all into new program (keep in mind params must be in reverse order)
	// (destroy rec. programs without destroying their quads)
}

// TODO think about if without else - which label to remove
static struct mCc_tac_program *mCc_tac_from_statement_if(struct mCc_ast_statement *stmt)
{
	// create quad label 1
	// create quad label 2
	// rec. create mCc_tac_program for condition
	// create quad [jumpfalse, condition_var, label 1]
	// rec. create mCc_tac_program for then-branch
	// create quad [jump, label 2]
	// rec. create mCc_tac_program for else-branch

	// create new program with memory for the three programs + 4 quads (jumpfalse, jump, 2 labels)
	// copy condition into new program
	// copy jumpfalse into new program
	// copy then-branch into new program
	// copy jump into new program
	// copy label 1 into new program
	// copy else-branch into new program
	// copy label 2 into new program
	// (destroy rec. programs without destroying their quads)
}

static struct mCc_tac_program *mCc_tac_from_statement_while(struct mCc_ast_statement *stmt)
{
	// create quad label 1
	// create quad label 2
	// rec. create mCc_tac_program for condition
	// create quad [jumpfalse, condition_var, label 2]
	// rec. create mCc_tac_program for loop body
	// create quad [jump, label 1]

	// create new program with memory for the program + 4 quads (jumpfalse, jump, 2 labels)
	// copy label 1 into new program
	// copy condition into new program
	// copy jumpfalse into new program
	// copy body into new program
	// copy label 2 into new program
	// (destroy rec. program without destroying its quads)
}

static struct mCc_tac_program *mCc_tac_from_statement_return(struct mCc_ast_statement *stmt)
{
	// create mCc_tac_program for return expression
	// TODO will probably need a quad for return?
}
