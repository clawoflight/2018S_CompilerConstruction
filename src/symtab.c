/**
 * @file symtab.c
 * @brief Implementation of the symbol table.
 * @author bennett
 * @date 2018-04-07
 */
#include "mCc/symtab.h"

/*********************************** Global array of scopes for print and gc */

/// Block size by which to increase array when reallocating
static const unsigned int global_scope_gc_block_size;
/// Number of entries for which memory was allocated
static unsigned int global_scope_gc_alloc_size;
/// Number of entries currently in array
static unsigned int gobal_scope_gc_count;

/// All scopes ever created, to use when printing or freeing
static struct mCc_symtab_scope **global_scope_gc_arr;

/*********************************** File-static helpers */

/**
 * @brief Symbol table (scope) constructor.
 *
 * TODO: whoever implements this, PLEASE initialize
 * #mCc_symtab_scope->hash_table to null!
 * ALSO, add to #global_scope_gc_arr ;)
 *
 * TODO: strcat "_" and name and parent->name so we can see the hierarchy
 *
 * @param parent The parent scope, to be inserted.
 * @param name The name suffix of the new scope
 *
 * @return The new scope
 */
static struct mCc_symtab_scope *
mCc_symtab_new_scope(struct mCc_symtab_scope *parent, char *name)
{
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
    struct mCc_ast_source_location *sloc, struct mCc_ast_identifier *identifier,
    enum mCc_ast_declaration_type decl_type, void *optarg)
{
}

/**
 * @brief Add an entry to a symbol table.
 *
 * Wraps around #HASH_ADD_PTR or whatever we will end up using
 *
 * @param self The scope to whose hash table the entry will be added
 * @param entry The entry to add
 */
static inline void mCc_symtab_scope_add_entry(struct mCc_symtab_scope *self,
                                              struct mCc_symtab_entry *entry)
{
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
}

int mCc_symtab_scope_add_decl(struct mCc_symtab_scope *self,
                              struct mCc_ast_declaration *decl)
{
}

int mCc_symtab_scope_link_ref_expression(struct mCc_symtab_scope *self,
                                         struct mCc_ast_expression *expr)
{
}

int mCc_symtab_scope_link_ref_assignment(struct mCc_symtab_scope *self,
                                         struct mCc_ast_statement *stmt)
{
}

/******************************* Destructors */
static void mCc_symtab_delete_scope(struct mCc_symtab_scope *scope) {}

static void mCc_symtab_delete_entry(struct mCc_symtab_entry *entry) {}

void mCc_symtab_delete_all_scopes(void) {}
