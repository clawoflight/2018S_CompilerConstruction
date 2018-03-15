#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(TDD_PARSER_BINOPS, LT)
{
	const char str[] = "true < false";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_LT, expr->op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_BINOPS, GT)
{
	const char str[] = "true > false";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_GT, expr->op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_BINOPS, LEQ)
{
	const char str[] = "true <= false";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_LEQ, expr->op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_BINOPS, GEQ)
{
	const char str[] = "true >= false";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_GEQ, expr->op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_BINOPS, AND)
{
	const char str[] = "true && false";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_AND, expr->op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_BINOPS, OR)
{
	const char str[] = "true || false";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_OR, expr->op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_BINOPS, EQ)
{
	const char str[] = "true == false";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_EQ, expr->op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_BINOPS, NEQ)
{
	const char str[] = "true != false";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_NEQ, expr->op);

	mCc_ast_delete_expression(expr);
}

TEST(TDD_PARSER_BINOPS_PREC,BIN_PREC)
{
	const char str[] = "3 * 5 + 2";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, expr->op);

	//next left
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->lhs->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_MUL, expr->lhs->op);

	//next right
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->rhs->type);
	ASSERT_EQ(2, expr->rhs->literal->i_value);

	auto subexpr = expr->lhs;

	//subexp left
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, subexpr->lhs->type);
	ASSERT_EQ(3, subexpr->lhs->literal->i_value);

	//subexp right
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, subexpr->rhs->type);
	ASSERT_EQ(5, subexpr->rhs->literal->i_value);

	mCc_ast_delete_expression(expr);

}