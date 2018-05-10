/**
 * @file tac_builder.c
 * @brief Implementation of the linked AST -> TAC converter.
 * @author bennett
 * @date 2018-04-27
 */
#include "mCc/tac_builder.h"
#include "mCc/symtab.h"

/*********************************** Global array of strings */

/// Block size by which to increase array when reallocating
static const unsigned int global_string_block_size = 10;
/// Number of entries for which memory was allocated
static unsigned int global_string_alloc_size = 0;
/// Number of entries currently in array
static unsigned int global_string_count = 0;

/// All strings ever created, to use when printing or freeing
static struct mCc_tac_quad_entry **global_string_arr = NULL;
static struct mCc_tac_quad_entry *
mCc_tac_from_expression(struct mCc_tac_program *prog,
                        struct mCc_ast_expression *exp);
static void mCc_tac_from_stmt(struct mCc_tac_program *prog,
                              struct mCc_ast_statement *stmt);
struct mCc_tac_quad_literal* mCc_get_quad_literal(struct mCc_ast_literal *literal);


static void mCc_tac_string_from_assgn(struct mCc_tac_quad_entry *entry,
                                      struct mCc_tac_quad_literal *lit)
{
    strcpy(entry->str_value, lit->strval);

    if (global_string_count < global_string_alloc_size) {
        global_string_arr[global_string_count++] = entry;
        return;
    }

    struct mCc_tac_quad_entry **tmp;
    global_string_alloc_size += global_string_block_size;
    if ((tmp = realloc(global_string_arr,
                  global_string_alloc_size * sizeof(*tmp))) == NULL)
        return;

    global_string_arr = tmp;
    global_string_arr[global_string_count++] = entry;
}

static void mCc_tac_entry_from_declaration(struct mCc_ast_declaration *decl)
{
	struct mCc_tac_quad_entry *entry = malloc(sizeof(entry));

    if (decl->decl_type != MCC_AST_TYPE_STRING)
        entry = mCc_tac_create_new_entry();
    else
        entry = mCc_tac_create_new_string();

    decl->decl_id->symtab_ref->tac_tmp = entry;
}

static struct mCc_tac_quad_entry *mCc_get_var_from_id(struct mCc_ast_identifier *id){
    //TODO Error if tmp was not found
    return id->symtab_ref->tac_tmp;
}

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
		op = expr->node.computed_type == MCC_AST_TYPE_FLOAT ? MCC_TAC_OP_BINARY_FLOAT_ADD
		                              : MCC_TAC_OP_BINARY_ADD;
		break;
	case MCC_AST_BINARY_OP_SUB:
		op = expr->node.computed_type == MCC_AST_TYPE_FLOAT ? MCC_TAC_OP_BINARY_FLOAT_SUB
		                              : MCC_TAC_OP_BINARY_SUB;
		break;
	case MCC_AST_BINARY_OP_MUL:
		op = expr->node.computed_type == MCC_AST_TYPE_FLOAT ? MCC_TAC_OP_BINARY_FLOAT_MUL
		                              : MCC_TAC_OP_BINARY_MUL;
		break;
	case MCC_AST_BINARY_OP_DIV:
		op = expr->node.computed_type == MCC_AST_TYPE_FLOAT ? MCC_TAC_OP_BINARY_FLOAT_DIV
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

	struct mCc_tac_quad_entry *new_result = mCc_tac_create_new_entry();

	struct mCc_tac_quad *binary_op =
	    mCc_tac_quad_new_op_binary(op, result1, result2, new_result);
	// TODO error checking
	mCc_tac_program_add_quad(prog, binary_op);
	// TODO error checking

	return new_result;
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

	struct mCc_tac_quad *result_quad = mCc_tac_quad_new_op_unary(op, result, result);
	mCc_tac_program_add_quad(prog, result_quad);
	// TODO error checking
    return result;
}

static struct mCc_tac_quad_entry *
mCc_tac_from_expression_arr_subscr(struct mCc_tac_program *prog,struct mCc_ast_expression *expr)
{
	// rec. create mCc_tac_program for array index
	// create quad [load, result_of_prog]
	struct mCc_tac_quad_entry *result = mCc_tac_create_new_entry();
	struct mCc_tac_quad_entry *result1 = mCc_get_var_from_id(expr->array_id);
	struct mCc_tac_quad_entry *result2 = mCc_tac_from_expression(prog, expr->subscript_expr);//array subscript
	struct mCc_tac_quad *array_subscr = mCc_tac_quad_new_load(result1, result2, result);
	mCc_tac_program_add_quad(prog, array_subscr);
	return result;
}

struct mCc_tac_label mCc_get_label_from_fun_name(struct mCc_ast_identifier *f_name)
{

    struct mCc_tac_label label;
    strcpy(label.str, f_name->id_value);

    return label;
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

