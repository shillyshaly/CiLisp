/**
* Name: Jamie Hernandez
* Lab: ciLisp Lab
* Date: 11/4/2020
**/
#include "cilisp.h"
#include "math.h"

#define RED             "\033[31m"
#define RESET_COLOR     "\033[0m"

// yyerror:
// Something went so wrong that the whole program should crash.
// You should basically never call this unless an allocation fails.
// (see the "yyerror("Memory allocation failed!")" calls and do the same.
// This is basically printf, but red, with "\nERROR: " prepended, "\n" appended,
// and an "exit(1);" at the end to crash the program.
// It's called "yyerror" instead of "error" so the parser will use it for errors too.
void yyerror(char *format, ...) {
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf(buffer, 255, format, args);

    printf(RED "\nERROR: %s\nExiting...\n" RESET_COLOR, buffer);
    fflush(stdout);

    va_end (args);
    exit(1);
}

// warning:
// Something went mildly wrong (on the user-input level, probably)
// Let the user know what happened and what you're doing about it.
// Then, move on. No big deal, they can enter more inputs. ¯\_(ツ)_/¯
// You should use this pretty often:
//      too many arguments, let them know and ignore the extra
//      too few arguments, let them know and return NAN
//      invalid arguments, let them know and return NAN
//      many more uses to be added as we progress...
// This is basically printf, but red, and with "\nWARNING: " prepended and "\n" appended.
void warning(char *format, ...) {
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf(buffer, 255, format, args);

    printf(RED "WARNING: %s\n" RESET_COLOR, buffer);
    fflush(stdout);

    va_end (args);
}

FUNC_TYPE resolveFunc(char *funcName) {
    // Array of string values for function names.
    // Must be in sync with members of the FUNC_TYPE enum in order for resolveFunc to work.
    // For example, funcNames[NEG_FUNC] should be "neg"
    char *funcNames[] = {
            "neg",
            "abs",
            "add",
            // TODO complete the array
            // the empty string below must remain the last element
            "sub",
            "mult",
            "div",
            "remainder",
            "exp",
            "exp2",
            "pow",
            "log",
            "sqrt",
            "cbrt",
            "hypot",
            "max",
            "min",
            ""
    };
    int i = 0;
    while (funcNames[i][0] != '\0') {
        if (strcmp(funcNames[i], funcName) == 0) {
            return i;
        }
        i++;
    }
    return CUSTOM_FUNC;
}

AST_NODE *createNumberNode(double value, NUM_TYPE type) {
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL) {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    // TODO complete the function
    // Populate "node", the AST_NODE * created above with the argument data.
    // node is a generic AST_NODE, don't forget to specify it is of type AST_NODE_NODE
    node->type = NUM_NODE_TYPE;
    node->data.number.value = value;
    node->data.number.type = type;

    return node;
}

AST_NODE *createFunctionNode(FUNC_TYPE func, AST_NODE *opList) {
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL) {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    // TODO complete the function
    // Populate the allocated AST_NODE *node's data
    node->type = FUNC_NODE_TYPE;
    node->data.function.opList = opList;
    node->data.function.func = func;


    return node;
}

AST_NODE *addExpressionToList(AST_NODE *newExpr, AST_NODE *exprList) {
    newExpr->next = exprList;
    return newExpr;
}

///
///  list of op functions
///

/*
 * unary functions
 **/
