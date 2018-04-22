/*
Three address code idea.
 Structure the whole TAC with a stack. Push the identifiers and the values and if op found pull calc the result save them in
 temp variables and push them on stack. The temp variables all need an unique name -- make this with a count and increase it

 Then a struct for TAC which includes assgn, operator, label, goto, cond_jump arr_index, call func, return
 Enum for types of TAC: add, sub, equally, ad or,

 Additional array for saving the TAC statements. The array is an array* tac_statement so it can take struct types.
    tac_stmt[0]=label L1
    tac_stmt[1]= t1= num;
    tac_stmt[2]= t2 = x;
    tac_stmt[3]= t3 = t2 >= t1;
    tac_stmt [4]= cjump t3 L2;
 */
typedef array mCc_tac;

enum mCc_tac_type_literal{
    MCC_TAC_TYPE_LITERAL_INT,
    MCC_TAC_TYPE_LITERAL_FLOAT,
    MCC_TAC_TYPE_LITERAL_BOOL,
    MCC_TAC_TYPE_LITERAL_STRING,
};

enum mCc_tac_type {
    MCC_TAC_TYPE_FUNC_CALL;
    MCC_TAC_TYPE_RETURN;
    MCC_TAC_TYPE_ASSIGNMENT;
    MCC_TAC_TYPE_CONDITION;
    MCC_TAC_TYPE_LABLE;
};

enum mCc_tac_expression_type {
    MCC_TAC_EXP_TYPE_BINARY_OP_ADD,
    MCC_TAC_EXP_TYPE_BINARY_OP_SUB,
    MCC_TAC_EXP_TYPE_BINARY_OP_MUL,
    MCC_TAC_EXP_TYPE_BINARY_OP_DIV,

    MCC_TAC_EXPR_BINARY_OP_FLOAT_ADD,
    MCC_TAC_EXPR_BINARY_OP__FLOAT_SUB,
    MCC_TAC_EXPR_BINARY_OP_FLOAT_MUL,
    MCC_TAC_EXPR_BINARY_OP_FLOAT_DIV,

    MCC_TAC_EXP_TYPE_BINARY_OP_LT,  ///< Less than
    MCC_TAC_EXP_TYPE_BINARY_OP_GT,  ///< Greater than
    MCC_TAC_EXP_TYPE_BINARY_OP_LEQ, ///< Less or equal
    MCC_TAC_EXP_TYPE_BINARY_OP_GEQ, ///< Greater or equal
    MCC_TAC_EXP_TYPE_BINARY_OP_AND, ///< AND
    MCC_TAC_EXP_TYPE_BINARY_OP_OR,  ///< OR
    MCC_TAC_EXP_TYPE_BINARY_OP_EQ,  ///< Equal
    MCC_TAC_EXP_TYPE_BINARY_OP_NEQ  ///< Not equal
};

enum mCc_tac_condition_type{
    MCC_TAC_COND_TYPE_WHILE;
    MCC_TAC_COND_TYPE_IF;
    MCC_TAC_COND_TYPE_IF_ELSE;
};

enum mCc_tac_type_expression_unary {
    MCC_TAC_EXPR_UNARY_NEG,
    MCC_TAC_EXPR_UNARY_NOT,
};

enum mCc_tac_entity_type{
    MCC_TAC_ENTITY_TYPE_LITERAL;
    MCC_TAC_ENTITY_TYPE_IDENTIFIER;
};

enum mCc_tac_type_assignment{
    MCC_TAC_TYPE_ASSIGN_ADDRESS,
    MCC_TAC_TYPE_ASSIGN_POINTER,
    MCC_TAC_TYPE_ASSIGN_FUNCTION_CALL,
    MCC_TAC_TYPE_ASSIGN_EXPRESSION,
    MCC_TAC_TYPE_ASSIGN_ARRAY,

};

struct mCc_tac{
    enum mCc_tac_type type;

    union {
        /**
         * MCC_TAC_TYPE_ASSIGNMENT**/
        struct mCc_tac_assignment *assignment;

        /**
         * MCC_TAC_TYPE_LABEL
         * MCC_TAC_TYPE_RETURN
         * MCC_TAC_TYPE_PARAM_SETUP
         * MCC_TAC_TYPE_FUNCTION_CALL
         */
        char* name;

        /**
         * MCC_TAC_TYPE_CONDITION**/
        struct mCc_tac_condition *condition;
    };
};

struct mCc_tac_expression{
    enum mCc_tac_expression_type type;
    union {

        //Binary expressions
        struct {
            enum mCc_tac_type_expression_binary binary_type;
            struct mCc_tac_entity *lhs;
            struct mCc_tac_entity *rhs;
        };
        //Unary Expression
        struct {
            enum mCc_tac_type_expression_unary unary_type;
            struct mCc_tac_entity *unary;
        };
    };
};

/**
Here we have the  condition  struct for both while and if, else
 all of them have a condition stmt which has to be evaluated.
 The lables has to be made on the fly.
 **/
struct mCc_tac_condition{
    enum mCc_tac_condition_type type;
    struct mCc_tac_expression *cond_expr;

};

struct mCc mCc_tac_entity{
        enum mCc_tac_type_entity type;
        //identifier
        union {
            char* identifier;
            struct mCc_tac_literal * literal;
        };
};

struct mCc_tac_literal{
    enum mCc_tac_type_literal type;
    union {
        /* MCC_AST_LITERAL_TYPE_INT */
        long i_value;
        /* MCC_TAC_LITERAL_TYPE_FLOAT */
        double f_value;
        /* MCC_TAC_LITERAL_TYPE_BOOL */
        bool b_value;
        /* MCC_TAC_LITERAL_TYPE_STRING */
        char* str_value;
    };
};

struct mCc_tac_assignment {
    enum mCc_tac_type_assignment type;
    char* assign_name;
    union {

        union {
            /**
             * MCC_TAC_TYPE_ASSIGN_POINTER
             * MCC_TAC_TYPE_ASSIGN_ADDRESS
             * MCC_TAC_TYPE_ASSIGN_ARRAY
             * MCC_TAC_TYPE_ASSIGN_FUNCTION_CALL
            */
            char* rhs;

            /**
             * * MCC_TAC_TYPE_ASSIGN_EXPRESSION
             */
            struct mCc_tac_expression *assignment_expr;

        };
    };

};


struct mCc_tac_stack{
    int size;
    struct mCc_tac_literal** data;      /// here all the data will be pushed on stack - all calculations
};
/**
 * @param literal: the literal which needs to be added to the stack
 * @return: stack
 */
struct mCc_tac_stack* push (struct mCc_tac_stack stack, enum mCc_tac_type_literal literal);


/**
 * @param amount: how many items should be popped fromt he stack
 * @return: stack
 */
struct mCc_tac_stack* pop (struct mCc_tac_stack stack, int amount);

