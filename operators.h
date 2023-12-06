#ifndef OPERATORS_HEADER
#define OPERATORS_HEADER

#include "tree.h"

const size_t NUM_OPERS = 15;

struct Operator
{
    const char* text;
    Types type;
    int   value;
};

const Operator OPERATORS[] = {  {"+",       OPER, ADD},
                                {"-",       OPER, SUB},
                                {"*",       OPER, MUL},
                                {"/",       OPER, DIV},
                                {"(",       OPER, LBRAC},
                                {")",       OPER, RBRAC},
                                {"sin",     FUNC, SIN},
                                {"cos",     FUNC, COS},
                                {"tg",      FUNC, ADD},
                                {"arctg",   FUNC, ADD},
                                {"arcsin",  FUNC, ADD},
                                {"arccos",  FUNC, ADD},
                                {"^",       FUNC, POW},
                                {"sqrt",    FUNC, POW},
                                {"ln",      FUNC, LN}};

#endif //OPERATORS_HEADER
