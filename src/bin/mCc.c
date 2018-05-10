#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mCc/ast.h"
#include "mCc/ast_symtab_link.h"
#include "mCc/parser.h"
#include "mCc/typecheck.h"
#include "mCc/tac_builder.h"

void print_usage(const char *prg)
{
	printf("usage: %s [--help] <FILE> [--print-tac <FILE>]\n\n", prg);
	printf("  <FILE>        Input filepath or - for stdin\n");
	printf("  --print-tac   Print the three-address code to the given path\n");
	printf("  --help        Print this message\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2 || strcmp("--help", argv[1]) == 0) {
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

	/* tac output TODO: move to getopt later if needed */
	FILE *tac_out = NULL;
	if (argc > 3 && strcmp("--print-tac", argv[3]) == 0) {
		if (strcmp("-", argv[4]) == 0) {
			tac_out = stdout;
		} else {
			tac_out = fopen(argv[4], "r");
			if (!tac_out) {
				perror("fopen");
				return EXIT_FAILURE;
			}
		}
	}

	struct mCc_ast_program *prog = NULL;

	/* parsing phase */
	{
		struct mCc_parser_result result = mCc_parser_parse_file(in);
		fclose(in);
		if (result.status != MCC_PARSER_STATUS_OK) {
			fprintf(stderr,
			        "Error in %s at %d:%d - %d:%d at \e[4m%s\e[24m: %s\n",
			        argv[1], result.err_loc.start_line,
			        result.err_loc.start_col, result.err_loc.end_line,
			        result.err_loc.end_col, result.err_text, result.err_msg);
			free((void *)result.err_msg);
			free((void *)result.err_text);

			return EXIT_FAILURE;
		}
		prog = result.program;
	}

	/* build symbol table */
	struct mCc_ast_symtab_build_result link_result = mCc_ast_symtab_build(prog);
	if (link_result.status) {
		fprintf(stderr, "Error in %s at %d:%d - %d:%d: %s\n", argv[1],
		        link_result.err_loc.start_line, link_result.err_loc.start_col,
		        link_result.err_loc.end_line, link_result.err_loc.end_col,
		        link_result.err_msg);
		mCc_symtab_delete_all_scopes();
		mCc_ast_delete_program(prog);
		return EXIT_FAILURE;
	}

	/* type checking */
	struct mCc_typecheck_result check_result = mCc_typecheck(prog);
	if (check_result.status) {
		fprintf(stderr, "Error in %s at %d:%d - %d:%d: %s\n", argv[1],
		        check_result.err_loc.start_line, check_result.err_loc.start_col,
		        check_result.err_loc.end_line, check_result.err_loc.end_col,
		        check_result.err_msg);
		mCc_symtab_delete_all_scopes();
		mCc_ast_delete_program(prog);
		return EXIT_FAILURE;
	}

	/* three-addess code generation */
	// TODO struct mCc_tac_program *prog = mCc_tac_build_program(prog);
	// TODO mCc_tac_program_print(tac, tac_out);

	if (tac_out && tac_out != stdout)
		fclose(tac_out);

	/*    TODO
	 * - create three-address code
	 * - do some optimisations
	 * - output assembly code
	 * - invoke backend compiler
	 */

	/* cleanup */
	// TODO mCc_tac_program_delete(tac, true);
	mCc_symtab_delete_all_scopes();
	mCc_ast_delete_program(prog);

	return EXIT_SUCCESS;
}
