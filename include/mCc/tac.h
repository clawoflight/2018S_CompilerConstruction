/**
 * @file tac.h
 * @brief Declarations for the three-address code
 * @author bennett
 * @date 2018-04-27
 */
#include "symtab.h"
#include <stdbool.h>

/******************************** Data Structures */

typedef int mCc_tac_label;

/// Binary Operators
enum mCc_tac_quad_binary_op {
	MCC_TAC_EXP_TYPE_BINARY_OP_ADD,
	MCC_TAC_EXP_TYPE_BINARY_OP_SUB,
	MCC_TAC_EXP_TYPE_BINARY_OP_MUL,
	MCC_TAC_EXP_TYPE_BINARY_OP_DIV,

	MCC_TAC_EXPR_BINARY_OP_FLOAT_ADD,
	MCC_TAC_EXPR_BINARY_OP_FLOAT_SUB,
	MCC_TAC_EXPR_BINARY_OP_FLOAT_MUL,
	MCC_TAC_EXPR_BINARY_OP_FLOAT_DIV,

	MCC_TAC_EXP_TYPE_BINARY_OP_LT,
	MCC_TAC_EXP_TYPE_BINARY_OP_GT,
	MCC_TAC_EXP_TYPE_BINARY_OP_LEQ,
	MCC_TAC_EXP_TYPE_BINARY_OP_GEQ,
	MCC_TAC_EXP_TYPE_BINARY_OP_AND,
	MCC_TAC_EXP_TYPE_BINARY_OP_OR,
	MCC_TAC_EXP_TYPE_BINARY_OP_EQ,
	MCC_TAC_EXP_TYPE_BINARY_OP_NEQ
};

/// Unary Operators
enum mCc_tac_quad_unary_op {
	MCC_TAC_EXPR_UNARY_NEG,
	MCC_TAC_EXPR_UNARY_NOT,
};

/// Type of a TAC-stmt
enum mCc_tac_quad_type {
	MCC_TAC_QUAD_ASSIGN,
	MCC_TAC_QUAD_OP_UNARY,
	MCC_TAC_QUAD_OP_BINARY,
	MCC_TAC_QUAD_JUMP,
	MCC_TAC_QUAD_JUMPFALSE,
	MCC_TAC_QUAD_LABEL,
	MCC_TAC_QUAD_PARAM,
	MCC_TAC_QUAD_CALL,
	MCC_TAC_QUAD_LOAD,
	MCC_TAC_QUAD_STORE,
	MCC_TAC_QUAD_ADDR_OF,  ///< TODO neceessary?
	MCC_TAC_QUAD_PTR_DEREF ///< TODO necessary?
};

/// Result of a TAC-stmt
struct mCc_tac_quad_result {
	union {
		mCc_tac_label label;
		struct mCc_symtab_ref *ref;
	};
};

enum mCc_tac_quad_literal_type {
	MCC_TAC_QUAD_LIT_INT,
	MCC_TAC_QUAD_LIT_FLOAT,
	MCC_TAC_QUAD_LIT_BOOL,
	/// TODO maybe it would be better to store literals in the text segment?
	MCC_TAC_QUAD_LIT_STR
};

/// Literal type for flexibility
struct mCc_tac_quad_literal {
	enum mCc_tac_quad_literal_type type;

	union {
		int ival;
		float fval;
		bool bval;
		char *strval;
	}
}

/**
 * A single TAC-stmt, stored as quad.
 */
struct mCc_tac_quad {
	enum mCc_tac_quad_type type;
	char *comment; ///< Comment to add when printing :)

	struct mCc_tac_quad_literal literal; ///< Only for literals
	struct mCc_symtab_ref *arg1;
	struct mCc_symtab_ref *arg2;

	/// The result can be a reference or a label
	union {
		mCc_tac_label label;
		struct mCc_symtab_ref *ref;
	} result;
};

