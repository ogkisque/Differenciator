#include "tree.h"

#include "Dotter.h"

const char* NAME_DOT        = "pic.dot";
const int   MAX_TEXT_SIZE   = 100;
const int   MAX_SIZE        = 100;

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

Error nodes_read (Tree* tree, Node** node, FILE* file)
{
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file.");

    char text[MAX_SIZE] = "";
    fscanf (file, "%s", text);
    if (strcmp (text, "(") != 0)
    {
        tree->size++;
        RETURN_ERROR(CORRECT, "");
    }

    Error error = new_node (NUM, 0, node);

    nodes_read (tree, &((*node)->left), file);

    error = read_value (file, node);
    PARSE_ERROR_WITHOUT_TREE(error);

    nodes_read (tree, &((*node)->right), file);
    fscanf (file, "%s", text);
    return error;
}

Error read_value (FILE* file, Node** node)
{
    char text[MAX_SIZE] = "";
    fscanf (file, "%s", text);
    double value = 0;
    if (sscanf (text, "%lf", &value) == 1)
    {
        (*node)->type = NUM;
        (*node)->value = value;
        RETURN_ERROR(CORRECT, "");
    }

    if (strcmp (text, "x") == 0)
    {
        (*node)->type = VAR;
        (*node)->value = 0;
        RETURN_ERROR(CORRECT, "");
    }

    (*node)->type = OPER;
    char oper[MAX_SIZE] = "";
    sscanf (text, "%s", oper);
    if (strcmp (oper, "+") == 0)
        (*node)->value = ADD;
    else if (strcmp (oper, "*") == 0)
        (*node)->value = MUL;
    if (strcmp (oper, "-") == 0)
        (*node)->value = SUB;
    if (strcmp (oper, "/") == 0)
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
