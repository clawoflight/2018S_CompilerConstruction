/**
 * @file asm.c
 * @brief Generation of the assembler code.
 * @author richard
 * @date 2018-05-16
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mCc/asm.h"

#define ARRAY_LENGTH 4096
static struct mCc_asm_stack_pos position[ARRAY_LENGTH];
static struct mCc_asm_stack_pos position_param[ARRAY_LENGTH];

static int current_elements_in_local_array = 0;
static int current_elements_in_param_array = 0;
//static int current_elements_in_fpu = 0;
static int current_frame_pointer = 0;
static int current_param_pointer = 4;
static int var_count=0;

static void mCc_asm_test_print(FILE *out)
{
	fprintf(out, "#=============== Local Stack\n");
	for (int i = 0; i < current_elements_in_local_array; ++i) {
		fprintf(out, "\n#Number: %d\n#Stack: %d\n#Enum Type: %d\n",
                position[i].tac_number, position[i].stack_ptr, position[i].lit_type);
	}

	fprintf(out, "#=============== Param Stack\n");
	for (int i = 0; i < current_elements_in_param_array; ++i) {
		fprintf(out, "\n#Number: %d\n#Stack: %d\n#Enum Type: %d\n", position_param[i].tac_number,
		        position_param[i].stack_ptr,  position_param[i].lit_type);
	}
}

static struct mCc_asm_stack_pos mCc_asm_get_stack_ptr_from_number(int number)
{
	for (int i = 0; i < current_elements_in_local_array; ++i) {
		if (position[i].tac_number == number)
			return position[i];
	}
	for (int i = 0; i < current_elements_in_param_array; ++i) {
		if (position_param[i].tac_number == number)
			return position_param[i];
	}
	struct mCc_asm_stack_pos tmp;
	tmp.tac_number = -1;
	return tmp;
}

static int mCc_asm_move_current_pointer(struct mCc_asm_stack_pos position,
                                        int pointer)
{
    int ret = 0;
    switch (position.lit_type){
        case MCC_TAC_QUAD_LIT_INT:
            ret = -4;
            break;
        case MCC_TAC_QUAD_LIT_BOOL:
            ret = -1;
            break;
        case MCC_TAC_QUAD_LIT_FLOAT:
			//ret = -8;
            break;
        case MCC_TAC_QUAD_LIT_STR:
            break;
        default:
            ret = -4;
            break;
    }
    if (pointer <= 0)
        return ret;
    else
        return -ret;
}

static void mCc_asm_print_assign_lit(struct mCc_tac_quad *quad, FILE *out)
{
	struct mCc_tac_quad_literal *lit;
	lit = quad->literal;

	struct mCc_asm_stack_pos result = mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);

	if (result.tac_number == -1) {
        struct mCc_asm_stack_pos new_number;
        new_number.lit_type = lit->type;
        current_frame_pointer += mCc_asm_move_current_pointer(new_number, current_frame_pointer);
        new_number.tac_number = quad->result.ref.number;
		new_number.stack_ptr = current_frame_pointer;


		position[current_elements_in_local_array++] = new_number;
		result = new_number;
	}

	switch (lit->type) {
	case MCC_TAC_QUAD_LIT_INT:
		fprintf(out, "\tmovl\t$%d, %d(%%ebp)\n", lit->ival, result.stack_ptr);
		break;
	case MCC_TAC_QUAD_LIT_FLOAT:
		/*fprintf(out, "\tfldl\t.LC%d\n",current_elements_in_fpu);
		fprintf(out, "\tfstpl\t%d(%%ebp)\n",result.stack_ptr);
		current_elements_in_fpu++;*/
		break;
	case MCC_TAC_QUAD_LIT_BOOL:
		fprintf(out, "\tmovb\t$%d, %d(%%ebp)\n", lit->bval ? 1 : 0, result.stack_ptr);
		break;
	case MCC_TAC_QUAD_LIT_STR:
		fprintf(out, "\tmovl\t$S%d, %d(%%ebp)\n", lit->label_num, result.stack_ptr);
		break;
	}
}

