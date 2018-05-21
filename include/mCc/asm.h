/**
 * @file asm.h
 * @brief Assembler declarations.
 * @author richard
 * @date 2018-05-16
 */
#ifndef MCC_ASM_H
#define MCC_ASM_H

#include "tac.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mCc_asm_stack_pos {
	int tac_number;
	int stack_ptr;
};

void mCc_asm_generate_assembly(struct mCc_tac_program *prog, FILE *out);

#ifdef __cplusplus
}
#endif

#endif // MCC_ASM_H
