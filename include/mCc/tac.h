/**
 * @file tac.h
 * @brief Declarations for the three-address code
 * @author bennett
 * @date 2018-04-27
 */
#ifndef MCC_TAC_H
#define MCC_TAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/******************************** Data Structures */

/// Size by which to increase compound_stmts when reallocing
static const int quad_alloc_block_size = 10;

/// Binary Operators
enum mCc_tac_quad_binary_op {
	MCC_TAC_OP_BINARY_ADD,
	MCC_TAC_OP_BINARY_SUB,
	MCC_TAC_OP_BINARY_MUL,
	MCC_TAC_OP_BINARY_DIV,

	MCC_TAC_OP_BINARY_FLOAT_ADD,
	MCC_TAC_OP_BINARY_FLOAT_SUB,
	MCC_TAC_OP_BINARY_FLOAT_MUL,
	MCC_TAC_OP_BINARY_FLOAT_DIV,

	MCC_TAC_OP_BINARY_LT,
	MCC_TAC_OP_BINARY_GT,
	MCC_TAC_OP_BINARY_LEQ,
	MCC_TAC_OP_BINARY_GEQ,
	MCC_TAC_OP_BINARY_AND,
	MCC_TAC_OP_BINARY_OR,
	MCC_TAC_OP_BINARY_EQ,
	MCC_TAC_OP_BINARY_NEQ
};

/// Unary Operators
enum mCc_tac_quad_unary_op {
	MCC_TAC_OP_UNARY_NEG,
	MCC_TAC_OP_UNARY_NOT,
};

/// Type of a TAC-stmt
enum mCc_tac_quad_type {
	MCC_TAC_QUAD_ASSIGN,
	MCC_TAC_QUAD_ASSIGN_LIT,
	MCC_TAC_QUAD_OP_UNARY,
	MCC_TAC_QUAD_OP_BINARY,
	MCC_TAC_QUAD_JUMP,
	MCC_TAC_QUAD_JUMPFALSE,
	MCC_TAC_QUAD_LABEL,
	MCC_TAC_QUAD_PARAM,
	MCC_TAC_QUAD_CALL,
	MCC_TAC_QUAD_LOAD,
	MCC_TAC_QUAD_STORE,
	MCC_TAC_QUAD_RETURN,
	MCC_TAC_QUAD_RETURN_VOID,
	// MCC_TAC_QUAD_ADDR_OF,  ///< TODO neceessary?
	// MCC_TAC_QUAD_PTR_DEREF ///< TODO necessary?
};

enum mCc_tac_quad_literal_type {
	MCC_TAC_QUAD_LIT_INT,
	MCC_TAC_QUAD_LIT_FLOAT,
	MCC_TAC_QUAD_LIT_BOOL,
	/// TODO maybe it would be better to store literals in the data segment?
	MCC_TAC_QUAD_LIT_STR,
	MCC_TAC_QUAD_LIT_VOID ///For error catching and void function catching
};

/// Literal type for flexibility
struct mCc_tac_quad_literal {
	enum mCc_tac_quad_literal_type type;

	union {
		int ival;
		float fval;
		bool bval;
		char *strval;
	};
	int label_num; ///< Optional, for strings
};

#define MCC_TAC_LABEL_LEN (4096)
/// Label with two alternative options
struct mCc_tac_label {
    char str[MCC_TAC_LABEL_LEN]; /// For function labels
    int num;                     /// For anonymous labels
    enum mCc_tac_quad_literal_type type; /// (Optional)For correct stack allocation later
};

/// this struct is the used as the type of the quad entries
#define MCC_TAC_STRING_LEN (4096)
struct mCc_tac_quad_entry {
	int number; /// Temporary. -1 will be used as array pointer to params
	char str_value[MCC_TAC_STRING_LEN]; /// (Optional)For Strings
    enum mCc_tac_quad_literal_type type; /// (Optional)For correct stack allocation later
    struct mCc_tac_label binary_label; /// (Optional)For bool bin_ops
};

/**
 * A single TAC-stmt, stored as quad.
 */
struct mCc_tac_quad {
	enum mCc_tac_quad_type type;
	char *comment; ///< Comment to add when printing :)
	union {
		struct mCc_tac_quad_literal *literal; ///< Only for literals
		enum mCc_tac_quad_binary_op bin_op;
		enum mCc_tac_quad_unary_op un_op;
	};
	struct mCc_tac_quad_entry arg1;
	struct mCc_tac_quad_entry arg2;

