#include "tree.h"

#define VERIFY(tree)                            \
        error = tree_verify (&tree);            \
        if (error.code != CORRECT)              \
        {                                       \
            tree_dump (&tree, error);           \
            tree_graph_dump (&tree, error);     \
            return error.code;                  \
        }

int main ()
{
    Tree tree = {};
    ReadStr str = {};
    Error error = {};
    TREE_CTOR(&tree);

    FILE* file = fopen ("input.txt", "r");
    read_file (file, &str);
    fclose (file);
    error = nodes_read (&tree, &(tree.root), &str);

    FILE* file1 = fopen ("output.txt", "w");
    error = nodes_print (tree.root, file1),
    fclose (file1);

    VERIFY(tree);
    //tree_dump (&tree, error);
    //tree_graph_dump (&tree, error);
    double value = eval (tree.root, 1);
    printf ("x = 1: value is %.3lf\n", value);

    Tree diff = {};
    TREE_CTOR(&diff);
    diff.root = dif (tree.root);
    VERIFY(diff);

    double value1 = eval (diff.root, 1);
    printf ("x = 1: value of diff is %.3lf\n", value1);
    diff.root = simple (diff.root);
    tree_dump (&diff, error);
    tree_graph_dump (&diff, error);

    tree_dtor (&tree);
    tree_dtor (&diff);
    return 0;
}
