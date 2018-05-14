/**
 * @file tac_builder.h
 * @brief Declarations for the three-address-code builder
 * @author bennett
 * @date 2018-04-27
 */
#ifndef MCC_TAC_BUILDER_H
#define MCC_TAC_BUILDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tac.h"
#include "ast.h"

/**
 * @brief Build a TAC program from an AST.
 *
 * @param prog The program to convert
 *
 * @return A TAC program, or NULL on error
 */
struct mCc_tac_program *mCc_tac_build(struct mCc_ast_program *prog);


void mCc_tac_free_global_string_array();
#ifdef __cplusplus
}
#endif
#endif // MCC_TAC_BUILDER_H
