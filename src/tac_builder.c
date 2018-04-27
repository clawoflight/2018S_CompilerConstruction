/**
 * @file tac_builder.c
 * @brief Implementation of the linked AST -> TAC converter.
 * @author bennett
 * @date 2018-04-27
 */
#include "tac_builder.h"

static inline struct mCc_tac_quad *mCc_tac_from_expression_id(struct mCc_ast_expression* expr)
{

}

static inline struct mCc_tac_program *mCc_tac_from_if(struct mCc_ast_statement *stmt)
{
	// create quad label 1
	// create quad label 2
	// rec. create mCc_tac_program for condition
	// create quad [jumpfalse, condition_var, label 1]
	// rec. create mCc_tac_program for then-branch
	// create quad [jump, label 2]
	// rec. create mCc_tac_program for else-branch

	// create new program with memory for the three programs + 4 quads (jumpfalse, 2 labels)
	// copy condition into new program
	// copy jumpfalse into new program
	// copy then-branch into new program
	// copy jump into new program
	// copy label 1 into new program
	// copy else-branch into new program
	// copy label 2 into new program
}
