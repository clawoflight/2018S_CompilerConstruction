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
/// count variables for assembly
static unsigned int global_var_count = 0;

/// struct for cfg
static struct mCc_cfg_block tmp_block;
static unsigned int anonym_block_count = 0;

/// All strings ever created, to use for generating assembly
static struct mCc_tac_quad_entry *global_string_arr = NULL;

static struct mCc_tac_quad_entry
mCc_tac_from_expression(struct mCc_tac_program *prog,
                        struct mCc_ast_expression *exp);

static int mCc_tac_from_stmt(struct mCc_tac_program *prog,
                             struct mCc_ast_statement *stmt);

struct mCc_tac_quad_literal *
mCc_get_quad_literal(struct mCc_ast_literal *literal);

static enum mCc_tac_quad_literal_type
mCc_tac_type_from_ast_type(enum mCc_ast_type ast_type) {
    switch (ast_type) {
        case MCC_AST_TYPE_BOOL:
            return MCC_TAC_QUAD_LIT_BOOL;
        case MCC_AST_TYPE_INT:
            return MCC_TAC_QUAD_LIT_INT;
        case MCC_AST_TYPE_FLOAT:
            return MCC_TAC_QUAD_LIT_FLOAT;
        case MCC_AST_TYPE_STRING:
            return MCC_TAC_QUAD_LIT_STR;
        case MCC_AST_TYPE_VOID:
            return MCC_TAC_QUAD_LIT_VOID;
        default:
            return MCC_TAC_QUAD_LIT_VOID;
    }
}

static int mCc_tac_string_from_assgn(struct mCc_tac_quad_entry entry,
                                     struct mCc_tac_quad_literal *lit) {
    strcpy(entry.str_value, lit->strval);
    if (global_string_count < global_string_alloc_size) {
        global_string_arr[global_string_count++] = entry;
        return 1;
    }

    struct mCc_tac_quad_entry *tmp;
    global_string_alloc_size += global_string_block_size;
    if ((tmp = realloc(global_string_arr,
                       global_string_alloc_size * sizeof(*tmp))) == NULL)
        return 1;

    global_string_arr = tmp;
    global_string_arr[global_string_count++] = entry;
    return 0;
}

static void mCc_tac_entry_from_declaration(struct mCc_ast_declaration *decl) {
    struct mCc_tac_quad_entry entry;

    entry = mCc_tac_create_new_entry();

    if (decl->decl_array_size) {
        global_var_count += decl->decl_array_size->i_value;
        entry.array_size = decl->decl_array_size->i_value;
    }
    decl->decl_id->symtab_ref->tac_tmp = entry;
}

static struct mCc_tac_quad_entry
mCc_get_var_from_id(struct mCc_ast_identifier *id) {
    return id->symtab_ref->tac_tmp;
}

