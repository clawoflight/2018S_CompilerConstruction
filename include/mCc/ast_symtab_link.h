/**
 * @file ast_symtab_link.h
 * @brief Declarations for building the symbol table
 * @author bennett
 * @date 2018-04-17
 */
#ifndef MCC_AST_SYMTAB_LINK_H
#define MCC_AST_SYMTAB_LINK_H

#include <stdio.h>

#include "mCc/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mCc_ast_symtab_build_result {
	int status;
	char *err_msg;
	struct mCc_ast_source_location err_loc;
	struct mCc_symtab_scope *root_symtab;
};

struct mCc_ast_symtab_build_result mCc_ast_symtab_build(struct mCc_ast_program *prog);

#ifdef __cplusplus
}
#endif

#endif
