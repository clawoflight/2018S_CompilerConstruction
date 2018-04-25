#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mCc/ast.h"
#include "mCc/ast_symtab_link.h"
#include "mCc/parser.h"

void print_usage(const char *prg)
{
	printf("usage: %s <FILE>\n\n", prg);
	printf("  <FILE>        Input filepath or - for stdin\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	/* determine input source */
	FILE *in;
	if (strcmp("-", argv[1]) == 0) {
		in = stdin;
	} else {
		in = fopen(argv[1], "r");
		if (!in) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}

	struct mCc_ast_program *prog = NULL;

	/* parsing phase */
	{
		struct mCc_parser_result result = mCc_parser_parse_file(in);
		fclose(in);
		if (result.status != MCC_PARSER_STATUS_OK) {
			return EXIT_FAILURE;
		}
		prog = result.program;
	}

	struct mCc_ast_symtab_build_result link_result = mCc_ast_symtab_build(prog);
	if (link_result.status) {
		fprintf(stderr, "Error in %s at %d:%d-%d:%d: %s\n", argv[1],
		        link_result.err_loc.start_line, link_result.err_loc.start_col,
		        link_result.err_loc.end_line, link_result.err_loc.end_col,
		        link_result.err_msg);
		mCc_ast_delete_program(prog);
		return EXIT_FAILURE;
	}

	/*    TODO
	 * - run semantic checks
	 * - create three-address code
	 * - do some optimisations
	 * - output assembly code
	 * - invoke backend compiler
	 */

	/* cleanup */
	mCc_ast_delete_program(prog);

	return EXIT_SUCCESS;
}