static struct mCc_tac_quad_entry
mCc_tac_from_expression_binary(struct mCc_tac_program *prog,
                               struct mCc_ast_expression *expr) {
    struct mCc_tac_quad_entry result1 =
            mCc_tac_from_expression(prog, expr->lhs);
    struct mCc_tac_quad_entry result2 =
            mCc_tac_from_expression(prog, expr->rhs);

    enum mCc_tac_quad_binary_op op = -1;
    switch (expr->op) {
        case MCC_AST_BINARY_OP_ADD:
            op = expr->node.computed_type == MCC_AST_TYPE_FLOAT
                 ? MCC_TAC_OP_BINARY_FLOAT_ADD
                 : MCC_TAC_OP_BINARY_ADD;
            break;
        case MCC_AST_BINARY_OP_SUB:
            op = expr->node.computed_type == MCC_AST_TYPE_FLOAT
                 ? MCC_TAC_OP_BINARY_FLOAT_SUB
                 : MCC_TAC_OP_BINARY_SUB;
            break;
        case MCC_AST_BINARY_OP_MUL:
            op = expr->node.computed_type == MCC_AST_TYPE_FLOAT
                 ? MCC_TAC_OP_BINARY_FLOAT_MUL
                 : MCC_TAC_OP_BINARY_MUL;
            break;
        case MCC_AST_BINARY_OP_DIV:
            op = expr->node.computed_type == MCC_AST_TYPE_FLOAT
                 ? MCC_TAC_OP_BINARY_FLOAT_DIV
                 : MCC_TAC_OP_BINARY_DIV;
            break;
        case MCC_AST_BINARY_OP_LT:
            op = MCC_TAC_OP_BINARY_LT;
            break;
        case MCC_AST_BINARY_OP_GT:
            op = MCC_TAC_OP_BINARY_GT;
            break;
        case MCC_AST_BINARY_OP_LEQ:
            op = MCC_TAC_OP_BINARY_LEQ;
            break;
        case MCC_AST_BINARY_OP_GEQ:
            op = MCC_TAC_OP_BINARY_GEQ;
            break;
        case MCC_AST_BINARY_OP_AND:
            op = MCC_TAC_OP_BINARY_AND;
            break;
        case MCC_AST_BINARY_OP_OR:
            op = MCC_TAC_OP_BINARY_OR;
            break;
        case MCC_AST_BINARY_OP_EQ:
            op = MCC_TAC_OP_BINARY_EQ;
            break;
        case MCC_AST_BINARY_OP_NEQ:
            op = MCC_TAC_OP_BINARY_NEQ;
            break;
    }

    struct mCc_tac_quad_entry new_result = mCc_tac_create_new_entry();
    global_var_count++;

    struct mCc_tac_quad *binary_op =
            mCc_tac_quad_new_op_binary(op, result1, result2, new_result);
    binary_op->cfg_node.number = tmp_block.number;
    binary_op->cfg_node.label_name = tmp_block.label_name;
    mCc_tac_program_add_quad(prog, binary_op);

    return new_result;
}

static struct mCc_tac_quad_entry
mCc_tac_from_expression_unary(struct mCc_tac_program *prog,
                              struct mCc_ast_expression *expr) {
    assert(expr);

    enum mCc_tac_quad_unary_op op = -1;
    switch (expr->unary_op) {
        case MCC_AST_UNARY_OP_NEG:
            op = MCC_TAC_OP_UNARY_NEG;
            break;
        case MCC_AST_UNARY_OP_NOT:
            op = MCC_TAC_OP_UNARY_NOT;
            break;
    }

    struct mCc_tac_quad_entry result =
            mCc_tac_from_expression(prog, expr->unary_expression);

    struct mCc_tac_quad *result_quad =
            mCc_tac_quad_new_op_unary(op, result, result);
    mCc_tac_program_add_quad(prog, result_quad);
    result_quad->cfg_node.number = tmp_block.number;
    result_quad->cfg_node.label_name = tmp_block.label_name;
    return result;
}

static struct mCc_tac_quad_entry
mCc_tac_from_expression_arr_subscr(struct mCc_tac_program *prog,
                                   struct mCc_ast_expression *expr) {
    // rec. create mCc_tac_program for array index
    // create quad [load, result_of_prog]

    struct mCc_tac_quad_entry result = mCc_tac_create_new_entry();
    struct mCc_tac_quad_entry array = mCc_get_var_from_id(expr->array_id);
    array.array_size = expr->identifier->symtab_ref->arr_size;
    struct mCc_tac_quad_entry index =
            mCc_tac_from_expression(prog, expr->subscript_expr); // array subscript
    struct mCc_tac_quad *array_subscr =
            mCc_tac_quad_new_load(array, index, result);
    array_subscr->cfg_node.number = tmp_block.number;
    array_subscr->cfg_node.label_name = tmp_block.label_name;
    if (mCc_tac_program_add_quad(prog, array_subscr)) {
        // TODO error handling
    }
    return result;
}

struct mCc_tac_label
mCc_get_label_from_fun_name(struct mCc_ast_identifier *f_name) {

    struct mCc_tac_label label;
    strcpy(label.str, f_name->id_value);
    label.num =
            -1; // for assembly to distinguish if we have a label or func. name

    return label;
}

