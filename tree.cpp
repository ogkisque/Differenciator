#include "tree.h"

#include "Dotter.h"

const char*     NAME_DOT        = "pic.dot";

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

    if (node->type == OPER)
        switch ((int) node->value)
        {
            case ADD: return left_val + right_val;
            case MUL: return left_val * right_val;
            case SUB: return left_val - right_val;
            case DIV: return left_val / right_val;
            default:  return 0;
        }
    else if (node->type == FUNC)
    {
        switch ((int) node->value)
        {
            case COS:  return cos   (right_val);
            case SIN:  return sin   (right_val);
            case POW:  return pow   (left_val, right_val);
            case SQRT: return sqrt  (right_val);
            case LN:   return log   (right_val);
            default:   return       0;
        }
    }
    printf ("Error!");
    return 0;
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
    else if (node->type == FUNC)
        dtNodeStyle ().fillcolor ("#F9FF15");
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

    if (node->type == FUNC)
    {
        switch ((int) node->value)
        {
            case COS:
                return _MUL(_MUL(_SIN(NULL, _COPY(_RIGHT)), create_node (NUM, -1, NULL, NULL)),
                            _D(_COPY(_RIGHT)));
            case SIN:
                return _MUL(_COS(NULL, _COPY(_RIGHT)),
                            _D(_COPY(_RIGHT)));
            case LN:
                return _MUL(_DIV(create_node (NUM, 1, NULL, NULL), _COPY(_RIGHT)),
                            _D(_COPY(_RIGHT)));
            case SQRT:
                return _MUL(_DIV(create_node (NUM, 1, NULL, NULL), _MUL(create_node (NUM, 2, NULL, NULL), _COPY(node))),
                            _D(_COPY(_RIGHT)));
            case POW:
                if (LTYP == NUM && RTYP != NUM)
                    return _MUL(_MUL(_LN(NULL, _COPY(_LEFT)), _COPY(node)),
                                _D(_COPY(_RIGHT)));
                else if (RTYP == NUM && LTYP != NUM)
                    return _MUL(_MUL(_COPY(_RIGHT), _POW(_COPY(_LEFT), create_node (NUM, RVAL - 1, NULL, NULL))),
                                _D(_COPY(_LEFT)));
                else
                    return _MUL(_COPY(node), _D(_MUL(_COPY(_RIGHT), _LN(NULL, _COPY(_LEFT)))));
            default:
                printf ("Getting into default in switch!\n");
                return NULL;
        }
    }

    if (node->type == OPER)
    {
        switch ((int) node->value)
        {
            case ADD:
                return _ADD(_D(_LEFT), _D(_RIGHT));
            case SUB:
                return _SUB(_D(_LEFT), _D(_RIGHT));
            case MUL:
                return _ADD(_MUL(_D(_LEFT), _COPY(_RIGHT)), _MUL(_COPY(_LEFT), _D(_RIGHT)));
            case DIV:
                return _DIV(_SUB(_MUL(_D(_LEFT), _COPY(_RIGHT)), _MUL(_COPY(_LEFT), _D(_RIGHT))), _MUL(_COPY(_RIGHT), _COPY(_RIGHT)));
            default:
                printf ("Getting into default in switch!\n");
                return NULL;
        }
    }

    printf ("Error!");
    return NULL;
}

void simple (Tree* tree)
{
    bool is_change1 = true;
    bool is_change2 = true;
    while (is_change1 || is_change2)
    {
        is_change1 = false;
        simple_nums (tree->root, &is_change1);
        is_change2 = false;
        simple_vars (tree->root, &is_change2);
    }
}

void simple_nums (Node* node, bool* is_change)
{
    if (!node)
        return;

    if (node->type != OPER && node->type != FUNC)
        return;

    simple_nums (node->left, is_change);
    simple_nums (node->right, is_change);

    if (node->type == OPER)
        simple_nums_oper (node, is_change);
    else if (node->type == FUNC)
        simple_nums_func (node, is_change);
}

