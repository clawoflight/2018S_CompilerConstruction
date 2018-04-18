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

enum mCc_tac_type {
    MCC_TAC_TYPE_FUNC_CALL;
    MCC_TAC_TYPE_FUNC_BEGIN;
    MCC_TAC_TYPE_FUNC_END;
    MCC_TAC_TYPE_RETURN;
    MCC_TAC_TYPE_ASSIGNMENT;
    MCC_TAC_TYPE_EXPRESSION;
    MCC_TAC_TYPE_CONDITION;
    MCC_TAC_TYPE_LABLE;
};

enum mCc_tac_expression_type {
    MCC_TAC_EXP_TYPE_BINARY_OP_ADD;
    MCC_TAC_EXP_TYPE_BINARY_OP_SUB;
    MCC_TAC_EXP_TYPE_BINARY_OP_MUL;
    MCC_TAC_EXP_TYPE_BINARY_OP_DIV;
    MCC_TAC_EXP_TYPE_BINARY_OP_LT,  ///< Less than
    MCC_TAC_EXP_TYPE_BINARY_OP_GT,  ///< Greater than
    MCC_TAC_EXP_TYPE_BINARY_OP_LEQ, ///< Less or equal
    MCC_TAC_EXP_TYPE_BINARY_OP_GEQ, ///< Greater or equal
    MCC_TAC_EXP_TYPE_BINARY_OP_AND, ///< AND
    MCC_TAC_EXP_TYPE_BINARY_OP_OR,  ///< OR
    MCC_TAC_EXP_TYPE_BINARY_OP_EQ,  ///< Equal
    MCC_TAC_EXP_TYPE_BINARY_OP_NEQ  ///< Not equal
    MCC_TAC_EXP_TYPE_UNARY_OP_NEG, ///< Numerical negation       ///should we differ between extra unary type
    MCC_TAC_EXP_TYPE_UNARY_OP_NOT, ///< Logical negation
};

enum mCc_tac_condition_type{
    MCC_TAC_COND_TYPE_WHILE;
    MCC_TAC_COND_TYPE_IF;
    MCC_TAC_COND_TYPE_IF_ELSE;
};

enum mCc_tac_entity_type{
    MCC_TAC_ENTITY_TYPE_LITERAL;
    MCC_TAC_ENTITY_TYPE_IDENTIFIER;
};
/**
 *
**/
struct mCc_tac{
    enum mCc_tac_type type;

    union {

        /**
         * Data if #type is #MCC_TAC_TYPE_FUNC_CALL
         */
        struct{
            struct mCc_tac_identifier *func_id;
        };
        /**
         * Data if #type is #MCC_TAC_TYPE_ASSIGNMENT
         */
        struct{
            struct mCc_tac_identifier *id_assgn; ///< not sure if necessary
            struct mCc_tac_expression *exp; ///< The lhs of the operation
        };
        /**
       * Data if #type is #MCC_TAC_TYPE_RETURN
       */
        struct{
            union {                 /// two types of return with value or nit
                char* return_val;
            };
        };
        /**
      * Data if #type is #MCC_TAC_TYPE_CONDITION
      */
        struct{
            struct mCc_tac_condition *cond;
        };

    };
};

struct mCc_tac_expression{
    enum mCc_tac_expression_type type;
    struct mCc_tac_entity *lhf;
    struct mCc_tac_entity *rhs;
};

/**
Here we have the  condition  struct for both while and if, else
 all of them have a condition stmt which has to be evaluated.
 The lables has to be made on the fly.
 **/
struct mCc_tac_condition{
    enum mCc_tac_condition_type type;
    struct mCc_tac_expression *cond_stmt;

};

struct mCc_tac_entity{
    enum mCc_tac_entity_type type;
    union {
        struct mCc_tac_literal *literal;
        struct mCc_tac_identifier *id;
    };
};

struct mCc_tac_identifier{          /// think about do we need a whole struct for one char*?
    char* id_name;
};