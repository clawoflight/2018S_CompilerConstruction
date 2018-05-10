/**
 * @file tac_builder.c
 * @brief Implementation of the linked AST -> TAC converter.
 * @author bennett
 * @date 2018-04-27
 */
#include "mCc/tac.h"
#include "mCc/tac_builder.h"

static void mCc_tac_entry_from_declaration(struct mCc_ast_declaration *decl)
{

    if (decl->decl_type != MCC_AST_TYPE_STRING){
        struct mCc_tac_quad_entry *entry = mCc_tac_create_new_entry();
        decl->decl_id->symtab_ref->tac_tmp = entry;
    }
    else {
        struct mCc_tac_quad_entry *entry = mCc_tac_create_new_string();
    }
}

static mCc_tac_quad_entry get_var_from_id(struct mCc_ast_program *prog,
                                          struct mCc_ast_identifier *id){
    //TODO Error if tmp was not found
    return id->symtab_ref->tac_tmp;
}

static void mCc_tac_from_stmt(struct mCc_ast_program *prog,
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
			mCc_tac_from_statement_while;
			break;
		case MCC_AST_STATEMENT_TYPE_DECL:
			mCc_tac_entry_from_declaration(prog,stmt->declaration);
			break;
		case MCC_AST_STATEMENT_TYPE_ASSGN:
			mCc_tac_entry_from_assg(prog,stmt);
			break;
		case MCC_AST_STATEMENT_TYPE_EXPR:
			mCc_tac_from_expression(prog,stmt->expression);
			break;
		case MCC_AST_STATEMENT_TYPE_CMPND:
            for(int i=0;i<(stmt->compound_stmt_count);i++) {
				mCc_tac_from_stmt(prog, stmt->compound_stmts[i]);
			}
			break;
	}
	return entry;
}

static struct mCc_tac_quad_entry *
		mCc_tac_from_expression(struct mCc_ast_program *prog,
								struct mCc_ast_expression *exp){

	struct mCc_tac_quad_entry *entry;

	switch (exp->type){
		case MCC_AST_EXPRESSION_TYPE_LITERAL:
			entry=mCc_tac_create_new_entry();
			struct mCc_tac_quad_literal lit=get_quad_literal(exp->literal);
			struct mCc_tac_quad lit_quad=mCc_tac_quad_new_assign_lit(lit,entry);
			mCc_tac_program_add_quad(prog, lit_quad);
			break;
		case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
			entry=exp->identifier->symtab_ref->tac_tmp;
			break;
		case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
			entry=mCc_tac_from_expression(prog,exp->unary_exp);
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
		case MCC_AST_EXPRESSION_TYPE_ARR_SUBSC:
			entry=mCc_tac_from_expression_arr_subscr(prog,exp);
			break;
	}
	return entry;

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
	struct mCc_tac_quad_entry *result = mCc_tac_creat_new_entry();
	struct mCC_tac_entry *result1 =	get_var_from_id(expr->array_id);
	struct mCC_tac_entry *result2 = mCc_tac_from_expression(prog, expr->subscript_expr);//array subscript
	struct mCc_tac_quad *array_subscr = mCc_tac_quad_new_load(result, result2, result);
	mCc_tac_program_add_quad(prog, array_subscr);
	return result;
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

	struct mCc_tac_label label_fun = get_label_from_fun_name(expr->f_name);
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

	struct mCc_tac_quad *label_after_if_quad = mCc_Tac_quad_new_label(label_after_if);
	mCc_tac_program_add_quad(prog, label_after_if_quad);
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
	struct mCc_tac_quad *label_after_if_quad = mCc_Tac_quad_new_label(label_after_if);
	mCc_tac_program_add_quad(prog, label_after_if_quad);
}

static struct mCc_tac_quad_entry mCc_tac_entry_from_assg(struct mCc_tac_program *prog,
						struct mCc_ast_statement *stmt){

	struct mCc_tac_quad new_quad;
	struct mCC_tac_entry *result=get_var_from_id(stmt->id_assgn);

	struct mCc_tac_quad_entry *result_lhs =
				mCc_tac_from_expression(prog, stmt->lhs_assgn);
	struct mCc_tac_quad_entry *result_rhs =
			mCc_tac_from_expression(prog, stmt->rhs_assgn);

	if (stmt->rhs_assgn->type==MCC_AST_EXPRESSION_TYPE_LITERAL){
		struct mCc_tac_quad_literal lit_result=get_quad_literal(stmt->rhs_assgn);
		new_quad =mCc_tac_quad_new_assign_lit(lit_result,result);
	} else if(stmt->lhs_assgn){
		new_quad = mCc_tac_quad_new_store(result_lhs,result_rhs,result);
	} else {
		new_quad = mCc_tac_quad_new_assign(result_rhs, result);
	}
	mCc_tac_program_add_quad(prog, new_quad);
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

	mCc_tac_from_statement(prog, stmt->while_stmt);
	struct mCc_tac_quad *jump_to_cond = mCc_tac_quad_new_jump(label_cond);
	mCc_tac_program_add_quad(prog, jump_to_cond);
	mCc_tac_program_add_quad(prog, label_after_while_quad);
}


static int mCc_tac_from_statement_return(struct mCc_tac_program *prog,
                                         struct mCc_ast_statement *stmt)
{
	struct mCc_tac_quad_entry entry;
	if(stmt->ret_val){
		entry=mCc_tac_from_expression(prog,stmt->ret_val);
	}
	new_quad=mCc_tac_quad_new_return(prog,stmt);

}

static int mCc_tac_from_function_def(struct mCc_tac_program *prog, struct mCc_ast_function_def *fun_def)
{
	struct mCc_tac_label label_fun = get_label_from_fun_name(fun_def->identifier);

	struct mCc_tac_quad *label_fun_quad = mCc_tac_quad_new_label(label_fun);
	mCc_tac_program_add_quad(prog, label_fun_quad);
	// TODO error checking

	mCc_tac_from_statement(prog, fun_def->body);
	// TODO error checking
}
