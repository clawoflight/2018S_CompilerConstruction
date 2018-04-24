#include <gtest/gtest.h>

#include "mCc/symtab.h"
#include "mCc/parser.h"
#include "mCc/ast_symtab_link.h"

TEST(TDD_PARSER_SYMTABLINK, TEST)
{

    const char str[] = "int main(){ int b; b=2;  }";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    mCc_ast_symtab_build(prog);
    mCc_ast_identifier *id= mCc_ast_new_identifier((char*)"b");

    ASSERT_EQ(0,strcmp(id->id_value,prog->func_defs[0]->body->compound_stmts[1]->id_assgn->symtab_ref->identifier->id_value));
    mCc_ast_delete_program(prog);
    mCc_symtab_delete_all_scopes();
}

TEST(TDD_PARSER_SYMTABLINK, TEST_ARR)
{

    const char str[] = "int main(){ int[2] b; b[0]=2; b[1]=1; }";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    mCc_ast_symtab_build(prog);
    mCc_ast_identifier *id= mCc_ast_new_identifier((char*)"b");

    ASSERT_STREQ(id->id_value,prog->func_defs[0]->body->compound_stmts[1]->id_assgn->symtab_ref->identifier->id_value);
    ASSERT_STREQ(id->id_value,prog->func_defs[0]->body->compound_stmts[2]->id_assgn->symtab_ref->identifier->id_value);
    ASSERT_EQ(MCC_SYMTAB_ENTRY_TYPE_ARR,prog->func_defs[0]->body->compound_stmts[2]->id_assgn->symtab_ref->entry_type);
    mCc_ast_delete_program(prog);
    mCc_symtab_delete_all_scopes();
}

TEST(TDD_PARSER_SYMTABLINK, TEST_FUNC)
{

    const char str[] = "void f(){ int a; a=2; return;  }  void main(){ int b; b=2; f(); return;  }";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    mCc_ast_symtab_build(prog);
    mCc_ast_identifier *ida= mCc_ast_new_identifier((char*)"a");
    mCc_ast_identifier *idb= mCc_ast_new_identifier((char*)"b");
    mCc_ast_identifier *idf= mCc_ast_new_identifier((char*)"f");

    ASSERT_STREQ(ida->id_value,prog->func_defs[0]->body->compound_stmts[1]->id_assgn->symtab_ref->identifier->id_value);
    ASSERT_STREQ(idb->id_value,prog->func_defs[1]->body->compound_stmts[1]->id_assgn->symtab_ref->identifier->id_value);
    ASSERT_STREQ(idf->id_value,prog->func_defs[1]->body->compound_stmts[2]->expression->f_name->symtab_ref->identifier->id_value);

    mCc_ast_delete_program(prog);
    mCc_symtab_delete_all_scopes();
}

TEST(TDD_PARSER_SYMTABLINK, TEST_FUNC_PARA)
{

const char str[] = "void f(int b){ b=2; return;  }  void main(){ int b; b=2; f(b); return;  }";
auto result = mCc_parser_parse_string(str);

ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
auto prog = result.program;

mCc_ast_symtab_build(prog);
mCc_ast_identifier *idb= mCc_ast_new_identifier((char*)"b");
mCc_ast_identifier *idf= mCc_ast_new_identifier((char*)"f");

ASSERT_STREQ(idb->id_value,prog->func_defs[1]->body->compound_stmts[1]->id_assgn->symtab_ref->identifier->id_value);
ASSERT_STREQ(idb->id_value,prog->func_defs[0]->body->compound_stmts[0]->id_assgn->symtab_ref->identifier->id_value);
ASSERT_STREQ(idf->id_value,prog->func_defs[1]->body->compound_stmts[2]->expression->f_name->symtab_ref->identifier->id_value);

mCc_ast_delete_program(prog);
mCc_symtab_delete_all_scopes();
}

TEST(TDD_PARSER_SYMTABLINK, TEST_FUNC_PARA2)
{

const char str[] = "int f(){ int b; b=2; return b;  }  void main(){ int b; b=2; int r; r=f(b); return;  }";
auto result = mCc_parser_parse_string(str);

ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
auto prog = result.program;

mCc_ast_symtab_build(prog);
mCc_ast_identifier *idb= mCc_ast_new_identifier((char*)"b");
mCc_ast_identifier *idr= mCc_ast_new_identifier((char*)"r");
mCc_ast_identifier *idf= mCc_ast_new_identifier((char*)"f");

ASSERT_STREQ(idb->id_value,prog->func_defs[1]->body->compound_stmts[1]->id_assgn->symtab_ref->identifier->id_value);
ASSERT_STREQ(idb->id_value,prog->func_defs[0]->body->compound_stmts[1]->id_assgn->symtab_ref->identifier->id_value);
ASSERT_STREQ(idr->id_value,prog->func_defs[1]->body->compound_stmts[3]->id_assgn->symtab_ref->identifier->id_value);
ASSERT_STREQ(idf->id_value,prog->func_defs[1]->body->compound_stmts[3]->rhs_assgn->f_name->symtab_ref->identifier->id_value);

mCc_ast_delete_program(prog);
mCc_symtab_delete_all_scopes();
}