static void mCc_asm_print_assign(struct mCc_tac_quad *quad, FILE *out)
{
    struct mCc_asm_stack_pos result = mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);
    struct mCc_asm_stack_pos source = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);

	if (result.tac_number == -1) {
		current_frame_pointer += mCc_asm_move_current_pointer(source, current_frame_pointer);
		struct mCc_asm_stack_pos new_number;
		new_number.tac_number = quad->result.ref.number;
		new_number.stack_ptr = current_frame_pointer;
        new_number.lit_type = source.lit_type;
		position[current_elements_in_local_array++] = new_number;
		result = new_number;
	}
	/*if(source.lit_type==MCC_TAC_QUAD_LIT_FLOAT){
		fprintf(out,"\tfldl\t%d(%%ebp)\n",source.stack_ptr);
		fprintf(out,"\tfstpl\t%d(%%ebp)\n",result.stack_ptr);
	}else {*/
    fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", source.stack_ptr);
    fprintf(out, "\tmovl\t%%eax, %d(%%ebp)\n", result.stack_ptr);
	//}
}

static void mCc_asm_print_un_op(struct mCc_tac_quad *quad, FILE *out)
{
    struct mCc_asm_stack_pos op1 = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);
    struct mCc_asm_stack_pos result = mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);
	if (result.tac_number == -1) {
		current_frame_pointer += mCc_asm_move_current_pointer(op1, current_frame_pointer);
		struct mCc_asm_stack_pos new_number;
		new_number.tac_number = quad->result.ref.number;
		new_number.stack_ptr = current_frame_pointer;
        new_number.lit_type = op1.lit_type;

		position[current_elements_in_local_array++] = new_number;
		result = new_number;
	}
	switch (quad->un_op) {
	case MCC_TAC_OP_UNARY_NEG: fprintf(out, "\tnegl\t%d(%%ebp)\n", op1.stack_ptr); break;
	case MCC_TAC_OP_UNARY_NOT: break;
	}
}

