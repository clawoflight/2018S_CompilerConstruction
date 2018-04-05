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

	enum mCc_symtab_entry_type entry_type;
	struct mCc_ast_identifier *identifier;
	struct mCc_ast_source_location sloc;

	union {
		/// If entry_type is #MCC_SYMTAB_ENTRY_TYPE_VAR
		enum mCc_ast_declaration_type var_type;

		/// If entry_type is #MCC_SYMTAB_ENTRY_TYPE_ARR
		struct {
			enum mCc_ast_declaration_type arr_type;
			unsigned int arr_size;
		};

		/// If entry_type is #MCC_SYMTAB_ENTRY_TYPE_FUNC
		struct {
			enum mCc_ast_declaration_type return_type;
			struct mCc_ast_parameters *params;
		};
	};
};

/* scope == table for our purposes, since a new table is opened for every scope.
 * data structure: linked list with extra object containing start and end node.
 * the nodes are #mCc_symtab_entry.
 */
struct mCc_symtab_scope {
	/// Parent scope, needed for lookup
	struct mCc_symtab_scope *parent;

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
 * - models shadowing if necessary (I think we will need to do that - but then
 *   we need to rename all subsequent calls?)
 *
 * We will also need to add a method on scopes to look up declarations,
 * and a method that takes an ast call_expr, id_expr or arr_subscr_expr and
 * - looks up the corresponding declaration (taking shadowing into account!!!)
 * - links the ast expression to the corresponding symtab entry (howtf should we
 *   do that!? Make id a struct with type enum?)
 * - returns an error if the id was not declared first
 * */

void mCc_symtab_delete_entry(struct mCc_symtab_entry *entry);
void mCc_symtab_delete_scope(struct mCc_symtab_scope *scope);

#ifdef __cplusplus
}
#endif

#endif // MCC_ST_H
