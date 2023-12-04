#include "diff.h"
#include "dump.h"
#include "graph.h"
#include "latex.h"
#include "tree.h"
#include "read.h"

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
    error = nodes_read (&(func.root), &str, &vars);
    //tree_graph_dump (&func, error);
    simple (&func);


    print_latex_func_vars (func.root, file_latex, &vars, "f");

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

    return 0;
}
