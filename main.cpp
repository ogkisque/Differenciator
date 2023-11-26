#include "tree.h"

#define VERIFY(tree)                            \
        error = tree_verify (&tree);            \
        if (error.code != CORRECT)              \
        {                                       \
            tree_dump (&tree, error);           \
            tree_graph_dump (&tree, error);     \
            return error.code;                  \
        }

const char* LATEX_NAME  = "latex.tex";
const char* INPUT_NAME  = "input.txt";
const char* OUTPUT_NAME = "output.txt";

int main ()
{
    Tree tree = {};
    ReadStr str = {};
    Error error = {};
    FILE* file_latex = fopen (LATEX_NAME, "w");
    print_latex_begin (file_latex);
    TREE_CTOR(&tree);

    FILE* file = fopen (INPUT_NAME, "r");
    read_file (file, &str);
    fclose (file);
    error = nodes_read (&tree, &(tree.root), &str);
    print_latex_func (tree.root, file_latex);

    FILE* file1 = fopen (OUTPUT_NAME, "w");
    error = nodes_print (tree.root, file1),
    fclose (file1);

    simple (&tree);
    double value = eval (tree.root, 1);
    printf ("x = 1: value is %.3lf\n", value);

    Tree diff = {};
    TREE_CTOR(&diff);
    diff.root = dif (tree.root);
    simple (&diff);
    print_latex_trans (diff.root, file_latex);

    double value1 = eval (diff.root, 1);
    printf ("x = 1: value of diff is %.3lf\n", value1);

    print_latex_end (file_latex);
    fclose (file_latex);
    generate_pdf (LATEX_NAME);

    tree_dtor (&diff);
    tree_dtor (&tree);
    return 0;
}
