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
struct mCc_ast_identifier;
struct mCc_ast_arguments;
struct mCc_symtab_entry;

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

/**
 * The available primitive types
 */
enum mCc_ast_type {
	MCC_AST_TYPE_BOOL,   ///< Boolean
	MCC_AST_TYPE_INT,    ///< Integer
	MCC_AST_TYPE_FLOAT,  ///< Floating-point number
	MCC_AST_TYPE_STRING, ///< String
	MCC_AST_TYPE_VOID    ///< No type, but cleaner here
};

/* Data contained by every AST node. */
/**
 * Attributes contained by every AST node.
 */
struct mCc_ast_node {
	struct mCc_ast_source_location sloc; ///< Source location of this node.
};

/* Don't move or remove this! It needs to be below #mCc_ast_node because that is
 * used in ast_statements.h */
#include "ast_statements.h"

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
	MCC_AST_BINARY_OP_AND, ///< AND
	MCC_AST_BINARY_OP_OR,  ///< OR
	MCC_AST_BINARY_OP_EQ,  ///< Equal
	MCC_AST_BINARY_OP_NEQ  ///< Not equal
};

/* ------------------------------------------------------------- Expressions */

/**
 * @brief Available expression types.
 */
enum mCc_ast_expression_type {
	MCC_AST_EXPRESSION_TYPE_LITERAL,    ///< Literal expression
	MCC_AST_EXPRESSION_TYPE_IDENTIFIER, ///< Identifier expression
	MCC_AST_EXPRESSION_TYPE_UNARY_OP,   ///< Unary Operation expression
	MCC_AST_EXPRESSION_TYPE_BINARY_OP,  ///< Binary operation expression
	MCC_AST_EXPRESSION_TYPE_PARENTH,    ///< Parenthesis expression
	MCC_AST_EXPRESSION_TYPE_CALL_EXPR,  ///< Call expression
	MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR  ///< Array Subscript expression
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

		/**
		 * Data if #type is #MCC_AST_EXPRESSION_TYPE_CALL_EXPR
		 */
		struct {
			struct mCc_ast_arguments *arguments; ///< argument list
			struct mCc_ast_identifier *f_name;   ///< function name
		};

		/**
		 * Data if #type is #MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR
		 */
		struct {
			struct mCc_ast_identifier *array_id; ///< identifier
			struct mCc_ast_expression
			    *subscript_expr; ///< array subscript expression
		};
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
 * Construct an expression for a call expression.
 *
 * @param identifier The Identifier
 * @param arguments The arguments list
 *
 * @return A new expression with type #MCC_AST_EXPRESSION_TYPE_CALL_EXPR
 */
struct mCc_ast_expression *
mCc_ast_new_expression_call_expr(struct mCc_ast_identifier *identifier,
                                 struct mCc_ast_arguments *arguments);

/**
 * Construct an expression for an identifier with array.
 *
 * @param array_id The Identifier
 * @param subscript_expr The subscript expression
 *
 * @return A new expression with type #MCC_AST_EXPRESSION_TYPE_ARR_SUBSCR
 */
struct mCc_ast_expression *
mCc_ast_new_expression_arr_subscr(struct mCc_ast_identifier *array_id,
                                  struct mCc_ast_expression *subscript_expr);

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

/* -------------------------------------------------------------- Identifiers */

/**
 * Node representing a identifier.
 */
struct mCc_ast_identifier {
	struct mCc_ast_node node; ///< Common attributes
	struct mCc_symtab_entry *symtab_ref;
	/**
	 * The ID string
	 */
	char *id_value;
};

/**
 * Create a new identifier.
 *
 * @param value The value
 *
 * @return A new identifier with type #MCC_AST_IDENTIFIER
 */
struct mCc_ast_identifier *mCc_ast_new_identifier(char *value);

/**
 * Delete an identifier.
 *
 * @param identifier The identifier to delete
 */
void mCc_ast_delete_identifier(struct mCc_ast_identifier *identifier);

/* -------------------------------------------------------------- Arguments */

/**
 * Node representing arguments.
 */
struct mCc_ast_arguments {
	struct mCc_ast_node node; ///< Common node attributes
	/// The concrete type of this statement (no inheritance in C)

	unsigned int arguments_alloc_block_size;
	unsigned int expression_count;           ///< Number of sub-expressions
	struct mCc_ast_expression **expressions; ///< Sub expressions
};

/**
 * @brief Construct arguments from an expression
 *
 * @param expression The underlying expression
 *
 * @return A new list of arguments
 */
struct mCc_ast_arguments *
mCc_ast_new_arguments(struct mCc_ast_expression *expression);

/**
 * @brief Add a subexpression to arguments.
 *
 * @param self arguments to which to add another
 * @param expression The sub-expression to add to the arguments
 *
 * @return self
 */
struct mCc_ast_arguments *
mCc_ast_arguments_add(struct mCc_ast_arguments *self,
                      struct mCc_ast_expression *expression);

/**
 * @brief Delete arguments.
 *
 * @param statement The arguments to delete
 */
void mCc_ast_delete_arguments(struct mCc_ast_arguments *arguments);

/* -------------------------------------------------------------- Parameter */

/**
 * Node representing parameter.
 */
struct mCc_ast_parameters {
	struct mCc_ast_node node; ///< Common node attributes

	unsigned int parameter_alloc_block_size;
	unsigned int decl_count;           ///< Number of sub-expressions
	struct mCc_ast_declaration **decl; ///< Sub expressions
};

/**
 * @brief Construct parameter from an expression
 *
 * @param decl The underlying declaration
 *
 * @return A new list of parameter
 */
struct mCc_ast_parameters *
mCc_ast_new_parameters(struct mCc_ast_declaration *decl);

/**
 * @brief Add a subexpression to parameter.
 *
 * @param self parameter to which to add another
 * @param decl The declaration to add to the parameter
 *
 * @return self
 */
struct mCc_ast_parameters *
mCc_ast_parameters_add(struct mCc_ast_parameters *self,
                       struct mCc_ast_declaration *decl);

/**
 * @brief Delete parameter.
 *
 * @param parameter The parameter to delete
 */
void mCc_ast_delete_parameters(struct mCc_ast_parameters *parameter);

/********************************************************* Program */

/**
 * @brief Node representing an entire program.
 */
struct mCc_ast_program {
	struct mCc_ast_node node; ///< Common node attributes

	/// Memory allocated for function definitions
	unsigned int func_def_alloc_size;
	unsigned int func_def_count;             ///< Number of function definitions
	struct mCc_ast_function_def **func_defs; ///< Function definitions
};

/**
 * @brief Create a new program from a function definition.
 *
 * @param fun_def The (optional) function definition to add
 *
 * @return A new program
 */
struct mCc_ast_program *
mCc_ast_new_program(struct mCc_ast_function_def *fun_def);

/**
 * @brief Add a function definition to a program
 *
 * @param self The program to which to add the function definition
 * @param fun_def The function definition to add
 *
 * @return self
 */
struct mCc_ast_program *
mCc_ast_program_add(struct mCc_ast_program *self,
                    struct mCc_ast_function_def *fun_def);

/**
 * @brief Delete a program
 *
 * @param self The program to delete
 */
void mCc_ast_delete_program(struct mCc_ast_program *self);

#ifdef __cplusplus
}
#endif

#endif
