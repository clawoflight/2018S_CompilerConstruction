/**
 * @file tac.h
 * @brief Declarations for the three-address code
 * @author bennett
 * @date 2018-04-27
 */
#include <stdbool.h>

/******************************** Data Structures */

typedef int mCc_tac_label;

enum mCc_tac_quad_type {
	MCC_TAC_QUAD_COPY,
	MCC_TAC_QUAD_OP_UNARY,
	MCC_TAC_QUAD_OP_BINARY,
	MCC_TAC_QUAD_JUMP,
	MCC_TAC_QUAD_JUMPFALSE,
	MCC_TAC_QUAD_PARAM,
	MCC_TAC_QUAD_CALL,
	MCC_TAC_QUAD_LOAD,
	MCC_TAC_QUAD_STORE
};

struct mCc_tac_quad {
	enum mCc_tac_quad_type type;
	char *comment; // remove if unneeded

	union {

	}
};

/**
 * @brief A program is a flat array of quads.
 *
 * This is our main TAC data structure.
 * When combining multiple programs, they shall be FLATTENED by copying the quads into the new program using #mCc_tac_program_add_program
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

struct mCc_tac_quad *mCc_tac_quad_new_TODO(TODO);

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
int mCc_tac_program_add_quad(struct mCc_tac_program *self, struct mCc_tac_quad *quad);

/**
 * @brief Append a program to a program.
 *
 * by copying all quad pointers from the old program to the end of the new one, calling #mCc_tac_program_add_quad for each.
 * This creates a FLAT program. No Nesting!
 *
 * @param self The program to which to add
 * @param other_prog The program whose quads to copy over
 *
 * @return 0 on success, non-zero on memory error
 */
int mCc_tac_program_add_program(struct mCc_tac_program *self, struct mCc_tac_program *other_prog);

/**
 * @brief Delete a program.
 *
 * When copying to a new program, don't delete the quads in the old one!
 *
 * @param self The program to delete
 * @param delete_quads_too Whether to delete all quads contained in the program
 */
void mCc_tac_program_delete(struct mCc_tac_program *self, bool delete_quads_too);
