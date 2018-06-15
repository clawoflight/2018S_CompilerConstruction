#include "mCc/cfg_print.h"

static int first_func = 1;

void mCc_cfg_program_print(struct mCc_tac_program *self, FILE *out)
{
    assert(self);
    assert(out);
    for (unsigned int i = 0; i < self->quad_count; i++) {
        mCc_cfg_quad_print(self->quads[i], out);
    }
    fprintf(out, "}\n");
}

void mCc_cfg_print_literal(struct mCc_tac_quad *self, FILE *out){
    switch (self->literal->type) {
        case MCC_TAC_QUAD_LIT_INT:
            fprintf(out, "t%d = %d\\n", self->result.ref.number,
                    self->literal->ival);
            break;
        case MCC_TAC_QUAD_LIT_FLOAT:
            fprintf(out, "t%d = %f\\n", self->result.ref.number,
                    self->literal->fval);
            break;
        case MCC_TAC_QUAD_LIT_BOOL:
            fprintf(out, "t%d = %s\\n", self->result.ref.number,
                    self->literal->bval ? "true" : "false");
            break;
        case MCC_TAC_QUAD_LIT_STR:
            fprintf(out, "t%d = \\\"%s\\\"\\n", self->result.ref.number,
                    self->literal->strval);
            break;
    }
    return;
}

void mCc_cfg_print_unary_op(struct mCc_tac_quad *self, FILE *out){
    switch (self->un_op) {
        case MCC_TAC_OP_UNARY_NEG:
            fprintf(out, "t%d = -t%d\\n", self->result.ref.number,
                    self->arg1.number);
            break;
        case MCC_TAC_OP_UNARY_NOT:
            fprintf(out, "t%d = !t%d\\n", self->result.ref.number,
                    self->arg1.number);
            break;
    }
    return;
}

void mCc_cfg_print_bin_op(struct mCc_tac_quad *self, FILE *out){
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
            fprintf(out, "t%d = t%d / t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_LT:
            fprintf(out, "t%d = t%d < t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_GT:
            fprintf(out, "t%d = t%d > t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_LEQ:
            fprintf(out, "t%d = t%d <= t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_GEQ:
            fprintf(out, "t%d = t%d >= t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_AND:
            fprintf(out, "t%d = t%d && t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_OR:
            fprintf(out, "t%d = t%d || t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_EQ:
            fprintf(out, "t%d = t%d == t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_NEQ:
            fprintf(out, "t%d = t%d != t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_FLOAT_ADD:
            fprintf(out, "t%d = t%d + t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_FLOAT_SUB:
            fprintf(out, "t%d = t%d - t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_FLOAT_MUL:
            fprintf(out, "t%d = t%d * t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
        case MCC_TAC_OP_BINARY_FLOAT_DIV:
            fprintf(out, "t%d = t%d / t%d\\n", self->result.ref.number,
                    self->arg1.number, self->arg2.number);
            break;
    }
    return;
}

void mCc_cfg_quad_print(struct mCc_tac_quad *quad,FILE *out){
    switch (quad->type) {
        case MCC_TAC_QUAD_ASSIGN:
            fprintf(out, "t%d = t%d\\n", quad->result.ref.number,
                    quad->arg1.number);
            break;
        case MCC_TAC_QUAD_ASSIGN_LIT: mCc_cfg_print_literal(quad, out); break;
        case MCC_TAC_QUAD_OP_UNARY: mCc_cfg_print_unary_op(quad, out); break;
        case MCC_TAC_QUAD_OP_BINARY: mCc_cfg_print_bin_op(quad, out); break;
        case MCC_TAC_QUAD_LABEL:
            if (quad->result.label.num > -1) {
                fprintf(out, "\"];\n");
                fprintf(out,"%d -> %d;\n",quad->cfg_node.number,quad->cfg_node.next);
                fprintf(out, "%d [shape=box label=\"",quad->cfg_node.number);
            }
            else {
				if (first_func)
					first_func = 0;
				else
					fprintf(out, "} ");

                fprintf(out, "strict digraph \"%s\" {\n", quad->result.label.str);
                fprintf(out, "%d [label=\"Start %s\"];\n",quad->cfg_node.number-1,quad->result.label.str);  //TODO find better way to deal with func label numbers
                fprintf(out,"%d -> %d;\n",quad->cfg_node.number-1,quad->cfg_node.number);
                fprintf(out, "%d [shape=box label=\"",quad->cfg_node.number);
            }
            break;
        case MCC_TAC_QUAD_JUMP:
            fprintf(out,"%d -> %d;\n",quad->cfg_node.number,quad->cfg_node.next);   //TODO here the edge will be printed inside a label --> well, then create a new anonymous block?
            break;
        case MCC_TAC_QUAD_JUMPFALSE:
            fprintf(out, "\"];\n");
            fprintf(out,"%d -> %d [label=\"True\"];\n",quad->cfg_node.number,quad->cfg_node.next);
            fprintf(out,"%d -> %d [label=\"False\"];\n",quad->cfg_node.number,quad->cfg_node.next+1);	// jump to return
            fprintf(out, "%d [shape=box label=\"",quad->cfg_node.next);
            break;
        case MCC_TAC_QUAD_PARAM:
            fprintf(out, "param t%d\\n", quad->arg1.number);
            break;
        case MCC_TAC_QUAD_LOAD:
            fprintf(out, "t%d = t%d[t%d]\\n", quad->result.ref.number,
                    quad->arg1.number, quad->arg2.number);
            break;
        case MCC_TAC_QUAD_STORE:
            fprintf(out, "t%d[t%d] = t%d\\n", quad->result.ref.number,
                    quad->arg2.number, quad->arg1.number);
            break;
        case MCC_TAC_QUAD_CALL:
            if (quad->arg1.number >= 0)
                fprintf(out, "t%d = call %s\\n", quad->arg1.number,quad->result.label.str);
            else
                fprintf(out,"call %s\\n",quad->result.label.str);
            break;
        case MCC_TAC_QUAD_RETURN:
            fprintf(out, "return t%d\"];\n", quad->arg1.number);
            break;
        case MCC_TAC_QUAD_RETURN_VOID:
            fprintf(out, "return \"];\n");
            break;
    }
}
