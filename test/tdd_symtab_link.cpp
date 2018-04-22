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
}