#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(TDD_PARSER_ASSGN, LITERAL)
{
	const char str[] = "a = 5;";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto assgn = result.statement;

	// root
	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_ASSGN, assgn->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, assgn->rhs_assgn->type);

	mCc_ast_delete_statement(assgn);
}

TEST(TDD_PARSER_ASSGN, EXPR)
{
	const char str[] = "foo = 3+4;";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto assgn = result.statement;

	// root
	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_ASSGN, assgn->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, assgn->rhs_assgn->type);

	mCc_ast_delete_statement(assgn);
}

TEST(TDD_PARSER_ASSGN, ARRAY)
{
	const char str[] = "a[4] = 5;";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto assgn = result.statement;

	// root
	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_ASSGN, assgn->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL, assgn->lhs_assgn->type);

	mCc_ast_delete_statement(assgn);
}

TEST(TDD_PARSER_ASSGN, ARRAY_EXPR)
{
	const char str[] = "a[4+5] = 5;";
	auto result = mCc_parser_parse_string(str);

	ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
	auto assgn = result.statement;

	// root
	ASSERT_EQ(MCC_AST_STATEMENT_TYPE_ASSGN, assgn->type);
	ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP, assgn->lhs_assgn->type);

	mCc_ast_delete_statement(assgn);
}

TEST(TDD_PARSER_ASSGN, DECL_DESTRUCTOR)
{
	const char str[] = "int a[4] = 5;";
	auto result = mCc_parser_parse_string(str);
	free((void *)result.err_msg);
	free((void *)result.err_text);
	ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
}

TEST(TDD_PARSER_ASSGN, LIT_DESTRUCTOR)
{
	const char str[] = "int a[4";
	auto result = mCc_parser_parse_string(str);
	free((void *)result.err_msg);
	free((void *)result.err_text);
	ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
}
