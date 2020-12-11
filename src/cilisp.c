/**
* Name: Jamie Hernandez
* Lab: ciLisp Lab
* Date: 11/4/2020
**/
#include "cilisp.h"
#include "math.h"
#include <ctype.h>

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

FUNC_TYPE resolveFunc(char *funcName) {//5
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
            //TASK 4 FUNCTIONS
            "rand",
            "read",
            "equal",
            "less",
            "greater",
            "print",
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

NUM_TYPE resolveNumType(char *type) { //1
    if (strcmp(type, "int") == 0) {
        return INT_TYPE;
    } else if (strcmp(type, "double") == 0) {
        return DOUBLE_TYPE;
    }
    return NO_TYPE;
}

/// create functions
AST_NODE *createNumberNode(double value, NUM_TYPE type) { //2//7
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL) { //super leaks
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

AST_NODE *createFunctionNode(FUNC_TYPE func, AST_NODE *opList, char *name) {//10
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
    node->data.function.id = name;
//    node->parent = NULL;

    while (opList) {
        opList->parent = node;
        opList = opList->next;
    }
    node->symbolTable = NULL;
    node->next = NULL;

    return node;
}

AST_NODE *createSymbolNode(char *id) {//6
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL) {
        yyerror("Memory allocation failed!");
        exit(1);
    }
    node->type = SYM_NODE_TYPE;
    node->data.symbol.id = id;
    node->data.symbol.numType = NO_TYPE;
    node->symbolTable = NULL;
    node->parent = NULL;
    node->next = NULL;

    return node;
}

AST_NODE *createScopeNode(SYMBOL_TABLE_NODE *stNode, AST_NODE *child) {//11
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL) {
        yyerror("Memory allocation failed!");
        exit(1);
    }

    node->type = SCOPE_NODE_TYPE;
    node->data.scope.child = child;
    child->parent = node;

    if (stNode->symbolType == LAMBDA_TYPE) {
        child->argList = stNode;
    } else {
        child->symbolTable = stNode;
    }

    while (stNode && stNode->value) {
        stNode->value->parent = child;
        stNode = stNode->next;
    }

    return node;
}

AST_NODE *createCondNode(AST_NODE *condition, AST_NODE *trueCond, AST_NODE *falseCond) {
    AST_NODE *result;

    result = (condition->data.number.value != 0) ? trueCond : falseCond;

    return result;
}

SYMBOL_TABLE_NODE *createStNode(NUM_TYPE numType, char *id, AST_NODE *value, SYMBOL_TYPE symbolType, SYMBOL_TABLE_NODE *arglist) {//3
    SYMBOL_TABLE_NODE *stNode;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((stNode = calloc(nodeSize, 1)) == NULL) { //leaky lots
        yyerror("Memory allocation failed!");
        exit(1);
    }

    stNode->id = id;
    stNode->value = value;
    stNode->numType = numType;
    stNode->symbolType = symbolType;

    if (numType == INT_TYPE && value->data.number.type == DOUBLE_TYPE) {
        warning("Precision loss on int cast from %f to %d.", value->data.number.value,
                (int) value->data.number.value);
    }

    if (stNode->value) {
        if (symbolType == LAMBDA_TYPE) {
            stNode->value->argList = arglist;
        }
        if (stNode->value->type == SYM_NODE_TYPE) {
            stNode->value->symbolTable->numType = numType;
        }
    }

    stNode->stack = NULL;

    if (arglist) {
        stNode->next = arglist;
    } else {
        stNode->next = NULL;
    }

    return stNode;
}
/// end create functions

/// addTo functions
SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *newExpr, SYMBOL_TABLE_NODE *symTblList) {
    SYMBOL_TABLE_NODE *currNode;
    SYMBOL_TABLE_NODE *temp;

    currNode = symTblList;

    while (currNode) {

        if (strcmp(newExpr->id, currNode->id) == 0) {
            if (currNode == symTblList) {
                symTblList = symTblList->next;
                freeStNode(currNode);
            } else {
                temp->next = currNode->next;
                freeNode((AST_NODE *) currNode);
            }
            break;
        }
        temp = currNode;
        currNode = currNode->next;
    }

    newExpr->next = symTblList;

    return newExpr;
}

