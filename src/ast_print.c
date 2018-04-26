/**
 * @file ast_print.c
 * @brief Implementation of the AST printing mechanism.
 * @author warhawk
 * @date 2018-03-08
 */
#include "mCc/ast_print.h"

#include <assert.h>

#include "mCc/ast_visit.h"

#define LABEL_SIZE 64

const char *mCc_ast_print_unary_op(enum mCc_ast_unary_op op)
{
	switch (op) {
	case MCC_AST_UNARY_OP_NEG: return "-";
	case MCC_AST_UNARY_OP_NOT: return "!";
	}

	return "unknown op";
}

const char *mCc_ast_print_binary_op(enum mCc_ast_binary_op op)
{
	switch (op) {
	case MCC_AST_BINARY_OP_ADD: return "+";
	case MCC_AST_BINARY_OP_SUB: return "-";
	case MCC_AST_BINARY_OP_MUL: return "*";
	case MCC_AST_BINARY_OP_DIV: return "/";
	case MCC_AST_BINARY_OP_LT: return "<";
	case MCC_AST_BINARY_OP_GT: return ">";
	case MCC_AST_BINARY_OP_LEQ: return "<=";
	case MCC_AST_BINARY_OP_GEQ: return ">=";
	case MCC_AST_BINARY_OP_AND: return "&&";
	case MCC_AST_BINARY_OP_OR: return "||";
	case MCC_AST_BINARY_OP_EQ: return "==";
	case MCC_AST_BINARY_OP_NEQ: return "!=";
	}

	return "unknown op";
}

/* ------------------------------------------------------------- DOT Printer */

static void print_dot_begin(FILE *out)
{
	assert(out);

	fprintf(out, "digraph \"AST\" {\n");
	fprintf(out, "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
	assert(out);

	fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label)
{
	assert(out);
	assert(node);
	assert(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node,
                           const void *dst_node, const char *label)
{
	assert(out);
	assert(src_node);
	assert(dst_node);
	assert(label);

	fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node,
	        label);
}

static void print_dot_statement_expr(struct mCc_ast_statement *statement,
                                     void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: expr");
	print_dot_edge(out, statement, statement->expression, "expression");
}

static void print_dot_statement_if(struct mCc_ast_statement *statement,
                                   void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: if");
	print_dot_edge(out, statement, statement->if_cond, "condition");
	print_dot_edge(out, statement, statement->if_stmt, "if stmt");
}

static void print_dot_statement_ifelse(struct mCc_ast_statement *statement,
                                       void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: ifelse");
	print_dot_edge(out, statement, statement->if_cond, "condition");
	print_dot_edge(out, statement, statement->if_stmt, "if stmt");
	print_dot_edge(out, statement, statement->else_stmt, "else stmt");
}

static void print_dot_statement_assgn(struct mCc_ast_statement *statement,
                                      void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: =");
	print_dot_edge(out, statement, statement->id_assgn, "id");
	if (statement->lhs_assgn)
		print_dot_edge(out, statement, statement->lhs_assgn, "index");
	print_dot_edge(out, statement, statement->rhs_assgn, "value");
}

static void print_dot_statement_while(struct mCc_ast_statement *statement,
                                      void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: while");
	print_dot_edge(out, statement, statement->while_cond, "condition");
	print_dot_edge(out, statement, statement->while_stmt, "while stmt");
}

static void print_dot_statement_return(struct mCc_ast_statement *statement,
                                       void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: return");
	print_dot_edge(out, statement, statement->ret_val, "return value");
}

static void print_dot_statement_return_void(struct mCc_ast_statement *statement,
                                            void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: return");
}

static void print_dot_statement_compound(struct mCc_ast_statement *statement,
                                         void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: cmpnd");
	for (unsigned int i = 0; i < statement->compound_stmt_count; ++i)
		print_dot_edge(out, statement, statement->compound_stmts[i],
		               "substatement");
}

static void print_dot_statement_decl(struct mCc_ast_statement *statement,
                                     void *data)
{
	assert(statement);
	assert(data);

	FILE *out = data;
	print_dot_node(out, statement, "stmt: decl");
	print_dot_edge(out, statement, statement->declaration, "declaration");
}

static void print_dot_expression_literal(struct mCc_ast_expression *expression,
                                         void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: lit");
	print_dot_edge(out, expression, expression->literal, "literal");
}

static void
print_dot_expression_identifier(struct mCc_ast_expression *expression,
                                void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: id");
	print_dot_edge(out, expression, expression->identifier, "id");
}

static void print_dot_expression_unary_op(struct mCc_ast_expression *expression,
                                          void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: %s",
	         mCc_ast_print_unary_op(expression->unary_op));
	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->unary_expression,
	               "subexpression");
}

