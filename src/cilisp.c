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
            // TODO complete the array - done
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

    // TODO complete the function - ???
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
    node->data.function.func = func;
    node->data.function.opList = opList;

    return node;
}

//TODO - fixxxxx mmmeeeeeee
AST_NODE *addExpressionToList(AST_NODE *newExpr, AST_NODE *exprList) {
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL) {
        yyerror("Memory allocation failed!");
        exit(1);
    }
    node->data = newExpr->data;
    node->data.function.opList = newExpr;
    node->next = exprList;


    return node;
}

///
///  list of functions
///

//double counter (AST_NODE *op){
//    double count = 0.0;
//
//    while (op != NULL){
//        count++;
//        op = op->next;
//    }
//    return count;
//}

/*
 * unary functions
 **/

RET_VAL *evalNeg(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));
//    double num = counter(op);

    if (op == NULL) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->value = -(op->data.function.opList->data.number.value);
    result->type = op->data.number.type;
    printf("result %f\n", result->value);

    return result;
}

RET_VAL *evalAbs(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->value = fabs(op->data.function.opList->data.number.value);
    result->type = (NUM_TYPE) op->type;
    printf("result %f\n", result->value);

    return result;
}

RET_VAL *evalExp(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->value = exp(op->data.function.opList->data.number.value);
    result->type = op->data.number.type;
    printf("result %f\n", result->value);

    return result;
}

RET_VAL *evalExp2(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }else if (op->next != NULL){
        warning("Too many arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    if (op->data.function.opList->data.number.value < 0){
        result->type = DOUBLE_TYPE;
    }else {
        result->type = op->data.number.type;
    }

    result->value = pow(2, op->data.function.opList->data.number.value);
    printf("result %f\n", result->value);

    return result;
}

RET_VAL *evalLog(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->value = log(op->data.function.opList->data.number.value);
    result->type = op->data.number.type;

    return result;
}

RET_VAL *evalSqrt(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }else if (op->next != NULL){
        warning("Too many arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->value = sqrt(op->data.function.opList->data.number.value);
    result->type = op->data.number.type;

    return result;
}

RET_VAL *evalCbrt(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }else if (op->next != NULL){
        warning("Too many arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->value = cbrt(op->data.function.opList->data.number.value);
    result->type = op->data.number.type;

    return result;
}

//binary functions
RET_VAL *evalSub(AST_NODE *op1, AST_NODE *op2) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if ((op1 == NULL || op2 == NULL) || (op1 && op2 == NULL)) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }else if (op1->next != NULL || op2->next != NULL){
        warning("Too many arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->type = op1->type || op2->type;
    result->value = (op1->data.function.opList->data.number.value - op2->data.function.opList->data.number.value);

    return result;
}

RET_VAL *evalDiv(AST_NODE *op1, AST_NODE *op2) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if ((op1 == NULL || op2 == NULL) || (op1 && op2 == NULL)) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }else if (op1->next != NULL || op2->next != NULL){
        warning("Too many arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->type = op1->type || op2->type;
    result->value = (op1->data.function.opList->data.number.value / op2->data.function.opList->data.number.value);

    return result;
}

RET_VAL *evalRem(AST_NODE *op1, AST_NODE *op2) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if ((op1 == NULL || op2 == NULL) || (op1 && op2 == NULL)) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }else if (op1->next != NULL || op2->next != NULL){
        warning("Too many arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->type = op1->type || op2->type;
    result->value = fmod(op1->data.function.opList->data.number.value, op2->data.function.opList->data.number.value);


    return result;
}

RET_VAL *evalPow(AST_NODE *op1, AST_NODE *op2) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if ((op1 == NULL || op2 == NULL) || (op1 && op2 == NULL)) {
        warning("Too few arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }else if (op1->next != NULL || op2->next != NULL){
        warning("Too many arguments in addition.");
        *result = NAN_RET_VAL;
        return result;
    }

    result->value = pow(op1->data.function.opList->data.number.value, op2->data.function.opList->data.number.value);
    result->type = op1->data.number.type;

    return result;
}

//n-ary functions
RET_VAL *evalAdd(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op == NULL) {
        warning("Too few arguments in addition.");
        return 0;
    }

    result->type = op->data.number.type;
    while (op != NULL){
        double number = op->data.function.opList->data.number.value;
        result->value += number;
        op = op->next;
    }

    printf("%f\n", result->value);

    return result;
}
RET_VAL *evalMult(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op->data.function.opList == NULL){
        warning("mult called with no operands!  nan returned!");
        *result = NAN_RET_VAL;
        return result;
    }

    result->type = op->data.number.type;
    result->value = 1;
    while (op != NULL){
        double number = op->data.function.opList->data.number.value;
        result->value *= number;
        op = op->next;
    }

    printf("%f\n", result->value);

    return result;
}
RET_VAL *evalHypot(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

    if (op->data.function.opList == NULL){
        warning("hypot called with no operands!  nan returned!");
        *result = NAN_RET_VAL;
        return result;
    }

    while (op != NULL){
        double number = pow(op->data.function.opList->data.number.value, 2);
        result->value += number;
        op = op->next;
    }

//    result->type = op->data.number.type;
    result->value = sqrt(result->value);
    printf("result: %f\n", result->value);

    return result;
}