void simple_nums_oper (Node* node, bool* is_change)
{
    if (LTYP == NUM && RTYP == NUM)
    {
        *is_change = true;
        node->type = NUM;
        switch ((int) node->value)
        {
            case ADD:
                node->value = LVAL + RVAL;
                break;
            case MUL:
                node->value = LVAL * RVAL;
                break;
            case SUB:
                node->value = LVAL - RVAL;
                break;
            case DIV:
                node->value = LVAL / RVAL;
                break;
            default:
                printf ("Getting into default in switch!\n");
                break;
        }

        nodes_dtor (node->left);
        nodes_dtor (node->right);
        node->left = NULL;
        node->right = NULL;
    }
}

void simple_nums_func (Node* node, bool* is_change)
{
    bool tmp_change = false;
    if (RTYP == NUM)
    {
        switch ((int) node->value)
        {
            case COS:
                node->value = cos (RVAL);
                tmp_change = true;
                break;
            case SIN:
                node->value = sin (RVAL);
                tmp_change = true;
                break;
            case LN:
                node->value = log (RVAL);
                tmp_change = true;
                break;
            case SQRT:
                node->value = sqrt (RVAL);
                tmp_change = true;
                break;
            case POW:
                if (LTYP == NUM)
                {
                    node->value = pow (LVAL, RVAL);
                    tmp_change = true;
                }
                break;
            default:
                printf ("Getting into default in switch!\n");
                break;
        }

        if (tmp_change)
        {
            node->type = NUM;
            *is_change = true;

            nodes_dtor (node->left);
            nodes_dtor (node->right);
            node->left = NULL;
            node->right = NULL;
        }
    }
}

void simple_vars (Node* node, bool* is_change)
{
    if (!node)
        return;

    if (node->type != OPER && node->type != FUNC)
        return;

    simple_vars (node->left, is_change);
    simple_vars (node->right, is_change);

    if (mul_zero (node) || div_zero (node))
    {
        node->type = NUM;
        node->value = 0;
        *is_change = true;

        nodes_dtor (node->left);
        nodes_dtor (node->right);

        node->left = NULL;
        node->right = NULL;
        return;
    }

    Node* new_simple_node = NULL;
    if (mul_one (node, &new_simple_node) || add_sub_zero (node, &new_simple_node) || div_one (node, &new_simple_node) || pow_one (node, &new_simple_node))
    {
        if (new_simple_node == _RIGHT)
            del_node (_LEFT);
        else
            del_node (_RIGHT);

        node->type = new_simple_node->type;
        node->value = new_simple_node->value;
        node->right = new_simple_node->right;
        node->left = new_simple_node->left;
        *is_change = true;

        del_node (new_simple_node);
        return;
    }

    if (pow_zero (node) || one_pow (node))
    {
        node->type = NUM;
        node->value = 1;
        *is_change = true;

        nodes_dtor (node->left);
        nodes_dtor (node->right);

        node->left = NULL;
        node->right = NULL;
        return;
    }
}

bool pow_zero (Node* node)
{
    return ((node->type == FUNC) && ((int) node->value == POW) &&
            (RTYP == NUM) && (is_zero (RVAL)));
}

bool one_pow (Node* node)
{
    return ((node->type == FUNC) && ((int) node->value == POW) &&
            (LTYP == NUM) && (is_zero (LVAL - 1)));
}

bool pow_one (Node* node, Node** new_simple_node)
{
    if ((node->type == FUNC) && ((int) node->value == POW))
    {
        if ((RTYP == NUM) && (is_zero (RVAL - 1)))
        {
            *new_simple_node = _LEFT;
            return true;
        }
    }
    return false;
}

bool mul_zero (Node* node)
{
    return ((node->type == OPER) &&
            ((int) node->value == MUL &&
            ((is_zero (LVAL) && LTYP == NUM) ||
            (is_zero (RVAL) && RTYP == NUM))));
}

bool mul_one (Node* node, Node** new_simple_node)
{
    if (node->type == OPER && (int) node->value == MUL)
    {
        if (is_zero (LVAL - 1) && LTYP == NUM)
        {
            *new_simple_node = _RIGHT;
            return true;
        }
        else if (is_zero (RVAL - 1) && RTYP == NUM)
        {
            *new_simple_node = _LEFT;
            return true;
        }
    }
    return false;
}

bool add_sub_zero (Node* node, Node** new_simple_node)
{
    if ((node->type == OPER) && ((int) node->value == ADD))
    {
        if (is_zero (LVAL) && LTYP == NUM)
        {
            *new_simple_node = _RIGHT;
            return true;
        }
        else if (is_zero (RVAL) && RTYP == NUM)
        {
            *new_simple_node = _LEFT;
            return true;
        }
    }
    return false;
}