AST_NODE *addExpressionToList(AST_NODE *newExpr, AST_NODE *exprList) {//8//9
    newExpr->next = exprList;
    return newExpr;
}
///end addTo functions

/// start list of op functions
/*
 * no arguments
 **/
RET_VAL evalRand() {
    RET_VAL result;

    result.value = (double) (rand()) / (double) (RAND_MAX);

    return result;
}

RET_VAL evalRead() {
    RET_VAL result;
    int size = 12;
    char buff[size];

    printf("read :: ");
    if (read_target != stdin) {
    }
    fgets(buff, size, read_target);

    bool isDub = false;
    for (int i = 0; i < strlen(buff); i++) {
        if (buff[0] == '.') {
            warning("Invalid read entry! NAN returned!");
            return NAN_RET_VAL;
        }
        if (isalpha(buff[i])) {
            warning("Invalid read entry! NAN returned!");
            return NAN_RET_VAL;
        }
        if (buff[i] == '.') {
            isDub = true;
        }
    }

    if (isDub) {
        result.type = DOUBLE_TYPE;
        result.value = strtod(buff, NULL);
    } else {
        result.type = INT_TYPE;
        result.value = strtod(buff, NULL);
    }

    return result;
}

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

    if (op->next != NULL) {
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

    if (op->next != NULL) {
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

    if (op->next != NULL) {
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

    if (op->next != NULL) {
        warning("Too many arguments in exp2.  Ignoring the rest.");
    }
    if (op->data.number.value < 0) {
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

    if (op->next != NULL) {
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

    if (op->next != NULL) {
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

    if (op->next != NULL) {
        warning("Too many arguments in cbrt.  Ignoring the rest.");
    }

    result.value = cbrt(result.value);

    return result;
}

RET_VAL evalPrint(AST_NODE *op) {
    RET_VAL result;

    if (op == NULL) {
        warning("Too few arguments in neg.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);

    if (op->next != NULL) {
        warning("Too many arguments in neg.  Ignoring the rest.");
    }

    printRetVal(result);

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

    if (op->next) {
        warning("Too many arguments.  Ignoring the rest.");
    }


    result2 = eval(op);
    result2.value = result.value - result2.value;
    if (result.type == DOUBLE_TYPE || result2.type == DOUBLE_TYPE) {
        result2.type = DOUBLE_TYPE;
    }


    return result2;
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

    if (op->next) {
        warning("Too many arguments div.  Ignoring the rest.");
    }

    result2 = eval(op);

    if (result.type == INT_TYPE && result2.type == INT_TYPE) {
        result.value = (int) (result.value / result2.value);
    } else {
        result.value = result.value / result2.value;
        result.type = DOUBLE_TYPE;
    }


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

    if (op->next) {
        warning("Too many arguments in rem.  Ignoring the rest.");
    }

    result2 = eval(op);
    result.value = fmod(result.value, result2.value);
    if (result.value < 0) {
        result.value = fabs(result2.value) + result.value;
    }

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

    if (op->next) {
        warning("Too many arguments in pow.  Ignoring the rest.");
    }

    result2 = eval(op);
    result.value = pow(result.value, result2.value);

    return result;
}

RET_VAL evalEqual(AST_NODE *op) {
    //check for binary
    RET_VAL result;
    RET_VAL result2;
    int output;

    if (op == NULL || op->next == NULL) {
        warning("Too few arguments in sub.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    if (op->next) {
        warning("Too many arguments.  Ignoring the rest.");
    }
    result2 = eval(op);
    //if numbers are equal
    if (result.value == result2.value) {
        //return int value 1
        output = 1;
        result.value = output;
    }
        //else
    else {
        //return 0
        output = 0;
        result.value = output;
    }

    return result;
}

RET_VAL evalLess(AST_NODE *op) {
    //check for binary
    RET_VAL result;
    RET_VAL result2;
    int output;

    if (op == NULL || op->next == NULL) {
        warning("Too few arguments in sub.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    if (op->next) {
        warning("Too many arguments.  Ignoring the rest.");
    }
    result2 = eval(op);
    //if number is less
    if (result.value < result2.value) {
        //return int value 1
        output = 1;
        result.value = output;
    }
        //else
    else {
        //return 0
        output = 0;
        result.value = output;
    }

    return result;
}

RET_VAL evalGreater(AST_NODE *op) {
    //check for binary
    RET_VAL result;
    RET_VAL result2;
    int output;

    if (op == NULL || op->next == NULL) {
        warning("Too few arguments in sub.");
        result = NAN_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    if (op->next) {
        warning("Too many arguments.  Ignoring the rest.");
    }
    result2 = eval(op);
    //if number is greater
    if (result.value > result2.value) {
        //return int value 1
        output = 1;
        result.value = output;
    }
        //else
    else {
        //return 0
        output = 0;
        result.value = output;
    }

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

        if (result2.type == DOUBLE_TYPE) {
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

        if (result2.type == DOUBLE_TYPE) {
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

RET_VAL evalMax(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;
    if (op == NULL) {
        warning("Too few arguments in max.");
        result = ZERO_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    while (op) {
        result2 = eval(op);
        result.value = fmax(result.value, result2.value);
        op = op->next;
    }

    return result;
}

RET_VAL evalMin(AST_NODE *op) {
    RET_VAL result;
    RET_VAL result2;
    if (op == NULL) {
        warning("Too few arguments in max.");
        result = ZERO_RET_VAL;
        return result;
    }

    result = eval(op);
    op = op->next;

    while (op) {
        result2 = eval(op);
        result.value = fmin(result.value, result2.value);
        op = op->next;
    }

    return result;
}
///  end list of op functions



///TEST AREA TEST AREA TEST AREA TEST AREA TEST AREA TEST AREA TEST AREA TEST AREA

//TODO 5 - create stack node function
STACK_NODE *createStackNode(RET_VAL value, STACK_NODE *list) {
    STACK_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(STACK_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL) {
        yyerror("Memory allocation failed!");
        exit(1);
    }
    node->next = list;
    node->value = value;

    return node;
}

//TODO - make evalCutom function
RET_VAL evalCustom(AST_NODE *op) {
    AST_NODE *opList = op->parent->data.function.opList;
    RET_VAL result;

    if (opList == NULL) {
        warning("custom called with no operands.");
        return NAN_RET_VAL;
    }

    SYMBOL_TABLE_NODE *symbolTableNode;
    if (op->symbolTable) {
        symbolTableNode = op->symbolTable;
        result = eval(op->symbolTable->value);
    } else {
        symbolTableNode = op->argList->value->symbolTable;
        result = eval(op->argList->value);
    }

    RET_VAL op1 = eval(opList);
    opList = opList->next;
    RET_VAL op2 = eval(opList);

    STACK_NODE *stackNode = createStackNode(op1, op->argList->stack);
    stackNode = createStackNode(op2, op->argList->stack);

    while (symbolTableNode) {
        symbolTableNode->stack = stackNode;
        symbolTableNode = symbolTableNode->next;
    }

    if (op->argList->numType != NO_TYPE) {
        if (op->argList->numType != INT_TYPE) {
            warning("precision lost");
        }
        result.type = op->argList->numType;
    }
    return result;

}


///TEST AREA TEST AREA TEST AREA TEST AREA TEST AREA TEST AREA TEST AREA TEST AREA



/// eval functions
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
        //no args
        case RAND_FUNC:
            return evalRand();
        case READ_FUNC:
            return evalRead();
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
        case PRINT_FUNC:
            return evalPrint(node->data.function.opList);
            //binary
        case SUB_FUNC:
            return evalSub(node->data.function.opList);
        case DIV_FUNC:
            return evalDiv(node->data.function.opList);
        case REMAINDER_FUNC:
            return evalRem(node->data.function.opList);
        case POW_FUNC:
            return evalPow(node->data.function.opList);
        case EQUAL_FUNC:
            return evalEqual(node->data.function.opList);
        case LESS_FUNC:
            return evalLess(node->data.function.opList);
        case GREATER_FUNC:
            return evalGreater(node->data.function.opList);
            //n-ary
        case ADD_FUNC:
            return evalAdd(node->data.function.opList);
        case MULT_FUNC:
            return evalMult(node->data.function.opList);
        case HYPOT_FUNC:
            return evalHypot(node->data.function.opList);
        case MAX_FUNC:
            return evalMax(node->data.function.opList);
        case MIN_FUNC:
            return evalMin(node->data.function.opList);
        case CUSTOM_FUNC:
            return evalCustom(node);
    }
}

RET_VAL evalNumNode(AST_NODE *node) {
    if (!node) {
        yyerror("NULL ast node passed into evalNumNode!");
        return NAN_RET_VAL;
    }

    // TODO complete the function
    RET_VAL result;
    result.value = node->data.number.value;
    result.type = node->data.number.type;

    return result;
}

RET_VAL evalSymbolNode(AST_NODE *node) {//16
    AST_NODE *currScope;
    SYMBOL_TABLE_NODE *stNode;

    if (!node) {
        yyerror("NULL ast node passed into evalSymbolNode!");
        return NAN_RET_VAL;
    }
    currScope = node;

    while (currScope) {

        stNode = currScope->symbolTable;

        while (stNode) {

            if (strcmp(stNode->id, node->data.symbol.id) == 0) {
                RET_VAL result = eval(stNode->value);

                if (stNode->numType == INT_TYPE) {
                    result.type = INT_TYPE;
                } else if (stNode->numType == DOUBLE_TYPE) {
                    result.type = DOUBLE_TYPE;
                }

                if (stNode->value->type != NUM_NODE_TYPE) {
                    freeNode(stNode->value);
                    stNode->value = createNumberNode(result.value, result.type);
                }
                return result;
            }
            stNode = stNode->next;
        }
        currScope = currScope->parent;
    }
    warning("The variable was undefined.");
    return NAN_RET_VAL;
}

RET_VAL evalScopeNode(AST_NODE *node) {//13
    if (!node) {
        yyerror("NULL ast node passed into evalScopeNode!");
        return NAN_RET_VAL;
    }
    return eval(node->data.scope.child);
}

RET_VAL evalCond(AST_NODE *node) {
    if (!node) {
        yyerror("NULL ast node passed into evalScopeNode!");
        return NAN_RET_VAL;
    }
    RET_VAL result;
    AST_COND temp = node->data.condition;

    createCondNode(temp.condition, temp.trueCase, temp.falseCase);

    return result;
}

RET_VAL eval(AST_NODE *node) {//4//12//14//15
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
        case SYM_NODE_TYPE:
            return evalSymbolNode(node);
        case SCOPE_NODE_TYPE:
            return evalScopeNode(node);
        case COND_TYPE:
            return evalCond(node);
    }
}
/// end eval functions


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

/**
/// FREE SECTION
**/

void freeFuncNode(AST_NODE *node) {
    if (!node) {
        return;
    }
    free(node->data.function.id);
    freeNode(node->data.function.opList);
}

void freeStackNode(STACK_NODE *node) {
    if (!node) {
        return;
    }
    freeStackNode(node->next);
}

void freeStNode(SYMBOL_TABLE_NODE *stNode) {
    if (!stNode) {
        return;
    }
    freeStNode(stNode->next);
    free(stNode->id);
    freeNode(stNode->value);
    freeStackNode(stNode->stack);
    free(stNode);
}

//TODO - UPDATE
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

    if (node->symbolTable != NULL) {
        freeStNode(node->symbolTable);
    }

    switch (node->type) {
        case FUNC_NODE_TYPE:
            freeFuncNode(node);
            break;
        case SYM_NODE_TYPE:
            free(node->data.symbol.id);
            break;
        case SCOPE_NODE_TYPE:
            freeNode(node->data.scope.child);
            break;
        case NUM_NODE_TYPE:
            break;
        case COND_TYPE:
            freeFuncNode(node);
            break;
    }

    // and, finally,
    free(node);
}
