/**
 * @file mC_to_dot.c
 * @brief Program that parses code and outputs the AST in DOT format.
 * @author warhawk
 * @date 2018-03-08
 */
#include <stdio.h>
#include <stdlib.h>

#include "mCc/ast.h"
#include "mCc/ast_print.h"
#include "mCc/parser.h"

int main(void)
{
	struct mCc_ast_expression *expr = NULL;
	struct mCc_ast_statement *stmt = NULL;

	/* parsing phase */
	{
		struct mCc_parser_result result = mCc_parser_parse_file(stdin);
		if (result.status != MCC_PARSER_STATUS_OK) {
			return EXIT_FAILURE;
		}
		expr = result.expression;
		stmt = result.statement;
	}

	if (expr) {
		mCc_ast_print_dot_expression(stdout, expr);
		mCc_ast_delete_expression(expr);
	} else if (stmt) {
		mCc_ast_print_dot_statement(stdout, stmt);
		mCc_ast_delete_statement(stmt);
	} else {
		fprintf(stderr, "Invalid top-level or forgotten to implement!");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
