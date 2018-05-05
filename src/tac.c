/**
 * @file tac.c
 * @brief Implementation of the tac.
 * @author bennett
 * @date 2018-04-27
 *
 */
#include <stdbool.h>
#include <stdlib.h>
#include "mCc/tac.h"

struct mCc_tac_quad *mCc_tac_quad_new_assign(struct mCc_tac_quad_entry *arg1,
                                             struct mCc_tac_quad_entry *result){
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

struct mCc_tac_quad *mCc_tac_quad_new_assign_lit(struct mCc_tac_quad_literal *arg1,
                                                 struct mCc_tac_quad_entry *result){
    assert(arg1);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_ASSIGN_LIT;
    quad->literal=arg1;
    quad->result.ref=result;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_op_unary(enum mCc_tac_quad_unary_op op,
                                               struct mCc_tac_quad_entry *arg1,
                                               struct mCc_tac_quad_entry *result){
    assert(arg1);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_OP_UNARY;
    quad->un_op=op;
    quad->arg1=arg1;
    quad->result.ref=result;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_op_binary(enum mCc_tac_quad_binary_op op,
                                                struct mCc_tac_quad_entry *arg1,
                                                struct mCc_tac_quad_entry *arg2,
                                                struct mCc_tac_quad_entry *result){
    assert(arg1);
    assert(arg2);
    assert(result);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

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
mCc_tac_quad_new_jumpfalse(struct mCc_tac_quad_entry *condition,
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

struct mCc_tac_quad *mCc_tac_quad_new_param(struct mCc_tac_quad_entry *value){

    assert(value);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_PARAM;
    quad->arg1=value;

    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_call(mCc_tac_label label){

    assert(label);

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->type=MCC_TAC_QUAD_CALL;
    quad->arg1=label;
    return quad;

}

struct mCc_tac_quad *mCc_tac_quad_new_load(struct mCc_tac_quad_entry *array,
                                           struct mCc_tac_quad_entry *index,
                                           struct mCc_tac_quad_entry *result){
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

struct mCc_tac_quad *mCc_tac_quad_new_store(struct mCc_tac_quad_entry *index,
                                            struct mCc_tac_quad_entry *value,
                                            struct mCc_tac_quad_entry *array){

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

void print_tac_bin_op(struct mCc_tac_quad *self, FILE *out){
    switch (self->bin_op) {
        case MCC_TAC_EXP_TYPE_BINARY_OP_ADD:
            printf("%s = %s + %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s + %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                    self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_SUB:
            printf("%s = %s - %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s - %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                    self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_MUL:
            printf("%s = %s * %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s * %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                    self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_DIV:
            printf("%s = %s / %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s / %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                    self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_LT:
            printf("%s = %s < %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
                   fprintf(out,"%s = %s < %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_GT:
            printf("%s = %s > %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
                   fprintf(out,"%s = %s > %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_LEQ:
            printf("%s = %s <= %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
                   fprintf(out,"%s = %s <= %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_GEQ:
            printf("%s = %s >= %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
                   fprintf(out,"%s = %s >= %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_AND:
            printf("%s = %s && %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
                   fprintf(out,"%s = %s && %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_OR:
            printf("%s = %s || %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
                   fprintf(out,"%s = %s || %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_EQ:
            printf("%s = %s == %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
                   fprintf(out,"%s = %s == %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXP_TYPE_BINARY_OP_NEQ:
            printf("%s = %s != %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
                   fprintf(out,"%s = %s != %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXP_BINARY_OP_FLOAT_ADD:
            printf("%s = %s + %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s + %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                    self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_BINARY_OP_FLOAT_SUB:
            printf("%s = %s - %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s - %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                    self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_BINARY_OP_FLOAT_MUL:
            printf("%s = %s * %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s * %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                    self->arg2->identifier->id_value);
            break;
        case MCC_TAC_EXP_BINARY_OP_FLOAT_DIV:
            printf("%s = %s / %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s / %s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value,
                    self->arg2->identifier->id_value);
            break;
    }
    return;
}

void print_tac_unary_op(struct mCc_tac_quad *self, FILE *out) {
    switch (self->un_op) {
        case MCC_TAC_EXPR_UNARY_NEG:
            printf("%s = -%s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            fprintf(out,"%s = -%s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
        case MCC_TAC_EXPR_UNARY_NOT:
            printf("%s = !%s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            fprintf(out,"%s = !%s", self->result.ref->identifier->id_value, self->arg1->identifier->id_value);
            break;
    }
    return;
}



void mCc_tac_quad_print(struct mCc_tac_quad *self, FILE *out){

    assert(self);
    assert(out);

    switch (self->type) {
        case MCC_TAC_QUAD_ASSIGN:
            printf("%s = %s\n",self->result.ref->identifier->id_value,self->arg1->identifier->id_value);
            fprintf(out, "%s = %s\n",self->result.ref->identifier->id_value,self->arg1->identifier->id_value);
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT:
            printf("%s = %s\n",self->result.ref->identifier->id_value,self->literal);
            fprintf(out, "%s = %s\n",self->result.ref->identifier->id_value,self->arg1->identifier->id_value);
            break;
        case MCC_TAC_QUAD_OP_UNARY:
            print_tac_unary_op(self, out);
            break;
        case MCC_TAC_QUAD_OP_BINARY:
            print_tac_bin_op(self,out);
            break;
        case MCC_TAC_QUAD_JUMP:
            printf("jump %s\n",self->result.label);
            fprintf(out, "jump %s\n",self->result.label);
            break;
        case MCC_TAC_QUAD_JUMPFALSE:
            printf("jumpfalse %s %s\n",self->arg->identifier->id_value, self->result.label);
            fprintf(out, "jumpfalse %s %s\n",self->arg->identifier->id_value, self->result.label);
            break;
        case MCC_TAC_QUAD_LABEL:
            printf("Label %s\n",self->result.label);
            fprintf(out,"Label %s\n",self->result.label);
            break;
        case MCC_TAC_QUAD_PARAM:
            printf("param %s\n", self->arg1->identifier->id_value);
            fprintf(out,"param %s\n", self->arg1->identifier->id_value);
            break;
        case MCC_TAC_QUAD_CALL:
            printf("call %s\n",self->arg1->identifier->id_value);
            fprintf(out,"call %s\n",self->arg1->identifier->id_value);
            break;
        case MCC_TAC_QUAD_LOAD:
            printf("%s = %s[%s]\n",self->result.ref->identifier->id_value,self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            fprintf(out,"%s = %s[%s]\n",self->result.ref->identifier->id_value,self->arg1->identifier->id_value,
                   self->arg2->identifier->id_value);
            break;
        case MCC_TAC_QUAD_STORE:
            printf("%s[%s] = %s\n",self->result.ref->identifier->id_value, self->arg2->identifier->id_value,
                   self->arg1->identifier->id_value);
            fprintf(out,"%s[%s] = %s\n",self->result.ref->identifier->id_value, self->arg2->identifier->id_value,
                   self->arg1->identifier->id_value);
            break;
    }
    return;
}

void mCc_tac_quad_delete(struct mCc_tac_quad *self){
    //TODO implement
}

struct mCc_tac_program *mCc_tac_program_new(int quad_alloc_size){

    struct mCc_tac_program *program = malloc(sizeof(*program));

    if(!program){
        return NULL;
    }
    program->quad_alloc_size=quad_alloc_size;
    program->quad_count=0;
    program->quads=NULL;

    if(quad_alloc_size>0 ){ //allocate memory if specified
        if((program->quads = malloc(quad_alloc_size * sizeof(program->quad))) == NULL) {
            return NULL;
        }
    }

    return program;
}
int mCc_tac_program_add_quad(struct mCc_tac_program *self,
                             struct mCc_tac_quad *quad){
    assert(self);
    assert(quad);

    if (self->quad_count < self->quad_alloc_size) {
        self->quads[self->quad_count++] = quad;
        return 0;
    }
    // Allocate additional memory if necessary
    struct mCc_tac_quad **tmp;
    self->quad_alloc_size += quad_alloc_block_size;
    if ((tmp = realloc(self->quads, self->quad_alloc_size *
                                             sizeof(self))) == NULL) {
        mCc_ast_delete_quad(self);
        return 1;
    }

    self->quads = tmp;
    self->quads[self->quad_count++] = quad;
    return 0;
}

int mCc_tac_program_add_program(struct mCc_tac_program *self,
                                struct mCc_tac_program *other_prog){
    assert(self);
    assert(other_prog);

    self->quad_count=other_prog->quad_count;
    self->quad_alloc_size=other_prog->quad_alloc_size;

    for (int i=0;i<other_prog->quad_count; i++){
        if(mCc_tac_program_add_quad(self,other_prog[i])!=0){
            mCc_tac_mCc_tac_program_delete(other_prog,true);
            mCc_tac_mCc_tac_program_delete(self,true);
            return 1;
        }
    }
    mCc_tac_program_delete(other_prog, false);

    return 0;
}

void mCc_tac_program_print(struct mCc_tac_program *self, FILE *out){
    assert(self);
    assert(out);
    for (int i =0; i < quad_count; i++){
        mCc_tac_quad_print(self->quads[i],out);
    }
    return ;
}

void mCc_tac_program_delete(struct mCc_tac_program *self,
                            bool delete_quads_too){
    if(delete_quads_too){
        for(int i =0;i < self->quad_count;i++){
            mCc_tac_quad_delete(self->quads[i]);
        }
    }
    free(self);
}