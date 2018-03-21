#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(Parser, BinaryOp_1)
{
	const char input[] = "192 + 3.14";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, expr->op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	ASSERT_EQ(192, expr->lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->rhs->type);

	// root -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, expr->rhs->literal->type);
	ASSERT_EQ(3.14, expr->rhs->literal->f_value);

	mCc_ast_delete_expression(expr);
}

TEST(Parser, StringLiteral_1)
{
	const char input[] = "\"3_n4f$f\"";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->type);

	// root -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_STRING, expr->literal->type);
	ASSERT_EQ(0, strcmp(expr->literal->s_value,"\"3_n4f$f\""));

	mCc_ast_delete_expression(expr);
}

TEST(Parser, StringLiteral_2)
{
	const char input[] = "\"f\"";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->type);

	// root -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_STRING, expr->literal->type);
	ASSERT_EQ(1, strcmp(expr->literal->s_value,"\"3f\""));

	mCc_ast_delete_expression(expr);
}

TEST(Parser, StringLiteral_Empty)
{
	const char input[] = "\"\"";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->type);

	// root -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_STRING, expr->literal->type);
	ASSERT_EQ(0, strcmp(expr->literal->s_value,"\"\""));

	mCc_ast_delete_expression(expr);
}

TEST(Parser, BoolLiteral_1)
{
	const char input[] = "true";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->type);

	// root -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_BOOL, expr->literal->type);
	ASSERT_EQ(true, expr->literal->b_value);

	mCc_ast_delete_expression(expr);
}

TEST(Parser, BoolLiteral_2)
{
	const char input[] = "false";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->type);

	// root -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_BOOL, expr->literal->type);
	ASSERT_EQ(false, expr->literal->b_value);

	mCc_ast_delete_expression(expr);
}

TEST(Parser, NestedExpression_1)
{
	const char input[] = "42 * (-192 + 3.14)";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_MUL, expr->op);

	// root -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	ASSERT_EQ(42, expr->lhs->literal->i_value);

	// root -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_PARENTH, expr->rhs->type);

	auto subexpr = expr->rhs->expression;

	// subexpr
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, subexpr->type);
	ASSERT_EQ(MCC_AST_BINARY_OP_ADD, subexpr->op);

	// subexpr -> lhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_UNARY_OP, subexpr->lhs->type);
	ASSERT_EQ(MCC_AST_UNARY_OP_NEG, subexpr->lhs->unary_op);

	// subexpr -> rhs
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->rhs->type);

	// subexpr -> rhs -> literal
	ASSERT_EQ(MCC_AST_LITERAL_TYPE_FLOAT, subexpr->rhs->literal->type);
	ASSERT_EQ(3.14, subexpr->rhs->literal->f_value);

    auto subsubexpr = subexpr->lhs->unary_expression;
    // subsubexpr -> literal
    ASSERT_EQ(MCC_AST_LITERAL_TYPE_INT, subsubexpr->literal->type);
    ASSERT_EQ(192, subsubexpr->literal->i_value);

	mCc_ast_delete_expression(expr);
}

TEST(Parser, MissingClosingParenthesis_1)
{
	const char input[] = "(42";
	auto result = mCc_parser_parse_string(input);

	ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
}

TEST(Parser, ArraySubscript)
{
	const char input[] = "a[3+4]";
	auto result = mCc_parser_parse_string(input);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);

	auto expr = result.expression;

	// root
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR, expr->type);
	ASSERT_EQ(0, strcmp("a",expr->array_id->id_value) );
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->subscript_expr->type);

	mCc_ast_delete_expression(expr);
}

TEST(Parser, LiteralDestructor)
{
const char input[] = "(\"weer\"";
auto result = mCc_parser_parse_string(input);

ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
}