/**
 * @file ast.h
 * @brief Declaration of the ASTs structure.
 *
 * Add new kinds of expressions etc. here.
 *
 * @author warhawk
 * @date 2018-03-08
 */
#ifndef MCC_AST_H
#define MCC_AST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* Forward Declarations */
struct mCc_ast_expression;
struct mCc_ast_literal;
struct mCc_ast_identifier;

/* ---------------------------------------------------------------- AST Node */

/**
 * Node attribute containing the source location.
 */
struct mCc_ast_source_location {
	int start_line;
	int start_col;
	int end_line;
	int end_col;
};

/* Data contained by every AST node. */
/**
 * Attributes contained by every AST node.
 */
struct mCc_ast_node {
	struct mCc_ast_source_location sloc; ///< Source location of this node.
};

/* --------------------------------------------------------------- Operators */

/**
 * @brief Available types of binary operators.
 */
enum mCc_ast_binary_op {
	MCC_AST_BINARY_OP_ADD, ///< Addition
	MCC_AST_BINARY_OP_SUB, ///< Subtraction
	MCC_AST_BINARY_OP_MUL, ///< Multiplication
	MCC_AST_BINARY_OP_DIV, ///< Division
	MCC_AST_BINARY_OP_LT,  ///< Less than
	MCC_AST_BINARY_OP_GT,  ///< Greater than
	MCC_AST_BINARY_OP_LEQ, ///< Less or equal
	MCC_AST_BINARY_OP_GEQ, ///< Greater or equal
	MCC_AST_BINARY_OP_AND, /// AND
	MCC_AST_BINARY_OP_OR,  /// OR
	MCC_AST_BINARY_OP_EQ,  /// Equal
	MCC_AST_BINARY_OP_NEQ  /// Not equal
};

/* ------------------------------------------------------------- Expressions */

/**
 * @brief Available expression types.
 */
enum mCc_ast_expression_type {
	MCC_AST_EXPRESSION_TYPE_LITERAL,   ///< Literal expression
	MCC_AST_EXPRESSION_TYPE_IDENTIFIER,///< Identifier expression
	MCC_AST_EXPRESSION_TYPE_BINARY_OP, ///< Binary operation expression
	MCC_AST_EXPRESSION_TYPE_PARENTH,   ///< Parenthesis expression
};

/**
 * @brief Node representing an expression.
 *
 * Note that it contains common attributes in a #mCc_ast_node.
 * It has a type in #type, and pointers to the child nodes depending on the
 * type. Only one of the union members can be set at any time.
 */
struct mCc_ast_expression {
	struct mCc_ast_node node; ///< Common node attributes
	/// The concrete type of this expression (no inheritance in C)
	enum mCc_ast_expression_type type;
	union {

		/**
		 * Data if #type is #MCC_AST_EXPRESSION_TYPE_LITERAL
		 */
		struct mCc_ast_literal *literal;

		/**
		 * Data if #type is #MCC_AST_EXPRESSION_TYPE_IDENTIFIER
		 */
		struct mCc_ast_identifier *identifier;

		/**
		 * Data if #type is #MCC_AST_EXPRESSION_TYPE_BINARY_OP
		 */
		struct {
			enum mCc_ast_binary_op op; ///< The type of the binary operation
			struct mCc_ast_expression *lhs; ///< The lhs of the operation
			struct mCc_ast_expression *rhs; ///< The rhs of the operation
		};

		/**
		 * Data if #type is #MCC_AST_EXPRESSION_TYPE_PARENTH
		 */
		struct mCc_ast_expression *expression;
	};
};

/**
 * Construct an expression from a literal.
 *
 * @param literal The literal
 *
 * @return A new expression with type #MCC_AST_EXPRESSION_TYPE_LITERAL
 */
struct mCc_ast_expression *
mCc_ast_new_expression_literal(struct mCc_ast_literal *literal);

/**
 * Construct an expression from a identifier.
 *
 * @param identifier The identifier
 *
 * @return A new expression with type #MCC_AST_EXPRESSION_TYPE_IDENTIFIER
 */
struct mCc_ast_expression *
mCc_ast_new_expression_identifier(struct mCc_ast_identifier *identifier);

