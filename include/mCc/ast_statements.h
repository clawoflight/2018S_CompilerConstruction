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
	MCC_AST_STATEMENT_TYPE_IF,       ///< If statement
	MCC_AST_STATEMENT_TYPE_IFELSE,   ///< If statement with else-branch
	MCC_AST_STATEMENT_TYPE_RET,      ///< Return statement
	MCC_AST_STATEMENT_TYPE_RET_VOID, ///< Return statement with no value
	MCC_AST_STATEMENT_TYPE_WHILE,    ///< While statement
	MCC_AST_STATEMENT_TYPE_DECL,     ///< Variable declaration statement
	MCC_AST_STATEMENT_TYPE_ASSGN,    ///< Variable assignment statement
	MCC_AST_STATEMENT_TYPE_EXPR,     ///< Expression statement
	MCC_AST_STATEMENT_TYPE_CMPND     ///< Compound statement
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

		/// Data if type is #MCC_AST_STATEMENT_TYPE_ASSGN
		struct {
			struct mCc_ast_identifier *id_assgn;
			struct mCc_ast_expression *lhs_assgn;
			struct mCc_ast_expression *rhs_assgn;
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
		/// Data return type is #MCC_AST_STATEMENT_TYPE_DECL
		struct mCc_ast_declaration *declaration;
	};
};

struct mCc_ast_declaration {
	struct mCc_ast_node node; ///< Common node attributes
	/// The concrete type of this statement (no inheritance in C)
	enum mCc_ast_type type;

	enum mCc_ast_type decl_type;
	struct mCc_ast_literal *decl_array_size;
	struct mCc_ast_identifier *decl_id;
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
 * @brief Construct a statement from an assignment
 *
 * @param id_assgn The variable to be assigned
 * @param lhs_assgn The (optional) index expression
 * @param rhs_assgn The expression which is assigned to lhs
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_ASSGN
 */
struct mCc_ast_statement *
mCc_ast_new_statement_assgn(struct mCc_ast_identifier *id_assgn,
                            struct mCc_ast_expression *lhs_assgn,
                            struct mCc_ast_expression *rhs_assgn);

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
 * @brief Construct a statement from a declaration
 *
 * @param declaration The underlying declaration
 *
 * @return A new statement with type #MCC_AST_STATEMENT_TYPE_DECL
 */
struct mCc_ast_statement *
mCc_ast_new_statement_declaration(struct mCc_ast_declaration *declaration);

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

/************************************************************* Declarations */

/**
 * @brief Construct a declaration
 *
 * @param decl_type declaration type
 * @param decl_array_size value of the declaration
 * @param decl_id identifier of the declaration
 *
 * @return A new declaraton
 */
struct mCc_ast_declaration *
mCc_ast_new_declaration(enum mCc_ast_type decl_type,
                        struct mCc_ast_literal *dec_array_size,
                        struct mCc_ast_identifier *decl_id);

/**
 * @brief Delete a declaration.
 *
 * @param decl The declaration to delete
 */
void mCc_ast_delete_declaration(struct mCc_ast_declaration *decl);

/************************************************************* Function */

struct mCc_ast_function_def {
	struct mCc_ast_node node; ///< Common node attributes

	enum mCc_ast_type func_type;
	struct mCc_ast_statement *body;
	struct mCc_ast_identifier *identifier;
	struct mCc_ast_parameters *para;
};

/**
 * @brief Construct void function definition
 *
 * @param an identifier, parameter (can be null) and a compound statement
 *
 * @return a new function
 */
struct mCc_ast_function_def *
mCc_ast_new_function_def_void(struct mCc_ast_identifier *id,
                              struct mCc_ast_parameters *para,
                              struct mCc_ast_statement *body);

/**
 * @brief Construct function definition with return value
 *
 * @param a return type, an identifier, parameter (can be null) and a compound
 * statement
 *
 * @return a new function
 */
struct mCc_ast_function_def *mCc_ast_new_function_def_type(
    enum mCc_ast_type type, struct mCc_ast_identifier *id,
    struct mCc_ast_parameters *para, struct mCc_ast_statement *body);
/**
 * @brief Delete function.
 *
 * @param function  to delete
 */
void mCc_ast_delete_func_def(struct mCc_ast_function_def *func);

#ifdef __cplusplus
}
#endif

#endif
