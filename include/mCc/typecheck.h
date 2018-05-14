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
	MCC_TYPECHECK_STATUS_ERROR ///< if there was an error
};

/**
 * @brief Contains all information pertaining to the type checking error.
 */
#define err_len (4096)
struct mCc_typecheck_result {
	enum mCc_typecheck_status status;
    enum mCc_ast_type type;
	char err_msg[err_len];
	struct mCc_ast_source_location err_loc;
    bool stmt_type;
};


/**
 * @brief Typecheck the given program.
 * @param program The program to typecheck
 *
 * @return an #mCc_typecheck_result containing status, error and location.
 */
struct mCc_typecheck_result mCc_typecheck(struct mCc_ast_program *program);

/**
 * check whether the main is declared in the right way (return type int, no parameter and name main)
 * @param scope
 * @return 0 on success or -1 on error
 */
int mCc_typecheck_check_main_properties(struct mCc_symtab_scope *scope );

/**
 * Dummy func for testing
 */
struct mCc_typecheck_result test_type_check(struct mCc_ast_expression *expression);
bool test_type_check_stmt(struct mCc_ast_statement *stmt);
struct mCc_typecheck_result test_type_check_program(struct mCc_ast_program *prog);


#ifdef __cplusplus
}
#endif
#endif // MCC_TYPECHECK_H
