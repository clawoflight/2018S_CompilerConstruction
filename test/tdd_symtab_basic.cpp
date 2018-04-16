#include <gtest/gtest.h>

#include "mCc/symtab.h"

TEST(SYMTAB_BASIC, SCOPE_NAMING)
{
	struct mCc_symtab_scope *root = mCc_symtab_new_scope_in(NULL, "root");
	ASSERT_STREQ("root", root->name);
	struct mCc_symtab_scope *child = mCc_symtab_new_scope_in(root, "child");
	ASSERT_STREQ("root_child", child->name);

	mCc_symtab_delete_all_scopes();
}

TEST(SYMTAB_BASIC, INSERT_LOOKUP_ENTRY)
{
	struct mCc_symtab_scope *scope = mCc_symtab_new_scope_in(NULL, "");
	ASSERT_NE((void*)NULL, scope);

	struct mCc_ast_identifier id;
	id.id_value = (char *) "id";

	struct mCc_ast_declaration decl;
	decl.decl_type = MCC_AST_TYPE_INT;
	decl.decl_id = &id;
	decl.decl_array_size = NULL;

	mCc_symtab_scope_add_decl(scope, &decl);

	struct mCc_symtab_entry *found = mCc_symtab_scope_lookup_id(scope, &id);
	ASSERT_NE((void *)NULL, found);
	ASSERT_STREQ(id.id_value, found->identifier->id_value);

	mCc_symtab_delete_all_scopes();
}

TEST(SYMTAB_FUNC, INSERT_LOOKUP_FUNC_NAME)
{
    struct mCc_symtab_scope *scope = mCc_symtab_new_scope_in(NULL, "");
    ASSERT_NE((void*)NULL, scope);

    struct mCc_ast_identifier id;
    id.id_value = (char *) "main";

    struct mCc_ast_function_def func ;
    func.func_type = MCC_AST_TYPE_INT;

    func.identifier = &id;
    func.body = NULL;
    func.para = NULL;

    struct mCc_ast_declaration decl;
    decl.decl_type = MCC_AST_TYPE_VOID;
    decl.decl_id = func.identifier;

    mCc_symtab_scope_add_decl(scope, &decl);

    struct mCc_symtab_entry *found = mCc_symtab_scope_lookup_id(scope, func.identifier);
    ASSERT_NE((void *)NULL, found);
    ASSERT_STREQ(func.identifier->id_value, found->identifier->id_value);

    mCc_symtab_delete_all_scopes();
}