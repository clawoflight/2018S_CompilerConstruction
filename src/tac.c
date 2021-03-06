/**
 * @file tac.c
 * @brief Implementation of the tac.
 * @author bennett
 * @date 2018-04-27
 *
 */
#include "mCc/tac.h"
#include "mCc/ast.h"
#include <assert.h>
#include <string.h>

struct mCc_tac_quad_entry mCc_tac_create_new_entry() {
    static int current_var = 0;

    struct mCc_tac_quad_entry entry;

    entry.number = current_var;
    current_var++;
    entry.array_size = 0;

    return entry;
}

struct mCc_tac_quad_entry mCc_tac_create_new_string() {
    static int current_string = 0;
    struct mCc_tac_quad_entry entry = mCc_tac_create_new_entry();

    entry.str_number = current_string;
    current_string++;

    return entry;
}

struct mCc_tac_label mCc_tac_get_new_label() {
    static int current_lab = 0;

    struct mCc_tac_label label = {0};

    label.num = current_lab;
    current_lab++;
    return label;
}

struct mCc_tac_quad *mCc_tac_quad_new_assign(struct mCc_tac_quad_entry arg1,
                                             struct mCc_tac_quad_entry result) {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_ASSIGN;
    quad->arg1 = arg1;
    quad->result.ref = result;
    quad->result.ref.type = arg1.type;
    return quad;
}

