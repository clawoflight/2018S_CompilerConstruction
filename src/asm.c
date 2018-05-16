/**
 * @file asm.c
 * @brief Generation of the assembler code.
 * @author richard
 * @date 2018-05-16
 */

#include <stdio.h>
#include <string.h>

#include "mCc/asm.h"

static void mCc_asm_assembly_from_quad(struct mCc_tac_quad *quad, FILE *out)
{
    switch(quad->type){
        case MCC_TAC_QUAD_ASSIGN:
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT:
            break;
        case MCC_TAC_QUAD_OP_UNARY:
            break;
        case MCC_TAC_QUAD_OP_BINARY:
            break;
        case MCC_TAC_QUAD_JUMP:
            break;
        case MCC_TAC_QUAD_JUMPFALSE:
            break;
        case MCC_TAC_QUAD_LABEL:
            break;
        case MCC_TAC_QUAD_PARAM:
            break;
        case MCC_TAC_QUAD_CALL:
            break;
        case MCC_TAC_QUAD_LOAD:
            break;
        case MCC_TAC_QUAD_STORE:
            break;
        case MCC_TAC_QUAD_RETURN:
            break;
        case MCC_TAC_QUAD_RETURN_VOID:
            break;
    }
}

void mCc_asm_generate_assembly(struct mCc_tac_program *prog, FILE *out)
{
    for (unsigned int i = 0; i < prog->quad_count; ++i){
        mCc_asm_assembly_from_quad(prog->quads[i], out);
    }
}