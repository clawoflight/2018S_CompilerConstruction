/**
 * @file symtab.h
 * @brief Symbol table declarations.
 * @author bennett
 * @date 2018-03-27
 */
#ifndef MCC_ST_H
#define MCC_ST_H

#include "ast.h"

#include "lib/uthash.h"

#ifdef __cplusplus
extern "C" {
#endif

enum mCc_symtab_entry_type {
	MCC_SYMTAB_ENTRY_TYPE_VAR,
	MCC_SYMTAB_ENTRY_TYPE_ARR,
	MCC_SYMTAB_ENTRY_TYPE_FUNC,
};

struct mCc_symtab_entry {
	/// Scope/Symbol Table containing this entry
	struct mCc_symtab_scope *scope;
	UT_hash_handle hh; ///< Internal data for hash table

	/// The kind of entry (var, arr, ...)
	enum mCc_symtab_entry_type entry_type;

	/// The source location
	struct mCc_ast_source_location sloc;
	/// The identifier of the entry
	struct mCc_ast_identifier *identifier;

	/// The primitive type (int, bool, ...)
	enum mCc_ast_type primitive_type;

	union {
		/// If entry_type is #MCC_SYMTAB_ENTRY_TYPE_ARR
		unsigned int arr_size;

		/// If entry_type is #MCC_SYMTAB_ENTRY_TYPE_FUNC
		struct mCc_ast_parameters *params;
	};
};

/* scope == table for our purposes, since a new table is opened for every scope.
 * data structure: embedded hash table using uthash,
 * the entries of which are #mCc_symtab_entry.
 */
struct mCc_symtab_scope {
	/// Parent scope, needed for lookup
	struct mCc_symtab_scope *parent;
	char *name; ///< Human-readable name for debugging

	/** Contains the entries.
	 *
	 * MUST BE INITIALIZED TO NULL.
	 *
	 * Not actually an array!
	 * Instead, it is used by uthash to store the hash table.  */
	struct mCc_symtab_entry *hash_table;
};

/************************************************ Functions */

/* We will want to design a clean api, as much as possible should be
 * file-static.
 *
 * Idea for adding entries: add a method on scopes that takes an ast declaration
 * and
 * - adds an entry to the scope if it is new
 * - returns an error if the id was already declared
 * - Impls SSA if necessary (I think we will need to do that - but then
 *   we need to rename all subsequent calls?)
 *
 * We will also need to add a method on scopes to look up declarations,
 *
 * and a method that takes an ast call_expr, id_expr or arr_subscr_expr and
 * - looks up the corresponding declaration (taking shadowing into account!!!)
 * - links the ast expression to the corresponding symtab entry (howtf should we
 *   do that!? give ID a type enum and allow it to contain a pointer to
 *   mCc_symtab_entry?)
 * - returns an error if the id was not declared first
 *
 * SYMTAB BUILDING:
 * Will be done using a specialized visitor that only handles the cases that it
 * needs. E.g. every declaration will trigger an #mCc_symtab_scope_add_decl.
 * This means the visitor will need to keep track of it's scope! Does that work?
 * Or should we just use a long nested loop abomination to walk the ast?
 *
 * The AST visitor will need to open a scope for each function declaration (for
 * the params) and another for each body.
 * */

/**
 * @brief Open a new scope inside an existing one.
 *
 * If childscope_name is dynamic, the caller must free it after this function
 * returns because it is copied.
 *
 * @param self The scope to add to
 * @param childscope_name The human-readable name of the new scope
 *
 * @return A pointer to the new scope, or NULL on failure.
 */
struct mCc_symtab_scope *mCc_symtab_new_scope_in(struct mCc_symtab_scope *self,
                                                 const char *childscope_name);

/**
 * @brief Record a declaration in a new entry in the given scope.
 *
 * An error is returned if the ID was already declared in this scope.
 * To do that, first check whether the hash table contains the ID.
 *
 * @param self The scope to record the declaration in
 * @param decl The declaration to record
 *
 * @return 0 on success, 1 if the ID was already declared in this scope, -1 on
 * memory error.
 */
int mCc_symtab_scope_add_decl(struct mCc_symtab_scope *self,
                              struct mCc_ast_declaration *decl);

/**
 * @brief Record a function definition in a new entry in the given scope.
 *
 * An error is returned if the ID was already declared in this scope.
 * To do that, first check whether the hash table contains the ID.
 *
 * @param self The scope to record the declaration in
 * @param func_def The function definition to record
 *
 * @return 0 on success, 1 if the ID was already declared in this scope, -1 on
 * memory error.
 */
int mCc_symtab_scope_add_func_def(struct mCc_symtab_scope *self,
                                  struct mCc_ast_function_def *func_def);

/// The possible errors when linking a reference to the symbol table.
enum MCC_SYMTAB_SCOPE_LINK_ERROR {
	MCC_SYMTAB_SCOPE_LINK_ERR_OK,                 ///< No error
	MCC_SYMTAB_SCOPE_LINK_ERR_UNDECLARED_ID,      ///< Use of undeclared id
	MCC_SYMTAB_SCOPE_LINK_ERR_FUN_WITHOUT_CALL,   ///< Use of function as var or
	                                              ///< arr
	MCC_SYMTAB_SCOPE_LINK_ERR_ASSIGN_TO_FUNCTION, ///< Use of function name as
	                                              ///< lvalue
	MCC_SYMTAB_SCOPE_LINK_ERR_VAR, ///< Use of variable like an array or
	                               ///< function
	MCC_SYMTAB_SCOPE_LINK_ERR_ARR_WITHOUT_BRACKS,  ///< Use of array like a
	                                               ///< variable or function
	MCC_SYMTAB_SCOPE_LINK_ERROR_INVALID_AST_OBJECT, ///< Attempt to link an ast
	                                               ///< object without ID
};

/**
 * @brief Recursively lookup an ID, starting from the given scope.
 *
 * Internally, this will perform a hash table lookup in each scope until a scope
 * without parent is reached.
 *
 * @param scope The scope to start lookup in
 * @param id The ID to look for
 *
 * @return A pointer to the entry, or NULL if the ID is undeclared.
 */
struct mCc_symtab_entry *
mCc_symtab_scope_lookup_id(struct mCc_symtab_scope *scope,
                           struct mCc_ast_identifier *id);

/**
 * check whether the main is declared in the right way (return type int, no parameter and name main)
 * @param scope
 * @return 0 on success or -1 on error
 */
int mCc_symtab_check_main_properties(struct mCc_symtab_scope *scope );

/**
 * @brief Link the identifier from an expression to the corresponding symtab
 * entry.
 *
 * TODO: call #mCc_symtab_scope_lookup_id internally, then set
 * expr->id->symtab_entry appropriately
 *
 * @param self The current scope, to begin lookup in
 * @param expr The id, call or arr_subscr expression to link
 *
 * @return 0 on success, or an #MCC_SYMTAB_SCOPE_LINK_ERROR
 */
enum MCC_SYMTAB_SCOPE_LINK_ERROR
mCc_symtab_scope_link_ref_expression(struct mCc_symtab_scope *self,
                                     struct mCc_ast_expression *expr);

/**
 * @brief Link the identifier from an assignment to the corresponding symtab
 * entry.
 *
 * TODO: call #mCc_symtab_scope_lookup_id internally, then set
 * expr->id->symtab_entry appropriately
 *
 * @param self The current scope, to begin lookup in
 * @param expr The id or arr_subscr expression to link
 *
 * @return 0 on success, or an #MCC_SYMTAB_SCOPE_LINK_ERROR
 */
enum MCC_SYMTAB_SCOPE_LINK_ERROR
mCc_symtab_scope_link_ref_assignment(struct mCc_symtab_scope *self,
									 struct mCc_ast_statement *stmt);

/**
 * @brief Free all scopes, their hash tables and entries.
 *
 * This is needed because the program usually only holds pointers to entries and
 * the top-level symbol table, which doesn't hold pointers to it's children.
 *
 * This should reset properly so that new scopes can be created afterwards.
 */
void mCc_symtab_delete_all_scopes(void);

#ifdef __cplusplus
}
#endif

#endif // MCC_ST_H

struct mCc_symtab_scope** mCc_symtab_get_global_array();