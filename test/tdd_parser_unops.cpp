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

TEST(TDD_PARSER_UNOPS_PREC, NEG_PREG)
{
	const char str[] = "-2 + 3";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, expr->op);

	// next left
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_UNARY_OP, expr->lhs->type);
	ASSERT_EQ(MCC_AST_UNARY_OP_NEG, expr->lhs->unary_op);

	// next right
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->rhs->type);
	ASSERT_EQ(3, expr->rhs->literal->i_value);

	// sub expression (-2)
	auto subexpr = expr->lhs->unary_expression;
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, subexpr->literal->type);
	ASSERT_EQ(2, subexpr->literal->i_value);

	mCc_ast_delete_expression(expr);
}
