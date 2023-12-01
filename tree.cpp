#include "tree.h"

Error new_node (Types type, double value, Node** adres)
{
    if (!adres)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pointer of adress.");

    *adres = (Node*) calloc (1, sizeof (Node));

    if (!(*adres))
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory of node.");

    Error error = node_ctor (type, value, *adres);
    return error;
}

Error node_ctor (Types type, double value, Node* node)
{
    node->left  = NULL;
    node->right = NULL;
    node->type  = type;
    node->value = value;
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
        sprintf (str, "x");
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
            case DIV:
                sprintf (str, " / ");
                return;
            default:
                return;
        }
    }
}

Error nodes_read (Tree* tree, Node** node, ReadStr* str)
{
    if (!str)
        RETURN_ERROR(NULL_POINTER, "Null pointer of str.");

    if (str->pos > str->size)
        RETURN_ERROR(POS_MORE_SIZE, "Position in str is more than its size.");

    char text[MAX_SIZE] = "";
    sscanf (str->str + str->pos, "%s", text);
    if (strcmp (text, "(") != 0)
        RETURN_ERROR(CORRECT, "");

    tree->size++;
    str->pos += 2;
    Error error = new_node (NUM, 0, node);

    nodes_read (tree, &((*node)->left), str);

    read_value (str, node);

    nodes_read (tree, &((*node)->right), str);
    str->pos += 2;
    return error;
}

void read_value (ReadStr* str, Node** node)
{
    if (read_num  (str, node)) return;
    if (read_var  (str, node)) return;
    if (read_oper (str, node)) return;
    if (read_func (str, node)) return;
}

bool read_func (ReadStr* str, Node** node)
{
    char text[MAX_SIZE] = "";
    int num_read = 0;
    sscanf (str->str + str->pos, "%s%n", text, &num_read);
    bool is_func = false;
    if (strcmp (text, "cos") == 0)
    {
        is_func = true;
        (*node)->left = NULL;
        (*node)->value = COS;
    }
    else if (strcmp (text, "sin") == 0)
    {
        is_func = true;
        (*node)->left = NULL;
        (*node)->value = SIN;
    }
    else if (strcmp (text, "^") == 0)
    {
        is_func = true;
        (*node)->value = POW;
    }
    else if (strcmp (text, "sqrt") == 0)
    {
        is_func = true;
        (*node)->left = NULL;
        (*node)->value = SQRT;
    }
    else if (strcmp (text, "ln") == 0)
    {
        is_func = true;
        (*node)->left = NULL;
        (*node)->value = LN;
    }
    else if (strcmp (text, "tg") == 0)
    {
        is_func = true;
        (*node)->left = NULL;
        (*node)->value = TG;
    }
    else if (strcmp (text, "arcsin") == 0)
    {
        is_func = true;
        (*node)->left = NULL;
        (*node)->value = ARCSIN;
    }
    else if (strcmp (text, "arccos") == 0)
    {
        is_func = true;
        (*node)->left = NULL;
        (*node)->value = ARCCOS;
    }
    else if (strcmp (text, "arctg") == 0)
    {
        is_func = true;
        (*node)->left = NULL;
        (*node)->value = ARCTG;
    }

    if (is_func)
    {
        str->pos += num_read + 1;
        (*node)->type = FUNC;
    }
    return is_func;
}

bool read_oper (ReadStr* str, Node** node)
{
    char text[MAX_SIZE] = "";
    int num_read = 0;
    sscanf (str->str + str->pos, "%s%n", text, &num_read);
    bool is_oper = false;
    if (strcmp (text, "+") == 0)
    {
        is_oper = true;
        (*node)->value = ADD;
    }
    else if (strcmp (text, "*") == 0)
    {
        is_oper = true;
        (*node)->value = MUL;
    }
    else if (strcmp (text, "-") == 0)
    {
        is_oper = true;
        (*node)->value = SUB;
    }
    else if (strcmp (text, "/") == 0)
    {
        is_oper = true;
        (*node)->value = DIV;
    }

    if (is_oper)
    {
        str->pos += num_read + 1;
        (*node)->type = OPER;
    }
    return is_oper;
}

bool read_var (ReadStr* str, Node** node)
{
    char text[MAX_SIZE] = "";
    sscanf (str->str + str->pos, "%s", text);
    if (strcmp (text, "x") == 0)
    {
        (*node)->type = VAR;
        (*node)->value = VAR_DEF_VAL;
        str->pos += 2;
        return true;
    }
    return false;
}

bool read_num (ReadStr* str, Node** node)
{
    int num_read = 0;
    double value = 0;
    if (sscanf (str->str + str->pos, "%lf%n", &value, &num_read) == 1)
    {
        (*node)->type = NUM;
        (*node)->value = value;
        str->pos += num_read + 1;
        return true;
    }
    return false;
}

Error read_file (FILE* file, ReadStr* str)
{
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file.");

    str->size = fread (str->str, sizeof (char), MAX_STR_SIZE, file);
    str->pos = 0;
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
