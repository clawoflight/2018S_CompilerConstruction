#include <gtest/gtest.h>

#include "mCc/typecheck.h"
#include "mCc/parser.h"

TEST(TYPE_CHECK, LITERAL)
{
    struct mCc_ast_literal *lit = mCc_ast_new_literal_int(3);
    struct mCc_ast_expression *expr = mCc_ast_new_expression_literal(lit);

    ASSERT_EQ(MCC_AST_TYPE_INT, test_type_check(expr));
}

TEST(TYPE_CHECK, IDENTIFIER)
{
    struct mCc_symtab_scope *scope = mCc_symtab_new_scope_in(NULL, "");
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

TEST(TYPE_CHECK_UNARY, UNARY_NEG_FLOAT)
{
    const char input[] = "-3.1415";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_FLOAT, test_type_check(expr));

    mCc_ast_delete_expression(expr);
}

TEST(TYPE_CHECK_UNARY, UNARY_NEG_STRING)
{
    const char input[] = "-\"foo\"";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_VOID, test_type_check(expr));

    mCc_ast_delete_expression(expr);
}

TEST(TYPE_CHECK_UNARY, UNARY_NOT_BOOL)
{
    const char input[] = "!true";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_BOOL, test_type_check(expr));

    mCc_ast_delete_expression(expr);
}

TEST(TYPE_CHECK_UNARY, UNARY_NOT_INT)
{
    const char input[] = "!3";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_VOID, test_type_check(expr));
    mCc_ast_delete_expression(expr);
}

TEST(TYPE_CHECK_BINARY, NOT_MATCHING_SIDES)
{
    const char input[] = "3-true";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_VOID, test_type_check(expr));
}


TEST(TYPE_CHECK_BINARY, BINARY_SUB_INT)
{
    const char input[] = "3-2";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_INT, test_type_check(expr));
}

TEST(TYPE_CHECK_BINARY, BINARY_ADD_STRING)
{
    const char input[] = "\"foo\"+\"bar\"";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_VOID, test_type_check(expr));
}

TEST(TYPE_CHECK_BINARY, BINARY_LT_FLOAT)
{
    const char input[] = "3.14<3.15";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_BOOL, test_type_check(expr));
}

TEST(TYPE_CHECK_BINARY, BINARY_GEQ_BOOL)
{
    const char input[] = "true>=false";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_VOID, test_type_check(expr));
}


TEST(TYPE_CHECK_BINARY, BINARY_AND_BOOL)
{
    const char input[] = "true&&true";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_BOOL, test_type_check(expr));
}


TEST(TYPE_CHECK_BINARY, BINARY_OR_INT)
{
    const char input[] = "1||2";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_VOID, test_type_check(expr));
}

TEST(TYPE_CHECK_BINARY, BINARY_EQ_STRING)
{
    const char input[] = "\"foo\"==\"foo\"";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_BOOL, test_type_check(expr));
}


TEST(TYPE_CHECK_BINARY, BINARY_NEQ_BOOL)
{
    const char input[] = "true!=false";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_BOOL, test_type_check(expr));
}

TEST(TYPE_CHECK_PARENTH, PARENTH)
{
    const char input[] = "(192)";
    auto result = mCc_parser_parse_string(input);
    auto expr = result.expression;

    ASSERT_EQ(MCC_AST_TYPE_INT, test_type_check(expr));
}