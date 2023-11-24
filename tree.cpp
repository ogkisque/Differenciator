#include "tree.h"

#include "Dotter.h"

const char*     NAME_DOT        = "pic.dot";
const int       MAX_TEXT_SIZE   = 200;
const int       MAX_SIZE        = 100;
const int       VAR_DEF_VAL     = 2;
const double    EPS             = 1e-5;

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

void tree_dump (const Tree* tree, Error error)
{
    printf (RED_COL);
    printf ("-------------------------------------\n");
    if (error.code != CORRECT)
    {
        print_error (error);
        if (!tree)
        {
            printf (OFF_COL);
            return;
        }

        printf ("Error in tree: %s\n"
                "Called from file: %s, func: %s, line: %d\n",
                tree->name, tree->file, tree->func, tree->line);
    }
    printf ("Size - %llu\n", tree->size);
    printf (YELLOW_COL);
    nodes_print (tree->root, stdout);
    printf ("\n");
    printf (RED_COL);
    printf ("-------------------------------------\n");
    printf (OFF_COL);
}

Error nodes_print (const Node* node, FILE* file)
{
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file.");

    if (!node)
        RETURN_ERROR(CORRECT, "");

    if (node->type == OPER)
        fprintf (file, "(");
    nodes_print (node->left, file);

    char str[MAX_SIZE] = "";
    val_to_str (node, str);
    fprintf (file, "%s", str);

    nodes_print (node->right, file);
    if (node->type == OPER)
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

    if (node->type == OPER)
    {
        switch ((int) node->value)
        {
            case ADD:
                strcpy (str, " + ");
                return;
            case MUL:
                strcpy (str, " * ");
                return;
            case SUB:
                strcpy (str, " - ");
                return;
            case DIV:
                strcpy (str, " / ");
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

    error = read_value (str, node);
    PARSE_ERROR_WITHOUT_TREE(error);

    nodes_read (tree, &((*node)->right), str);
    str->pos += 2;
    return error;
}

Error read_value (ReadStr* str, Node** node)
{
    char text[MAX_SIZE] = "";
    int num_read = 0;
    double value = 0;
    if (sscanf (str->str + str->pos, "%lf%n", &value, &num_read) == 1)
    {
        (*node)->type = NUM;
        (*node)->value = value;
        str->pos += num_read + 1;
        RETURN_ERROR(CORRECT, "");
    }

    sscanf (str->str + str->pos, "%s", text);
    if (strcmp (text, "x") == 0)
    {
        (*node)->type = VAR;
        (*node)->value = VAR_DEF_VAL;
        str->pos += 2;
        RETURN_ERROR(CORRECT, "");
    }

    (*node)->type = OPER;
    sscanf (str->str + str->pos, "%s%n", text, &num_read);
    str->pos += num_read + 1;
    if (strcmp (text, "+") == 0)
        (*node)->value = ADD;
    else if (strcmp (text, "*") == 0)
        (*node)->value = MUL;
    if (strcmp (text, "-") == 0)
        (*node)->value = SUB;
    if (strcmp (text, "/") == 0)
        (*node)->value = DIV;

    RETURN_ERROR(CORRECT, "");
}

Error read_file (FILE* file, ReadStr* str)
{
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file.");

    str->size = fread (str->str, sizeof (char), MAX_STR_SIZE, file);
    str->pos = 0;
    RETURN_ERROR (CORRECT, "");
}

double eval (const Node* node, double x)
{
    if (!node)
        return NAN;

    if (node->type == NUM)
        return node->value;

    if (node->type == VAR)
        return x;

    double left_val = eval (node->left, x);
    double right_val = eval (node->right, x);

    switch ((int) node->value)
    {
        case ADD: return left_val + right_val;
        case MUL: return left_val * right_val;
        case SUB: return left_val - right_val;
        case DIV: return left_val / right_val;
        default:  return 0;
    }
}

void print_error (Error error)
{
    printf ("Error: %s\n"
            "Code: %d\n"
            "File: %s, function: %s, line: %d\n",
            error.message,
            error.code,
            error.file, error.func, error.line);
}

void tree_graph_dump (const Tree* tree, Error error)
{
    dtBegin (NAME_DOT);
    nodes_graph_dump (tree->root, 1);
    if (error.code != CORRECT)
        error_graph_dump (tree, error);
    dtEnd ();
    dtRender (NAME_DOT);
}

void nodes_graph_dump (const Node* node, size_t counter)
{
    char text[MAX_TEXT_SIZE] = "";
    dtNodeStyle ().shape        ("box")
                  .style        ("rounded, filled")
                  .fontcolor    ("black")
                  .fillcolor    ("#F77152");
    dtLinkStyle ().style        ("bold")
                  .color        ("#4682B4");

    if (node->type == OPER)
        dtNodeStyle ().fillcolor ("#70FC48");
    else if (node->type == VAR)
        dtNodeStyle ().fillcolor ("#4871FC");
    val_to_str (node, text);
    dtNode ((int) counter, text);
    if (node->left)
    {
        nodes_graph_dump (node->left, counter * 2 + 1);
        dtLink ((int) counter, (int) counter * 2 + 1);
    }
    if (node->right)
    {
        nodes_graph_dump (node->right, counter * 2 + 2);
        dtLink ((int) counter, (int) counter * 2 + 2);
    }
}

void error_graph_dump (const Tree* tree, Error error)
{
    char text[MAX_TEXT_SIZE] = "";
    dtNodeStyle ().shape        ("box")
                  .style        ("rounded, filled")
                  .fontcolor    ("black")
                  .fillcolor    ("#FFFF00");

    if (!tree)
        sprintf (text,
                "Error: %s\n"
                "Code: %d\n"
                "File: %s, function: %s, line: %d\n",
                error.message, error.code, error.file, error.func, error.line);
    else
        sprintf (text,
                "Error in list: %s\n"
                "Called from file: %s, func: %s, line: %d\n"
                "Error: %s\n"
                "Code: %d\n"
                "File: %s, function: %s, line: %d\n",
                tree->name, tree->file, tree->func, tree->line,
                error.message, error.code, error.file, error.func, error.line);

    dtNode (0, text);
}

bool is_cycles (Node* node)
{
    Node* points[MAX_SIZE] = {};
    get_points (node, points, 0);

    qsort (points, MAX_SIZE, sizeof (Node*), comparator);
    for (int i = 1; points[i]; i++)
    {
        if (points[i - 1] == points[i])
            return true;
    }

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

Node* create_node (Types type, double value, Node* left, Node* right)
{
    Node* node = NULL;
    new_node (type, value, &node);
    node->left = left;
    node->right = right;
    return node;
}

Node* copy_node (const Node* old_node)
{
    if (!old_node)
        return NULL;

    Node* node = NULL;
    new_node (old_node->type, old_node->value, &node);
    node->left = copy_node (old_node->left);
    node->right = copy_node (old_node->right);
    return node;
}

Node* dif (const Node* node)
{
    if (!node)
        return NULL;

    if (node->type == NUM)
        return create_node (NUM, 0, NULL, NULL);

    if (node->type == VAR)
        return create_node (NUM, 1, NULL, NULL);

    switch ((int) node->value)
    {
        case ADD:
            return create_node (OPER, ADD, dif (node->left), dif (node->right));
        case SUB:
            return create_node (OPER, SUB, dif (node->left), dif (node->right));
        case MUL:
            return create_node (OPER, ADD,
                                create_node (OPER, MUL, dif (node->left), copy_node (node->right)),
                                create_node (OPER, MUL, copy_node (node->left), dif (node->right)));
        case DIV:
            return create_node (OPER, DIV,
                                create_node (OPER, SUB,
                                             create_node (OPER, MUL, dif (node->left), copy_node (node->right)),
                                             create_node (OPER, MUL, copy_node (node->left), dif (node->right))),
                                create_node (OPER, MUL, copy_node (node->right), copy_node (node->right)));
        default:
            printf ("Getting into default in switch!\n");
            return NULL;
    }
}

Node* simple (Node* node)
{
    if (!node)
        return NULL;

    if (node->type == NUM || node->type == VAR)
        return node;

    node->left = simple (node->left);
    node->right = simple (node->right);

    if (node->left->type == NUM && node->right->type == NUM)
    {
        node->type = NUM;
        switch ((int) node->value)
        {
            case ADD:
                node->value = node->left->value + node->right->value;
                break;
            case MUL:
                node->value = node->left->value * node->right->value;
                break;
            case SUB:
                node->value = node->left->value - node->right->value;
                break;
            case DIV:
                node->value = node->left->value / node->right->value;
                break;
            default:
                printf ("Getting into default in switch!\n");
                break;
        }

        nodes_dtor (node->left);
        nodes_dtor (node->right);

        node->left = NULL;
        node->right = NULL;
        return node;
    }

    if ((int) node->value == MUL && (is_zero (node->left->value) || is_zero (node->right->value)))
    {
        node->type = NUM;
        node->value = 0;

        nodes_dtor (node->left);
        nodes_dtor (node->right);

        node->left = NULL;
        node->right = NULL;
        return node;
    }

    if ((int) node->value == MUL &&
        ((is_zero (node->left->value - 1) && node->left->type == NUM)
        || (is_zero (node->right->value - 1) && node->right->type == NUM)))
    {
        node->type = VAR;
        node->value = VAR_DEF_VAL;

        nodes_dtor (node->left);
        nodes_dtor (node->right);

        node->left = NULL;
        node->right = NULL;
        return node;
    }

    return node;
}

bool is_zero (double x)
{
    return (abs (x) < EPS);
}