static struct mCc_tac_quad_entry
mCc_tac_from_expression_call(struct mCc_tac_program *prog,
                             struct mCc_ast_expression *expr) {
    // Compute all params in reverse order and push them
    // them. That would make this far more annoying - use variable-length array
    // to store results maybe?
    if (expr->arguments) {
        for (int i = expr->arguments->expression_count - 1; i >= 0; --i) {
            struct mCc_tac_quad_entry param_temporary =
                    mCc_tac_from_expression(prog, expr->arguments->expressions[i]);
            struct mCc_tac_quad *param =
                    mCc_tac_quad_new_param(param_temporary);
            //param->cfg_node.number=global_block_count;
            mCc_tac_program_add_quad(prog, param);
        }
    }

    struct mCc_tac_label label_fun = mCc_get_label_from_fun_name(expr->f_name);
    struct mCc_tac_quad_entry retval = mCc_tac_create_new_entry();
    retval.type =
            mCc_tac_type_from_ast_type(expr->f_name->symtab_ref->primitive_type);
    struct mCc_tac_quad *jump_to_fun = mCc_tac_quad_new_call(
            label_fun,
            expr->arguments ? expr->arguments->expression_count : (unsigned int) 0,
            retval);
    jump_to_fun->cfg_node.number = tmp_block.number;
    jump_to_fun->cfg_node.label_name = tmp_block.label_name;
    mCc_tac_program_add_quad(prog, jump_to_fun);
    global_var_count++;
    return retval;
}

static int mCc_tac_from_statement_if(struct mCc_tac_program *prog,
                                     struct mCc_ast_statement *stmt) {

    struct mCc_tac_label label_after_if = mCc_tac_get_new_label();

    struct mCc_tac_quad_entry cond =
            mCc_tac_from_expression(prog, stmt->if_cond);

    struct mCc_tac_quad *jump_after_if =
            mCc_tac_quad_new_jumpfalse(cond, label_after_if);
    jump_after_if->comment = "Evaluate if condition";

    //jump to anon block
    mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                            "", anonym_block_count, "True");

    //jump to label
    mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                            "L", label_after_if.num, "False");


    jump_after_if->cfg_node.number = anonym_block_count;

    jump_after_if->cfg_node.label_name = "";


    tmp_block.number = anonym_block_count;
    tmp_block.label_name = "";

    ++anonym_block_count;

    if (mCc_tac_program_add_quad(prog, jump_after_if))
        return 1;

    struct mCc_tac_quad *label_after_if_quad =
            mCc_tac_quad_new_label(label_after_if);

    mCc_tac_from_stmt(prog, stmt->if_stmt);

    label_after_if_quad->comment = "End of if";
    label_after_if_quad->cfg_node.number = label_after_if_quad->result.label.num;


    label_after_if_quad->cfg_node.label_name = "L";

    //if to result connection
    if (prog->quads[prog->quad_count - 1]->type != MCC_TAC_QUAD_RETURN_VOID &&
        prog->quads[prog->quad_count - 1]->type != MCC_TAC_QUAD_RETURN) {
        mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                                "L", label_after_if.num, "");
    }


    if (mCc_tac_program_add_quad(prog, label_after_if_quad))
        return 1;


    tmp_block.label_name = "L";
    tmp_block.number = label_after_if.num;

    return 0;
}

