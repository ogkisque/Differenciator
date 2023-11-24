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

const int MAX_STR_SIZE = 500;

enum Types
{
    NUM  = 1,
    OPER = 2,
    VAR  = 3
};

enum Opers
{
    ADD = 1,
    MUL = 2,
    SUB = 3,
    DIV = 4
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
Error   read_value          (ReadStr* str, Node** node);
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
void    get_points          (Node* node, Node* points[], size_t pos);
int     comparator          (const void* p1, const void* p2);
Node*   create_node         (Types type, double value, Node* left, Node* right);
Node*   copy_node           (const Node* old_node);
Node*   dif                 (const Node* node);
Node*   simple              (Node* node);
bool    is_zero             (double x);

#endif //TREE_HEADER
