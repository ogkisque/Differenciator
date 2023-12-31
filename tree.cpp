#include "tree.h"

Error new_node (Types type, double value, char* name, Node** adres)
{
    if (!adres)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pointer of adress.");

    *adres = (Node*) calloc (1, sizeof (Node));

    if (!(*adres))
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory of node.");

    Error error = node_ctor (type, value, name, *adres);
    return error;
}

Error node_ctor (Types type, double value, char* name, Node* node)
{
    node->left  = NULL;
    node->right = NULL;
    node->type  = type;
    node->value = value;
    if (name)
    {
        node->name  = strdup (name);
        if (!node->name)
            RETURN_ERROR(MEM_ALLOC, "Error of allocation memory for name of var");
    }
    RETURN_ERROR(CORRECT, "");
}

Error tree_ctor (Tree* tree, const char* name, const char* file, const char* func, int line)
{
    if (!tree)
        RETURN_ERROR(NULL_POINTER, "Null pointer of tree.");

    if (!name || !file || !func || line < 0)
        RETURN_ERROR(INCOR_PARAMS, "Incorrect parameters in constructor");

    tree->size = 0;
    tree->root = NULL;
    tree->name = name;
    tree->file = file;
    tree->func = func;
    tree->line = line;

    RETURN_ERROR(CORRECT, "");
}

Error tree_dtor (Tree* tree)
{
    if (!tree)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pointer of tree.");

    nodes_dtor (tree->root);
    tree->size = 0;
    tree->root = NULL;
    RETURN_ERROR(CORRECT, "");
}

void nodes_dtor (Node* node)
{
    if (!node)
        return;
    if (node->left)
        nodes_dtor (node->left);
    if (node->right)
        nodes_dtor (node->right);
    if (node->name)
    {
        free (node->name);
        node->name = NULL;
    }
    node->value = 0;
    free (node);
    node = NULL;
}

Error tree_verify (const Tree* tree)
{
    if (!tree)                                      RETURN_ERROR(NULL_POINTER,      "Null pointer of tree.");
    if (!tree->file || !tree->func || !tree->name)  RETURN_ERROR(INCOR_PARAMS,      "Null pointer of parameters of tree.");
    if (is_cycles (tree->root))                     RETURN_ERROR(CYCLES,            "There are cycles in tree.");
    RETURN_ERROR(CORRECT, "");
}

Error nodes_print (const Node* node, FILE* file)
{
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file.");

    if (!node)
        RETURN_ERROR(CORRECT, "");

    if (node->type == OPER || node->type == FUNC)
        fprintf (file, "(");
    nodes_print (node->left, file);

    char str[MAX_SIZE] = "";
    val_to_str (node, str);
    fprintf (file, "%s", str);

    nodes_print (node->right, file);
    if (node->type == OPER || node->type == FUNC)
        fprintf (file, ")");

    RETURN_ERROR(CORRECT, "");
}

void val_to_str (const Node* node, char* str)
{
    if (node->type == NUM)
    {
        sprintf (str, "%.3lf", node->value);
        return;
    }

    if (node->type == VAR)
    {
        sprintf (str, "%s", node->name);
        return;
    }

    if (node->type == FUNC)
    {
        switch ((int) node->value)
        {
            case COS:
                sprintf (str, "cos ");
                return;
            case SIN:
                sprintf (str, "sin ");
                return;
            case SQRT:
                sprintf (str, "sqrt ");
                return;
            case POW:
                sprintf (str, " ^ ");
                return;
            case LN:
                sprintf (str, "ln ");
                return;
            case TG:
                sprintf (str, "tg ");
                return;
            case ARCSIN:
                sprintf (str, "arcsin ");
                return;
            case ARCCOS:
                sprintf (str, "arccos ");
                return;
            case ARCTG:
                sprintf (str, "arctg ");
                return;
            default:
                printf ("Unknown function!\n");
                return;
        }
    }

    if (node->type == OPER)
    {
        switch ((int) node->value)
        {
            case ADD:
                sprintf (str, " + ");
                return;
            case MUL:
                sprintf (str, " * ");
                return;
            case SUB:
                sprintf (str, " - ");
                return;
            case UNSUB:
                sprintf (str, " - ");
                return;
            case DIV:
                sprintf (str, " / ");
                return;
            default:
                return;
        }
    }
}

bool found_var (Vars* vars, char* name)
{
    for (int i = 0; i < vars->num_vars; i++)
        if (strcmp (name, vars->vars[i].name) == 0)
            return true;

    return false;
}

Error append_var (Vars* vars, char* name)
{
    if (vars->num_vars == MAX_NUM_VARS)
        RETURN_ERROR(VARS_OVERFLOW, "Number of variables is more than max number of variables");

    vars->vars[vars->num_vars].value = VAR_DEF_VAL;
    vars->vars[vars->num_vars].name = strdup (name);
    if (!(vars->vars[vars->num_vars].name))
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory for name of var");

    vars->num_vars++;
    RETURN_ERROR(CORRECT, "");
}

Error read_file (FILE* file, ReadStr* str)
{
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file.");

    str->size = fread (str->str, sizeof (char), MAX_STR_SIZE, file);
    str->pos = 0;
    str->str[str->size - 1] = '\0';
    RETURN_ERROR (CORRECT, "");
}

bool is_cycles (Node* node)
{
    Node* points[MAX_SIZE] = {};
    get_points (node, points, 0);

    qsort (points, MAX_SIZE, sizeof (Node*), comparator);
    for (int i = 1; points[i]; i++)
        if (points[i - 1] == points[i])
            return true;

    return false;
}

void get_points (Node* node, Node* points[], size_t pos)
{
    if (!node)
        return;

    points[pos] = node;
    get_points (node->left, points, pos * 2 + 1);
    get_points (node->right, points, pos * 2 + 2);
}

int comparator (const void* p1, const void* p2)
{
    if ((const Node*) p1 > (const Node*) p2)
        return 1;
    else if ((const Node*) p1 < (const Node*) p2)
        return -1;
    return 0;
}

bool is_zero (double x)
{
    return (abs (x) < EPS);
}

Error vars_dtor (Vars* vars)
{
    if (!vars)
        RETURN_ERROR(NULL_POINTER, "NUll pointer of struct variables");

    for (int i = 0; i < vars->num_vars; i++)
    {
        free (vars->vars[i].name);
        vars->vars[i].name = NULL;
        vars->vars[i].value = 0;
    }
    vars->num_vars = -1;
    RETURN_ERROR(CORRECT, "");
}
