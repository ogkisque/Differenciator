#include "diff.h"

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

Node* dif (const Node* node, FILE* file, bool need_latex)
{
    if (!node)
        return NULL;

    Node* tmp = NULL;

    if (node->type == NUM)
    {
        tmp = create_node (NUM, 0, NULL, NULL);
        if (need_latex)
        {
            print_latex_func (node, file);
            print_latex_phrase (file);
            print_latex_trans (tmp, file);
        }
        return tmp;
    }

    if (node->type == VAR)
    {
        tmp = create_node (NUM, 1, NULL, NULL);
        if (need_latex)
        {
            print_latex_func (node, file);
            print_latex_phrase (file);
            print_latex_trans (tmp, file);
        }
        return tmp;
    }

    if (node->type == FUNC)
    {
        switch ((int) node->value)
        {
            case COS:
                tmp = _MUL(_MUL(_SIN(NULL, _COPY(_RIGHT)), create_node (NUM, -1, NULL, NULL)),
                            _D(_RIGHT, need_latex));
                break;
            case SIN:
                tmp = _MUL(_COS(NULL, _COPY(_RIGHT)),
                            _D(_RIGHT, need_latex));
                break;
            case LN:
                tmp = _MUL(_DIV(create_node (NUM, 1, NULL, NULL), _COPY(_RIGHT)),
                            _D(_RIGHT, need_latex));
                break;
            case SQRT:
                tmp = _MUL(_DIV(create_node (NUM, 1, NULL, NULL), _MUL(create_node (NUM, 2, NULL, NULL), _COPY(node))),
                            _D(_RIGHT, need_latex));
                break;
            case POW:
                if (LTYP == NUM && RTYP != NUM)
                    tmp = _MUL(_MUL(_LN(NULL, _COPY(_LEFT)), _COPY(node)),
                                _D(_RIGHT, need_latex));
                else if (RTYP == NUM && LTYP != NUM)
                    tmp = _MUL(_MUL(_COPY(_RIGHT), _POW(_COPY(_LEFT), create_node (NUM, RVAL - 1, NULL, NULL))),
                                _D(_LEFT, need_latex));
                else
                    tmp = _MUL(_COPY(node), _D(_MUL(_COPY(_RIGHT), _LN(NULL, _COPY(_LEFT))), need_latex));
                break;
            case TG:
                tmp = _MUL(_DIV(create_node (NUM, 1, NULL, NULL), _MUL(_COS(NULL, _COPY(_RIGHT)), _COS(NULL, _COPY(_RIGHT)))),
                            _D(_RIGHT, need_latex));
                break;
            case ARCSIN:
                tmp = _MUL(_DIV(create_node (NUM, 1, NULL, NULL),
                                 _SQRT(NULL, _SUB(create_node (NUM, 1, NULL, NULL),
                                                  _MUL(_COPY(_RIGHT),
                                                       _COPY(_RIGHT))))),
                            _D(_RIGHT, need_latex));
                break;
            case ARCTG:
                tmp = _MUL(_DIV(create_node (NUM, 1, NULL, NULL),
                                 _ADD(create_node (NUM, 1, NULL, NULL),
                                      _MUL(_COPY(_RIGHT),
                                           _COPY(_RIGHT)))),
                            _D(_RIGHT, need_latex));
                break;
            case ARCCOS:
                tmp = _MUL(_MUL(create_node (NUM, -1, NULL, NULL),
                                 _DIV(create_node (NUM, 1, NULL, NULL),
                                      _SQRT(NULL, _SUB(create_node (NUM, 1, NULL, NULL),
                                                       _MUL(_COPY(_RIGHT),
                                                            _COPY(_RIGHT)))))),
                            _D(_RIGHT, need_latex));
                break;
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
                tmp = _ADD(_D(_LEFT, need_latex), _D(_RIGHT, need_latex));
                break;
            case SUB:
                tmp = _SUB(_D(_LEFT, need_latex), _D(_RIGHT, need_latex));
                break;
            case MUL:
                tmp = _ADD(_MUL(_D(_LEFT, need_latex), _COPY(_RIGHT)), _MUL(_COPY(_LEFT), _D(_RIGHT, need_latex)));
                break;
            case DIV:
                tmp = _DIV(_SUB(_MUL(_D(_LEFT, need_latex), _COPY(_RIGHT)), _MUL(_COPY(_LEFT), _D(_RIGHT, need_latex))), _MUL(_COPY(_RIGHT), _COPY(_RIGHT)));
                break;
            default:
                printf ("Getting into default in switch!\n");
                return NULL;
        }
    }

    if (need_latex)
    {
        print_latex_func (node, file);
        print_latex_phrase (file);
        print_latex_trans (tmp, file);
    }

    return tmp;
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
            case TG:
                node->value = tan (RVAL);
                tmp_change = true;
                break;
            case ARCSIN:
                node->value = asin (RVAL);
                tmp_change = true;
                break;
            case ARCCOS:
                node->value = acos (RVAL);
                tmp_change = true;
                break;
            case ARCTG:
                node->value = atan (RVAL);
                tmp_change = true;
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
    if ((node->type == OPER) && ((int) node->value == ADD || (int) node->value == SUB))
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

void del_node (Node* node)
{
    node->left = NULL;
    node->right = NULL;
    free (node);
}

Node* get_taylor (Tree* func, double x, size_t order, FILE* file)
{
    Tree taylor = {};
    Tree old_diff = {};
    Tree new_diff = {};
    TREE_CTOR(&new_diff);
    TREE_CTOR(&old_diff);
    TREE_CTOR(&taylor);
    double val = eval (func->root, x);
    taylor.root = create_node (NUM, val, NULL, NULL);
    old_diff.root = _COPY(func->root);
    new_diff.root = _COPY(func->root);
    for (size_t degree = 1; degree <= order; degree++)
    {
        new_diff.root = dif (old_diff.root, file, false);
        val = eval (new_diff.root, x);
        taylor.root = _ADD(taylor.root, _DIV(_MUL(create_node (NUM, val, NULL, NULL),
                                                  _POW(_SUB(create_node (VAR, VAR_DEF_VAL, NULL, NULL), create_node (NUM, x, NULL, NULL)),
                                                       create_node (NUM, (double) degree, NULL, NULL))),
                                             create_node (NUM, factorial (degree), NULL, NULL)));
        nodes_dtor (old_diff.root);
        old_diff.root = _COPY(new_diff.root);
        nodes_dtor (new_diff.root);
    }
    nodes_dtor (old_diff.root);
    simple (&taylor);
    return taylor.root;
}

double factorial (size_t x)
{
    size_t fact = 1;
    for (size_t i = 1; i <= x; i++)
        fact *= i;
    return (double) fact;
}

Node* get_difference (Tree* tree1, Tree* tree2)
{
    return _SUB(_COPY(tree1->root), _COPY(tree2->root));
}

double get_delta (const Node* node, double x)
{
    double x_max = x;
    while (abs (eval (node, x_max)) < DELTA)
        x_max += DELTA_STEP;
    return (x_max - x);
}

Node* get_tangent (const Node* func, const Node* diff, double x)
{
    return _ADD(create_node (NUM, eval (func, x), NULL, NULL),
                _MUL(create_node (NUM, eval (diff, x), NULL, NULL),
                     _SUB(create_node (VAR, VAR_DEF_VAL, NULL, NULL),
                          create_node (NUM, x, NULL, NULL))));
}

void get_points_taylor_tangent (double* x_taylor, double* x_tangent, size_t* order)
{
    printf ("# Enter the decomposition point:\n");
    scanf ("%lf", x_taylor);
    printf ("# Enter the decomposition order:\n");
    scanf ("%llu", order);
    printf ("# Enter the point where you want to draw the tangent:\n");
    scanf ("%lf", x_tangent);
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
            case COS:       return cos      (right_val);
            case SIN:       return sin      (right_val);
            case POW:       return pow      (left_val, right_val);
            case SQRT:      return sqrt     (right_val);
            case LN:        return log      (right_val);
            case TG:        return tan      (right_val);
            case ARCTG:     return atan     (right_val);
            case ARCSIN:    return asin     (right_val);
            case ARCCOS:    return acos     (right_val);
            default:        return          0;
        }
    }
    printf ("Error!");
    return 0;
}
