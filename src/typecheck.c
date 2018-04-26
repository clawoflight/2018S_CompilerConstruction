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
static inline bool mCc_check_statement(struct mCc_ast_statement *stmt);

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


/************** EXPRESSIONS */

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

static inline enum mCc_ast_type mCc_check_arr_subscr(struct mCc_ast_expression *arr_subscr)
{
    enum mCc_ast_type subscript_type = mCc_check_expression(arr_subscr->subscript_expr);

    if (subscript_type != MCC_AST_TYPE_INT)
        return MCC_AST_TYPE_VOID;
        //TODO better error

    return arr_subscr->array_id->symtab_ref->primitive_type;
}

static inline bool mCc_check_paramaters(struct mCc_ast_arguments *args,
                                        struct mCc_ast_parameters *params)
{
    if ((!params || params->decl_count == 0) && (!args || args->expression_count == 0))
        return true;

    if (params->decl_count == args->expression_count){
        for (int i = 0; i < params->decl_count; ++i){
            if(params->decl[i]->decl_type != mCc_check_expression(args->expressions[i]))
                return false;
        }
        return true;
    }
    return false;
}

static inline enum mCc_ast_type mCc_check_func_type(struct mCc_ast_function_def *func)
{
    /**
     * For now checks for only one return
     */
    enum mCc_ast_type func_type = func->identifier->symtab_ref->primitive_type;

}

static inline enum mCc_ast_type mCc_check_call_expr(struct mCc_ast_expression *call)
{
//TODO TBD
    /**
     * mCc_check_parameters returns true or false, if the signature matches
     * mCc_check_func_type returns the type of the called function
     *
     * The function type check is missing, this means check return
     * of the function itself, this is just for the call expr,
     * which in the end has to call the check for the correct return type
     * later on. This check can be done by extending the if with a call to
     * mCc_check_return or a similar name
     *
     */
    if (mCc_check_paramaters(call->arguments, call->f_name->symtab_ref->params))
        return call->f_name->symtab_ref->primitive_type;
    return MCC_AST_TYPE_VOID; //TODO better "Error", since fkt can be void
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

        case MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR:
            expr->node.computed_type = mCc_check_arr_subscr(expr);
            break;

        case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
            expr->node.computed_type = mCc_check_call_expr(expr);
            break;

        default:
            //Or should i even be here?
            break;
    }
    return expr->node.computed_type;
}

/************** STATEMENTS */

static inline bool mCc_check_if(struct mCc_ast_statement *stmt)
{
    if (mCc_check_expression(stmt->if_cond) != MCC_AST_TYPE_BOOL)
        return false; //TODO Better Error

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
    /**
     * I have to think this through.
     * I'll write this last
     */
}

static inline bool mCc_check_while(struct mCc_ast_statement *stmt)
{
    if (mCc_check_expression(stmt->while_cond) != MCC_AST_TYPE_BOOL)
        return false;

    bool while_type = mCc_check_statement(stmt->while_stmt);
    return while_type;
}

static inline bool mCc_check_assign(struct mCc_ast_statement *stmt)
{
    enum mCc_ast_type id_type = stmt->id_assgn->symtab_ref->primitive_type;
    enum mCc_ast_type rhs_type = mCc_check_expression(stmt->rhs_assgn);

    if (id_type != rhs_type)
        return false;

    if (stmt->lhs_assgn)
        if (mCc_check_expression(stmt->lhs_assgn) != MCC_AST_TYPE_INT)
            return false;

    return true;
}

static inline bool mCc_check_cmpnd(struct mCc_ast_statement *stmt)
{
    bool all_correct = true;
    for (int i=0; i < stmt->compound_stmt_count; ++i){
        if (!mCc_check_statement(stmt->compound_stmts[i])){
            all_correct = false;
            break;
        }
    }
    return all_correct;
}

static inline bool mCc_check_statement(struct mCc_ast_statement *stmt)
{
    switch(stmt->type){
        case MCC_AST_STATEMENT_TYPE_IF:
        case MCC_AST_STATEMENT_TYPE_IFELSE:

            return mCc_check_if(stmt);

        case MCC_AST_STATEMENT_TYPE_RET:
            return mCc_check_ret(stmt); //TODO TBD


        case MCC_AST_STATEMENT_TYPE_RET_VOID:
            return mCc_check_ret(stmt); //TODO TBD

        case MCC_AST_STATEMENT_TYPE_WHILE:
            return mCc_check_while(stmt);

        case MCC_AST_STATEMENT_TYPE_DECL:
            return true;
            //break; //TODO: braucht das überhaupt einen check?
                   //TODO: Ich glaube nicht

        case MCC_AST_STATEMENT_TYPE_ASSGN:
            return mCc_check_assign(stmt);

        case MCC_AST_STATEMENT_TYPE_EXPR:
            if (mCc_check_expression(stmt->expression) != MCC_AST_TYPE_VOID)
                return true;
            return false;

        case MCC_AST_STATEMENT_TYPE_CMPND:
            return mCc_check_cmpnd(stmt);

        default:
            //Should not be here
            break;
    }
}

//TODO Brauchen wir diesen visitor überhaupt?

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

            .expression_literal = mCc_check_expression,
            .expression_identifier = mCc_check_expression,
            .expression_unary_op = mCc_check_unary,
            .expression_binary_op = mCc_check_binary,
            .expression_parenth = mCc_check_expression,
            .expression_call_expr = mCc_check_call_expr,
            .expression_arr_subscr = mCc_check_arr_subscr,

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
 * Dummy Functions for testing
 */
enum mCc_ast_type test_type_check(struct mCc_ast_expression *expression)
{
    return mCc_check_expression(expression);
}

bool test_type_check_stmt(struct mCc_ast_statement *stmt)
{
    return mCc_check_statement(stmt);
}