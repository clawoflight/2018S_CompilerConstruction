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

///Forward declaration
static inline enum mCc_ast_type mCc_check_expression(struct mCc_ast_expression *expr);

static inline enum mCc_ast_type convert_literal_type_to_type(
                                enum mCc_ast_literal_type lit_type)
{
    switch (lit_type){
        case MCC_AST_LITERAL_TYPE_INT:
            return MCC_AST_TYPE_INT;

        case MCC_AST_LITERAL_TYPE_FLOAT:
            return MCC_AST_TYPE_FLOAT;

        case MCC_AST_LITERAL_TYPE_BOOL:
            return MCC_AST_TYPE_BOOL;

        case MCC_AST_LITERAL_TYPE_STRING:
            return MCC_AST_TYPE_STRING;
        default:
            return MCC_AST_TYPE_VOID;
    }
}

static inline enum mCc_ast_type mCc_check_call_expr(struct mCc_ast_statement *stmt)
{


}

static inline enum mCc_ast_type mCc_check_assign(struct mCc_ast_statement *stmt)
{

}

static inline enum mCc_ast_type mCc_check_unary(struct mCc_ast_expression *unary)
{
    enum mCc_ast_type computed_type = mCc_check_expression(unary->unary_expression);

    switch(unary->unary_op){
        case MCC_AST_UNARY_OP_NEG:
            if ((computed_type != MCC_AST_TYPE_INT) &&
                    (computed_type != MCC_AST_TYPE_FLOAT))
                return MCC_AST_TYPE_VOID;
                //TODO better error
            break;

        case MCC_AST_UNARY_OP_NOT:
            if (computed_type != MCC_AST_TYPE_BOOL)
                return MCC_AST_TYPE_VOID;
                //TODO better error
            break;

        default:
            //Should not be here
            break;
    }
        return computed_type;
}

static inline enum mCc_ast_type mCc_check_binary(struct mCc_ast_expression *binary)
{
    enum mCc_ast_type computed_type_left = mCc_check_expression(binary->lhs);
    enum mCc_ast_type computed_type_right = mCc_check_expression(binary->rhs);

    if (computed_type_left != computed_type_right)
        return MCC_AST_TYPE_VOID;

    switch(binary->op){

        case MCC_AST_BINARY_OP_ADD:
        case MCC_AST_BINARY_OP_SUB:
        case MCC_AST_BINARY_OP_MUL:
        case MCC_AST_BINARY_OP_DIV:
            if ((computed_type_left != MCC_AST_TYPE_INT) &&
                (computed_type_left != MCC_AST_TYPE_FLOAT))
                return MCC_AST_TYPE_VOID;
            //TODO better error
            break;

        case MCC_AST_BINARY_OP_LT:
        case MCC_AST_BINARY_OP_GT:
        case MCC_AST_BINARY_OP_LEQ:
        case MCC_AST_BINARY_OP_GEQ:
            if (computed_type_left == MCC_AST_TYPE_BOOL)
                return MCC_AST_TYPE_VOID;
            //TODO better error
            return MCC_AST_TYPE_BOOL;

        case MCC_AST_BINARY_OP_AND:
        case MCC_AST_BINARY_OP_OR:
            if (computed_type_left != MCC_AST_TYPE_BOOL)
                return MCC_AST_TYPE_VOID;
            //TODO better error
            return MCC_AST_TYPE_BOOL;

        case MCC_AST_BINARY_OP_EQ:
        case MCC_AST_BINARY_OP_NEQ:
            return MCC_AST_TYPE_BOOL;

        default:
            //Should not be here
            break;
    }
    return computed_type_left;

}

static inline enum mCc_ast_type mCc_check_arr_subscr(struct mCc_ast_expression *expr)
{

}

static inline enum mCc_ast_type mCc_check_expression(struct mCc_ast_expression *expr)
{

    switch(expr->type){
        case MCC_AST_EXPRESSION_TYPE_LITERAL:
            expr->node.computed_type = convert_literal_type_to_type(expr->literal->type);
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

        case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
            expr->node.computed_type = mCc_check_call_expr(expr);
            break;

        case MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR:
            expr->node.computed_type = mCc_check_arr_subscr(expr);
            break;

        default:
            //Or should i even be here?
            break;
    }
    return expr->node.computed_type;
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
            .expression_unary_op = mCc_check_unary,
            .expression_binary_op = mCc_check_binary,
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
    return;
}

/**
 * Dummy Function for testing
 */
enum mCc_ast_type test_type_check(struct mCc_ast_expression *expression)
{
    return mCc_check_expression(expression);
}

