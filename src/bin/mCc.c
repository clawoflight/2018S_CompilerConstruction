#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mCc/asm.h"
#include "mCc/ast.h"
#include "mCc/ast_symtab_link.h"
#include "mCc/parser.h"
#include "mCc/tac_builder.h"
#include "mCc/typecheck.h"
#include "mCc/cfg_print.h"

static const char* VERSION = "0.3.0";

static void print_usage(const char *prg)
{
	printf("usage: %s [options] <FILE>\n\n", prg);
	puts("Options:");
	printf("  <FILE>                  Input file, or - for stdin\n");
	printf("  -h|--help               Print this message\n");
	printf("  -v|--version            Print the version\n");
	printf("  -o|--output <FILE>      Path to generated executable, default is a.out\n");
	printf("  -O|--optimize           Prints optimization in doc/optimisation.md and cfg in doc/images\n");
	printf("  --print-symtab[=FILE]   Print the symbol tables\n");
	printf("  --print-tac[=FILE]      Print the three-address code\n");
	printf("  --print-asm[=FILE]      Print the assembler code\n");
	printf("  --print-cfg[=FILE]      Print the control-flow graphs in DOT format\n");
	printf("\nPrinting anything disables compilation. Printing without specifying a file prints to stdout.\n");
}

static int compile(char *source, char *executable)
{
	int pid;
	if ((pid = fork()) == 0) {
		execlp("gcc", "gcc", "-m32", source, "../src/mC_builtins.c", "-o",
		       executable, (char *)NULL);
		// exec* only returns on error
		perror("gcc");
		exit(errno);
	} else { // parent
		int wstatus;
		if (waitpid(pid, &wstatus, 0) < 0) {
			perror("waitpid");
			return EXIT_FAILURE;
		}
		// return gcc's exit status
		return WIFEXITED(wstatus) ? WEXITSTATUS(wstatus) : EXIT_FAILURE;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	// Variables set by the command-line options
	FILE *st_out = NULL; // symtab
	int print_st = 0;

	FILE *tac_out = NULL;
	int print_tac = 0;

	FILE *cfg_out = NULL;
	int print_cfg = 0;

    FILE *op_out = NULL;
    int print_op = 0;
	char str[100];

	FILE *asm_out = fopen("a.s", "w");
	int print_asm = 0;
	if (!asm_out) {
		perror("fopen");
		return EXIT_FAILURE;
	}
	char *executable = "a.out";
    char *optimization ="../doc/optimisation.md";

	while (1) {
		int c;
		static struct option long_options[] = {
			{ "help", no_argument, 0, 'h' },
			{ "version", no_argument, 0, 'v' },
			{ "print-tac", optional_argument, 0, 't' },
			{ "print-symtab", optional_argument, 0, 's' },
			{ "print-asm", optional_argument, 0, 'a' },
			{ "print-cfg", optional_argument, 0, 'c' },
			{ "output", required_argument, 0, 'o' },
			{ "optimize", no_argument, 0, 'O' },
			{ 0, 0, 0, 0 }
		};
		if ((c = getopt_long(argc, argv, "hvoOt:", long_options, NULL)) == -1)
			break;

		switch (c) {
		case 'h': print_usage(argv[0]); return EXIT_SUCCESS;
		case 'v':
			puts(VERSION);
			return EXIT_SUCCESS;
		case '?':
			// getopt prints the error message itself
			return EXIT_FAILURE;
		case 'o':
			executable = optarg;
			break;
        case 'O':
            if (!(op_out = fopen(optimization, "w"))) {
                perror("fopen");
                return EXIT_FAILURE;
            }
            print_op = 1;
            break;
		case 't':
			if (!optarg || strcmp("-", optarg) == 0) {
				tac_out = stdout;
			} else if (!(tac_out = fopen(optarg, "w"))) {
				perror("fopen");
				return EXIT_FAILURE;
			}
			print_tac = 1;
			break;
		case 's':
			if (!optarg || strcmp("-", optarg) == 0) {
				st_out = stdout;
			} else if (!(st_out = fopen(optarg, "w"))) {
				perror("fopen");
				return EXIT_FAILURE;
			}
			print_st = 1;
			break;
		case 'a':
			if (!optarg || strcmp("-", optarg) == 0) {
				asm_out = stdout;
			} else if (!(asm_out = fopen(optarg, "w"))) {
				perror("fopen");
				return EXIT_FAILURE;
			}
			print_asm = 1;
			break;
		case 'c':
			if (!optarg || strcmp("-", optarg) == 0) {
				cfg_out = stdout;
			} else if (!(cfg_out = fopen(optarg, "w"))) {
				perror("fopen");
				return EXIT_FAILURE;
			}
			print_cfg = 1;
			break;
		}
	}
	// Now, first non-option arg is in argv[optind]
	if (optind >= argc) {
		fputs("Missing required positional argument: <FILE>\n", stderr);
		return EXIT_FAILURE;
	}

	/* determine input source */
	char *filename;
	FILE *in;
	if (strcmp("-", argv[optind]) == 0) {
		in = stdin;
		filename = "read from stdin";
	} else {
		filename = basename(argv[optind]);
		in = fopen(argv[optind], "r");
		if (!in) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}

	struct mCc_ast_program *prog = NULL;

	/* parsing phase */
	{
		struct mCc_parser_result result = mCc_parser_parse_file(in);
		if(print_op){
			rewind(in);
			fprintf(op_out,"---------------------The input program---------------------\n");
            while (fgets(str, 100, in) != NULL){
                fprintf(op_out, "%s\n", str);
			}
		}
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
		if (result.program == NULL) {
			fprintf(stderr, "Error in %s: Top-level is not a program!\n", argv[1]);
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
	if (print_st)
		mCc_symtab_print_all_scopes(st_out);
	if (st_out && st_out != stdout)
		fclose(st_out);

	/* type checking */
	struct mCc_typecheck_result check_result =
	    mCc_typecheck(prog, link_result.root_symtab);

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
	struct mCc_tac_program *tac = mCc_tac_build(prog);
	if (print_tac && tac)
		mCc_tac_program_print(tac, tac_out);
	else if (!tac) {
		fputs("Memory error while building the TAC!\n", stderr);
		if (tac_out && tac_out != stdout)
			fclose(tac_out);
		mCc_symtab_delete_all_scopes();
		mCc_ast_delete_program(prog);
		return EXIT_FAILURE;
	}
	if (tac_out && tac_out != stdout)
		fclose(tac_out);
	if (print_cfg)
		mCc_cfg_program_print(tac, cfg_out);
    if (print_op) {
        fprintf(op_out,"---------------------The dot cfg of the program---------------------\n");
        mCc_cfg_program_print(tac, op_out);

        //create image folder if it doesnt exist
        mkdir("../doc/images", 0777);

        //create separate dot file for CFG to create image
        char img_file_name[200];
        FILE *img_file = NULL;
        snprintf(img_file_name, sizeof img_file_name, "../doc/images/%s.dot", strtok(filename, "."));
        if (!(img_file = fopen(img_file_name, "w"))) {
            perror("fopen");
            return EXIT_FAILURE;
        }
        mCc_cfg_program_print(tac, img_file);

        fclose(img_file);
        char system_call[PATH_MAX];

        //get current dir since C doesnt like relative paths
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("getcwd() error");

        //make the dot compile command
        snprintf(system_call, sizeof system_call, "dot -Tpng %s/%s -o %s/../doc/images/%s.png",cwd, img_file_name, cwd, strtok(filename, "."));
        if(system(system_call) < 0){
            perror("img_create");
            return EXIT_FAILURE;
        }
        //delete the tmp dot file
        remove(img_file_name);
    }

	if (print_cfg && cfg_out != stdout)
		fclose(cfg_out);
    if (print_op && tac) {
        fprintf(op_out, "---------------------The Three Address Code before optimizations---------------------\n");
        mCc_tac_program_print(tac, op_out);
    }
	/*    TODO
	 * - do some optimisations
	 */

	/* Assembler code generation */
	mCc_asm_generate_assembly(tac, asm_out, filename);

	if (asm_out && asm_out != stdout)
		fclose(asm_out);

	int exit_status = EXIT_SUCCESS;
	// Only compile if nothing was printed
	if (!(print_st || print_tac || print_asm || print_cfg))
		exit_status = compile("a.s", executable);

	/* cleanup */
	mCc_tac_program_delete(tac);
	mCc_tac_free_global_string_array();
	mCc_symtab_delete_all_scopes();
	mCc_ast_delete_program(prog);

	return exit_status;
}
