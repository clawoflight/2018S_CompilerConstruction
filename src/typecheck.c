/**
 * @file ast_typecheck.c
 * @brief Implementation of the type checking mechanism
 * @author bennett
 * @date 2018-04-17
 */

#include "mCc/ast_visit.h"
#include "mCc/typecheck.h"
#include <stdio.h>

/// Global Var to save the current func we're in
static struct mCc_ast_function_def *curr_func;

static struct mCc_typecheck_result typecheck_result = {
	MCC_TYPECHECK_STATUS_OK
};

/// Forward declarations
static inline enum mCc_ast_type
mCc_check_expression(struct mCc_ast_expression *expr);
static inline bool mCc_check_statement(struct mCc_ast_statement *stmt);
static inline bool mCc_check_if_else_return(struct mCc_ast_statement *stmt);
static inline bool mCc_check_if_return(struct mCc_ast_statement *stmt);

static inline void set_not_matching_types_error(
    char *expected_as_string, enum mCc_ast_type expected_as_type,
    enum mCc_ast_type given_type, struct mCc_ast_source_location sloc)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_OK) {
		typecheck_result.err_loc = sloc;

#pragma GCC diagnostic ignored "-Wsign-compare"
		if (expected_as_type != -1) {
			switch (expected_as_type) {
			case MCC_AST_TYPE_BOOL: expected_as_string = "Bool"; break;
			case MCC_AST_TYPE_INT: expected_as_string = "Integer"; break;
			case MCC_AST_TYPE_FLOAT: expected_as_string = "Float"; break;
			case MCC_AST_TYPE_STRING: expected_as_string = "String"; break;
			case MCC_AST_TYPE_VOID: expected_as_string = "Void"; break;
			default: expected_as_string = "UNDEFINED"; break;
			}
		}

		char *given;

		switch (given_type) {
		case MCC_AST_TYPE_BOOL: given = "Bool"; break;
		case MCC_AST_TYPE_INT: given = "Integer"; break;
		case MCC_AST_TYPE_FLOAT: given = "Float"; break;
		case MCC_AST_TYPE_STRING: given = "String"; break;
		case MCC_AST_TYPE_VOID: given = "Void"; break;
		default: given = "UNDEFINED"; break;
		}
		snprintf(typecheck_result.err_msg, err_len,
		         "Expected type %s, but found %s", expected_as_string, given);

		typecheck_result.status = MCC_TYPECHECK_STATUS_ERROR;
	}
	return;
}

static inline enum mCc_ast_type
convert_literal_type_to_type(enum mCc_ast_literal_type lit_type)
{
	switch (lit_type) {
	case MCC_AST_LITERAL_TYPE_INT: return MCC_AST_TYPE_INT;

	case MCC_AST_LITERAL_TYPE_FLOAT: return MCC_AST_TYPE_FLOAT;

	case MCC_AST_LITERAL_TYPE_BOOL: return MCC_AST_TYPE_BOOL;

	case MCC_AST_LITERAL_TYPE_STRING: return MCC_AST_TYPE_STRING;
	default: return MCC_AST_TYPE_VOID;
	}
}

/************** EXPRESSIONS */

static inline enum mCc_ast_type
mCc_check_unary(struct mCc_ast_expression *unary)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return MCC_AST_TYPE_VOID;

	enum mCc_ast_type computed_type =
	    mCc_check_expression(unary->unary_expression);

	switch (unary->unary_op) {
	case MCC_AST_UNARY_OP_NEG:
		if ((computed_type != MCC_AST_TYPE_INT) &&
		    (computed_type != MCC_AST_TYPE_FLOAT)) {
			set_not_matching_types_error("Integer or Float", -1, computed_type,
			                             unary->node.sloc);
			return MCC_AST_TYPE_VOID;
		}
		break;

	case MCC_AST_UNARY_OP_NOT:
		if (computed_type != MCC_AST_TYPE_BOOL) {
			set_not_matching_types_error("Bool", -1, computed_type,
			                             unary->node.sloc);
			return MCC_AST_TYPE_VOID;
		}
		break;

	default:
		// Should not be here
		break;
	}
	return computed_type;
}