/**
 * Construct an expression from a binary operation.
 *
 * @param op The type of the operation
 * @param lhs The lhs
 * @param rhs The rhs
 *
 * @return A new expression with type #MCC_AST_EXPRESSION_TYPE_BINARY_OP
 */
struct mCc_ast_expression *
mCc_ast_new_expression_binary_op(enum mCc_ast_binary_op op,
                                 struct mCc_ast_expression *lhs,
                                 struct mCc_ast_expression *rhs);

/**
 * Construct an expression surrounded by parentheses.
 *
 * @param expression The inner expression
 *
 * @return A new expression with type #MCC_AST_EXPRESSION_TYPE_PARENTH
 */
struct mCc_ast_expression *
mCc_ast_new_expression_parenth(struct mCc_ast_expression *expression);

/**
 * Delete an expression.
 *
 * @param expression The expression
 */
void mCc_ast_delete_expression(struct mCc_ast_expression *expression);

/* ---------------------------------------------------------------- Literals */

/**
 * Available Literal types.
 */
enum mCc_ast_literal_type {
	MCC_AST_LITERAL_TYPE_INT,    ///< Integer literal
	MCC_AST_LITERAL_TYPE_FLOAT,  ///< Float literal
	MCC_AST_LITERAL_TYPE_BOOL,   ///< Boolean literal
	MCC_AST_LITERAL_TYPE_STRING, ///< String literal
};

enum mCc_ast_identifier_type {
	MCC_AST_IDENTIFIER_TYPE,			///< Identifier
};

/**
 * Node representing a identifier.
 */
struct mCc_ast_identifier {
	struct mCc_ast_node node; ///< Common attributes

	/**
	 * The type of this identifier.
	 *
	 * Type-specific data is contained in the union.
	 */
	enum mCc_ast_identifier_type type;
    char *id_value;
};

/**
 * Node representing a literal.
 */
struct mCc_ast_literal {
	struct mCc_ast_node node; ///< Common attributes

	/**
	 * The type of this literal.
	 *
	 * Type-specific data is contained in the union.
	 */
	enum mCc_ast_literal_type type;
	union {
		/**
		 * Data if #type is #MCC_AST_LITERAL_TYPE_INT
		 */
		long i_value;

		/**
		 * Data if #type is #MCC_AST_LITERAL_TYPE_FLOAT
		 */
		double f_value;

		/**
		 * Data if #type is #MCC_AST_LITERAL_TYPE_STRING
		 */
		char *s_value;

		/**
		 * Data if #type is #MCC_AST_LITERAL_TYPE_BOOL
		 */
		bool b_value;
	};
};

/**
 * Create a new integer literal.
 *
 * @param value The value
 *
 * @return A new literal with type #MCC_AST_LITERAL_TYPE_INT
 */
struct mCc_ast_literal *mCc_ast_new_literal_int(long value);

/**
 * Create a new float literal.
 *
 * @param value The value
 *
 * @return A new literal with type #MCC_AST_LITERAL_TYPE_FLOAT
 */
struct mCc_ast_literal *mCc_ast_new_literal_float(double value);

/**
 * Create a new boolean literal.
 *
 * @param value The value
 *
 * @return A new literal with type #MCC_AST_LITERAL_TYPE_BOOL
 */
struct mCc_ast_literal *mCc_ast_new_literal_bool(bool value);

/**
 * Create a new string literal.
 *
 * @param value The value
 *
 * @return A new literal with type #MCC_AST_LITERAL_TYPE_STRING
 */
struct mCc_ast_literal *mCc_ast_new_literal_string(char *value);

/**
 * Create a new identifier.
 *
 * @param value The value
 *
 * @return A new identifier with type #MCC_AST_IDENTIFIER
 */
struct mCc_ast_identifier *mCc_ast_new_identifier(char *value);

/**
 * Delete a literal.
 *
 * @param literal The literal to delete
 */
void mCc_ast_delete_literal(struct mCc_ast_literal *literal);

/**
 * Delete a identifier.
 *
 * @param identifier The identifier to delete
 */
void mCc_ast_delete_identifier(struct mCc_ast_identifier *identifier);

#ifdef __cplusplus
}
#endif

#endif
