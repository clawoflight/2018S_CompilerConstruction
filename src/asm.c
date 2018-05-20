/**
 * @file asm.c
 * @brief Generation of the assembler code.
 * @author richard
 * @date 2018-05-16
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "mCc/asm.h"

#define ARRAY_LENGTH 4096
static struct mCc_asm_stack_pos position[ARRAY_LENGTH];
static struct mCc_asm_stack_pos position_param[ARRAY_LENGTH];

static int current_elements_in_local_array = 0;
static int current_elements_in_param_array = 0;
static int current_frame_pointer = 0;
static int current_param_pointer = 4;
static bool first_function = true;

static void mCc_asm_test_print(FILE *out)
{
    fprintf(out, "=============== Local Stack");
    for (int i = 0; i < current_elements_in_local_array; ++i){
        fprintf(out, "\nNumber: %d\nStack: %d\n",
            position[i].tac_number, position[i].stack_ptr);
    }

    fprintf(out, "=============== Param Stack");
    for (int i = 0; i < current_elements_in_param_array; ++i){
        fprintf(out, "\nNumber: %d\nStack: %d\n",
                position_param[i].tac_number, position_param[i].stack_ptr);
    }
}

static int mCc_asm_get_param_ptr_from_number(int number)
{
    for (int i = 0; i < current_elements_in_param_array; ++i){
        if (position_param[i].tac_number == number)
            return position_param[i].stack_ptr;
    }
    return -1;
}


static int mCc_asm_get_stack_ptr_from_number(int number)
{
    for (int i = 0; i < current_elements_in_local_array; ++i){
        if (position[i].tac_number == number)
            return position[i].stack_ptr;
    }
    return mCc_asm_get_param_ptr_from_number(number);
}

static void mCc_asm_print_assign_lit(struct mCc_tac_quad *quad, FILE *out)
{
    struct mCc_tac_quad_literal *lit;
    lit = quad->literal;

    int result = mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);

    if (result == -1){
        current_frame_pointer -= 4;
        struct mCc_asm_stack_pos new_number;
        new_number.tac_number = quad->result.ref.number;
        new_number.stack_ptr = current_frame_pointer;

        position[current_elements_in_local_array++] = new_number;
        result = current_frame_pointer;
    }

    switch (lit->type){
        case MCC_TAC_QUAD_LIT_INT:
          //  if ()
            fprintf(out, "\tmovl\t$%d, %d(%%ebp)\n", lit->ival,
                    result);
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
    int result = mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);
    int source = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);

    if (result == -1){
        current_frame_pointer -= 4;
        struct mCc_asm_stack_pos new_number;
        new_number.tac_number = quad->result.ref.number;
        new_number.stack_ptr = current_frame_pointer;

        position_param[current_elements_in_local_array++] = new_number;
        result = current_frame_pointer;
    }

    fprintf(out,"\tmovl\t%d(%%ebp), %%eax\n",source);
    fprintf(out,"\tmovl\t%%eax, %d(%%ebp)\n",result);
}

static void mCc_asm_print_un_op(struct mCc_tac_quad *quad,FILE *out)
{
    int op1=mCc_asm_get_stack_ptr_from_number(quad->arg1.number);
    int result=mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);
    if (result == -1){
        current_frame_pointer -= 4;
        struct mCc_asm_stack_pos new_number;
        new_number.tac_number = quad->result.ref.number;
        new_number.stack_ptr = current_frame_pointer;

        position[current_elements_in_local_array++] = new_number;
        result = current_frame_pointer;
    }
    switch (quad->un_op){
        case MCC_TAC_OP_UNARY_NEG:
            fprintf(out, "\tnegl\t%d(%%ebp)\n",op1);
            break;
        case MCC_TAC_OP_UNARY_NOT:
            break;
    }
}

static void mCc_asm_print_bin_op(struct mCc_tac_quad *quad,FILE *out){
    int result = mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);
    int op1 = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);
    int op2 = mCc_asm_get_stack_ptr_from_number(quad->arg2.number);

    if (result == -1){
        current_frame_pointer -= 4;
        struct mCc_asm_stack_pos new_number;
        new_number.tac_number = quad->result.ref.number;
        new_number.stack_ptr = current_frame_pointer;

        position[current_elements_in_local_array++] = new_number;
        result = current_frame_pointer;
    }

    fprintf(out,"\tmovl\t%d(%%ebp), %%edx\n",op1);
    switch (quad->bin_op) {
        case MCC_TAC_OP_BINARY_ADD:
            fprintf(out,"\tmovl\t%d(%%ebp), %%eax\n",op2);
            fprintf(out,"\taddl\t%%edx,%%eax\n");
            break;
        case MCC_TAC_OP_BINARY_SUB:
            fprintf(out,"\tsubl\t%d(%%ebp), %%eax\n",op2);
            break;
        case MCC_TAC_OP_BINARY_MUL:
            fprintf(out,"\timull\t%d(%%ebp), %%eax\n",op2);
            break;
        case MCC_TAC_OP_BINARY_DIV:
            fprintf(out,"\tcltd\n");
            fprintf(out,"\tidivl\t%d(%Debp)\n",op2);
            break;
        case MCC_TAC_OP_BINARY_LT:
            fprintf(out,"\tcmpl\t%d(%%ebp), %%eax\n",op2);
            fprintf(out,"\tsetl\t%%al\n");
            fprintf(out,"\tmovb\t%%al, %%eax\n");               //movlbz for move long to bool
            break;
        case MCC_TAC_OP_BINARY_GT:
            fprintf(out,"\tcmpl\t%d(%%ebp), %%eax\n",op2);
            fprintf(out,"\tsetg\t%%al\n");
            fprintf(out,"\tmovb\t%%al, %%eax\n");
            break;
        case MCC_TAC_OP_BINARY_LEQ:
            fprintf(out,"\tcmpl\t%d(%%ebp), %%eax\n",op2);
            fprintf(out,"\tsetle\t%%al\n");
            fprintf(out,"\tmovb\t%%al, %%eax\n");
            break;
        case MCC_TAC_OP_BINARY_GEQ:
            fprintf(out,"\tcmpl\t%d(%%ebp), %%eax\n",op2);
            fprintf(out,"\tsetge\t%%al\n");
            fprintf(out,"\tmovb\t%%al, %%eax\n");
            break;
        case MCC_TAC_OP_BINARY_AND:
            fprintf(out,"\tandl\t%d(%%ebp), %%eax\n",op2);
            break;
        case MCC_TAC_OP_BINARY_OR:
            fprintf(out,"\torl\t%d(%%ebp), %%eax\n",op2);
            break;
        case MCC_TAC_OP_BINARY_EQ:
            fprintf(out,"\tcmpl\t%d(%%ebp), %%eax\n",op2);
            fprintf(out,"\tsete\t%%al\n");
            fprintf(out,"\tmovb\t%%al, %%eax\n");
            break;
        case MCC_TAC_OP_BINARY_NEQ:
            fprintf(out,"\tcmpl \t%d(%%ebp), %%eax\n",op2);
            fprintf(out,"\tsetne\t%%al\n");
            fprintf(out,"\tmovb\t%%al, %%eax\n");
        case MCC_TAC_OP_BINARY_FLOAT_ADD:
            break;
        case MCC_TAC_OP_BINARY_FLOAT_SUB:
            break;
        case MCC_TAC_OP_BINARY_FLOAT_MUL:
            break;
        case MCC_TAC_OP_BINARY_FLOAT_DIV:
            break;
    }
    fprintf(out,"\tmovl\t%%eax, %d(%%ebp)\n",result);

}

static void mCc_asm_print_label(struct mCc_tac_quad *quad, FILE *out)
{

    if(quad->result.label.num > -1){
        fprintf(out,".L%d:\n",quad->result.label.num);
    }
    else {
        current_frame_pointer = 0;
        current_param_pointer = 0;
        if (!first_function){
            fprintf(out, "\tleave\n");
            fprintf(out, "\tret\n");
        }
        first_function = 0;
        fprintf(out, "\t.globl\t%s\n", quad->result.label.str);
        fprintf(out, "\t.type\t%s, @function\n", quad->result.label.str);
        fprintf(out, "%s:\n", quad->result.label.str);
        fprintf(out, "\tpushl\t%%ebp\n");
        fprintf(out, "\tmovl\t%%esp, %%ebp\n");
    }
}

static void mCc_asm_print_jump_false(struct mCc_tac_quad *quad, FILE *out){
    int condition=mCc_asm_get_stack_ptr_from_number(quad->arg1.number);
    fprintf(out,"\tcmpl\t$1, %d(%%ebp)\n",condition);           // compare always with true
    fprintf(out,"\tjne\t.L%d\n",quad->result.label.num);
}

static void mCc_asm_handle_load(struct mCc_tac_quad *quad){
    int result=mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);
    if (result == -1){
        current_param_pointer += 4;
        struct mCc_asm_stack_pos new_number;
        new_number.tac_number = quad->result.ref.number;
        new_number.stack_ptr = current_param_pointer;

        position_param[current_elements_in_param_array++] = new_number;
        result = current_param_pointer;
    }
}

static void mCc_asm_print_param(struct mCc_tac_quad *quad, FILE *out)
{

}

static void mCc_asm_assembly_from_quad(struct mCc_tac_quad *quad, FILE *out)
{

   // fprintf(out, "type: %d\n", quad->type);
    switch(quad->type){
        case MCC_TAC_QUAD_ASSIGN:
            mCc_asm_print_assign(quad,out);
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT:
            mCc_asm_print_assign_lit(quad, out);
            break;
        case MCC_TAC_QUAD_OP_UNARY:
            mCc_asm_print_un_op(quad,out);
            break;
        case MCC_TAC_QUAD_OP_BINARY:
            mCc_asm_print_bin_op(quad,out);
            break;
        case MCC_TAC_QUAD_JUMP:
            fprintf(out,"\tjmp\t.L%d\n",quad->result.label.num);
            break;
        case MCC_TAC_QUAD_JUMPFALSE:
            mCc_asm_print_jump_false(quad,out);
            break;
        case MCC_TAC_QUAD_LABEL:
            mCc_asm_print_label(quad, out);
            break;
        case MCC_TAC_QUAD_PARAM:
            mCc_asm_print_param(quad, out);
            break;
        case MCC_TAC_QUAD_CALL:
            break;
        case MCC_TAC_QUAD_LOAD:
            mCc_asm_handle_load(quad);
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
    fprintf(out, "\tleave\n");
    fprintf(out, "\tret\n");
    mCc_asm_test_print(out);
}