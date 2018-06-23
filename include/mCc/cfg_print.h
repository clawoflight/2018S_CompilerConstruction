#ifndef MCC_CFG_H
#define MCC_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tac.h"
#include <string.h>

struct connect{
    int from;
    int to;
};

void mCc_cfg_program_print(struct mCc_tac_program *self, FILE *out);
void mCc_cfg_quad_print(struct mCc_tac_program *self, struct mCc_tac_quad *quad,FILE *out);
void mCc_cfg_print_connections(struct mCc_tac_program *self, FILE *out);

#ifdef __cplusplus
}
#endif
#endif // MCC_CFG_H