struct mCc_tac_quad *
mCc_tac_quad_new_assign_lit(struct mCc_tac_quad_literal *arg1,
                            struct mCc_tac_quad_entry result) {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_ASSIGN_LIT;
    quad->literal = arg1;
    quad->result.ref = result;
    quad->result.ref.type = arg1->type;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_op_unary(enum mCc_tac_quad_unary_op op,
                                               struct mCc_tac_quad_entry arg1,
                                               struct mCc_tac_quad_entry result) {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_OP_UNARY;
    quad->un_op = op;
    quad->arg1 = arg1;
    quad->result.ref = result;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_op_binary(
        enum mCc_tac_quad_binary_op op, struct mCc_tac_quad_entry arg1,
        struct mCc_tac_quad_entry arg2, struct mCc_tac_quad_entry result) {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_OP_BINARY;
    quad->bin_op = op;
    quad->arg1 = arg1;
    quad->arg2 = arg2;
    quad->result.ref = result;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_jump(struct mCc_tac_label label) {

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_JUMP;
    quad->result.label = label;
    return quad;
}

struct mCc_tac_quad *
mCc_tac_quad_new_jumpfalse(struct mCc_tac_quad_entry condition,
                           struct mCc_tac_label label) {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_JUMPFALSE;
    quad->arg1 = condition;
    quad->result.label = label;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_label(struct mCc_tac_label label) {

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_LABEL;
    quad->result.label = label;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_param(struct mCc_tac_quad_entry value) {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_PARAM;
    quad->arg1 = value;

    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_call(struct mCc_tac_label label,
                                           unsigned int param_count,
                                           struct mCc_tac_quad_entry result) {

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_CALL;
    quad->arg1 = result;
    quad->result.label = label;
    quad->result.label.type = result.type;
    quad->var_count = param_count;
    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_load(struct mCc_tac_quad_entry array,
                                           struct mCc_tac_quad_entry index,
                                           struct mCc_tac_quad_entry result) {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_LOAD;
    quad->arg1 = array;
    quad->arg2 = index;
    quad->result.ref = result;
    quad->result.ref.type = index.type;

    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_store(struct mCc_tac_quad_entry index,
                                            struct mCc_tac_quad_entry value,
                                            struct mCc_tac_quad_entry array) {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));
    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_STORE;
    quad->arg1 = value;
    quad->arg2 = index;
    quad->result.ref = array;

    return quad;
}

struct mCc_tac_quad *mCc_tac_quad_new_return_void() {
    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_RETURN_VOID;

    return quad;
}

struct mCc_tac_quad *
mCc_tac_quad_new_return(struct mCc_tac_quad_entry ret_value) {

    struct mCc_tac_quad *quad = malloc(sizeof(*quad));

    if (!quad) {
        return NULL;
    }

    quad->comment = NULL;
    quad->type = MCC_TAC_QUAD_RETURN;
    quad->arg1 = ret_value;

    return quad;
}

static inline void mCc_tac_print_label(struct mCc_tac_label label, FILE *out) {
    if (label.str[0]) {
        fputs(label.str, out);
    } else {
        fprintf(out, "L%d", label.num);
    }
}

static void mCc_tac_print_bin_op(struct mCc_tac_quad *self, FILE *out) {
    switch (self->bin_op) {
        case MCC_TAC_OP_BINARY_ADD:
            fprintf(out, "\tt%d = t%d + t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_SUB:
            fprintf(out, "\tt%d = t%d - t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_MUL:
            fprintf(out, "\tt%d = t%d * t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_DIV:
            fprintf(out, "\t%d = t%d / t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_LT:
            fprintf(out, "\tt%d = t%d < t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_GT:
            fprintf(out, "\tt%d = t%d > t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_LEQ:
            fprintf(out, "\tt%d = t%d <= t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_GEQ:
            fprintf(out, "\tt%d = t%d >= t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_AND:
            fprintf(out, "\tt%d = t%d && t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_OR:
            fprintf(out, "\tt%d = t%d || t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_EQ:
            fprintf(out, "\tt%d = t%d == t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_NEQ:
            fprintf(out, "\tt%d = t%d != t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_FLOAT_ADD:
            fprintf(out, "\tt%d = t%d + t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_FLOAT_SUB:
            fprintf(out, "\tt%d = t%d - t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_FLOAT_MUL:
            fprintf(out, "\tt%d = t%d * t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_FLOAT_DIV:
            fprintf(out, "\tt%d = t%d / t%d\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
    }
    return;
}

static void mCc_tac_print_unary_op(struct mCc_tac_quad *self, FILE *out) {
    switch (self->un_op) {
        case MCC_TAC_OP_UNARY_NEG:
            fprintf(out, "\tt%d = -t%d\n", self->result.ref.number,
                    self->arg1.number);
            break;
        case MCC_TAC_OP_UNARY_NOT:
            fprintf(out, "\tt%d = !t%d\n", self->result.ref.number,
                    self->arg1.number);
            break;
    }
    return;
}

static void mCc_tac_print_literal(struct mCc_tac_quad *self, FILE *out) {
    switch (self->literal->type) {
        case MCC_TAC_QUAD_LIT_INT:
            fprintf(out, "\tt%d = %d\n", self->result.ref.number,
                    self->literal->ival);
            break;
        case MCC_TAC_QUAD_LIT_FLOAT:
            fprintf(out, "\tt%d = %f\n", self->result.ref.number,
                    self->literal->fval);
            break;
        case MCC_TAC_QUAD_LIT_BOOL:
            fprintf(out, "\tt%d = %s\n", self->result.ref.number,
                    self->literal->bval ? "true" : "false");
            break;
        case MCC_TAC_QUAD_LIT_STR:
            fprintf(out, "\tt%d = \"%s\"\n", self->result.ref.number,
                    self->literal->strval);
            break;
        case MCC_TAC_QUAD_LIT_VOID:
            break;
    }
    return;
}

void mCc_tac_quad_print(struct mCc_tac_quad *self, FILE *out) {

    assert(self);
    assert(out);
    if (self->comment) {
        fprintf(out, "; %s\n", self->comment);
    }
    switch (self->type) {
        case MCC_TAC_QUAD_ASSIGN:
            fprintf(out, "\tt%d = t%d\n", self->result.ref.number,
                    self->arg1.number);
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT:
            mCc_tac_print_literal(self, out);
            break;
        case MCC_TAC_QUAD_OP_UNARY:
            mCc_tac_print_unary_op(self, out);
            break;
        case MCC_TAC_QUAD_OP_BINARY:
            mCc_tac_print_bin_op(self, out);
            break;
        case MCC_TAC_QUAD_JUMP:
            fputs("\tjump ", out);
            mCc_tac_print_label(self->result.label, out);
            fputc('\n', out);
            break;
        case MCC_TAC_QUAD_JUMPFALSE:
            fprintf(out, "\tjumpfalse t%d ", self->arg1.number);
            mCc_tac_print_label(self->result.label, out);
            fputc('\n', out);
            break;
        case MCC_TAC_QUAD_LABEL:
            mCc_tac_print_label(self->result.label, out);
            fputs(":\n", out);
            break;
        case MCC_TAC_QUAD_PARAM:
            fprintf(out, "\tparam t%d\n", self->arg1.number);
            break;
        case MCC_TAC_QUAD_CALL:
            if (self->arg1.number >= 0)
                fprintf(out, "\tt%d = call ", self->arg1.number);
            else
                fputs("\tcall ", out);
            mCc_tac_print_label(self->result.label, out);
            fputc('\n', out);
            break;
        case MCC_TAC_QUAD_LOAD:
            fprintf(out, "\tt%d = t%d[t%d]\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_QUAD_STORE:
            fprintf(out, "\tt%d[t%d] = t%d\n", self->result.ref.number,
                    self->arg2.number, self->arg1.number);
            break;
        case MCC_TAC_QUAD_RETURN:
            fprintf(out, "\treturn t%d\n", self->arg1.number);
            break;
        case MCC_TAC_QUAD_RETURN_VOID:
            fprintf(out, "\treturn \n");
            break;
    }
    return;
}

void mCc_tac_quad_delete(struct mCc_tac_quad *self) {
    assert(self);

    switch (self->type) {
        case MCC_TAC_QUAD_ASSIGN:
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT:
            mCc_tac_quad_literal_delete(self->literal);
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
    // Don't free comment because that is a string literal

    free(self);
    return;
}

void mCc_tac_quad_literal_delete(struct mCc_tac_quad_literal *lit) {
    free(lit);
    return;
}

struct mCc_tac_program *mCc_tac_program_new(int quad_alloc_size) {

    struct mCc_tac_program *program = malloc(sizeof(*program));

    if (!program) {
        return NULL;
    }
    program->quad_alloc_size = quad_alloc_size;
    program->quad_count = 0;
    program->quads = NULL;

    if (quad_alloc_size > 0) { // allocate memory if specified
        if ((program->quads =
                     malloc(quad_alloc_size * sizeof(*program->quads))) == NULL) {
            free(program);
            return NULL;
        }
    }

    return program;
}

struct mCc_tac_program *mCc_tac_new_cfg(struct mCc_tac_program *self,
                                        int cfg_alloc_size) {
    assert(self);
    self->cfg_alloc_size = cfg_alloc_size;
    self->cfg_count = 0;
    self->cfgs = NULL;

    if (cfg_alloc_size > 0) { // allocate memory if specified
        if ((self->cfgs =
                     malloc(cfg_alloc_size * sizeof(*self->cfgs))) == NULL) {
            free(self);
            return NULL;
        }
    }
    return self;
}

int mCc_tac_program_add_quad(struct mCc_tac_program *self,
                             struct mCc_tac_quad *quad) {
    assert(self);
    assert(quad);

    if (self->quad_count < self->quad_alloc_size) {
        self->quads[self->quad_count++] = quad;
        return 0;
    }
    // Allocate additional memory if necessary
    struct mCc_tac_quad **tmp;
    self->quad_alloc_size += quad_alloc_block_size;
    if ((tmp = realloc(self->quads, self->quad_alloc_size * sizeof(*tmp))) ==
        NULL) {
        mCc_tac_program_delete(self);
        return 1;
    }

    self->quads = tmp;
    self->quads[self->quad_count++] = quad;
    return 0;
}

int mCc_tac_program_add_cfg(struct mCc_tac_program *self,
                            char *from_label, int from_number, char *to_label, int to_number, char *label) {
    assert(self);
    char connection[MAX_NAME_LENGTH];

    //if a function we dont need a number
    if ((strcmp("", from_label) != 0) && (strcmp("L", from_label) != 0)) {
        sprintf(connection,
                "%s -> %s%d \n[label=\"%s\"];\n", from_label, to_label, to_number, label);
    } else {
        sprintf(connection,
                "%s%d -> %s%d \n[label=\"%s\"];\n", from_label, from_number, to_label, to_number, label);
    }


    if (self->cfg_count < self->cfg_alloc_size) {
        self->cfgs[self->cfg_count] = malloc(sizeof(connection));
        strcpy(self->cfgs[self->cfg_count++], connection); // = de connection bauen
        return 0;
    }
    // Allocate additional memory if necessary
    char **tmp;
    self->cfg_alloc_size += cfg_alloc_block_size;
    if ((tmp = realloc(self->cfgs, self->cfg_alloc_size * sizeof(*tmp))) ==
        NULL) {
        mCc_tac_program_delete(self);
        return 1;
    }

    self->cfgs = tmp;
    self->cfgs[self->cfg_count] = malloc(sizeof(connection));
    strcpy(self->cfgs[self->cfg_count++], connection); //connection = String
    return 0;
}

void mCc_tac_program_print(struct mCc_tac_program *self, FILE *out) {
    assert(self);
    assert(out);
    for (unsigned int i = 0; i < self->quad_count; i++) {
        mCc_tac_quad_print(self->quads[i], out);
    }
}

void mCc_tac_program_delete(struct mCc_tac_program *self) {
    assert(self);
    for (unsigned int i = 0; i < self->quad_count; i++) {
        mCc_tac_quad_delete(self->quads[i]);
    }
    free(self->quads);
    free(self);
}
