#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(Parser, Identifier_1)
{
    const char input[] = "e";
    auto result = mCc_parser_parse_string(input);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    auto expr = result.expression;

    // root
    ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_IDENTIFIER, expr->type);

    // root -> literal
    ASSERT_EQ(0, strcmp(expr->identifier->id_value,"e"));

    mCc_ast_delete_expression(expr);
}

TEST(Parser, Identifier_2)
{
    const char input[] = "4r";
    auto result = mCc_parser_parse_string(input);

    ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);

    mCc_ast_delete_expression(result.expression);
}

TEST(Parser, Identifier_3)
{
    const char input[] = "en43r";
    auto result = mCc_parser_parse_string(input);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    auto expr = result.expression;

    // root
    ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_IDENTIFIER, expr->type);

    // root -> literal
    ASSERT_EQ(0, strcmp(expr->identifier->id_value,"en43r"));

    mCc_ast_delete_expression(expr);
}

TEST(Parser, Identifier_4)
{
    const char input[] = "e4_r";
    auto result = mCc_parser_parse_string(input);

    ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

    auto expr = result.expression;

    // root
    ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_IDENTIFIER, expr->type);

    // root -> literal
    ASSERT_EQ(0, strcmp(expr->identifier->id_value,"e4_r"));

    mCc_ast_delete_expression(expr);
}