static int mCc_tac_from_statement_if_else(struct mCc_tac_program *prog,
                                          struct mCc_ast_statement *stmt) {

    struct mCc_tac_label label_else = mCc_tac_get_new_label();
    struct mCc_tac_label label_after_if = mCc_tac_get_new_label();

    // Compute condition
    struct mCc_tac_quad_entry cond =
            mCc_tac_from_expression(prog, stmt->if_cond);
    struct mCc_tac_quad *jump_to_else =
            mCc_tac_quad_new_jumpfalse(cond, label_else);
    jump_to_else->comment = "Evaluate if condition";

    jump_to_else->cfg_node.number = anonym_block_count;
    jump_to_else->cfg_node.label_name = "";

    //if connection
    mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                            "", anonym_block_count, "True");

    //else connection
    mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                            "L", label_else.num, "False");

    if (mCc_tac_program_add_quad(prog, jump_to_else))
        return 1;

    tmp_block.label_name = "";
    tmp_block.number = anonym_block_count;

    ++anonym_block_count;

    //go into if branch
    mCc_tac_from_stmt(prog, stmt->if_stmt);

    //if to result connection
    if (prog->quads[prog->quad_count - 1]->type != MCC_TAC_QUAD_RETURN_VOID &&
        prog->quads[prog->quad_count - 1]->type != MCC_TAC_QUAD_RETURN) {

        mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                                "L", label_after_if.num, "");
    }

    struct mCc_tac_quad *jump_after_if = mCc_tac_quad_new_jump(label_after_if);
    jump_after_if->comment = "Jump after if";

    jump_after_if->cfg_node.number = tmp_block.number;
    jump_after_if->cfg_node.label_name = "";

    if (mCc_tac_program_add_quad(prog, jump_after_if))
        return 1;


    struct mCc_tac_quad *label_else_quad = mCc_tac_quad_new_label(label_else);
    label_else_quad->comment = "Else branch";

    label_else_quad->cfg_node.number = label_else.num;
    label_else_quad->cfg_node.label_name = "L";

    if (mCc_tac_program_add_quad(prog, label_else_quad))
        return 1;

    tmp_block.label_name = "L";
    tmp_block.number = label_else.num;

    static int return_in_else = -1;
    //Go into else branch
    mCc_tac_from_stmt(prog, stmt->else_stmt);

    if (return_in_else == -1 &&
        (prog->quads[prog->quad_count - 1]->type != MCC_TAC_QUAD_RETURN ||
        prog->quads[prog->quad_count - 1]->type != MCC_TAC_QUAD_RETURN_VOID)){
        return_in_else = prog->quads[prog->quad_count - 1]->type;
    }

    //else to result connection
    if (return_in_else != MCC_TAC_QUAD_RETURN_VOID &&
        return_in_else != MCC_TAC_QUAD_RETURN) {

        mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                                "L", label_after_if.num, "");
    }

    struct mCc_tac_quad *label_after_if_quad =
            mCc_tac_quad_new_label(label_after_if);
    label_after_if_quad->comment = "End of if";

    label_after_if_quad->cfg_node.number = jump_after_if->result.label.num;
    label_after_if_quad->cfg_node.label_name = "L";

    if (mCc_tac_program_add_quad(prog, label_after_if_quad))
        return 1;

    tmp_block.number = label_after_if.num;

    return 0;
}

static int mCc_tac_entry_from_assg(struct mCc_tac_program *prog,
                                   struct mCc_ast_statement *stmt) {
    struct mCc_tac_quad *new_quad;
    struct mCc_tac_quad_entry result = mCc_get_var_from_id(stmt->id_assgn);

    struct mCc_tac_quad_entry result_lhs;
    struct mCc_tac_quad_entry result_rhs;
    if (stmt->lhs_assgn)
        result_lhs = mCc_tac_from_expression(prog, stmt->lhs_assgn);

    if (stmt->rhs_assgn->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
        struct mCc_tac_quad_literal *lit_result =
                mCc_get_quad_literal(stmt->rhs_assgn->literal);
        new_quad = mCc_tac_quad_new_assign_lit(lit_result, result);
        if (stmt->rhs_assgn->literal->type == MCC_AST_LITERAL_TYPE_STRING) {
            struct mCc_tac_quad_entry string = mCc_tac_create_new_string();
            mCc_tac_string_from_assgn(string, lit_result);
            lit_result->label_num = string.str_number;
        }
    }
    if (stmt->lhs_assgn) {
        result_rhs = mCc_tac_from_expression(prog, stmt->rhs_assgn);
        new_quad = mCc_tac_quad_new_store(result_lhs, result_rhs, result);
        global_var_count += 2; // lhs && rhs
    } else {
        result_rhs = mCc_tac_from_expression(prog, stmt->rhs_assgn);
        new_quad = mCc_tac_quad_new_assign(result_rhs, result);
        global_var_count++; // rhs
    }
    global_var_count++; // result
    new_quad->cfg_node.label_name = tmp_block.label_name;
    new_quad->cfg_node.number = tmp_block.number;
    if (!new_quad || mCc_tac_program_add_quad(prog, new_quad))
        return 1;
    return 0;
}

