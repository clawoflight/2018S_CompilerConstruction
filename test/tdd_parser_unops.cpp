#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(TDD_PARSER_UNOPS, NEG)
{
	const char str[] = "-(5)";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_UNARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_UNARY_OP_NEG, expr->unary_op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_UNOPS, NOT)
{
	const char str[] = "!true";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_UNARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_UNARY_OP_NOT, expr->unary_op);

	mCc_ast_delete_expression(expr);
}