	struct mCc_tac_label label_fun = mCc_get_label_from_fun_name(expr->f_name);
	struct mCc_tac_quad *jump_to_fun = mCc_tac_quad_new_jump(label_fun);
	mCc_tac_program_add_quad(prog, jump_to_fun);
}

static struct mCc_tac_quad_entry *
mCc_tac_from_statement_if(struct mCc_tac_program *prog,
                          struct mCc_ast_statement *stmt)
{
	// TODO error handling everywhere
	struct mCc_tac_label label_after_if = mCc_tac_get_new_label();

	struct mCc_tac_quad_entry *cond =
	    mCc_tac_from_expression(prog, stmt->if_cond);
	mCc_tac_from_stmt(prog, stmt->if_stmt);

	struct mCc_tac_quad *jump_after_if = mCc_tac_quad_new_jump(label_after_if);
	mCc_tac_program_add_quad(prog, jump_after_if);

	struct mCc_tac_quad *label_after_if_quad = mCc_tac_quad_new_label(label_after_if);
	mCc_tac_program_add_quad(prog, label_after_if_quad);

    return cond;
}

static struct mCc_tac_quad_entry *
mCc_tac_from_statement_if_else(struct mCc_tac_program *prog,
						  struct mCc_ast_statement *stmt)
{
	// TODO error handling everywhere
	struct mCc_tac_label label_else = mCc_tac_get_new_label();
	struct mCc_tac_label label_after_if = mCc_tac_get_new_label();

	struct mCc_tac_quad_entry *cond =
			mCc_tac_from_expression(prog, stmt->if_cond);
	struct mCc_tac_quad *jump_to_else =
			mCc_tac_quad_new_jumpfalse(cond, label_else);
	mCc_tac_program_add_quad(prog, jump_to_else);
	mCc_tac_from_stmt(prog, stmt->if_stmt);

	struct mCc_tac_quad *jump_after_if = mCc_tac_quad_new_jump(label_after_if);
	mCc_tac_program_add_quad(prog, jump_after_if);

	mCc_tac_from_stmt(prog, stmt->else_stmt);
	struct mCc_tac_quad *label_after_if_quad = mCc_tac_quad_new_label(label_after_if);
	mCc_tac_program_add_quad(prog, label_after_if_quad);

    return cond;
}

static void mCc_tac_entry_from_assg(struct mCc_tac_program *prog,
						struct mCc_ast_statement *stmt){

	struct mCc_tac_quad *new_quad;
	struct mCc_tac_quad_entry *result = mCc_get_var_from_id(stmt->id_assgn);

	struct mCc_tac_quad_entry *result_lhs =
				mCc_tac_from_expression(prog, stmt->lhs_assgn);
	struct mCc_tac_quad_entry *result_rhs =
			mCc_tac_from_expression(prog, stmt->rhs_assgn);

	if (stmt->rhs_assgn->type==MCC_AST_EXPRESSION_TYPE_LITERAL){
		struct mCc_tac_quad_literal *lit_result=mCc_get_quad_literal(stmt->rhs_assgn->literal);
		new_quad =mCc_tac_quad_new_assign_lit(lit_result,result);
        if (stmt->rhs_assgn->literal->type == MCC_AST_LITERAL_TYPE_STRING)
            mCc_tac_string_from_assgn(result, lit_result);
	} else if(stmt->lhs_assgn){
		new_quad = mCc_tac_quad_new_store(result_lhs,result_rhs,result);
	} else {
		new_quad = mCc_tac_quad_new_assign(result_rhs, result);
	}
	mCc_tac_program_add_quad(prog, new_quad);

}
static int mCc_tac_from_statement_return(struct mCc_tac_program *prog,
										 struct mCc_ast_statement *stmt)
{
	struct mCc_tac_quad_entry *entry;
	if(stmt->ret_val){
		entry=mCc_tac_from_expression(prog,stmt->ret_val);
	}
	struct mCc_tac_quad *new_quad = mCc_tac_quad_new_return(entry);//TODO save the variable which gets the return value back
	mCc_tac_program_add_quad(prog, new_quad);
	return;
}

static int mCc_tac_from_statement_while(struct mCc_tac_program *prog,
                                        struct mCc_ast_statement *stmt)
{
	// TODO error handling everywhere
	struct mCc_tac_label label_cond = mCc_tac_get_new_label();
	struct mCc_tac_label label_after_while = mCc_tac_get_new_label();
	struct mCc_tac_quad *label_cond_quad = mCc_tac_quad_new_label(label_cond);
	struct mCc_tac_quad *label_after_while_quad = mCc_tac_quad_new_label(label_after_while);

	mCc_tac_program_add_quad(prog, label_cond_quad);
	struct mCc_tac_quad_entry *cond =
	    mCc_tac_from_expression(prog, stmt->while_cond);
	struct mCc_tac_quad *jump_after_while =
	    mCc_tac_quad_new_jumpfalse(cond, label_after_while);
	mCc_tac_program_add_quad(prog, jump_after_while);

