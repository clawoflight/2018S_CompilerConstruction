#include <gtest/gtest.h>

#include "mCc/typecheck.h"

TEST(TYPE_CHECK, LITERAL)
{
    struct mCc_ast_literal *lit = mCc_ast_new_literal_int(3);
    struct mCc_ast_expression *expr = mCc_ast_new_expression_literal(lit);

    ASSERT_EQ(MCC_AST_TYPE_INT, test_type_check(expr));

    mCc_symtab_delete_all_scopes();
}

TEST(TYPE_CHECK, IDENTIFIER)
{
    struct mCc_symtab_scope *scope = mCc_symtab_new_scope_in(NULL, "");
    ASSERT_NE((void *)NULL, scope);

    struct mCc_ast_identifier *id = mCc_ast_new_identifier((char *)"foo");

    struct mCc_ast_declaration *decl = mCc_ast_new_declaration(MCC_AST_TYPE_STRING,
                                                               NULL, id);

    mCc_symtab_scope_add_decl(scope, decl);

    struct mCc_symtab_entry *found =
        mCc_symtab_scope_lookup_id(scope, decl->decl_id);

    struct mCc_ast_expression *expr = mCc_ast_new_expression_identifier(id);

    expr->identifier->symtab_ref = found;

    ASSERT_EQ(MCC_AST_TYPE_STRING, test_type_check(expr));

    mCc_symtab_delete_all_scopes();
}