	/// The result can be a reference or a label
	union {
		struct mCc_tac_label label;
		struct mCc_tac_quad_entry ref;
	} result;
	/// variable count for assembly
	unsigned int var_count;
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

	/// String literals used in the program
	struct mCc_tac_quad_entry *string_literals;
	unsigned int string_literal_count;
};

/********************************** Quad Functions */

struct mCc_tac_quad_entry mCc_tac_create_new_entry();
struct mCc_tac_quad_entry mCc_tac_create_new_string();
struct mCc_tac_label mCc_tac_get_new_label();

struct mCc_tac_quad *mCc_tac_quad_new_assign(struct mCc_tac_quad_entry arg1,
                                             struct mCc_tac_quad_entry result);

struct mCc_tac_quad *
mCc_tac_quad_new_assign_lit(struct mCc_tac_quad_literal *arg1,
                            struct mCc_tac_quad_entry result);

struct mCc_tac_quad *
mCc_tac_quad_new_op_unary(enum mCc_tac_quad_unary_op op,
                          struct mCc_tac_quad_entry arg1,
                          struct mCc_tac_quad_entry result);

struct mCc_tac_quad *mCc_tac_quad_new_op_binary(
    enum mCc_tac_quad_binary_op op, struct mCc_tac_quad_entry arg1,
    struct mCc_tac_quad_entry arg2, struct mCc_tac_quad_entry result);

/**
 * @return  new quadruple in the style: MCC_TAC_QUAD_JUMP - - label
 */
struct mCc_tac_quad *mCc_tac_quad_new_jump(struct mCc_tac_label label);

/**
 * New quadruple in the style MCC_TAC_QUAD_JUMPFALSE condition Label -
 */
struct mCc_tac_quad *
mCc_tac_quad_new_jumpfalse(struct mCc_tac_quad_entry condition,
                           struct mCc_tac_label label);

struct mCc_tac_quad *mCc_tac_quad_new_label(struct mCc_tac_label label);
/**
 * New quadruple in the style MCC_TAC_QUAD_PARAM value - -
 */
struct mCc_tac_quad *mCc_tac_quad_new_param(struct mCc_tac_quad_entry value);

/**
 * new "goto" quadruple MCC_TAC_QUAD_CALL Label - -
 */
struct mCc_tac_quad *mCc_tac_quad_new_call(struct mCc_tac_label label,
                                           struct mCc_tac_quad_entry result);
/**
 * Loading a value from an array and saving it
 * @return a quadruple in the style MCC_TAC_QUAD_LOAD array index result
 */
struct mCc_tac_quad *mCc_tac_quad_new_load(struct mCc_tac_quad_entry array,
                                           struct mCc_tac_quad_entry index,

                                           struct mCc_tac_quad_entry result);

struct mCc_tac_quad *mCc_tac_quad_new_return_void();
struct mCc_tac_quad *
mCc_tac_quad_new_return(struct mCc_tac_quad_entry ret_value);

/**
 * Storing an value in an index of an array
 * @return a quadruple in the style MCC_TAC_QUAD_STORE value index array
 */
struct mCc_tac_quad *mCc_tac_quad_new_store(struct mCc_tac_quad_entry index,
                                            struct mCc_tac_quad_entry value,
                                            struct mCc_tac_quad_entry array);

/**
 * @brief Print a quad.
 * @param self
 * @param out
 */
void mCc_tac_quad_print(struct mCc_tac_quad *self, FILE *out);

void mCc_tac_quad_delete(struct mCc_tac_quad *self);
void mCc_tac_quad_entry_delete(struct mCc_tac_quad_entry entry);
void mCc_tac_quad_literal_delete(struct mCc_tac_quad_literal *lit);

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
 * @brief Print a program by serially printing it's quads.
 *
 * @param self The program to print
 * @param out The file to which to print
 */
void mCc_tac_program_print(struct mCc_tac_program *self, FILE *out);

/**
 * @brief Delete a program.
 *
 * @param self The program to delete
 */
void mCc_tac_program_delete(struct mCc_tac_program *self);

#ifdef __cplusplus
}
#endif
#endif // MCC_TAC_H
