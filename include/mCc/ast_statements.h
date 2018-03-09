/**
 * @file ast_statements.h
 * @brief Declarations for the AST statement node and related things.
 * @author bennett
 * @date 2018-03-09
 */
#ifndef MCC_AST_STMT_H
#define MCC_AST_STMT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ast.h"

/* -------------------------------------------------------------- Statements */

/**
 * The available statement types
 */
enum mCc_ast_statement_type {
	MCC_AST_STATEMENT_TYPE_IF,     ///< If statement
	MCC_AST_STATEMENT_TYPE_IFELSE, ///< If statement with else-branch
	/* MCC_AST_STATEMENT_TYPE_RET,   ///< Return statement */
	/* MCC_AST_STATEMENT_TYPE_WHILE, ///< While statement */
	/* MCC_AST_STATEMENT_TYPE_DECL,  ///< Variable declaration assignment */
	/* MCC_AST_STATEMENT_TYPE_ASSGN, ///< Variable assignment statement */
	MCC_AST_STATEMENT_TYPE_EXPR, ///< Expression statement
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
			struct mCc_ast_expression *if_cond;
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
 * @param if_cond The if-condition
 * @param if_stmt The statement inside the if-clause
 * @param else_stmt The (optional) statement inside the else-clause
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_IF or
 * #MCC_AST_STATEMENT_TYPE_IFELSE
 */
struct mCc_ast_statement *
mCc_ast_new_statement_if(struct mCc_ast_expression *if_cond,
                         struct mCc_ast_statement *if_stmt,
                         struct mCc_ast_statement *else_stmt);

/**
 * @brief Delete a statement.
 *
 * @param statement The statement to delete
 */
void mCc_ast_delete_statement(struct mCc_ast_statement *statement);

#ifdef __cplusplus
}
#endif

#endif