static int mCc_tac_from_statement_return(struct mCc_tac_program *prog,
                                         struct mCc_ast_statement *stmt) {
    struct mCc_tac_quad_entry entry;
    struct mCc_tac_quad *new_quad;
    if (stmt->ret_val) {
        entry = mCc_tac_from_expression(prog, stmt->ret_val);
    }
    if (stmt->type == MCC_AST_STATEMENT_TYPE_RET)
        new_quad = mCc_tac_quad_new_return(entry);
    else
        new_quad = mCc_tac_quad_new_return_void();
    new_quad->cfg_node.label_name = tmp_block.label_name;
    if (!new_quad || mCc_tac_program_add_quad(prog, new_quad))
        return 1;
    return 0;
}

static int mCc_tac_from_statement_while(struct mCc_tac_program *prog,
                                        struct mCc_ast_statement *stmt) {
    struct mCc_tac_label label_cond = mCc_tac_get_new_label();
    struct mCc_tac_label label_after_while = mCc_tac_get_new_label();
    struct mCc_tac_quad *label_cond_quad = mCc_tac_quad_new_label(label_cond);
    struct mCc_tac_quad *label_after_while_quad =
            mCc_tac_quad_new_label(label_after_while);
    label_after_while_quad->comment = "End of while";

    label_cond_quad->cfg_node.number = label_cond.num;
    label_cond_quad->cfg_node.label_name = "L";

    mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                            "L", label_cond.num, "");


    mCc_tac_program_add_quad(prog, label_cond_quad);
    struct mCc_tac_quad_entry cond =
            mCc_tac_from_expression(prog, stmt->while_cond);
    struct mCc_tac_quad *jump_after_while =
            mCc_tac_quad_new_jumpfalse(cond, label_after_while);
    jump_after_while->comment = "Evaluate while condition";
    jump_after_while->cfg_node.number = anonym_block_count;
    jump_after_while->cfg_node.label_name = "";

    if (mCc_tac_program_add_quad(prog, jump_after_while))
        return 1;

    tmp_block.label_name = "";
    tmp_block.number = anonym_block_count;

    mCc_tac_program_add_cfg(prog, "L", label_cond.num,
                            "", tmp_block.number, "True");

    ++anonym_block_count;
    mCc_tac_from_stmt(prog, stmt->while_stmt);

    if (prog->quads[prog->quad_count - 1]->type != MCC_TAC_QUAD_RETURN_VOID &&
        prog->quads[prog->quad_count - 1]->type != MCC_TAC_QUAD_RETURN) {

        mCc_tac_program_add_cfg(prog, tmp_block.label_name, tmp_block.number,
                                "L", label_cond.num, "");
    }
    struct mCc_tac_quad *jump_to_cond = mCc_tac_quad_new_jump(label_cond);
    jump_to_cond->comment = "Repeat Loop";
    jump_to_cond->cfg_node.number = anonym_block_count;
    jump_to_cond->cfg_node.label_name = "";

    if (mCc_tac_program_add_quad(prog, jump_to_cond))
        return 1;

    label_after_while_quad->cfg_node.number = label_after_while.num;
    label_after_while_quad->cfg_node.label_name = "L";
    if (mCc_tac_program_add_quad(prog, label_after_while_quad))
        return 1;

    mCc_tac_program_add_cfg(prog, "L", label_cond.num,
                            "L", label_after_while.num, "False");

    tmp_block.label_name = "L";
    tmp_block.number = label_after_while.num;

    return 0;
}