static inline enum mCc_ast_type
mCc_check_binary(struct mCc_ast_expression *binary)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return MCC_AST_TYPE_VOID;

	enum mCc_ast_type computed_type_left = mCc_check_expression(binary->lhs);
	enum mCc_ast_type computed_type_right = mCc_check_expression(binary->rhs);

	if (computed_type_left != computed_type_right) {

		set_not_matching_types_error(NULL, computed_type_left,
		                             computed_type_right, binary->node.sloc);

		return MCC_AST_TYPE_VOID;
	}

	switch (binary->op) {

	case MCC_AST_BINARY_OP_ADD:
	case MCC_AST_BINARY_OP_SUB:
	case MCC_AST_BINARY_OP_MUL:
	case MCC_AST_BINARY_OP_DIV:
		if ((computed_type_left != MCC_AST_TYPE_INT) &&
		    (computed_type_left != MCC_AST_TYPE_FLOAT)) {
			set_not_matching_types_error("Integer or Float", -1,
			                             computed_type_left, binary->node.sloc);
			return MCC_AST_TYPE_VOID;
		}
		break;

	case MCC_AST_BINARY_OP_LT:
	case MCC_AST_BINARY_OP_GT:
	case MCC_AST_BINARY_OP_LEQ:
	case MCC_AST_BINARY_OP_GEQ:
		if (computed_type_left == MCC_AST_TYPE_BOOL) {
			set_not_matching_types_error("Integer, Float or String", -1,
			                             computed_type_left, binary->node.sloc);
			return MCC_AST_TYPE_VOID;
		}
		return MCC_AST_TYPE_BOOL;

	case MCC_AST_BINARY_OP_AND:
	case MCC_AST_BINARY_OP_OR:
		if (computed_type_left != MCC_AST_TYPE_BOOL) {
			set_not_matching_types_error("Bool", computed_type_left, -1,
			                             binary->node.sloc);
			return MCC_AST_TYPE_VOID;
		}
		return MCC_AST_TYPE_BOOL;

	case MCC_AST_BINARY_OP_EQ:
	case MCC_AST_BINARY_OP_NEQ: return MCC_AST_TYPE_BOOL;

	default:
		// Should not be here
		break;
	}
	return computed_type_left;
}

static inline enum mCc_ast_type
mCc_check_arr_subscr(struct mCc_ast_expression *arr_subscr)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return MCC_AST_TYPE_VOID;

	enum mCc_ast_type subscript_type =
	    mCc_check_expression(arr_subscr->subscript_expr);

	if (subscript_type != MCC_AST_TYPE_INT) {
		set_not_matching_types_error("Integer", -1, subscript_type,
		                             arr_subscr->subscript_expr->node.sloc);
		return MCC_AST_TYPE_VOID;
	}

	return arr_subscr->array_id->symtab_ref->primitive_type;
}

static inline bool mCc_check_paramaters(struct mCc_ast_arguments *args,
                                        struct mCc_ast_parameters *params,
                                        struct mCc_ast_source_location sloc)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return MCC_AST_TYPE_VOID;

	if ((!params || params->decl_count == 0) &&
	    (!args || args->expression_count == 0))
		return true;

	if (params && args && params->decl_count && args->expression_count &&
	    params->decl_count == args->expression_count) {
		for (unsigned int i = 0; i < params->decl_count; ++i) {
			enum mCc_ast_type computed_type =
			    mCc_check_expression(args->expressions[i]);
			if (params->decl[i]->decl_type != computed_type) {
				set_not_matching_types_error(NULL, params->decl[i]->decl_type,
				                             computed_type, sloc);
				return false;
			}
		}
		return true;
	}
	set_not_matching_types_error(NULL, MCC_AST_TYPE_VOID, MCC_AST_TYPE_VOID,
	                             sloc);
	snprintf(typecheck_result.err_msg, err_len,
	         "Mismatched number of arguments");
	typecheck_result.err_loc = sloc;
	return false;
}

