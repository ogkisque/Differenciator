#ifndef TREE_HEADER
#define TREE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <time.h>

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

const int       MAX_STR_SIZE    = 500;
const int       MAX_TEXT_SIZE   = 200;
const int       MAX_SIZE        = 100;
const int       VAR_DEF_VAL     = 2;
const double    EPS             = 1e-3;
const int       MAX_NUM_VARS    = 10;

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
    COS     = 1,
    SIN     = 2,
    POW     = 3,
    SQRT    = 4,
    LN      = 5,
    TG      = 6,
    ARCSIN  = 7,
    ARCCOS  = 8,
    ARCTG   = 9
};

struct Var
{
    char*   name;
    double  value;
};

struct Vars
{
    Var vars[MAX_NUM_VARS];
    int num_vars;
};

struct Node
{
    Types   type;
    double  value;
    char*   name;
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

Error   tree_ctor                   (Tree* tree, const char* name, const char* file, const char* func, int line);
Error   tree_dtor                   (Tree* tree);
Error   read_file                   (FILE* file, ReadStr* str);
Error   nodes_print                 (const Node* node, FILE* file);
void    val_to_str                  (const Node* node, char* str);
//Error   nodes_read                  (Tree* tree, Node** node, ReadStr* str, Vars* vars);
Error   read_value                  (ReadStr* str, Node** node, Vars* vars);
Error   new_node                    (Types type, double value, char* name, Node** adres);
Error   node_ctor                   (Types type, double value, char* name, Node* node);
void    nodes_dtor                  (Node* node);
Error   tree_verify                 (const Tree* tree);
bool    is_cycles                   (Node* node);
void    get_points                  (Node* node, Node** points, size_t pos);
int     comparator                  (const void* p1, const void* p2);
bool    read_func                   (ReadStr* str, Node** node);
bool    read_oper                   (ReadStr* str, Node** node);
Error   read_var                    (ReadStr* str, Node** node, Vars* vars);
bool    read_num                    (ReadStr* str, Node** node);
bool    is_zero                     (double x);
Error   vars_dtor                   (Vars* vars);
Error   append_var                  (Vars* vars, char* name);
bool    found_var                   (Vars* vars, char* name);


#endif //TREE_HEADER
