#include <gtest/gtest.h>

#include "mCc/typecheck.h"

TEST(TYPE_CHECK, ASSIGNMENT)
{
  //  struct mCc_symtab_scope *root = mCc_symtab_new_scope_in(NULL, "");

   // struct mCc_ast_identifier *id = mCc_ast_new_identifier((char *)"foo");
  //  struct mCc_ast_identifier *func_id = mCc_ast_new_identifier((char *)"func");

    enum mCc_ast_unary_op op = MCC_AST_UNARY_OP_NEG;

    struct mCc_ast_literal *number = mCc_ast_new_literal_int(3);
    struct mCc_ast_expression *unary = mCc_ast_new_expression_literal(number);

    //struct mCc_ast_statement *assgn = mCc_ast_new_statement_assgn(id, NULL, rhs);

    //struct mCc_ast_function_def *func = mCc_ast_new_function_def_void(func_id, NULL, assgn);

   // struct mCc_ast_program *prog = mCc_ast_new_program(func);

    ASSERT_EQ(MCC_AST_TYPE_INT, test_type_check(unary));

    mCc_symtab_delete_all_scopes();
}