static inline enum mCc_ast_type
mCc_check_func_type(struct mCc_ast_function_def *func)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return MCC_AST_TYPE_VOID;

	enum mCc_ast_type func_type = func->identifier->symtab_ref->primitive_type;
	return func_type;
}

static inline enum mCc_ast_type
mCc_check_call_expr(struct mCc_ast_expression *call)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return MCC_AST_TYPE_VOID;

	if (mCc_check_paramaters(call->arguments, call->f_name->symtab_ref->params,
	                         call->node.sloc))
		return call->f_name->symtab_ref->primitive_type;
	return MCC_AST_TYPE_VOID;
}

static inline enum mCc_ast_type
mCc_check_expression(struct mCc_ast_expression *expr)
{

	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR) {
		expr->node.computed_type = MCC_AST_TYPE_VOID;
		return MCC_AST_TYPE_VOID;
	}

	switch (expr->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		expr->node.computed_type =
		    convert_literal_type_to_type(expr->literal->type);
		break;

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		expr->node.computed_type = expr->identifier->symtab_ref->primitive_type;
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		expr->node.computed_type = mCc_check_unary(expr);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		expr->node.computed_type = mCc_check_binary(expr);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		expr->node.computed_type = mCc_check_expression(expr->expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR:
		expr->node.computed_type = mCc_check_arr_subscr(expr);
		break;

	case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
		expr->node.computed_type = mCc_check_call_expr(expr);
		break;

	default:
		// Should i even be here?
		break;
	}
	typecheck_result.type = expr->node.computed_type;
	return expr->node.computed_type;
}

/************** STATEMENTS */

static inline bool mCc_check_if(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	enum mCc_ast_type computed_type = mCc_check_expression(stmt->if_cond);
	if (computed_type != MCC_AST_TYPE_BOOL) {
		set_not_matching_types_error("Bool", -1, computed_type,
		                             stmt->node.sloc);
		return false;
	}

	bool if_type = mCc_check_statement(stmt->if_stmt);
	bool else_type = true;

	if (stmt->type == MCC_AST_STATEMENT_TYPE_IFELSE)
		else_type = mCc_check_statement(stmt->else_stmt);

	if (if_type && else_type)
		return true;

	return false;
}

static inline bool mCc_check_ret(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	enum mCc_ast_type ret_type = mCc_check_expression(stmt->ret_val);

	if (ret_type != curr_func->func_type) {
		set_not_matching_types_error(NULL, curr_func->func_type, ret_type,
		                             stmt->node.sloc);
		return false;
	}
	return true;
}

static inline bool mCc_check_ret_void(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	enum mCc_ast_type ret_type = MCC_AST_TYPE_VOID;
	if (ret_type != curr_func->func_type) {
		set_not_matching_types_error(NULL, curr_func->func_type, ret_type,
		                             stmt->node.sloc);
		return false;
	}
	return true;
}

static inline bool mCc_check_while(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	enum mCc_ast_type computed_type = mCc_check_expression(stmt->while_cond);
	if (computed_type != MCC_AST_TYPE_BOOL) {
		set_not_matching_types_error("Bool", -1, computed_type,
		                             stmt->node.sloc);
		return false;
	}

	bool while_type = mCc_check_statement(stmt->while_stmt);
	return while_type;
}

static inline bool mCc_check_assign(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	enum mCc_ast_type id_type = stmt->id_assgn->symtab_ref->primitive_type;
	enum mCc_ast_type rhs_type = mCc_check_expression(stmt->rhs_assgn);

	if (id_type != rhs_type) {
		set_not_matching_types_error(NULL, id_type, rhs_type, stmt->node.sloc);
		return false;
	}

	if (stmt->lhs_assgn) {
		enum mCc_ast_type lhs_type = mCc_check_expression(stmt->lhs_assgn);
		if (lhs_type != MCC_AST_TYPE_INT) {
			set_not_matching_types_error("Integer", -1, lhs_type,
			                             stmt->node.sloc);
			return false;
		}
	}
	return true;
}

