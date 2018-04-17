/**
 * @file typecheck.h
 * @brief Type checking functionality.
 * @author bennett
 * @date 2018-04-17
 */
#ifndef MCC_TYPECHECK_H
#define MCC_TYPECHECK_H

#include "mCc/ast_symtab_link.h"
#include "mCc/symtab.h"
#include "mCc/ast.h"
#include "mCc/ast_statements.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief The status of a typecheck execution.
 */
enum mCc_typecheck_status {
	MCC_TYPECHECK_STATUS_OK, ///< No error
	                         // TODO not sure what else we need here.
};

/**
 * @brief Contains all information pertaining to the type checking error.
 */
struct mCc_typecheck_result {
	enum mCc_typecheck_status status;
	char *err_msg;
	// TODO should mCc_typecheck simply set a string, or do we want to pass more
	// information to the caller?
};
/**
 * @brief Typecheck the given program.
 *
 * TODO: This should in DFS, I think a visitor would be the easiest.
 * Alternatively, use our own non-tail-recursive system.
 * Call *after* linking to the symbol table, obviously!
 *
 * My biggest question is how we would return an error...?
 * I think this function will malloc a struct, pass it to the callbacks as void*
 * data, and the visitor callbacks can check the current status and return
 * immediately if necessary to speed up the checking.
 * If a callback encounters an error, it can simply set the struct members
 * accordingly.
 *
 * @param program The program to typecheck
 *
 * @return an #mCc_typecheck_result containing status and error.
 */
struct mCc_typecheck_result mCc_typecheck(struct mCc_ast_program *program);

#ifdef __cplusplus
}
#endif
#endif // MCC_TYPECHECK_H