bool div_one (Node* node, Node** new_simple_node)
{
    if ((node->type == OPER) && ((int) node->value == DIV))
    {
        if (RTYP == NUM && is_zero (RVAL - 1))
        {
            *new_simple_node = _LEFT;
            return true;
        }
    }
    return false;
}

bool div_zero (Node* node)
{
    return ((node->type == OPER) &&
            (((int) node->value == DIV) &&
            (LTYP == NUM && is_zero (LVAL))));
}

bool is_zero (double x)
{
    return (abs (x) < EPS);
}

void del_node (Node* node)
{
    node->left = NULL;
    node->right = NULL;
    free (node);
}

void print_latex_begin (FILE* file)
{
    fprintf (file,
    "\\documentclass[a4paper,12pt]{article}\n"
    "\\usepackage[T1]{fontenc}\n"
    "\\usepackage[utf8]{inputenc}\n"
    "\\usepackage[english,russian]{babel}\n"
    "\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}\n"
    "\\usepackage[left=10mm, top=10mm, right=10mm, bottom=20mm, nohead, nofoot]{geometry}\n"
    "\\usepackage{wasysym}\n"
    "\\author{\\LARGEМерзляков Арсений}\n"
    "\\title{Производная}\n"
    "\\pagestyle {empty}\n"
    "\\begin{document}\n"
    "\\maketitle\n"
    "\\begin{flushleft}\n"
    "\\Large\n");
}

void print_latex_end (FILE* file)
{
    fprintf (file,
    "\\end{flushleft}\n"
    "\\end{document}");
}

void print_latex_trans (Node* node, FILE* file)
{
    fprintf (file, "Абсолютно тривиально, что это будет:\n\n$");
    print_form (node, file);
    fprintf (file, "$\n\n");
}

void print_form (Node* node, FILE* file)
{
    if (!node)
        return;

    if (node->type == NUM)
    {
        if (node->value < 0)
            fprintf (file, "(%.3lf)", node->value);
        else
            fprintf (file, "%.3lf", node->value);
        return;
    }

    if (node->type == VAR)
    {
        fprintf (file, "x");
        return;
    }

    if (node->type == OPER)
    {
        switch ((int) node->value)
        {
            case DIV:
                fprintf (file, " \\dfrac{");
                print_form (node->left, file);
                fprintf (file, "}{");
                print_form (node->right, file);
                fprintf (file, "} ");
                break;
            case MUL:
                print_form (node->left, file);
                fprintf (file, " \\cdot ");
                print_form (node->right, file);
                break;
            case ADD:
                fprintf (file, "(");
                print_form (node->left, file);
                fprintf (file, "+");
                print_form (node->right, file);
                fprintf (file, ")");
                break;
            case SUB:
                fprintf (file, "(");
                print_form (node->left, file);
                fprintf (file, "-");
                print_form (node->right, file);
                fprintf (file, ")");
                break;
            default:
                printf ("Getting in default in latex_print!\n");
                break;
        }
    }
    else if (node->type == FUNC)
    {
        switch ((int) node->value)
        {
            case COS:
                fprintf (file, "cos");
                print_form (node->right, file);
                break;
            case SIN:
                fprintf (file, "sin");
                print_form (node->right, file);
                break;
            case LN:
                fprintf (file, "\\ln{(");
                print_form (node->right, file);
                fprintf (file, ")}");
                break;
            case SQRT:
                fprintf (file, "\\sqrt{");
                print_form (node->right, file);
                fprintf (file, "}");
                break;
            case POW:
                fprintf (file, "(");
                print_form (node->left, file);
                fprintf (file, ")^{");
                print_form (node->right, file);
                fprintf (file, "}");
                break;
            default:
                printf ("Getting in default in latex_print!\n");
                break;
        }
    }
}

void generate_pdf (const char* file_name)
{
    char text[MAX_TEXT_SIZE] = "";
    sprintf (text, "pdflatex %s", file_name);
    system (text);
}

void print_latex_func (Node* node, FILE* file)
{
    fprintf (file, "$f(x) = ");
    print_form (node, file);
    fprintf (file, "$\n\nПосчитаем производную $f^{'}(x)$\n\n");
}