static inline bool mCc_check_cmpnd(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	bool all_correct = true;
	for (unsigned int i = 0; i < stmt->compound_stmt_count; ++i) {
		if (!mCc_check_statement(stmt->compound_stmts[i])) {
			all_correct = false;
			break;
		}
	}
	return all_correct;
}

static inline bool mCc_check_cmpnd_return(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	struct mCc_ast_statement *curr_stmt;

	bool all_ret = true;
	bool if_path_return = true;

	for (unsigned int i = 0; i < stmt->compound_stmt_count; i++) {

		curr_stmt = stmt->compound_stmts[i];

		if (stmt->node.outside_if == false)
			curr_stmt->node.outside_if = false;
		else
			curr_stmt->node.outside_if = true;

		if (curr_stmt->type == MCC_AST_STATEMENT_TYPE_IF) {
			curr_stmt->node.outside_if = false;
			if_path_return = mCc_check_if_return(curr_stmt->if_stmt);

		} else if (curr_stmt->type == MCC_AST_STATEMENT_TYPE_IFELSE) {
			if_path_return = mCc_check_if_else_return(curr_stmt);
		} else if (curr_stmt->type == MCC_AST_STATEMENT_TYPE_RET) {
			all_ret = mCc_check_ret(curr_stmt);
		} else if (curr_stmt->type == MCC_AST_STATEMENT_TYPE_RET_VOID) {
			all_ret = mCc_check_ret_void(curr_stmt);
		}
	}

	return (all_ret && if_path_return);
}

static inline bool mCc_check_if_return(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	stmt->node.outside_if = false;
	struct mCc_ast_statement *inner_stmt = stmt;
	inner_stmt->node.outside_if = false;
	bool ret = true;

	if (inner_stmt->type == MCC_AST_STATEMENT_TYPE_CMPND)
		ret = mCc_check_cmpnd_return(inner_stmt);

	if (inner_stmt->type == MCC_AST_STATEMENT_TYPE_RET)
		ret = mCc_check_ret(inner_stmt);

	if (inner_stmt->type == MCC_AST_STATEMENT_TYPE_RET_VOID)
		ret = mCc_check_ret_void(inner_stmt);

	return ret;
}

static inline bool mCc_check_if_else_return(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	bool if_branch = false;
	bool else_branch = false;

	struct mCc_ast_statement *if_stmt = stmt->if_stmt;
	struct mCc_ast_statement *else_stmt = stmt->else_stmt;

	if_branch = mCc_check_if_return(if_stmt);

	if (else_stmt->type == MCC_AST_STATEMENT_TYPE_CMPND)
		else_branch = mCc_check_cmpnd_return(else_stmt);

	if (else_stmt->type == MCC_AST_STATEMENT_TYPE_RET)
		else_branch = mCc_check_ret(else_stmt);

	if (else_stmt->type == MCC_AST_STATEMENT_TYPE_RET_VOID)
		else_branch = mCc_check_ret_void(else_stmt);

	return (if_branch && else_branch);
}

static inline bool mCc_check_function(struct mCc_ast_function_def *func)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	if ((func->func_type == MCC_AST_TYPE_VOID) && !func->body) {
		return true;
	} else if ((func->func_type != MCC_AST_TYPE_VOID) && !func->body) {
		set_not_matching_types_error(NULL, func->func_type, func->func_type,
		                             func->node.sloc);
		snprintf(typecheck_result.err_msg, err_len,
		         "Function %s needs a return statement",
		         func->identifier->id_value);
		return false;
	}

	func->body->node.outside_if = true;
	bool check_func_for_return = mCc_check_cmpnd_return(func->body);
	bool check_func = mCc_check_statement(func->body);
	bool general_ret = false;

	for (unsigned int i = 0; i < func->body->compound_stmt_count; i++) {

		if (func->body->compound_stmts[i]->node.outside_if == true) {
			general_ret = true;
		}
	}

	if (func->func_type == MCC_AST_TYPE_VOID) {
		return (check_func_for_return && check_func);
	}

	if (!general_ret) {
		typecheck_result.status = MCC_TYPECHECK_STATUS_ERROR;
		snprintf(typecheck_result.err_msg, err_len,
		         "Function %s may not reach a return",
				 func->identifier->id_value);
		typecheck_result.err_loc = func->node.sloc;
	}

	return (check_func_for_return && check_func && general_ret);
}

