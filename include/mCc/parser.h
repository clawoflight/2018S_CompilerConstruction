#ifndef MCC_PARSER_H
#define MCC_PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mCc_parser_status {
	MCC_PARSER_STATUS_OK,
	MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM,
	MCC_PARSER_STATUS_UNKNOWN_ERROR,
	MCC_PARSER_STATUS_PARSE_ERROR,
};

struct mCc_parser_result {
	enum mCc_parser_status status;

	struct mCc_ast_expression *expression;
	struct mCc_ast_statement *statement;
	struct mCc_ast_program *program;

	/// Human-readable error description.
	/// This *must be freed* if it is not NULL!
	const char *err_msg;
	/// The text at the error location. Also free this!
	const char *err_text;
	struct mCc_ast_source_location err_loc;
};

struct mCc_parser_result mCc_parser_parse_string(const char *input);

struct mCc_parser_result mCc_parser_parse_file(FILE *input);

#ifdef __cplusplus
}
#endif

#endif
