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
struct mCc_ast_statement;

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
 * Available types of unary operators.
 */
enum mCc_ast_unary_op {
	MCC_AST_UNARY_OP_NEG, ///< Numerical negation
	MCC_AST_UNARY_OP_NOT, ///< Logical negation
};

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
	MCC_AST_EXPRESSION_TYPE_UNARY_OP,  ///< Unary Operation expression
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
		 * Data if #type is #MCC_AST_EXPRESSION_TYPE_BINARY_OP
		 */
		struct {
			enum mCc_ast_binary_op op; ///< The type of the binary operation
			struct mCc_ast_expression *lhs; ///< The lhs of the operation
			struct mCc_ast_expression *rhs; ///< The rhs of the operation
		};

		/**
		 * Data if #type is #MCC_AST_EXPRESSION_TYPE_UNARY_OP
		 */
		struct {
			enum mCc_ast_unary_op unary_op; ///< The type of the unary operation
			struct mCc_ast_expression
			    *unary_expression; ///< subexpression of unary op
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
 * Construct an expression for a unary operation.
 *
 * @param op The type of the operation
 * @param subexpression The subexpression
 *
 * @return A new expression with type #MCC_AST_EXPRESSION_TYPE_UNARY_OP
 */
struct mCc_ast_expression *
mCc_ast_new_expression_unary_op(enum mCc_ast_unary_op op,
                                struct mCc_ast_expression *subexpression);

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

/* -------------------------------------------------------------- Statements */

/**
 * The available statement types
 */
enum mCc_ast_statement_type {
	MCC_AST_STATEMENT_TYPE_IF,    ///< If statement
	MCC_AST_STATEMENT_TYPE_IFELSE, ///< If statement with else-branch
	/* MCC_AST_STATEMENT_TYPE_RET,   ///< Return statement */
	/* MCC_AST_STATEMENT_TYPE_WHILE, ///< While statement */
	/* MCC_AST_STATEMENT_TYPE_DECL,  ///< Variable declaration assignment */
	/* MCC_AST_STATEMENT_TYPE_ASSGN, ///< Variable assignment statement */
	MCC_AST_STATEMENT_TYPE_EXPR,  ///< Expression statement
	/* MCC_AST_STATEMENT_TYPE_CPND   ///< Compound statement */
};

/**
 * Node representing a statement.
 */
struct mCc_ast_statement {
	struct mCc_ast_node node; ///< Common node attributes
	/// The concrete type of this statement (no inheritance in C)
	enum mCc_ast_statement_type type;

	union {

		/// Data if type is #MCC_AST_STATEMENT_TYPE_IF
		/// or #MCC_AST_STATEMENT_TYPE_IFELSE
		struct {
			struct mCc_ast_expression *if_expr;
			struct mCc_ast_statement *if_stmt;
			struct mCc_ast_statement *else_stmt;
		};

		/// Data if type is #MCC_AST_STATEMENT_TYPE_EXPR
		struct mCc_ast_expression *expression;
	};
};

/**
 * @brief Construct a statement from an expression
 *
 * @param expression The underlying expression
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_EXPR
 */
struct mCc_ast_statement *
mCc_ast_new_statement_expression(struct mCc_ast_expression *expression);

/**
 * @brief Construct a statement from an if-statement
 *
 * @param if_expr The expression in parentheses
 * @param if_stmt The statement inside the if-clause
 * @param else_stmt The (optional) statement inside the else-clause
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_IF or
 * #MCC_AST_STATEMENT_TYPE_IFELSE
 */
struct mCc_ast_statement *
mCc_ast_new_statement_if(struct mCc_ast_expression *if_expr,
                         struct mCc_ast_statement *if_stmt,
                         struct mCc_ast_statement *else_stmt);

/**
 * @brief Delete a statement.
 *
 * @param statement The statement to delete
 */
void mCc_ast_delete_statement(struct mCc_ast_statement *statement);

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
 * Delete a literal.
 *
 * @param literal The literal to delete
 */
void mCc_ast_delete_literal(struct mCc_ast_literal *literal);

#ifdef __cplusplus
}
#endif

#endif