/**
 * @brief A program is a flat array of quads.
 *
 * This is our main TAC data structure.
 * When combining multiple programs, they shall be FLATTENED by copying the
 * quads into the new program using #mCc_tac_program_add_program
 */
struct mCc_tac_program {
	/// For how many quads memory was allocated in this program
	unsigned int quad_alloc_size;
	/// The number of quads in this program
	unsigned int quad_count;
	/// The quads contained in this program
	struct mCc_tac_quad **quads;
};

/********************************** Quad Functions */

struct mCc_tac_quad *mCc_tac_quad_new_assign(struct mCc_symtab_ref *arg1,
                                             struct mCc_symtab_ref *result);

struct mCc_tac_quad *mCc_tac_quad_new_op_unary(enum mCc_tac_quad_unary_op,
                                               struct mCc_symtab_ref *arg1,
                                               struct mCc_symtab_ref *result);

struct mCc_tac_quad *mCc_tac_quad_new_op_binary(enum mCc_tac_quad_binary_op,
                                                struct mCc_symtab_ref *arg1,
                                                struct mCc_symtab_ref *arg2,
                                                struct mCc_symtab_ref *result);

struct mCc_tac_quad *mCc_tac_quad_new_jump(mCc_tac_label label);

struct mCc_tac_quad *
mCc_tac_quad_new_jumpfalse(struct mCc_symtab_ref *condition,
                           mCc_tac_label label);

struct mCc_tac_quad *mCc_tac_quad_new_label(mCc_tac_label label);

struct mCc_tac_quad *mCc_tac_quad_new_param(struct mCc_symtab_ref *value);

struct mCc_tac_quad *mCc_tac_quad_new_call(mCc_tac_label label);

struct mCc_tac_quad *mCc_tac_quad_new_load(struct mCc_symtab_ref *array,
                                           struct mCc_symtab_ref *index,
                                           struct mCc_symtab_ref *result);

struct mCc_tac_quad *mCc_tac_quad_new_store(struct mCc_symtab_ref *index,
                                            struct mCc_symtab_ref *value,
                                            struct mCc_symtab_ref *array);

/**
 * @brief Print a quad.
 *
 * @param self The quad to print
 * @param out The file to which to print
 */
void mCc_tac_quad_print(struct mCc_tac_quad *self, FILE *out);

void mCc_tac_quad_delete(struct mCc_tac_quad *self);

/********************************** Program Functions */

/**
 * @brief Create a new program.
 *
 * @param quad_alloc_size Number of quads for which memory is to be reserved
 *
 * @return The new program, with memory pre-allocated if specified
 */
struct mCc_tac_program *mCc_tac_program_new(int quad_alloc_size);

/**
 * @brief Append a quad to a program.
 *
 * @param self The program
 * @param quad The quad to append
 *
 * @return 0 on success, non-zero on memory error
 */
int mCc_tac_program_add_quad(struct mCc_tac_program *self,
                             struct mCc_tac_quad *quad);

/**
 * @brief Append a program to a program.
 *
 * by copying all quad pointers from the old program to the end of the new one,
 * calling #mCc_tac_program_add_quad for each. This creates a FLAT program. No
 * Nesting!
 *
 * @param self The program to which to add
 * @param other_prog The program whose quads to copy over
 *
 * @return 0 on success, non-zero on memory error
 */
int mCc_tac_program_add_program(struct mCc_tac_program *self,
                                struct mCc_tac_program *other_prog);

/**
 * @brief Print a program by serially printing it's quads.
 *
 * @param self The program to print
 * @param out The file to which to print
 */
void mCc_tac_program_print(struct mCc_tac_program *self, FILE *out);

/**
 * @brief Delete a program.
 *
 * When copying to a new program, don't delete the quads in the old one!
 *
 * @param self The program to delete
 * @param delete_quads_too Whether to delete all quads contained in the program
 */
void mCc_tac_program_delete(struct mCc_tac_program *self,
                            bool delete_quads_too);