static int mCc_tac_from_function_def(struct mCc_tac_program *prog,
                                     struct mCc_ast_function_def *fun_def) {
    global_var_count = 0;
    struct mCc_tac_label label_fun =
            mCc_get_label_from_fun_name(fun_def->identifier);

    struct mCc_tac_quad *label_fun_quad = mCc_tac_quad_new_label(label_fun);

    tmp_block.label_name = label_fun.str;
    tmp_block.number = anonym_block_count;

    mCc_tac_program_add_cfg(prog, label_fun.str, 0,
                            "", anonym_block_count, "");

    ++anonym_block_count;

    tmp_block.label_name = "";
    label_fun_quad->cfg_node.label_name = tmp_block.label_name;
    label_fun_quad->cfg_node.number = tmp_block.number;

    if (mCc_tac_program_add_quad(prog, label_fun_quad)) {
        return 1;
    };
    // Copy arguments to new temporaries
    struct mCc_tac_quad_entry virtual_pointer_to_arguments = {.number = -1};
    if (fun_def->para) {
        for (unsigned int i = 0; i < fun_def->para->decl_count; ++i) {
            // Load argument index into a quad
            struct mCc_tac_quad_literal *lit = malloc(sizeof(*lit));
            lit->type =
                    mCc_tac_type_from_ast_type(fun_def->para->decl[i]->decl_type);

            lit->ival = i; // For the correct stack ptr in tac

            struct mCc_tac_quad_entry entry = mCc_tac_create_new_entry();
            struct mCc_tac_quad *quad = mCc_tac_quad_new_assign_lit(lit, entry);
            global_var_count++;
            quad->cfg_node.label_name = tmp_block.label_name;
            if (mCc_tac_program_add_quad(prog, quad))
                return 1;

            // Load argument from stack into new temporary
            struct mCc_tac_quad_entry new_entry = mCc_tac_create_new_entry();
            entry.type = lit->type;
            new_entry.array_size = 0;
            struct mCc_tac_quad *load_param = mCc_tac_quad_new_load(
                    virtual_pointer_to_arguments, entry, new_entry);

            load_param->comment = "load param from stack to temporary";
            if (mCc_tac_program_add_quad(prog, load_param))
                return 1;

            fun_def->para->decl[i]->decl_id->symtab_ref->tac_tmp = new_entry;
        }
    }
    // if there is an empty body for a void func
    // add a return stmt at the end
    if (!fun_def->body && fun_def->func_type == MCC_AST_TYPE_VOID)
        fun_def->body =
                mCc_ast_new_statement_compound(mCc_ast_new_statement_return(NULL));

    if (fun_def->body) {
        tmp_block.label_name = "";
        if (fun_def->func_type == MCC_AST_TYPE_VOID &&
            fun_def->body
                    ->compound_stmts[fun_def->body->compound_stmt_count - 1]
                    ->type != MCC_AST_STATEMENT_TYPE_RET_VOID) {
            fun_def->body = mCc_ast_compound_statement_add(
                    fun_def->body, mCc_ast_new_statement_return(NULL));
        }
        if (mCc_tac_from_stmt(prog, fun_def->body)) {
            return 1;
        }
    }
    label_fun_quad->var_count = global_var_count;
    /* fprintf(stderr, "global_var_count: %s %d\n",
     * fun_def->identifier->id_value, global_var_count); */
    return 0;
}

struct mCc_tac_quad_literal *
mCc_get_quad_literal(struct mCc_ast_literal *literal) {
    struct mCc_tac_quad_literal *lit_quad = malloc(sizeof(*lit_quad));
    if (!lit_quad)
        return NULL;

    switch (literal->type) {
        case MCC_AST_LITERAL_TYPE_INT:
            lit_quad->type = MCC_TAC_QUAD_LIT_INT;
            lit_quad->ival = literal->i_value;
            break;
        case MCC_AST_LITERAL_TYPE_FLOAT:
            lit_quad->type = MCC_TAC_QUAD_LIT_FLOAT;
            lit_quad->fval = literal->f_value;
            break;
        case MCC_AST_LITERAL_TYPE_BOOL:
            lit_quad->type = MCC_TAC_QUAD_LIT_BOOL;
            lit_quad->bval = literal->b_value;
            break;
        case MCC_AST_LITERAL_TYPE_STRING:
            lit_quad->type = MCC_TAC_QUAD_LIT_STR;
            lit_quad->strval = literal->s_value;
            break;
    }
    return lit_quad;
}

