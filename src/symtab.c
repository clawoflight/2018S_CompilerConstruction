/**
 * @file symtab.c
 * @brief Implementation of the symbol table.
 * @author bennett
 * @date 2018-04-07
 */
#include "mCc/symtab.h"
#include <assert.h>

static void mCc_symtab_delete_scope(struct mCc_symtab_scope *scope);
static void mCc_symtab_delete_entry(struct mCc_symtab_entry *entry);

/*********************************** Global array of scopes for print and gc */

/// Block size by which to increase array when reallocating
static const unsigned int global_scope_gc_block_size = 15;
/// Number of entries for which memory was allocated
static unsigned int global_scope_gc_alloc_size = 0;
/// Number of entries currently in array
static unsigned int global_scope_gc_count = 0;

/// All scopes ever created, to use when printing or freeing
static struct mCc_symtab_scope **global_scope_gc_arr;

/*********************************** File-static helpers */

static int mCc_symtab_add_scope_to_gc(struct mCc_symtab_scope *scope)
{
	assert(scope);

	if (global_scope_gc_count < global_scope_gc_alloc_size) {
		global_scope_gc_arr[global_scope_gc_count++] = scope;
		return 0;
	}

	struct mCc_symtab_scope **tmp;
	global_scope_gc_alloc_size += global_scope_gc_block_size;
	if ((tmp = realloc(global_scope_gc_arr,
	                   global_scope_gc_alloc_size * sizeof(*tmp))) == NULL) {
		return 1; // Caller must delete all scopes if wanted
	}

	global_scope_gc_arr = tmp;
	global_scope_gc_arr[global_scope_gc_count++] = scope;
	return 0;
}

/**
 * @brief Symbol table (scope) constructor.
 *
 * @param parent The parent scope, to be inserted.
 * @param name The name suffix of the new scope
 *
 * @return The new scope
 */
static struct mCc_symtab_scope *
mCc_symtab_new_scope(struct mCc_symtab_scope *parent, char *name)
{
	assert(name);

	struct mCc_symtab_scope *new_scope = malloc(sizeof(*new_scope));
	if (!new_scope)
		return NULL;

	new_scope->parent = parent;
	new_scope->hash_table = NULL; // Important for uthash to funtion properly
	new_scope->name = name;

	if (mCc_symtab_add_scope_to_gc(new_scope))
		mCc_symtab_delete_scope(new_scope);
	return NULL;
}

/**
 * @brief Symbol table entry constructor.
 *
 * @param scope The containing scope
 * @param entry_type The type of the entry
 * @param sloc The source code location of the declaration
 * @param identifier The identifier (hash table key!)
 * @param decl_type The primitive type of the declaration
 * @param optarg Array size or #mCc_ast_parameters* if required by entry_type
 *
 * @return The new symtab entry, ready to be inserted
 */
static struct mCc_symtab_entry *mCc_symtab_new_entry(
    struct mCc_symtab_scope *scope, enum mCc_symtab_entry_type entry_type,
    struct mCc_ast_source_location sloc, struct mCc_ast_identifier *identifier,
    enum mCc_ast_declaration_type decl_type, void *optarg)
{
	assert(scope);
	assert(identifier);

	struct mCc_symtab_entry *new_entry = malloc(sizeof(*new_entry));
	if (!new_entry)
		return NULL;

	new_entry->scope = scope;
	new_entry->entry_type = entry_type;
	new_entry->sloc = sloc;
	new_entry->identifier = identifier;
	new_entry->decl_type = decl_type;

	switch (entry_type) {
	case MCC_SYMTAB_ENTRY_TYPE_ARR:
		new_entry->arr_size = (unsigned int)optarg;
		break;
	case MCC_SYMTAB_ENTRY_TYPE_FUNC: new_entry->params = optarg; break;
	case MCC_SYMTAB_ENTRY_TYPE_VAR: break;
	}

	return new_entry;
}

/**
 * @brief Add an entry to a symbol table.
 *
 * Wraps around #HASH_ADD_KEYPTR
 *
 * @param self The scope to whose hash table the entry will be added
 * @param entry The entry to add
 */
static inline void mCc_symtab_scope_add_entry(struct mCc_symtab_scope *self,
                                              struct mCc_symtab_entry *entry)
{
	HASH_ADD_KEYPTR(hh, self->hash_table, entry->identifier->id_value,
	                strlen(entry->identifier->id_value), entry);
}

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
static struct mCc_symtab_entry *
mCc_symtab_scope_lookup_id(struct mCc_symtab_scope *scope,
                           struct mCc_ast_identifier *id)
{
}

/******************************* Public Functions */

struct mCc_symtab_scope *mCc_symtab_new_scope_in(struct mCc_symtab_scope *self,
                                                 char *childscope_name)
{
	assert(childscope_name);

	// create the scope name by concatenating it to the parent scope's name
	char *name;
	if (!self) {
		name = childscope_name;
	} else {
		name = malloc(strlen(self->name) + 2); // _ + null byte
		if (!name)
			return NULL;
		strcpy(name, self->name);
		strcat(name, "_");
		strcat(name, childscope_name);
	}

	// create scope
	return mCc_symtab_new_scope(self, name);
}

int mCc_symtab_scope_add_decl(struct mCc_symtab_scope *self,
                              struct mCc_ast_declaration *decl)
{
}

enum MCC_SYMTAB_SCOPE_LINK_ERROR
mCc_symtab_scope_link_ref_expression(struct mCc_symtab_scope *self,
                                     struct mCc_ast_expression *expr)
{
}

enum MCC_SYMTAB_SCOPE_LINK_ERROR
mCc_symtab_scope_link_ref_assignment(struct mCc_symtab_scope *self,
                                     struct mCc_ast_statement *stmt)
{
}

/******************************* Destructors */
static void mCc_symtab_delete_scope(struct mCc_symtab_scope *scope) {}

static void mCc_symtab_delete_entry(struct mCc_symtab_entry *entry) {}

void mCc_symtab_delete_all_scopes(void)
{
	for (unsigned int i = 0; i < global_scope_gc_count; ++i) {
		mCc_symtab_delete_scope(global_scope_gc_arr[i]);
	}
	global_scope_gc_count = 0;
	free(global_scope_gc_arr);
	global_scope_gc_alloc_size = 0;
}