RET_VAL evalNeg(AST_NODE *op) {
    RET_VAL result;

    if (op == NULL) {
        warning("Too few arguments in neg.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);

    if (op->next != NULL){
        warning("Too many arguments in neg.  Ignoring the rest.");
    }

    result.value = -result.value;

    return result;
}

RET_VAL evalAbs(AST_NODE *op) {
    RET_VAL result;

    if (op == NULL) {
        warning("Too few arguments in abs.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);

    if (op->next != NULL){
        warning("Too many arguments in abs.  Ignoring the rest.");
    }

    result.value = fabs(result.value);

    return result;
}

RET_VAL evalExp(AST_NODE *op) {
    RET_VAL result;

    if (op == NULL) {
        warning("Too few arguments in exp.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    result.type = DOUBLE_TYPE;

    if (op->next != NULL){
        warning("Too many arguments in exp.  Ignoring the rest.");
    }

    result.value = exp(result.value);

    return result;
}

RET_VAL evalExp2(AST_NODE *op) {
    RET_VAL result;

    if (op == NULL) {
        warning("Too few arguments in exp2.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);

    if (op->next != NULL){
        warning("Too many arguments in exp2.  Ignoring the rest.");
    }
    if (op->data.number.value < 0){
        result.type = DOUBLE_TYPE;
    }

    result.value = exp2(result.value);

    return result;
}

RET_VAL evalLog(AST_NODE *op) {
    RET_VAL result;

    if (op == NULL) {
        warning("Too few arguments in log.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    result.type = DOUBLE_TYPE;

    if (op->next != NULL){
        warning("Too many arguments in log.  Ignoring the rest.");
    }

    result.value = log(result.value);

    return result;
}

RET_VAL evalSqrt(AST_NODE *op) {
    RET_VAL result;

    if (op == NULL) {
        warning("Too few arguments in sqrt.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    result.type = DOUBLE_TYPE;

    if (op->next != NULL){
        warning("Too many arguments in sqrt.  Ignoring the rest.");
    }

    result.value = sqrt(result.value);

    return result;
}

RET_VAL evalCbrt(AST_NODE *op) {
    RET_VAL result;

    if (op == NULL) {
        warning("Too few arguments in cbrt.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    result.type = DOUBLE_TYPE;

    if (op->next != NULL){
        warning("Too many arguments in cbrt.  Ignoring the rest.");
    }

    result.value = cbrt(result.value);

    return result;
}

/*
 * binary functions
 **/
RET_VAL evalSub(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;

    if (op == NULL || op->next == NULL) {
        warning("Too few arguments in sub.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    if (op->next){
        warning("Too many arguments.  Ignoring the rest.");
    }


    result2 = eval(op);
    result.value = result.value - result2.value;

    return result;
}

RET_VAL evalDiv(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;

    if (op == NULL || op->next == NULL) {
        warning("Too few arguments in div.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    if (op->next){
        warning("Too many arguments div.  Ignoring the rest.");
    }

    result2 = eval(op);
    result.value = result.value / result2.value;

    return result;
}

RET_VAL evalRem(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;

    if (op == NULL || op->next == NULL) {
        warning("Too few arguments in rem.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    if (op->next){
        warning("Too many arguments in rem.  Ignoring the rest.");
    }

    result2 = eval(op);
//    result.value = result.value % result2.value;
    result.value = fmod(result.value, result2.value);

    return result;
}

RET_VAL evalPow(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;

    if (op == NULL || op->next == NULL) {
        warning("Too few arguments in pow.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    if (op->next){
        warning("Too many arguments in pow.  Ignoring the rest.");
    }

    result2 = eval(op);
    result.value = pow(result.value, result2.value);

    return result;
}

/*
 * n-ary functions
 **/
RET_VAL evalAdd(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;
    if (op == NULL) {
        warning("Too few arguments in add.");
        result = ZERO_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    while (op) {
        result2 = eval(op);
        result.value = result.value + result2.value;

        if (result2.type == DOUBLE_TYPE){
            result.type = DOUBLE_TYPE;
        }

        op = op->next;
    }

    return result;
}

RET_VAL evalMult(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;
    if (op == NULL) {
        warning("Too few arguments in mult.");
        result.value = 1;
        return result;
    }

    result = eval(op);
    op = op->next;

    while (op) {
        result2 = eval(op);
        result.value = result.value * result2.value;

        if (result2.type == DOUBLE_TYPE){
            result.type = DOUBLE_TYPE;
        }

        op = op->next;
    }

    return result;
}

RET_VAL evalHypot(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;
    if (op == NULL) {
        warning("Too few arguments in hypot.");
        result = ZERO_RET_VAL;
        return result;
    }

    result = eval(op);
    result.type = DOUBLE_TYPE;
    result.value = pow(result.value, 2);

    op = op->next;

    while (op) {
        result2 = eval(op);
        result.value += pow(result2.value, 2);

        op = op->next;
    }

    result.value = sqrt(result.value);

    return result;
}

RET_VAL *evalMax(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        warning("Too few arguments in max.");
        result->value = 0;
        return result;
    }

    *result = eval(op);

    while (op->next != NULL) {
        result->value = fmax(result->value, op->next->data.number.value);
        op = op->next;
    }

    printf("%f\n", result->value);

    return result;
}

RET_VAL *evalMin(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        yyerror("Too few arguments in min.");
        result->value = 0;
        return result;
    }

    *result = eval(op);

    while (op->next != NULL) {
        result->value = fmin(result->value, op->next->data.number.value);
        op = op->next;
    }

    printf("%f\n", result->value);

    return result;
}

///
///  end list of functions
///

RET_VAL evalFuncNode(AST_NODE *node) {
    if (!node) {
        yyerror("NULL ast node passed into evalFuncNode!");
        return NAN_RET_VAL; // unreachable but kills a clang-tidy warning
    }

    // TODO complete the function
    // HINT:
    // the helper functions that it calls will need to be defined above it
    // because they are not declared in the .h file (and should not be)
    switch (node->data.function.func) {
        //unary
        case NEG_FUNC:
            return evalNeg(node->data.function.opList);
        case ABS_FUNC:
            return evalAbs(node->data.function.opList);
        case EXP_FUNC:
            return evalExp(node->data.function.opList);
        case EXP2_FUNC:
            return evalExp2(node->data.function.opList);
        case LOG_FUNC:
            return evalLog(node->data.function.opList);
        case SQRT_FUNC:
            return evalSqrt(node->data.function.opList);
        case CBRT_FUNC:
            return evalCbrt(node->data.function.opList);
            //binary
        case SUB_FUNC:
            return evalSub(node->data.function.opList);
        case DIV_FUNC:
            return evalDiv(node->data.function.opList);
        case REMAINDER_FUNC:
            return evalRem(node->data.function.opList);
        case POW_FUNC:
            return evalPow(node->data.function.opList);
            //n-ary
        case ADD_FUNC:
            return evalAdd(node->data.function.opList);
        case MULT_FUNC:
            return evalMult(node->data.function.opList);
        case HYPOT_FUNC:
            return evalHypot(node->data.function.opList);
        case MAX_FUNC:
            evalMax(node->data.function.opList);
            break;
        case MIN_FUNC:
            evalMin(node->data.function.opList);
            break;
        case CUSTOM_FUNC:
            break;
    }
}

RET_VAL evalNumNode(AST_NODE *node) {
    if (!node) {
        yyerror("NULL ast node passed into evalNumNode!");
        return NAN_RET_VAL;
    }

    // TODO complete the function
    RET_VAL *result = malloc(sizeof(RET_VAL));
    result->value = node->data.number.value;
    result->type = node->data.number.type;

    return *result;
}

RET_VAL eval(AST_NODE *node) {
    if (!node) {
        yyerror("NULL ast node passed into eval!");
        return NAN_RET_VAL;
    }

    // TODO complete the function
    switch (node->type) {
        case NUM_NODE_TYPE:
            return evalNumNode(node);
        case FUNC_NODE_TYPE:
            return evalFuncNode(node);
    }
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val) {
    switch (val.type) {
        case INT_TYPE:
            printf("Integer : %.lf\n", val.value);
            break;
        case DOUBLE_TYPE:
            printf("Double : %lf\n", val.value);
            break;
        default:
            printf("No Type : %lf\n", val.value);
            break;
    }
}

void freeFuncNode(AST_NODE *node) {
    if (!node) {
        return;
    }
    free(node->data.function.opList);

}

void freeNode(AST_NODE *node) {
    if (!node) {
        return;
    }

    // TODO complete the function

    // look through the AST_NODE struct, decide what
    // referenced data should have freeNode called on it
    // (hint: it might be part of an s_expr_list, with more
    // nodes following it in the list)

    // if this node is FUNC_TYPE, it might have some operands
    // to free as well (but this should probably be done in
    // a call to another function, named something like
    // freeFunctionNode)
    freeNode(node->next);

    if (node->type == FUNC_NODE_TYPE) {
        freeFuncNode(node);
    }

    // and, finally,
    free(node);
}
