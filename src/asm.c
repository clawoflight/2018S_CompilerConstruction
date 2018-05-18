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

static void mCc_asm_print_assign(struct mCc_tac_quad *quad,FILE *out){
    if(quad->arg1.number<0){
        fprintf(out,"\t\tmovl\t-4(%%ebp), %%eax\n");
    }else {
        fprintf(out, "\t\tmovl\t8(%%ebp), %%eax\n");
    }

}

static void mCc_asm_print_bin_op(struct mCc_tac_quad *quad,FILE *out){
    switch (quad->bin_op) {
        case MCC_TAC_OP_BINARY_ADD:
            break;
        case MCC_TAC_OP_BINARY_SUB:
            break;
        case MCC_TAC_OP_BINARY_MUL:
            break;
        case MCC_TAC_OP_BINARY_DIV:
            break;
        case MCC_TAC_OP_BINARY_LT:
            break;
        case MCC_TAC_OP_BINARY_GT:
            break;
        case MCC_TAC_OP_BINARY_LEQ:
            break;
        case MCC_TAC_OP_BINARY_GEQ:
            break;
        case MCC_TAC_OP_BINARY_AND:
            break;
        case MCC_TAC_OP_BINARY_OR:
            break;
        case MCC_TAC_OP_BINARY_EQ:
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
            mCc_asm_print_assign(quad,out);
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT:
            mCc_asm_print_assign_lit(quad->literal, out);
            break;
        case MCC_TAC_QUAD_OP_UNARY:
            break;
        case MCC_TAC_QUAD_OP_BINARY:
            mCc_asm_print_bin_op(quad,out);
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