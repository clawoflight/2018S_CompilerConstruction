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
 * The available primitive types
 */
enum mCc_ast_declaration_type {
    MCC_AST_TYPE_BOOL,  ///< Boolean
    MCC_AST_TYPE_INT,   ///< Integer
    MCC_AST_TYPE_FLOAT, ///< Floating-point number
    MCC_AST_TYPE_STRING ///< String
};

/**
 * The available statement types
 */
enum mCc_ast_statement_type {
	MCC_AST_STATEMENT_TYPE_IF,       ///< If statement
	MCC_AST_STATEMENT_TYPE_IFELSE,   ///< If statement with else-branch
	MCC_AST_STATEMENT_TYPE_RET,      ///< Return statement */
	MCC_AST_STATEMENT_TYPE_RET_VOID, ///< Return statement with no value
	MCC_AST_STATEMENT_TYPE_WHILE,    ///< While statement
    MCC_AST_STATEMENT_TYPE_DECL,  ///< Variable declaration assignment */
	/* MCC_AST_STATEMENT_TYPE_ASSGN, ///< Variable assignment statement */
	MCC_AST_STATEMENT_TYPE_EXPR, ///< Expression statement
	MCC_AST_STATEMENT_TYPE_CMPND ///< Compound statement
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
		/// Data while type is #MCC_AST_STATEMENT_TYPE_WHILE
		struct {
			struct mCc_ast_expression *while_cond;
			struct mCc_ast_statement *while_stmt;
		};
		/// Data return type is #MCC_AST_STATEMENT_TYPE_RET
		/// or #MCC_AST_STATEMENT_TYPE_RET_VOID
		struct {
			struct mCc_ast_expression *ret_val;
		};
		/// Data if type is #MCC_AST_STATEMENT_TYPE_EXPR
		struct mCc_ast_expression *expression;

		/// Data if type is #MCC_AST_STATEMENT_TYPE_CMPND
		struct {
			/// Number of statements for which pointer memory was allocated
			unsigned int compound_stmt_alloc_size;
			unsigned int compound_stmt_count; ///< Number of sub-statements
			struct mCc_ast_statement **compound_stmts; ///< Sub-statements
		};
        /// Data return type is #MCC_AST_STATEMENT_TYPE_DEC
        struct {
            enum mCc_ast_declaration_type dec_type;
            long dec_val;
            struct mCc_ast_identifier *dec_id;
        };
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
 * @brief Construct a statement from an while-statement
 *
 * @param while_cond The while-condition
 * @param while_stmt The statement inside the while-clause
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_WHILE
 */
struct mCc_ast_statement *
mCc_ast_new_statement_while(struct mCc_ast_expression *while_cond,
                            struct mCc_ast_statement *while_stmt);

/**
 * @brief Construct a statement from multiple statements.
 *
 * @param substatement The (optional) initial substatement.
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_CMPND
 */
struct mCc_ast_statement *
mCc_ast_new_statement_compound(struct mCc_ast_statement *substatement);

/**
 * @brief Construct a statement from an declaration-statement
 *
 * @param dec_type declaration type
 * @param dec_val value of the declaration
 * @param dec_id identifier of the declaration
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_DEC
 */
struct mCc_ast_statement *
mCc_ast_new_statement_declaration(enum mCc_ast_declaration_type dec_type, long dec_val,
                            struct mCc_ast_identifier *dec_id);

/**
 * @brief Add a subexpression to a compound statement.
 *
 * @param self Statement with type #MCC_AST_STATEMENT_TYPE_CMPND to which to add
 * another
 * @param statement The substatement to add to the compound statement
 *
 * @return self
 */
struct mCc_ast_statement *
mCc_ast_compound_statement_add(struct mCc_ast_statement *self,
                               struct mCc_ast_statement *statement);

/**
 * @brief Construct a statement from an return-statement
 *
 * @param return val The return-value
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_RET or
 * #MCC_AST_STATEMENT_TYPE_RET_VOID
 */
struct mCc_ast_statement *
mCc_ast_new_statement_return(struct mCc_ast_expression *ret_val);

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
