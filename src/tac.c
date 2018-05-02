/**
 * @file tac.c
 * @brief Implementation of the tac.
 * @author bennett
 * @date 2018-04-27
 */
#include <stdbool.h>

struct mCc_tac_quad *mCc_tac_quad_new_assign(struct mCc_symtab_ref *arg1,
                                             struct mCc_symtab_ref *result){
    assert(arg1);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_ASSIGN;
    quad->arg1=arg1;
    quad->result.ref=result;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_assign_lit(struct mCc_ast_literal *arg1,
                                                 struct mCc_symtab_ref *result){
    assert(arg1);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_ASSIGN_LITERAL;
    quad->literal=arg1;
    quad->result.ref=result;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_op_unary(enum mCc_tac_quad_unary_op,
                                               struct mCc_symtab_ref *arg1,
                                               struct mCc_symtab_ref *result){
    assert(arg1);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_OP_UNARY;
    quad->arg1=arg1;
    quad->result.ref=result;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_op_binary(enum mCc_tac_quad_binary_op op,
                                                struct mCc_symtab_ref *arg1,
                                                struct mCc_symtab_ref *arg2,
                                                struct mCc_symtab_ref *result){
    assert(arg1);
    assert(arg2);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }
    //TODO add mCc_tac_quad_binary_op op ? In a quadruple we can only add mCc_tac_quad_type not mCc_tac_quad_binary_op?
    quad->type=MCC_TAC_QUAD_OP_BINARY;
    quad->bin_op=op;
    quad->arg1=arg1;
    quad->arg2=arg2;
    quad->result.ref=result;
    return quad;

}

struct mCc_tac_quad *mCc_tac_quad_new_jump(mCc_tac_label label){

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_JUMP;
    quad->result.label=label;
    return quad;
}

struct mCc_tac_quad *
mCc_tac_quad_new_jumpfalse(struct mCc_symtab_ref *condition,
                           mCc_tac_label label){
    assert(condition);
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_JUMPFALSE;
    quad->arg1=condition;
    quad->result.label=label;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_label(mCc_tac_label label){

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }
    quad->type=MCC_TAC_QUAD_LABEL;
    quad->result.label=label;
}

struct mCc_tac_quad *mCc_tac_quad_new_param(struct mCc_symtab_ref *value){

    assert(arg1);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_PARAM;
    quad->arg1=value;

    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_call(mCc_tac_label label){

    assert(arg1);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_CALL;
    quad->arg1=label;
    return quad;

}

struct mCc_tac_quad *mCc_tac_quad_new_load(struct mCc_symtab_ref *array,
                                           struct mCc_symtab_ref *index,
                                           struct mCc_symtab_ref *result){
    assert(array);
    assert(index);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }
    quad->type=MCC_TAC_QUAD_LOAD;
    quad->arg1=array;
    quad->arg2=index;
    quad->result.ref=result;

    return quad;

}

struct mCc_tac_quad *mCc_tac_quad_new_store(struct mCc_symtab_ref *index,
                                            struct mCc_symtab_ref *value,
                                            struct mCc_symtab_ref *array){

    assert(array);
    assert(index);
    assert(value);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }
    quad->type=MCC_TAC_QUAD_STORE;
    quad->arg1=value;
    quad->arg2=index;
    quad->result.ref=array;

    return quad;

}


void mCc_tac_quad_print(struct mCc_tac_quad *self, FILE *out){

    assert(self);
    assert(out);

    //out = fopen("", "a+");

    switch (self->type) {
        case MCC_TAC_QUAD_ASSIGN:
            printf("%s = %s\n",self->result.ref->identifier->id_value,self->arg1->identifier->id_value);
            //fprintf(out, "%s = %s\n",self->result.ref->identifier->id_value,self->arg1->identifier->id_value);
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT:
            printf("%s = %s\n",self->result.ref->identifier->id_value,self->literal);
            break;
        case MCC_TAC_QUAD_OP_UNARY:
            printf_tac_unary_op(self, out);
            break;
        case MCC_TAC_QUAD_OP_BINARY:
            print_tac_bin_op(self,out);
            break;
        case MCC_TAC_QUAD_JUMP:
            printf("jump %s\n",self->result.label);
            break;
        case MCC_TAC_QUAD_JUMPFALSE:
            printf("jumpfalse %s %s\n",self->arg->identifier->id_value, self->result.label);
            break;
        case MCC_TAC_QUAD_LABEL:
            printf("Label %s\n",self->result.label);
            break;
        case MCC_TAC_QUAD_PARAM:
            printf("param %s\n", self->arg1->identifier->id_value);
            break;
        case MCC_TAC_QUAD_CALL:
            printf("call %s\n",self->arg1->identifier->id_value);
            break;
        case MCC_TAC_QUAD_LOAD:
            printf("%s = %s[%s]\n",self->result.ref->identifier->id_value,self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_QUAD_STORE:
            printf("%s[%s] = %s\n",self->result.ref->identifier->id_value, self->arg2->identifier->id_value,
                   self->arg1->identifier->id_value);
            break;
    }
    //fclose(out);
    return;
}

void print_tac_bin_op(struct mCc_tac_quad *self, FILE *out){
    switch (self->bin_op) {
        case MCC_TAC_EXP_TYPE_BINARY_OP_ADD:
            printf("%s = %s + %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_SUB:
            printf("%s = %s - %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_MUL:
            printf("%s = %s * %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_DIV:
            printf("%s = %s / %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_LT:
            printf("%s = %s < %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_GT:
            printf("%s = %s > %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_LEQ:
            printf("%s = %s <= %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_GEQ:
            printf("%s = %s >= %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_AND:
            printf("%s = %s && %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_OR:
            printf("%s = %s || %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_EQ:
            printf("%s = %s == %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_NEQ:
            printf("%s = %s != %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
            break;
        case MCC_TAC_EXPR_BINARY_OP_FLOAT_ADD:
            printf("%s = %s + %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_FLOAT_SUB:
            printf("%s = %s - %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_FLOAT_MUL:
            printf("%s = %s * %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_FLOAT_DIV:
            printf("%s = %s / %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
    }
    return;
}

void print_tac_unary_op(struct mCc_tac_quad *self, FILE *out) {
    switch (self->bin_op) {
        case MCC_TAC_EXPR_UNARY_NEG:
            printf("%s = -%s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXPR_UNARY_NOT:
            printf("%s = !%s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
    }
    return;
}


void mCc_tac_quad_delete(struct mCc_tac_quad *self){

}

struct mCc_tac_program *mCc_tac_program_new(int quad_alloc_size){

    struct mCc_tac_program *program = malloc(sizeof(*program));

    if(!program){
        return NULL;
    }
    program->quad_alloc_size=quad_alloc_size;
    program->quad_count=0;
  //  program->quads=NULL;                        // allocate memory if specified

    return program;
}
int mCc_tac_program_add_quad(struct mCc_tac_program *self,
                             struct mCc_tac_quad *quad){

}

int mCc_tac_program_add_program(struct mCc_tac_program *self,
                                struct mCc_tac_program *other_prog){

}

void mCc_tac_program_print(struct mCc_tac_program *self, FILE *out){

}

void mCc_tac_program_delete(struct mCc_tac_program *self,
                            bool delete_quads_too){

}