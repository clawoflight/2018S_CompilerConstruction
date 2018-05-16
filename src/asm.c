/**
 * @file asm.c
 * @brief Generation of the assembler code.
 * @author richard
 * @date 2018-05-16
 */

#include <stdio.h>
#include <string.h>

#include "mCc/asm.h"

static int current_frame_pointer = 0;

static void mCc_asm_print_assign_lit(struct mCc_tac_quad_literal *lit, FILE *out)
{
    switch (lit->type){
        case MCC_TAC_QUAD_LIT_INT:
            current_frame_pointer -= 4;
            fprintf(out, "\t\tmovl\t$%d, %d(%%ebp)\n", lit->ival,
                    current_frame_pointer);
            break;
        case MCC_TAC_QUAD_LIT_FLOAT:
            break;
        case MCC_TAC_QUAD_LIT_BOOL:
            break;
        case MCC_TAC_QUAD_LIT_STR:
            break;
    }
}

static void mCc_asm_print_label(struct mCc_tac_quad *quad, FILE *out)
{
    //Check if it's a function
    //TODO: additional checks required
    if (quad->result.label.str){
        fprintf(out, "%s:\n", quad->result.label.str);
    }
}

static void mCc_asm_assembly_from_quad(struct mCc_tac_quad *quad, FILE *out)
{
   // fprintf(out, "type: %d\n", quad->type);
    switch(quad->type){
        case MCC_TAC_QUAD_ASSIGN:
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT:
            mCc_asm_print_assign_lit(quad->literal, out);
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
            mCc_asm_print_label(quad, out);
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