static void mCc_asm_print_bin_op(struct mCc_tac_quad *quad, FILE *out)
{
    struct mCc_asm_stack_pos result = mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);
    struct mCc_asm_stack_pos op1 = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);
    struct mCc_asm_stack_pos op2 = mCc_asm_get_stack_ptr_from_number(quad->arg2.number);

	if (result.tac_number == -1) {
		current_frame_pointer += mCc_asm_move_current_pointer(op1, current_frame_pointer);
		struct mCc_asm_stack_pos new_number;
		new_number.tac_number = quad->result.ref.number;
		new_number.stack_ptr = current_frame_pointer;
        new_number.lit_type = op1.lit_type;

		position[current_elements_in_local_array++] = new_number;
		result = new_number;
	}


	switch (quad->bin_op) {
	case MCC_TAC_OP_BINARY_ADD:
        fprintf(out, "\tmovl\t%d(%%ebp), %%edx\n", op1.stack_ptr);
		fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", op2.stack_ptr);
		fprintf(out, "\taddl\t%%edx,%%eax\n");
        fprintf(out, "\tmovl\t%%eax, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_SUB:
        fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\tsubl\t%d(%%ebp), %%eax\n", op2.stack_ptr);
        fprintf(out, "\tmovl\t%%eax, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_MUL:
        fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\timull\t%d(%%ebp), %%eax\n", op2.stack_ptr);
        fprintf(out, "\tmovl\t%%eax, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_DIV:
        fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\tcltd\n");
		fprintf(out, "\tidivl\t%d(%%ebp)\n", op2.stack_ptr);
        fprintf(out, "\tmovl\t%%eax, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_LT:
        fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\tcmpl\t%d(%%ebp), %%eax\n", op2.stack_ptr);
		fprintf(out, "\tsetl\t%%al\n");
		fprintf(out, "\tmovb\t%%al, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_GT:
        fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\tcmpl\t%d(%%ebp), %%eax\n", op2.stack_ptr);
		fprintf(out, "\tsetg\t%%al\n");
        fprintf(out, "\tmovb\t%%al, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_LEQ:
        fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\tcmpl\t%d(%%ebp), %%eax\n", op2.stack_ptr);
		fprintf(out, "\tsetle\t%%al\n");
        fprintf(out, "\tmovb\t%%al, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_GEQ:
        fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\tcmpl\t%d(%%ebp), %%eax\n", op2.stack_ptr);
		fprintf(out, "\tsetge\t%%al\n");
        fprintf(out, "\tmovb\t%%al, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_AND:
		fprintf(out, "\tcmpb\t$0, %d(%%ebp)\n",op1.stack_ptr);
        fprintf(out,"\tje\t.L%d\n",quad->result.ref.binary_label.num);
        fprintf(out,"\tcmpb\t$0, %d(%%ebp)\n",op2.stack_ptr);
        fprintf(out,"\tje\t.L%d\n",quad->result.ref.binary_label.num);
        fprintf(out,"\tmovl\t$1, %%eax\n");
        fprintf(out,"\tjmp .L%d\n",quad->result.ref.binary_label.num+1);
        fprintf(out,".L%d:\n",quad->result.ref.binary_label.num);
        fprintf(out,"\tmovl\t$0, %%eax\n");
        fprintf(out,".L%d:\n",quad->result.ref.binary_label.num+1);
        fprintf(out,"\tmovb\t%%al, %d(%%ebp)\n",result.stack_ptr);
        fprintf(out,"\tandb\t$1, %d(%%ebp)\n",result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_OR:
        fprintf(out,"\tcmpb $0, %d(%%ebp)\n",op1.stack_ptr);
        fprintf(out,"\tjne .L%d\n",quad->result.ref.binary_label.num);
        fprintf(out,"\tcmpb $0, %d(%%ebp)\n",op2.stack_ptr);
        fprintf(out,"\tje .L%d\n",quad->result.ref.binary_label.num+1);
        fprintf(out,".L%d:\n",quad->result.ref.binary_label.num);
        fprintf(out,"\tmovl $1, %%eax\n");
        fprintf(out,"\tjmp .L%d\n",quad->result.ref.binary_label.num+2);
        fprintf(out,".L%d:\n",quad->result.ref.binary_label.num+1);
        fprintf(out,"\tmovl $0, %%eax\n");
        fprintf(out,".L%d:\n",quad->result.ref.binary_label.num+2);
        fprintf(out,"\tmovb %%al, %d(%%ebp)\n",result.stack_ptr);
        fprintf(out,"\tandb $1, %d(%%ebp)\n",result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_EQ:
        fprintf(out, "\tmovzbl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\tcmpb\t%d(%%ebp), %%eax\n", op2.stack_ptr);
		fprintf(out, "\tsete\t%%al\n");
        fprintf(out, "\tmovb\t%%al, %d(%%ebp)\n", result.stack_ptr);
		break;
	case MCC_TAC_OP_BINARY_NEQ:
        fprintf(out, "\tmovzbl\t%d(%%ebp), %%eax\n", op1.stack_ptr);
		fprintf(out, "\tcmpl \t%d(%%ebp), %%eax\n", op2.stack_ptr);
		fprintf(out, "\tsetne\t%%al\n");
		fprintf(out, "\tmovzbl\t%%al, %%eax\n");
		break;
	case MCC_TAC_OP_BINARY_FLOAT_ADD:
		/*fprintf(out,"\tfldl\t%d(%%ebp)\n",op1.stack_ptr);
		fprintf(out,"\tfaddl\t%d(%%ebp)\n",op2.stack_ptr);
		fprintf(out,"\tfstpl\t%d(%%ebp)\n",result.stack_ptr);*/
		break;
	case MCC_TAC_OP_BINARY_FLOAT_SUB:
      /*  fprintf(out,"\tfldl\t%d(%%ebp)\n",op1.stack_ptr);
        fprintf(out,"\tfmull\t%d(%%ebp)\n",op2.stack_ptr);
        fprintf(out,"\tfstpl\t%d(%%ebp)\n",result.stack_ptr);*/
        break;
	case MCC_TAC_OP_BINARY_FLOAT_MUL:
      /*  fprintf(out,"\tfldl\t%d(%%ebp)\n",op1.stack_ptr);
        fprintf(out,"\tfmull\t%d(%%ebp)\n",op2.stack_ptr);
        fprintf(out,"\tfstpl\t%d(%%ebp)\n",result.stack_ptr);*/
        break;
	case MCC_TAC_OP_BINARY_FLOAT_DIV:
        /*fprintf(out,"\tfldl\t%d(%%ebp)\n",op1.stack_ptr);
        fprintf(out,"\tfdivl\t%d(%%ebp)\n",op2.stack_ptr);
        fprintf(out,"\tfstpl\t%d(%%ebp)\n",result.stack_ptr);*/
        break;
	}
}

static void mCc_asm_print_label(struct mCc_tac_quad *quad, FILE *out)
{

	if (quad->result.label.num > -1) {
		fprintf(out, ".L%d:\n", quad->result.label.num);
	} else {
		current_frame_pointer = 0;
		current_param_pointer = 4;

		fprintf(out, ".global\t%s\n", quad->result.label.str);
		fprintf(out, ".type\t%s, @function\n", quad->result.label.str);
		fprintf(out, "%s:\n", quad->result.label.str);
		fprintf(out, "\tpushl\t%%ebp\n");
		fprintf(out, "\tmovl\t%%esp, %%ebp\n");
		if(quad->var_count<5){
			var_count=16;
		}else if(quad->var_count<9&&quad->var_count>=5){
			var_count=32;
		}else if(quad->var_count<13&&quad->var_count>=9){
			var_count=48;
		}else if(quad->var_count<17&&quad->var_count>=13){
			var_count=64;
		}else if(quad->var_count<21&&quad->var_count>=17){
			var_count=80;
		}
        fprintf(out, "\tsubl\t$%d, %%esp\n",var_count);
	}
}

static void mCc_asm_print_jump_false(struct mCc_tac_quad *quad, FILE *out)
{
    struct mCc_asm_stack_pos condition = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);
	fprintf(out, "\tcmpl\t$1, %d(%%ebp)\n",
	        condition.stack_ptr); // compare always with true
	fprintf(out, "\tjne\t.L%d\n", quad->result.label.num);
}

static void mCc_asm_handle_load(struct mCc_tac_quad *quad)
{
    struct mCc_asm_stack_pos result = mCc_asm_get_stack_ptr_from_number(quad->result.ref.number);
	if (result.tac_number == -1) {
        struct mCc_asm_stack_pos new_number;
		new_number.lit_type = quad->result.ref.type;
        current_param_pointer += mCc_asm_move_current_pointer(new_number, current_param_pointer);
		new_number.tac_number = quad->result.ref.number;
		new_number.stack_ptr = current_param_pointer;
        //TODO think about what lit type to assign here
		position_param[current_elements_in_param_array++] = new_number;
	}
}

static void mCc_asm_print_return_void(struct mCc_tac_quad *quad, FILE *out)
{
	fprintf(out, "\tmovl\t$0, %%eax\t# return zero because of main\n");
	if (current_param_pointer > 4) { // parameter were initialized
		fprintf(out, "\tpopl\t%%ebp\n");
	} else { // no init param
		fprintf(out, "\tleave\n");
	}
	fprintf(out, "\tret\n");
}

static void mCc_asm_print_return(struct mCc_tac_quad *quad, FILE *out)
{
    struct mCc_asm_stack_pos ret_val = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);
	fprintf(out, "\tmovl\t%d(%%ebp), %%eax\n", ret_val.stack_ptr);
	if (current_param_pointer > 4) { // parameter
		fprintf(out, "\tpopl\t%%ebp\n");
	} else { // local
		fprintf(out, "\tleave\n");
	}
	fprintf(out, "\tret\n");
}

static void mCc_asm_print_param(struct mCc_tac_quad *quad, FILE *out)
{
    struct mCc_asm_stack_pos result = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);
	if (result.tac_number == -1) {
        struct mCc_asm_stack_pos new_number;
        current_frame_pointer += mCc_asm_move_current_pointer(new_number, current_frame_pointer);
		new_number.tac_number = quad->result.ref.number;
		new_number.stack_ptr = current_frame_pointer;
        //TODO think about what lit type to assign here

		position[current_elements_in_local_array++] = new_number;
		result = new_number;
	}
	fprintf(out, "\tpushl\t%d(%%ebp)\n", result.stack_ptr);
}

static void mCc_asm_print_call(struct mCc_tac_quad *quad, FILE *out)
{
    struct mCc_asm_stack_pos result = mCc_asm_get_stack_ptr_from_number(quad->arg1.number);

	if (result.tac_number == -1) {
        struct mCc_asm_stack_pos new_number;
		new_number.lit_type = quad->result.ref.type;
        current_frame_pointer += mCc_asm_move_current_pointer(new_number, current_frame_pointer);
		new_number.tac_number = quad->arg1.number;
		new_number.stack_ptr = current_frame_pointer;
        //TODO WHAT TO DO HERE
		position[current_elements_in_local_array++] = new_number;
		result = new_number;
	}
	fprintf(out, "\tcall\t%s\n", quad->result.label.str);
	fprintf(out, "\tmovl\t%%eax, %d(%%ebp)\n", result.stack_ptr);
}

static void mCc_asm_assembly_from_quad(struct mCc_tac_quad *quad, FILE *out)
{

	if (quad->comment) {
		fprintf(out, "# %s\n", quad->comment);
	}

	// fprintf(out, "type: %d\n", quad->type);
	switch (quad->type) {
	case MCC_TAC_QUAD_ASSIGN: mCc_asm_print_assign(quad, out); break;
	case MCC_TAC_QUAD_ASSIGN_LIT: mCc_asm_print_assign_lit(quad, out); break;
	case MCC_TAC_QUAD_OP_UNARY: mCc_asm_print_un_op(quad, out); break;
	case MCC_TAC_QUAD_OP_BINARY: mCc_asm_print_bin_op(quad, out); break;
	case MCC_TAC_QUAD_JUMP:
		fprintf(out, "\tjmp\t.L%d\n", quad->result.label.num);
		break;
	case MCC_TAC_QUAD_JUMPFALSE: mCc_asm_print_jump_false(quad, out); break;
	case MCC_TAC_QUAD_LABEL: mCc_asm_print_label(quad, out); break;
	case MCC_TAC_QUAD_PARAM: mCc_asm_print_param(quad, out); break;
	case MCC_TAC_QUAD_CALL: mCc_asm_print_call(quad, out); break;
	case MCC_TAC_QUAD_LOAD: mCc_asm_handle_load(quad); break;
	case MCC_TAC_QUAD_STORE: break;
	case MCC_TAC_QUAD_RETURN: mCc_asm_print_return(quad, out); break;
	case MCC_TAC_QUAD_RETURN_VOID: mCc_asm_print_return_void(quad, out); break;
	}
}

static void mCc_asm_print_string_literals(struct mCc_tac_program *prog, FILE *out)
{
	for (unsigned int i = 0; i < prog->string_literal_count; ++i) {
		fprintf(out, "S%d:\n", prog->string_literals[i].number);
		fprintf(out, ".string \"%s\"\n", prog->string_literals[i].str_value);
	}
}

void mCc_asm_generate_assembly(struct mCc_tac_program *prog, FILE *out,
                               char *source_filename)
{
	fprintf(out, ".file\t\"%s\"\n", source_filename);
	fprintf(out, ".text\n");
	fprintf(out, ".section .rodata\n");
	mCc_asm_print_string_literals(prog, out);
	fprintf(out, ".text\n");
	for (unsigned int i = 0; i < prog->quad_count; ++i) {
		mCc_asm_assembly_from_quad(prog->quads[i], out);
	}
	// TODO need a ret in assembly
	mCc_asm_test_print(out);
}
