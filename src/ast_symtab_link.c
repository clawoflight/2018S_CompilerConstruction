/**
 * @file ast_symtab_link.c
 * @brief Implementation of the symtab building mechanism
 * @author bennett
 * @date 2018-04-17
 */
#include "mCc/ast_symtab_link.h"
#include "mCc/symtab.h"
#include <assert.h>

static struct mCc_ast_symtab_build_result tmp_result = { 0 };

// Important!
#define MCC_AST_VISIT_SYMTAB_MODE
#include "mCc/ast_visit.h"

#include <stdio.h>

static void no_op() {}

/******************************************************* Linking */

static void handle_assign(struct mCc_ast_statement *stmt, void *data)
{
	if (tmp_result.status)
		return; // Return if an error happened
	struct mCc_symtab_scope *scope = (struct mCc_symtab_scope *)data;

	enum MCC_SYMTAB_SCOPE_LINK_ERROR retval =
	    mCc_symtab_scope_link_ref_assignment(scope, stmt);
	switch (retval) {
	case MCC_SYMTAB_SCOPE_LINK_ERR_OK: return;
	case MCC_SYMTAB_SCOPE_LINK_ERR_UNDECLARED_ID:
		if ((snprintf(tmp_result.err_msg, err_len, "Use of undeclared id: '%s'",
		              stmt->id_assgn->id_value) == -1)) {
			strcpy(tmp_result.err_msg, "Use of undeclared id");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERR_ASSIGN_TO_FUNCTION:
		if ((snprintf(tmp_result.err_msg, err_len,
		              "Assignment to function name: '%s'",
		              stmt->id_assgn->id_value)) == -1) {
			strcpy(tmp_result.err_msg, "Assignment to function name");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERR_VAR:
		if ((snprintf(tmp_result.err_msg, err_len,
		              "Use of subscript on variable: '%s'",
		              stmt->id_assgn->id_value) == -1)) {
			strcpy(tmp_result.err_msg, "Use of subscript on variable");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERR_ARR_WITHOUT_BRACKS:
		if ((snprintf(tmp_result.err_msg, err_len,
		              "Use of array without subscript: '%s'",
		              stmt->id_assgn->id_value) == -1)) {
			strcpy(tmp_result.err_msg, "Use of array without subscript");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERR_FUN_WITHOUT_CALL: /* Fallthrough */
	case MCC_SYMTAB_SCOPE_LINK_ERROR_INVALID_AST_OBJECT:
		strcpy(tmp_result.err_msg,
		       "Development error! This error should not have happened here.");
		break;
	}

	// If an error happened, set status
	if (tmp_result.err_msg[0]) {
		tmp_result.err_loc = stmt->node.sloc;
		tmp_result.status = 1;
	}
}

static void handle_return(struct mCc_ast_statement *stmt, void *data)
{
    if (tmp_result.status)
        return; // Return if an error happened
    struct mCc_symtab_scope *scope = (struct mCc_symtab_scope *)data;

    enum MCC_SYMTAB_SCOPE_LINK_ERROR retval =
	            mCc_symtab_scope_link_ref_return(scope, stmt);
    switch (retval) {
        case MCC_SYMTAB_SCOPE_LINK_ERR_OK: return;
        case MCC_SYMTAB_SCOPE_LINK_ERR_UNDECLARED_ID:
            if ((snprintf(tmp_result.err_msg, err_len, "Use of undeclared id: '%s'",
                          stmt->id_assgn->id_value) == -1)) {
                strcpy(tmp_result.err_msg, "Use of undeclared id");
            }
            break;
        case MCC_SYMTAB_SCOPE_LINK_ERR_ASSIGN_TO_FUNCTION: //Fallthrough
        case MCC_SYMTAB_SCOPE_LINK_ERR_VAR:
            if ((snprintf(tmp_result.err_msg, err_len,
                          "Use of subscript on variable: '%s'",
                          stmt->id_assgn->id_value) == -1)) {
                strcpy(tmp_result.err_msg, "Use of subscript on variable");
            }
            break;
        case MCC_SYMTAB_SCOPE_LINK_ERR_ARR_WITHOUT_BRACKS:
            if ((snprintf(tmp_result.err_msg, err_len,
                          "Use of array without subscript: '%s'",
                          stmt->id_assgn->id_value) == -1)) {
                strcpy(tmp_result.err_msg, "Use of array without subscript");
            }
            break;
        case MCC_SYMTAB_SCOPE_LINK_ERR_FUN_WITHOUT_CALL: /* Fallthrough */
        case MCC_SYMTAB_SCOPE_LINK_ERROR_INVALID_AST_OBJECT:
            strcpy(tmp_result.err_msg,
                   "Development error! This error should not have happened here.");
            break;
    }

    // If an error happened, set status
    if (tmp_result.err_msg[0]) {
        tmp_result.err_loc = stmt->node.sloc;
        tmp_result.status = 1;
    }
}

static void handle_expression(struct mCc_ast_expression *expr, void *data)
{
	if (tmp_result.status)
		return; // Return if an error happened
	struct mCc_symtab_scope *scope = (struct mCc_symtab_scope *)data;

	enum MCC_SYMTAB_SCOPE_LINK_ERROR retval =
	    mCc_symtab_scope_link_ref_expression(scope, expr);
	switch (retval) {
	case MCC_SYMTAB_SCOPE_LINK_ERR_OK: return;
	case MCC_SYMTAB_SCOPE_LINK_ERR_UNDECLARED_ID:
		if ((snprintf(tmp_result.err_msg, err_len, "Use of undeclared id: '%s'",
		              expr->identifier->id_value) == -1)) {
			strcpy(tmp_result.err_msg, "Snprintf error");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERR_ASSIGN_TO_FUNCTION:
		if ((snprintf(tmp_result.err_msg, err_len,
		              "Assignment to function name: '%s'",
		              expr->identifier->id_value) == -1)) {
			strcpy(tmp_result.err_msg, "Snprintf error");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERR_FUN_WITHOUT_CALL:
		if (snprintf(tmp_result.err_msg, err_len,
		             "Use of a function without a call: '%d'", expr->type)) {
			strcpy(tmp_result.err_msg, "Snprintf error");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERR_ARR_WITHOUT_BRACKS:
		if ((snprintf(tmp_result.err_msg, err_len,
		              "Use of an array without brackets: '%d'",
		              expr->type) == -1)) {
			strcpy(tmp_result.err_msg, "Snprintf error");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERR_VAR:
		if ((snprintf(tmp_result.err_msg, err_len,
		              "Use of undeclared variable: '%d'", expr->type) == -1)) {
			strcpy(tmp_result.err_msg, "Snprintf error");
		}
		break;
	case MCC_SYMTAB_SCOPE_LINK_ERROR_INVALID_AST_OBJECT:
		strcpy(tmp_result.err_msg,
		       "Development error! This error should not haave happened here.");
		break;
	}

	// If an error happened, set status
	if (tmp_result.err_msg[0]) {
		tmp_result.err_loc = expr->node.sloc;
		tmp_result.status = 1;
	}
}

/******************************************************* Adding */

static void handle_declaration(struct mCc_ast_declaration *decl, void *data)
{
	if (tmp_result.status)
		return; // Return if an error happened
	struct mCc_symtab_scope *scope = (struct mCc_symtab_scope *)data;
	int retval = mCc_symtab_scope_add_decl(scope, decl);
	switch (retval) {
	case 1:
		if (snprintf(tmp_result.err_msg, err_len, "Redeclared id: '%s'",
		             decl->decl_id->id_value) == -1) {
			strcpy(tmp_result.err_msg, "Redeclared id");
		};
		break;
	case -1: strcpy(tmp_result.err_msg, "Memory allocation error"); break;
	default: break;
	}

	if (tmp_result.err_msg[0]) {
		tmp_result.err_loc = decl->node.sloc;
		tmp_result.status = 1;
	}
}

/* static void handle_func_def(struct mCc_ast_function_def *fn, void *data) */
/* { */
/* 	if (tmp_result.status) */
/* 		return; // Return if an error happened */
/* 	struct mCc_symtab_scope *scope = *(struct mCc_symtab_scope **)data; */
/* 	int retval = mCc_symtab_scope_add_func_def(scope, fn); */
/* 	switch (retval) { */
/* 	case 1: */
/* 		if (snprintf(tmp_result.err_msg, err_len, "Redeclared function '%s'", */
/* 		             fn->identifier->id_value) == -1) { */
/* 			strcpy(tmp_result.err_msg, "Redeclared function"); */
/* 		}; */
/* 		break; */
/* 	case -1: strcpy(tmp_result.err_msg, "Memory allocation error"); break; */
/* 	default: break; */
/* 	} */
/* 	// If an error happened, set status */
/* 	if (tmp_result.err_msg[0]) { */
/* 		tmp_result.err_loc = fn->node.sloc; */
/* 		tmp_result.status = 1; */
/* 	} */
/* } */

/***************************************************** Main things */

static struct mCc_ast_visitor
symtab_visitor(struct mCc_symtab_scope *curr_scope_ptr)
{
	return (struct mCc_ast_visitor){ .traversal = MCC_AST_VISIT_DEPTH_FIRST,
		                             .order = MCC_AST_VISIT_PRE_ORDER,
		                             .mode = MCC_AST_VISIT_MODE_SYMTAB_REF,

		                             // Important!
		                             .userdata = curr_scope_ptr,
		                             .statement_if = no_op,
		                             .statement_ifelse = no_op,
		                             .statement_while = no_op,
		                             .statement_return = no_op,
		                             .statement_return_void = no_op,
		                             .statement_compound = no_op,
		                             .statement_assgn = handle_assign,
		                             .statement_decl = no_op,

		                             .declaration = handle_declaration,

		                             .expression_literal = no_op,
		                             .expression_identifier = handle_expression,
		                             .expression_unary_op = no_op,
		                             .expression_parenth = no_op,
		                             .expression_call_expr = handle_expression,
		                             .expression_arr_subscr = handle_expression,

		                             .identifier = no_op,
		                             .arguments = no_op,
		                             .parameter = no_op,
		                             .function_def = no_op,

		                             .literal_int = no_op,
		                             .literal_float = no_op,
		                             .literal_string = no_op,
		                             .literal_bool = no_op };
}

struct mCc_ast_symtab_build_result
mCc_ast_symtab_build(struct mCc_ast_program *program)
{
	// To allow unit tests, reset globals
	memset(&tmp_result, 0, sizeof(tmp_result));
	struct mCc_symtab_scope *root_scope = mCc_symtab_new_scope_in(NULL, "");
	if (root_scope == NULL) {
		// TODO set error msg and status and return
	}

	// This will be used by the callbacks to store the scope they are currently
	// in
	struct mCc_symtab_scope *curr_scope_ptr = root_scope;

	// Before visiting, enter all functions into the symbol table because they
	// can be used before their declaration
	// TODO: check if this works!
	for (unsigned int i = 0; i < program->func_def_count; ++i) {
		int retval =
		    mCc_symtab_scope_add_func_def(root_scope, program->func_defs[i]);
		if (retval == 1)
			snprintf(tmp_result.err_msg, err_len, "Redefined function: %s",
			         program->func_defs[i]->identifier->id_value);
		if (retval == -1)
			strcpy(tmp_result.err_msg, "Memory error");
		if (retval) {
			tmp_result.err_loc = program->func_defs[i]->node.sloc;
			tmp_result.status = 1;
			goto end;
		}
	}

	tmp_result.root_symtab = root_scope;
	struct mCc_ast_visitor visitor = symtab_visitor(curr_scope_ptr);
	mCc_ast_visit_program(program, &visitor);

end:
	if (tmp_result.status) {
		mCc_symtab_delete_all_scopes();
		tmp_result.root_symtab = NULL;
	}
	return tmp_result;
}