	mCc_tac_from_stmt(prog, stmt->while_stmt);
	struct mCc_tac_quad *jump_to_cond = mCc_tac_quad_new_jump(label_cond);
	mCc_tac_program_add_quad(prog, jump_to_cond);
	mCc_tac_program_add_quad(prog, label_after_while_quad);
	return;
}

static int mCc_tac_from_function_def(struct mCc_tac_program *prog, struct mCc_ast_function_def *fun_def)
{
	struct mCc_tac_label label_fun = mCc_get_label_from_fun_name(fun_def->identifier);

	struct mCc_tac_quad *label_fun_quad = mCc_tac_quad_new_label(label_fun);
	mCc_tac_program_add_quad(prog, label_fun_quad);
	// TODO error checking

	mCc_tac_from_stmt(prog, fun_def->body);
	// TODO error checking
    return ;
}

struct mCc_tac_quad_literal* mCc_get_quad_literal(struct mCc_ast_literal *literal){
	struct mCc_tac_quad_literal *lit_quad;
	switch (literal->type){
		case MCC_AST_LITERAL_TYPE_INT:
			lit_quad->type=MCC_TAC_QUAD_LIT_INT;
			lit_quad->ival=literal->i_value;
			break;
		case MCC_AST_LITERAL_TYPE_FLOAT:
			lit_quad->type=MCC_TAC_QUAD_LIT_FLOAT;
			lit_quad->fval=literal->f_value;
			break;
		case MCC_AST_LITERAL_TYPE_BOOL:
			lit_quad->type=MCC_TAC_QUAD_LIT_BOOL;
			lit_quad->bval=literal->b_value;
			break;
		case MCC_AST_LITERAL_TYPE_STRING:
			lit_quad->type=MCC_TAC_QUAD_LIT_STR;
			lit_quad->strval=literal->s_value;
			break;
	}
	return lit_quad;
}


static void mCc_tac_from_stmt(struct mCc_tac_program *prog,
                              struct mCc_ast_statement *stmt){

    switch (stmt->type){
        case MCC_AST_STATEMENT_TYPE_IF:
            mCc_tac_from_statement_if(prog,stmt);
            break;
        case MCC_AST_STATEMENT_TYPE_IFELSE:
            mCc_tac_from_statement_if_else(prog,stmt);
            break;
        case MCC_AST_STATEMENT_TYPE_RET:
            mCc_tac_from_statement_return(prog,stmt);
            break;
        case MCC_AST_STATEMENT_TYPE_RET_VOID:
            mCc_tac_from_statement_return(prog,stmt);
            break;
        case MCC_AST_STATEMENT_TYPE_WHILE:
            mCc_tac_from_statement_while(prog,stmt);
            break;
        case MCC_AST_STATEMENT_TYPE_DECL:
            mCc_tac_entry_from_declaration(stmt->declaration);
            break;
        case MCC_AST_STATEMENT_TYPE_ASSGN:
            mCc_tac_entry_from_assg(prog,stmt);
            break;
        case MCC_AST_STATEMENT_TYPE_EXPR:
            mCc_tac_from_expression(prog,stmt->expression);
            break;
        case MCC_AST_STATEMENT_TYPE_CMPND:
            for(unsigned int i=0 ; i < (stmt->compound_stmt_count); i++) {
                mCc_tac_from_stmt(prog, stmt->compound_stmts[i]);
            }
            break;
    }
    return ;
}

static struct mCc_tac_quad_entry *
mCc_tac_from_expression(struct mCc_tac_program *prog,
                        struct mCc_ast_expression *exp){

    struct mCc_tac_quad_entry *entry;

    switch (exp->type){
        case MCC_AST_EXPRESSION_TYPE_LITERAL:
            entry=mCc_tac_create_new_entry();
            struct mCc_tac_quad_literal *lit=mCc_get_quad_literal(exp->literal);
            struct mCc_tac_quad *lit_quad=mCc_tac_quad_new_assign_lit(lit,entry);
            mCc_tac_program_add_quad(prog, lit_quad);
            break;
        case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
            entry=exp->identifier->symtab_ref->tac_tmp;
            break;
        case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
            entry=mCc_tac_from_expression_unary(prog,exp->unary_expression);
            break;
        case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
            entry=mCc_tac_from_expression_binary(prog,exp);
            break;
        case MCC_AST_EXPRESSION_TYPE_PARENTH:
            entry=mCc_tac_from_expression(prog,exp->expression);
            break;
        case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
            entry=mCc_tac_from_expression_call(prog,exp);
            break;
        case MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR:
            entry=mCc_tac_from_expression_arr_subscr(prog,exp);
            break;
    }
    return entry;

}

struct mCc_tac_program *mCc_tac_build(struct mCc_tac_program *prog)
{
	// TODO
}
