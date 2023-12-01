#include "diff.h"
#include "dump.h"
#include "graph.h"
#include "latex.h"
#include "tree.h"

#define VERIFY(tree)                            \
        error = tree_verify (&tree);            \
        if (error.code != CORRECT)              \
        {                                       \
            tree_dump (&tree, error);           \
            tree_graph_dump (&tree, error);     \
            return error.code;                  \
        }

const char*     LATEX_NAME              = "latex/latex.tex";
const char*     INPUT_NAME              = "input.txt";
const char*     OUTPUT_NAME             = "output.txt";

int main ()
{
    FILE* file_latex = fopen (LATEX_NAME, "w");
    print_latex_begin (file_latex);

    Vars vars = {};
    Tree func = {};
    ReadStr str = {};
    Error error = {};
    TREE_CTOR(&func);

    FILE* file = fopen (INPUT_NAME, "r");
    read_file (file, &str);
    fclose (file);
    error = nodes_read (&func, &(func.root), &str, &vars);
    simple (&func);

    print_latex_func_vars (func.root, file_latex, &vars);

    if (vars.num_vars == 1)
        exec_one_var (&func, file_latex, vars.vars[0].name);
    else
        exec_vars (&func, file_latex, &vars);

    print_latex_end (file_latex);
    fclose (file_latex);
    generate_pdf (LATEX_NAME);

    tree_dtor (&func);
    vars_dtor (&vars);

    fclose (file_latex);
/*
    double x_taylor = 0.0;
    double x_tangent = 0.0;
    size_t order = 0;
    get_points_taylor_tangent (&x_taylor, &x_tangent, &order);

    srand ((unsigned int) time (0));
    Tree func = {};
    ReadStr str = {};
    Error error = {};
    FILE* file_latex = fopen (LATEX_NAME, "w");
    print_latex_begin (file_latex);
    TREE_CTOR(&func);

    FILE* file = fopen (INPUT_NAME, "r");
    read_file (file, &str);
    fclose (file);
    error = nodes_read (&func, &(func.root), &str);

    simple (&func);
    print_latex_func (func.root, file_latex);
    fprintf (file_latex, "Посчитаем то, что считается устно в садике - производную:\n\n");
    Tree diff = {};
    TREE_CTOR(&diff);
    diff.root = dif (func.root, file_latex, true);
    simple (&diff);
    print_latex_simple (diff.root, file_latex);

    Tree tangent = {};
    TREE_CTOR(&tangent);
    tangent.root = get_tangent (func.root, diff.root, x_tangent);

    Tree taylor = {};
    TREE_CTOR(&taylor);
    taylor.root = get_taylor (&func, x_taylor, order, file_latex);
    print_latex_taylor (taylor.root, file_latex, x_taylor, order);

    Tree diff_func_taylor = {};
    TREE_CTOR(&diff_func_taylor);
    diff_func_taylor.root = get_difference (&func, &taylor);

    double delta = get_delta (diff_func_taylor.root, x_taylor);

    get_two_graph_pdf (func.root, tangent.root, x_tangent, 5,
                       "График и касательная", NAME_1GRAPH_SCRIPT, NAME_1GRAPH_PDF);

    get_two_graph_pdf (func.root, taylor.root, x_taylor, delta,
                       "Сравнение графиков функции и её разложения", NAME_2GRAPH_SCRIPT, NAME_2GRAPH_PDF);

    get_graph_pdf (diff_func_taylor.root, x_taylor, delta,
                   "Разность функции и её разложения", NAME_3GRAPH_SCRIPT, NAME_3GRAPH_PDF);

    print_latex_graph (file_latex, NAME_1GRAPH_PDF);
    print_latex_graph (file_latex, NAME_2GRAPH_PDF);
    print_latex_graph (file_latex, NAME_3GRAPH_PDF);
    print_latex_end (file_latex);
    fclose (file_latex);
    generate_pdf (LATEX_NAME);

    tree_dtor (&diff);
    tree_dtor (&func);
    tree_dtor (&taylor);
    tree_dtor (&diff_func_taylor);
*/
    return 0;
}
