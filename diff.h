#ifndef DIFF_HEADER
#define DIFF_HEADER

#include "tree.h"
#include "latex.h"
#include "graph.h"
#include "dump.h"

#define LVAL                    node->left->value
#define RVAL                    node->right->value
#define LTYP                    node->left->type
#define RTYP                    node->right->type
#define _ADD(left, right)       create_node (OPER,  ADD,            NULL, left, right)
#define _MUL(left, right)       create_node (OPER,  MUL,            NULL, left, right)
#define _DIV(left, right)       create_node (OPER,  DIV,            NULL, left, right)
#define _SUB(left, right)       create_node (OPER,  SUB,            NULL, left, right)
#define _UNSUB(left, right)     create_node (OPER,  UNSUB,          NULL, left, right)
#define _COS(left, right)       create_node (FUNC,  COS,            NULL, left, right)
#define _SIN(left, right)       create_node (FUNC,  SIN,            NULL, left, right)
#define _SQRT(left, right)      create_node (FUNC,  SQRT,           NULL, left, right)
#define _POW(left, right)       create_node (FUNC,  POW,            NULL, left, right)
#define _LN(left, right)        create_node (FUNC,  LN,             NULL, left, right)
#define _TG(left, right)        create_node (FUNC,  TG,             NULL, left, right)
#define _ARCSIN(left, right)    create_node (FUNC,  ARCSIN,         NULL, left, right)
#define _ARCCOS(left, right)    create_node (FUNC,  ARCSIN,         NULL, left, right)
#define _ARCTG(left, right)     create_node (FUNC,  ARCTG,          NULL, left, right)
#define _NUM(value)             create_node (NUM,   value,          NULL, NULL, NULL)
#define _VAR(name)              create_node (VAR,   VAR_DEF_VAL,    name, NULL, NULL)
#define _LBRAC                  create_node (OPER,  LBRAC,          NULL, NULL, NULL)
#define _RBRAC                  create_node (OPER,  RBRAC,          NULL, NULL, NULL)
#define _END                    create_node (OPER,  END,            NULL, NULL, NULL)
#define _LEFT                   node->left
#define _RIGHT                  node->right
#define _COPY(node)             copy_node (node)
#define _D(node)                dif (node, file, var_name, need_latex)

const double    DELTA           = 0.5;
const double    DELTA_STEP      = 0.1;

Node*   create_node                 (Types type, double value, char* name, Node* left, Node* right);
Node*   copy_node                   (const Node* old_node);
Node*   dif                         (const Node* node, FILE* file, const char* var_name, bool need_latex);
void    simple                      (Tree* tree);
bool    mul_zero                    (Node* node);
bool    mul_one                     (Node* node, Node** new_simple_node);
bool    add_sub_zero                (Node* node, Node** new_simple_node);
bool    div_one                     (Node* node, Node** new_simple_node);
bool    div_zero                    (Node* node);
void    simple_vars                 (Node* node, bool* is_change);
void    simple_nums                 (Node* node, bool* is_change);
void    simple_nums_oper            (Node* node, bool* is_change);
void    simple_nums_func            (Node* node, bool* is_change);
bool    pow_zero                    (Node* node);
bool    one_pow                     (Node* node);
bool    pow_one                     (Node* node, Node** new_simple_node);
void    del_node                    (Node* node);
Node*   get_taylor                  (Tree* func, double x, char* var_name, size_t order, FILE* file);
double  factorial                   (size_t x);
Node*   get_difference              (Tree* tree1, Tree* tree2);
double  get_delta                   (const Node* node, double x);
Node*   get_tangent                 (const Node* func, const Node* diff, double x, char* var_name);
void    get_points_taylor_tangent   (double* x_taylor, double* x_tangent, size_t* order);
double  eval                        (const Node* node, double x);
void    exec_one_var                (Tree* func, FILE* file_latex, char* var_name);
void    exec_vars                   (Tree* func, FILE* file_latex, Vars* vars);
void    eval_difs                   (Tree* func, FILE* file_latex, Vars* vars, Tree diffs[]);

#endif //DIFF_HEADER