RET_VAL *evalMax(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

//    if (op->data.function.opList == NULL){
//        warning("max called with no operands!  nan returned!");
//        *result = NAN_RET_VAL;
//        return result;
//    }

    result->type = op->data.number.type;
    result->value = op->data.function.opList->data.number.value;
    while (op != NULL){
        result->value = fmax(result->value, op->data.function.opList->data.number.value);
        op = op->next;
    }

    printf("result %f\n", result->value);

    return result;
}

RET_VAL *evalMin(AST_NODE *op) {
    RET_VAL *result = malloc(sizeof(RET_VAL));

//    if (op->data.function.opList == NULL){
//        warning("min called with no operands!  nan returned!");
//        *result = NAN_RET_VAL;
//        return result;
//    }

    result->type = op->data.number.type;
    result->value = op->data.function.opList->data.number.value;

    while (op != NULL) {
        result->value = fmin(result->value, op->data.function.opList->data.number.value);
        op = op->next;
    }

    printf("result %f\n", result->value);

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
            evalNeg(node->data.function.opList);
            break;
        case ABS_FUNC:
            evalAbs(node->data.function.opList);
            break;
        case EXP_FUNC:
            evalExp(node->data.function.opList);
            break;
        case EXP2_FUNC:
            evalExp2(node->data.function.opList);
            break;
        case LOG_FUNC:
            evalLog(node->data.function.opList);
            break;
        case SQRT_FUNC:
            evalSqrt(node->data.function.opList);
            break;
        case CBRT_FUNC:
            evalCbrt(node->data.function.opList);
            break;
            //binary
        case SUB_FUNC:
            evalSub(node->data.function.opList, node->data.function.opList->next);
            break;
        case DIV_FUNC:
            evalDiv(node->data.function.opList, node->data.function.opList->next);
            break;
        case REMAINDER_FUNC:
            evalRem(node->data.function.opList, node->data.function.opList->next);
            break;
        case POW_FUNC:
            evalPow(node->data.function.opList, node->data.function.opList->next);
            break;
        //n-ary
        case ADD_FUNC:
            evalAdd(node->data.function.opList);
            break;
        case MULT_FUNC:
            evalMult(node->data.function.opList);
            break;
        case HYPOT_FUNC:
            evalHypot(node->data.function.opList);
            break;
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

    // TODO complete the function - done???
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

    // TODO complete the function - done???
    switch (node->type) {
        case NUM_NODE_TYPE:
            evalNumNode(node);
            break;
        case FUNC_NODE_TYPE:
            evalFuncNode(node);
            break;
    }

    return NAN_RET_VAL;
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

    // and, finally,
    free(node);
}
