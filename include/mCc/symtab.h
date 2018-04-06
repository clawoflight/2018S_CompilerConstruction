/**
 * @file symtab.h
 * @brief Symbol table declarations.
 * @author bennett
 * @version
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
	MCC_SYMTAB_ENTRY_TYPE_FUNC
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
	// TODO(ramona): we should really rename this struct to primitive type, it's
	// confusing that we cannot reuse it.
	enum mCc_ast_declaration_type decl_type;

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

	/// Child scopes, needed for dump and free
	struct mCc_symtab_scope **child_scopes;
	unsigned int child_scope_alloc_size; ///< Memory allocated for child scopes
	unsigned int child_scope_count;      ///< Number of child scopes

	/** Contains the entries.
	 *
	 * MUST BE INITIALIZED TO NULL.
	 *
	 * Not actually an array!
	 * Instead, it is used by uthash to store the hash table.  */
	struct mCc_symtab_entry **hash_table;
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

// Make static in the implementation file if possible?
mCc_symtab_scope *mCc_symtab_new_scope(struct mCc_symtab_scope *parent,
                                       char *name);
// Make static in the implementation file!
mCc_symtab_entry *mCc_symtab_new_entry(struct mCc_symtab_scope *scope,
                                       enum mCc_symtab_entry_type entry_type,
                                       struct mCc_ast_source_location *sloc,
                                       struct mCc_ast_identifier *identifier,
                                       enum mCc_ast_declaration_type decl_type,
                                       void *optarg);

// Make static in the implementation file!
inline void mCc_symtab_scope_add_entry(struct mCc_symtab_scope *self,
                                       struct mCc_symtab_entry *entry);

/**
 * @brief Add a new child scope to an existing one.
 *
 * @param self The scope to add to
 * @param childscope_name The human-readable name of the new scope
 *
 * @return A pointer to the new scope, or NULL on failure.
 */
mCc_symtab_scope *mCc_symtab_scope_add_childscope(struct mCc_symtab_scope *self,
                                                  char *childscope_name);

/**
 * @brief Record a declaration in a new entry in the given scope.
 *
 * An error is returned if the ID was already declared in this scope.
 * To do that, first check whether the hash table contains the ID.
 *
 * @param self The scope to record the declaration in
 * @param decl The declaration to record
 *
 * @return 0 on success, non-zero if the ID was already declared in this scope.
 */
int mCc_symtab_scope_add_decl(struct mCc_symtab_scope *self,
                              struct mCc_ast_declaration *decl);

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
mCc_symtab_entry *mCc_symtab_lookup(struct mCc_symtab_scope *scope,
                                    struct mCc_ast_identifier *id);

void mCc_symtab_delete_entry(struct mCc_symtab_entry *entry);
void mCc_symtab_delete_scope(struct mCc_symtab_scope *scope);

#ifdef __cplusplus
}
#endif

#endif // MCC_ST_H
