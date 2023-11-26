#ifndef TREE_HEADER
#define TREE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#include "colors.h"
#include "error.h"

#define RETURN_ERROR_AND_DUMP(list, code, message)                                          \
        {                                                                                   \
            tree_dump (tree, Error {code, __LINE__, __FILE__, __func__, message});          \
            tree_graph_dump (tree, Error {code, __LINE__, __FILE__, __func__, message});    \
            return Error {code, __LINE__, __FILE__, __func__, message};                     \
        }

#define PARSE_ERROR(tree, error)            \
        if (error.code != CORRECT)          \
        {                                   \
            tree_dump (tree, error);        \
            tree_graph_dump (tree, error);  \
            return error;                   \
        }

#define PARSE_ERROR_WITHOUT_TREE(error)     \
        if (error.code != CORRECT)          \
        {                                   \
            return error;                   \
        }

#define TREE_CTOR(tree) \
        tree_ctor (tree, #tree, __FILE__, __func__, __LINE__)

#define LVAL                node->left->value
#define RVAL                node->right->value
#define LTYP                node->left->type
#define RTYP                node->right->type
#define _ADD(left, right)   create_node (OPER, ADD, left, right)
#define _MUL(left, right)   create_node (OPER, MUL, left, right)
#define _DIV(left, right)   create_node (OPER, DIV, left, right)
#define _SUB(left, right)   create_node (OPER, SUB, left, right)
#define _COS(left, right)   create_node (FUNC, COS, left, right)
#define _SIN(left, right)   create_node (FUNC, SIN, left, right)
#define _SQRT(left, right)  create_node (FUNC, SQRT, left, right)
#define _POW(left, right)   create_node (FUNC, POW, left, right)
#define _LN(left, right)    create_node (FUNC, LN, left, right)
#define _LEFT               node->left
#define _RIGHT              node->right
#define _COPY(node)         copy_node (node)
#define _D(node)            dif (node)

const int       MAX_STR_SIZE    = 500;
const int       MAX_TEXT_SIZE   = 200;
const int       MAX_SIZE        = 100;
const int       VAR_DEF_VAL     = 2;
const double    EPS             = 1e-5;

enum Types
{
    NUM  = 1,
    OPER = 2,
    VAR  = 3,
    FUNC = 4
};

enum Opers
{
    ADD = 1,
    MUL = 2,
    SUB = 3,
    DIV = 4
};

enum Funcs
{
    COS  = 1,
    SIN  = 2,
    POW  = 3,
    SQRT = 4,
    LN   = 5
};

struct Node
{
    Types   type;
    double  value;
    Node*   left;
    Node*   right;
};

struct Tree
{
    Node*       root;
    size_t      size;

    const char* name;
    const char* file;
    const char* func;
    int         line;
};

struct ReadStr
{
    char    str[MAX_STR_SIZE];
    size_t  pos;
    size_t  size;
};

Error   tree_ctor           (Tree* tree, const char* name, const char* file, const char* func, int line);
Error   tree_dtor           (Tree* tree);
double  eval                (const Node* node, double x);
Error   read_file           (FILE* file, ReadStr* str);
Error   nodes_print         (const Node* node, FILE* file);
void    val_to_str          (const Node* node, char* str);
Error   nodes_read          (Tree* tree, Node** node, ReadStr* str);
void    read_value          (ReadStr* str, Node** node);
void    tree_graph_dump     (const Tree* tree, Error error);
Error   new_node            (Types type, double value, Node** adres);
Error   node_ctor           (Types type, double value, Node* node);
void    nodes_dtor          (Node* node);
Error   tree_verify         (const Tree* tree);
void    tree_dump           (const Tree* tree, Error error);
void    nodes_graph_dump    (const Node* node, size_t counter);
void    error_graph_dump    (const Tree* tree, Error error);
void    print_error         (Error error);
bool    is_cycles           (Node* node);
void    get_points          (Node* node, Node** points, size_t pos);
int     comparator          (const void* p1, const void* p2);
Node*   create_node         (Types type, double value, Node* left, Node* right);
Node*   copy_node           (const Node* old_node);
Node*   dif                 (const Node* node);
void    simple              (Tree* tree);
bool    is_zero             (double x);
bool    mul_zero            (Node* node);
bool    mul_one             (Node* node, Node** new_simple_node);
bool    add_sub_zero        (Node* node, Node** new_simple_node);
bool    div_one             (Node* node, Node** new_simple_node);
bool    div_zero            (Node* node);
void    simple_vars         (Node* node, bool* is_change);
void    simple_nums         (Node* node, bool* is_change);
bool    read_func           (ReadStr* str, Node** node);
bool    read_oper           (ReadStr* str, Node** node);
bool    read_var            (ReadStr* str, Node** node);
bool    read_num            (ReadStr* str, Node** node);
void    simple_nums_oper    (Node* node, bool* is_change);
void    simple_nums_func    (Node* node, bool* is_change);
bool    pow_zero            (Node* node);
bool    one_pow             (Node* node);
bool    pow_one             (Node* node, Node** new_simple_node);
void    del_node            (Node* node);
void    print_form          (Node* node, FILE* file);
void    print_latex_begin   (FILE* file);
void    print_latex_end     (FILE* file);
void    print_latex_trans   (Node* node, FILE* file);
void    print_latex_func    (Node* node, FILE* file);
void    generate_pdf        (const char* file_name);
void    print_latex_taylor  (Node* node, FILE* file);
Node*   get_taylor          (Tree* func, double x, size_t order);
double  factorial           (size_t x);

#endif //TREE_HEADER