static inline bool mCc_check_statement(struct mCc_ast_statement *stmt)
{
	if (typecheck_result.status == MCC_TYPECHECK_STATUS_ERROR)
		return false;

	switch (stmt->type) {
	case MCC_AST_STATEMENT_TYPE_IF:
	case MCC_AST_STATEMENT_TYPE_IFELSE: return mCc_check_if(stmt);

	case MCC_AST_STATEMENT_TYPE_RET: return mCc_check_ret(stmt);

	case MCC_AST_STATEMENT_TYPE_RET_VOID: return mCc_check_ret_void(stmt);

	case MCC_AST_STATEMENT_TYPE_WHILE: return mCc_check_while(stmt);

	case MCC_AST_STATEMENT_TYPE_DECL: return true;

	case MCC_AST_STATEMENT_TYPE_ASSGN: return mCc_check_assign(stmt);

	case MCC_AST_STATEMENT_TYPE_EXPR:
		if (mCc_check_expression(stmt->expression) != MCC_AST_TYPE_VOID)
			return true;
		return false;

	case MCC_AST_STATEMENT_TYPE_CMPND: return mCc_check_cmpnd(stmt);

	default:
		// Should not be here
		break;
	}
	return false; ///< Should never be here
}

int mCc_typecheck_check_main_properties(struct mCc_symtab_scope *scope)
{
	struct mCc_ast_identifier id;
	id.id_value = "main";

	struct mCc_symtab_entry *entry = mCc_symtab_scope_lookup_id(scope, &id);

	if (entry) {
		if (entry->primitive_type == MCC_AST_TYPE_VOID) {
			if (!entry->params) {
				return 0; /// if all properties are full filled
			}
		}
		set_not_matching_types_error(NULL, MCC_AST_TYPE_VOID, MCC_AST_TYPE_VOID,
		                             entry->sloc);
		snprintf(
		    typecheck_result.err_msg, err_len,
		    "Main function has to be void and does not take any arguments");
		return -1;
	}
	typecheck_result.status = MCC_TYPECHECK_STATUS_ERROR;
	snprintf(typecheck_result.err_msg, err_len,
	         "Main function has to be present");
	return -1;
}

struct mCc_typecheck_result mCc_typecheck(struct mCc_ast_program *program,
                                          struct mCc_symtab_scope *scope)
{

	if (mCc_typecheck_check_main_properties(scope) == -1) {
		return typecheck_result;
	}

	bool all_correct = true;
	for (unsigned int i = 0; i < program->func_def_count; i++) {
		curr_func = program->func_defs[i];
		all_correct = mCc_check_function(curr_func);
		if (!all_correct)
			break;
	}

	return typecheck_result;
}

/**
 * Dummy Functions for testing
 */
struct mCc_typecheck_result
mCc_typecheck_test_type_check(struct mCc_ast_expression *expression)
{
	typecheck_result.status = MCC_TYPECHECK_STATUS_OK;
	typecheck_result.type = mCc_check_expression(expression);
	return typecheck_result;
}

struct mCc_typecheck_result
mCc_typecheck_test_type_check_stmt(struct mCc_ast_statement *stmt)
{
	typecheck_result.status = MCC_TYPECHECK_STATUS_OK;
	if (!mCc_check_statement(stmt))
		typecheck_result.status = MCC_TYPECHECK_STATUS_ERROR;
	return typecheck_result;
}

struct mCc_typecheck_result
mCc_typecheck_test_type_check_program(struct mCc_ast_program *prog)
{
	typecheck_result.status = MCC_TYPECHECK_STATUS_OK;

	bool all_correct = true;
	for (unsigned int i = 0; i < prog->func_def_count; i++) {
		curr_func = prog->func_defs[i];
		all_correct = mCc_check_function(curr_func);
		if (!all_correct)
			break;
	}
	return typecheck_result;
}