static int mCc_tac_from_stmt(struct mCc_tac_program *prog,
                             struct mCc_ast_statement *stmt) {

    switch (stmt->type) {
        case MCC_AST_STATEMENT_TYPE_IF:
            return mCc_tac_from_statement_if(prog, stmt);
        case MCC_AST_STATEMENT_TYPE_IFELSE:
            return mCc_tac_from_statement_if_else(prog, stmt);
        case MCC_AST_STATEMENT_TYPE_RET:
            return mCc_tac_from_statement_return(prog, stmt);
        case MCC_AST_STATEMENT_TYPE_RET_VOID:
            return mCc_tac_from_statement_return(prog, stmt);
        case MCC_AST_STATEMENT_TYPE_WHILE:
            return mCc_tac_from_statement_while(prog, stmt);
        case MCC_AST_STATEMENT_TYPE_DECL:
            mCc_tac_entry_from_declaration(stmt->declaration);
            return 0;
        case MCC_AST_STATEMENT_TYPE_ASSGN:
            return mCc_tac_entry_from_assg(prog, stmt);
        case MCC_AST_STATEMENT_TYPE_EXPR:
            mCc_tac_from_expression(prog, stmt->expression);
            return 0;
        case MCC_AST_STATEMENT_TYPE_CMPND:
            for (unsigned int i = 0; i < (stmt->compound_stmt_count); i++) {
                if (mCc_tac_from_stmt(prog, stmt->compound_stmts[i])) {
                    return 1;
                }
            }
            return 0;
    }
    return 0;
}

static struct mCc_tac_quad_entry
mCc_tac_from_expression(struct mCc_tac_program *prog,
                        struct mCc_ast_expression *exp) {
    assert(prog);
    assert(exp);
    struct mCc_tac_quad_entry entry;
    // entry.type = mCc_tac_type_from_ast_type(exp->node.computed_type);
    entry.array_size = 0;

    switch (exp->type) {
        case MCC_AST_EXPRESSION_TYPE_LITERAL:
            if (exp->literal->type != MCC_AST_LITERAL_TYPE_STRING) {
                struct mCc_tac_quad_literal *lit =
                        mCc_get_quad_literal(exp->literal);
                entry = mCc_tac_create_new_entry();
                struct mCc_tac_quad *lit_quad =
                        mCc_tac_quad_new_assign_lit(lit, entry);
                global_var_count++;
                lit_quad->cfg_node.number = tmp_block.number;
                lit_quad->cfg_node.label_name = tmp_block.label_name;
                mCc_tac_program_add_quad(prog, lit_quad);
            } else {
                struct mCc_tac_quad_literal *lit =
                        mCc_get_quad_literal(exp->literal);
                entry = mCc_tac_create_new_string();
                mCc_tac_string_from_assgn(entry, lit);
                lit->label_num = entry.str_number;
                struct mCc_tac_quad *lit_quad =
                        mCc_tac_quad_new_assign_lit(lit, entry);
                global_var_count++;
                lit_quad->cfg_node.number = tmp_block.number;
                lit_quad->cfg_node.label_name = tmp_block.label_name;
                mCc_tac_program_add_quad(prog, lit_quad);
            }
            break;
        case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
            entry = exp->identifier->symtab_ref->tac_tmp;
            break;
        case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
            entry = mCc_tac_from_expression_unary(prog, exp);
            break;
        case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
            entry = mCc_tac_from_expression_binary(prog, exp);
            break;
        case MCC_AST_EXPRESSION_TYPE_PARENTH:
            entry = mCc_tac_from_expression(prog, exp->expression);
            break;
        case MCC_AST_EXPRESSION_TYPE_CALL_EXPR:
            entry = mCc_tac_from_expression_call(prog, exp);
            break;
        case MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR:
            entry = mCc_tac_from_expression_arr_subscr(prog, exp);
            break;
    }
    return entry;
}

struct mCc_tac_program *mCc_tac_build(struct mCc_ast_program *prog) {
    struct mCc_tac_program *tac = mCc_tac_program_new(42);
    tac = mCc_tac_new_cfg(tac, 10);

    if (!tac)
        return NULL;

    for (unsigned int i = 0; i < prog->func_def_count; ++i) {
        if (mCc_tac_from_function_def(tac, prog->func_defs[i])) {
            mCc_tac_program_delete(tac);
            return NULL;
        }
    }

    tac->string_literals = global_string_arr;
    tac->string_literal_count = global_string_count;
    return tac;
}

void mCc_tac_free_global_string_array() {
    free(global_string_arr);
}
