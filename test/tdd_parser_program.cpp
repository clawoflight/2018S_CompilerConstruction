#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(TDD_PARSER_PROGRAM, PROGRAM)
{
    const char str[] = "void f(int a){ return a;}";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    // root -> program
    ASSERT_EQ((unsigned int) 1, prog->func_def_count);

    mCc_ast_delete_program(prog);
}

TEST(TDD_PARSER_PROGRAM, PROGRAMS)
{
    const char str[] = "int f(){ return a;} void g(int a){ bool a;}";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    // root -> program
    ASSERT_EQ((unsigned int) 2, prog->func_def_count);

    mCc_ast_delete_program(prog);
}

TEST(TDD_PARSER_PROGRAM, PROGRAM_VOID)
{
const char str[] = "void f(int a){ bool a;}";
auto result = mCc_parser_parse_string(str);

ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
auto prog = result.program;

// root -> function
ASSERT_EQ(MCC_AST_FUNCTION_DEF_VOID, prog->func_defs[0]->type);
ASSERT_EQ(0, strcmp(prog->func_defs[0]->identifier->id_value,"f"));

mCc_ast_delete_program(prog);
}


TEST(TDD_PARSER_PROGRAM, FUNC_INT)
{
    const char str[] = "int f(int a){ return a;}";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    // root -> function
    ASSERT_EQ(MCC_AST_FUNCTION_DEF_TYPE, prog->func_defs[0]->type);
    ASSERT_EQ(MCC_AST_TYPE_INT,prog->func_defs[0]->func_type);
    ASSERT_EQ(0, strcmp(prog->func_defs[0]->identifier->id_value,"f"));

    mCc_ast_delete_program(prog);
}

TEST(TDD_PARSER_PROGRAM, FUNC_VOID)
{
    const char str[] = "void f(int a){ bool a;}";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    // root -> function
    ASSERT_EQ(MCC_AST_FUNCTION_DEF_VOID, prog->func_defs[0]->type);
    ASSERT_EQ(0, strcmp(prog->func_defs[0]->identifier->id_value,"f"));

    mCc_ast_delete_program(prog);
}

TEST(TDD_PARSER_PROGRAM, PARAMETERS)
{
    const char str[] = "int f(int a, bool b){ return a;}";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    // root -> literal
    ASSERT_EQ((unsigned int) 2, prog->func_defs[0]->para->decl_count);
    ASSERT_EQ(0, strcmp(prog->func_defs[0]->para->decl[0]->decl_id->id_value,"a"));
    ASSERT_EQ(0, strcmp(prog->func_defs[0]->para->decl[1]->decl_id->id_value,"b"));

    mCc_ast_delete_program(prog);
}

TEST(TDD_PARSER_PROGRAM, PARAMETER_VOID)
{
    const char str[] = "int f(){ return a;}";
    auto result = mCc_parser_parse_string(str);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
    auto prog = result.program;

    // root -> literal
    ASSERT_EQ(NULL, prog->func_defs[0]->para);

    mCc_ast_delete_program(prog);
}
