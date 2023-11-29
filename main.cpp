#include "tree.h"

#define VERIFY(tree)                            \
        error = tree_verify (&tree);            \
        if (error.code != CORRECT)              \
        {                                       \
            tree_dump (&tree, error);           \
            tree_graph_dump (&tree, error);     \
            return error.code;                  \
        }

const char*     LATEX_NAME              = "latex.tex";
const char*     INPUT_NAME              = "input.txt";
const char*     OUTPUT_NAME             = "output.txt";
const char*     NAME_1GRAPH_PDF         = "graph1.pdf";
const char*     NAME_1GRAPH_SCRIPT      = "script1.txt";
const char*     NAME_2GRAPH_PDF         = "graph2.pdf";
const char*     NAME_2GRAPH_SCRIPT      = "script2.txt";
const char*     NAME_3GRAPH_PDF         = "graph3.pdf";
const char*     NAME_3GRAPH_SCRIPT      = "script3.txt";

int main ()
{
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

    FILE* file1 = fopen (OUTPUT_NAME, "w");
    error = nodes_print (func.root, file1),
    fclose (file1);

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
    return 0;
}
