/**
 * @file ast_typecheck.c
 * @brief Implementation of the type checking mechanism
 * @author bennett
 * @date 2018-04-17
 */

#include "mCc/typecheck.h"
#include <stdio.h>

static struct mCc_symtab_scope stack[4096];
#define err_len (4096)
static struct mCc_ast_symtab_build_result tmp_result = { 0 };

// Important!
#define MCC_AST_VISIT_SYMTAB_MODE
#include "mCc/ast_visit.h"

static void no_op() {}

static inline void mCc_check_assign(struct mCc_ast_statement *stmt, void *data)
{
    struct mCc_ast_statement *assgn = stmt;
    struct mCc_symtab_scope *scope = data;

}

static inline void mCc_check_unary(struct mCc_ast_expression *expr, void *data)
{
    struct mCc_ast_expression *unary = expr;
    struct mCc_symtab_scope *scope = data;

}

static inline void mCc_check_binary(struct mCc_ast_expression *expr, void *data)
{
    struct mCc_ast_expression *binary = expr;
    struct mCc_symtab_scope *scope = data;

}


static struct mCc_ast_visitor typecheck_visitor(void)
{
    return (struct mCc_ast_visitor){ .traversal = MCC_AST_VISIT_DEPTH_FIRST,
            .order = MCC_AST_VISIT_PRE_ORDER,

            .statement_if = no_op,
            .statement_ifelse = no_op,
            .statement_while = no_op,
            .statement_return = no_op,
            .statement_return_void = no_op,
            .statement_compound = no_op,
            .statement_assgn = mCc_check_assign,
            .statement_decl = no_op,

            .declaration = no_op,

            .expression_literal = no_op,
            .expression_identifier = no_op,
       //     .expression_unary_op = check_unary,
       //     .expression_binary_op = check_binary,
            .expression_parenth = no_op,
            .expression_call_expr = no_op,
            .expression_arr_subscr = no_op,

            .identifier = no_op,
            .arguments = no_op,
            .parameter = no_op,
            .function_def = no_op,

            .literal_int = no_op,
            .literal_float = no_op,
            .literal_string = no_op,
            .literal_bool = no_op };
}


struct mCc_typecheck_result mCc_typecheck(struct mCc_ast_program *program)
{
    mCc_check_assign(program->func_defs[0]->body, NULL);
    return;
}

