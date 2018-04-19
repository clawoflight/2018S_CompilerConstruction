#include <gtest/gtest.h>

#include "mCc/symtab.h"
#include "mCc/parser.h"
#include "mCc/ast_symtab_link.h"

TEST(TDD_PARSER_SYMTABLINK, TEST)
{

    const char str[] = "int main(){ return;}";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    mCc_ast_symtab_build(prog);
}