static void
print_dot_expression_binary_op(struct mCc_ast_expression *expression,
                               void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "expr: %s",
	         mCc_ast_print_binary_op(expression->op));

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->lhs, "lhs");
	print_dot_edge(out, expression, expression->rhs, "rhs");
}

static void print_dot_expression_parenth(struct mCc_ast_expression *expression,
                                         void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "( )");
	print_dot_edge(out, expression, expression->expression, "expression");
}

static void
print_dot_expression_call_expr(struct mCc_ast_expression *expression,
                               void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: call");
	print_dot_edge(out, expression, expression->f_name, "fName");
	if (expression->arguments != NULL)
		print_dot_edge(out, expression, expression->arguments, "arguments");
}

static void
print_dot_expression_arr_subscr(struct mCc_ast_expression *expression,
                                void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: arr");
	print_dot_edge(out, expression, expression->array_id, "id");
	print_dot_edge(out, expression, expression->subscript_expr, "index");
}

static void print_dot_literal_int(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%ld", literal->i_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_float(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%f", literal->f_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_string(struct mCc_ast_literal *literal,
                                     void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", literal->s_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_bool(struct mCc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", literal->b_value ? "true" : "false");

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_identifier(struct mCc_ast_identifier *identifier,
                                 void *data)
{
	assert(identifier);
	assert(data);

	char label[LABEL_SIZE] = { 0 };
	snprintf(label, sizeof(label), "%s", identifier->id_value);

	FILE *out = data;
	print_dot_node(out, identifier, label);
}

static void print_dot_arguments(struct mCc_ast_arguments *arguments, void *data)
{
	assert(arguments);
	assert(data);

	FILE *out = data;
	print_dot_node(out, arguments, "args: expr");
	for (unsigned int i = 0; i < arguments->expression_count; ++i)
		print_dot_edge(out, arguments, arguments->expressions[i], "expression");
}

static void print_dot_parameter(struct mCc_ast_parameters *parameter,
                                void *data)
{
	assert(parameter);
	assert(data);

	FILE *out = data;
	print_dot_node(out, parameter, "args: decl");
	for (unsigned int i = 0; i < parameter->decl_count; ++i)
		print_dot_edge(out, parameter, parameter->decl[i], "declaration");
}

static void print_dot_declaration(struct mCc_ast_declaration *decl, void *data)
{
	assert(decl);
	assert(data);

	FILE *out = data;
	switch (decl->decl_type) {
	case MCC_AST_TYPE_BOOL:
		print_dot_node(out, decl, "declaration bool");
		break;
	case MCC_AST_TYPE_INT: print_dot_node(out, decl, "declaration int"); break;
	case MCC_AST_TYPE_FLOAT:
		print_dot_node(out, decl, "declaration float");
		break;
	case MCC_AST_TYPE_STRING:
		print_dot_node(out, decl, "declaration string");
		break;
	}

	print_dot_edge(out, decl, decl->decl_id, "id");
	if (decl->decl_array_size) {
		print_dot_edge(out, decl, decl->decl_array_size, "arr size");
	}
}

static void print_dot_function_def(struct mCc_ast_function_def *func,
                                   void *data)
{
	assert(func);
	assert(data);

	FILE *out = data;
	switch (func->func_type) {
	case MCC_AST_TYPE_BOOL: print_dot_node(out, func, "bool function"); break;
	case MCC_AST_TYPE_INT: print_dot_node(out, func, "int function"); break;
	case MCC_AST_TYPE_FLOAT: print_dot_node(out, func, "float function"); break;
	case MCC_AST_TYPE_STRING:
		print_dot_node(out, func, "string function");
		break;
	case MCC_AST_TYPE_VOID: print_dot_node(out, func, "void function"); break;
	}
	print_dot_edge(out, func, func->identifier, "id");
	if (func->body) {
		print_dot_edge(out, func, func->body, "body");
	}
	if (func->para) {
		print_dot_edge(out, func, func->para, "params");
	}
}

static void print_dot_program(struct mCc_ast_program *prog, void *data)
{
	assert(prog);
	assert(data);

	FILE *out = data;
	print_dot_node(out, prog, "program");
	for (unsigned int i = 0; i < prog->func_def_count; ++i)
		print_dot_edge(out, prog, prog->func_defs[i], "func def");
}

static struct mCc_ast_visitor print_dot_visitor(FILE *out)
{
	assert(out);

	return (struct mCc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_PRE_ORDER,

		.userdata = out,

		.statement_expr = print_dot_statement_expr,
		.statement_if = print_dot_statement_if,
		.statement_ifelse = print_dot_statement_ifelse,
		.statement_while = print_dot_statement_while,
		.statement_return = print_dot_statement_return,
		.statement_return_void = print_dot_statement_return_void,
		.statement_compound = print_dot_statement_compound,
		.statement_assgn = print_dot_statement_assgn,
		.statement_decl = print_dot_statement_decl,
		.declaration = print_dot_declaration,

		.expression_literal = print_dot_expression_literal,
		.expression_identifier = print_dot_expression_identifier,
		.expression_unary_op = print_dot_expression_unary_op,
		.expression_binary_op = print_dot_expression_binary_op,
		.expression_parenth = print_dot_expression_parenth,
		.expression_call_expr = print_dot_expression_call_expr,
		.expression_arr_subscr = print_dot_expression_arr_subscr,

		.identifier = print_dot_identifier,

		.arguments = print_dot_arguments,
		.parameter = print_dot_parameter,
		.function_def = print_dot_function_def,
		.program = print_dot_program,

		.literal_int = print_dot_literal_int,
		.literal_float = print_dot_literal_float,
		.literal_string = print_dot_literal_string,
		.literal_bool = print_dot_literal_bool,
	};
}

void mCc_ast_print_dot_statement(FILE *out, struct mCc_ast_statement *statement)
{
	assert(out);
	assert(statement);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_statement(statement, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_arguments(FILE *out, struct mCc_ast_arguments *arguments)
{
	assert(out);
	assert(arguments);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_arguments(arguments, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_parameter(FILE *out,
                                 struct mCc_ast_parameters *parameter)
{
	assert(out);
	assert(parameter);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_parameter(parameter, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_declaration(FILE *out, struct mCc_ast_declaration *decl)
{
	assert(out);
	assert(decl);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_declaration(decl, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_expression(FILE *out,
                                  struct mCc_ast_expression *expression)
{
	assert(out);
	assert(expression);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_expression(expression, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_literal(FILE *out, struct mCc_ast_literal *literal)
{
	assert(out);
	assert(literal);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_literal(literal, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_identifier(FILE *out,
                                  struct mCc_ast_identifier *identifier)
{
	assert(out);
	assert(identifier);

	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_identifier(identifier, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_function_def(FILE *out,
                                    struct mCc_ast_function_def *func)
{
	assert(out);
	assert(func);
	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_function_def(func, &visitor);

	print_dot_end(out);
}

void mCc_ast_print_dot_program(FILE *out, struct mCc_ast_program *prog)
{
	assert(out);
	assert(prog);
	print_dot_begin(out);

	struct mCc_ast_visitor visitor = print_dot_visitor(out);
	mCc_ast_visit_program(prog, &visitor);

	print_dot_end(out);
}
