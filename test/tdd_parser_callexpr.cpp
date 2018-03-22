#include <gtest/gtest.h>

#include "mCc/ast.h"
#include "mCc/parser.h"

TEST(TDD_PARSER_CALL_EXPR, EXPRESSION) {
  const char str[] = "foo(1,2)";
  auto result = mCc_parser_parse_string(str);

  ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
  auto call = result.expression;

  // root
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_CALL_EXPR, call->type);

  ASSERT_EQ(0, strcmp("foo", call->f_name->id_value));

  mCc_ast_delete_expression(call);
}

TEST(TDD_PARSER_CALL_EXPR, ARGUMENT_EMPTY) {
  const char str[] = "foo()";
  auto result = mCc_parser_parse_string(str);

  ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
  auto call = result.expression;

  // ROOT
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_CALL_EXPR, call->type);
  ASSERT_EQ(0, strcmp("foo", call->f_name->id_value));
  ASSERT_EQ(NULL, call->arguments);

  mCc_ast_delete_expression(call);
}

TEST(TDD_PARSER_CALL_EXPR, ARGUMENT_MULTIPLE) {
  const char str[] = "foo(3+5,1)";
  auto result = mCc_parser_parse_string(str);

  ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
  auto call = result.expression;

  // ROOT
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_CALL_EXPR, call->type);
  ASSERT_EQ((unsigned int)2, call->arguments->expression_count);

  // sub 1
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP,
            call->arguments->expressions[0]->type);

  // sub 2
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
            call->arguments->expressions[1]->type);

  mCc_ast_delete_expression(call);
}

TEST(TDD_PARSER_CALL_EXPR, ARGUMENT_REALLOC) {
  const char str[] = "foo(1,2,3,4,5,6,7,8,9,3-2,42)";
  auto result = mCc_parser_parse_string(str);

  ASSERT_EQ(MCC_PARSER_STATUS_OK, result.status);
  auto call = result.expression;

  // ROOT
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_CALL_EXPR, call->type);
  ASSERT_EQ((unsigned int)11, call->arguments->expression_count);

  // sub 1
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
            call->arguments->expressions[0]->type);

  // sub 10
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_BINARY_OP,
            call->arguments->expressions[9]->type);

  // sub 11
  ASSERT_EQ(MCC_AST_EXPRESSION_TYPE_LITERAL,
            call->arguments->expressions[10]->type);

  mCc_ast_delete_expression(call);
}

TEST(TDD_PARSER_ASSGN, ARG_ID_DESTRUCTOR)
{
  const char str[] = "f(1,2,)";
  auto result = mCc_parser_parse_string(str);

  ASSERT_NE(MCC_PARSER_STATUS_OK, result.status);
}
