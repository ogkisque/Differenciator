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
    double value = eval (tree.root, 2.5);
    printf ("value is %.3lf\n", value);
    tree_dump (&tree, error);
    tree_graph_dump (&tree, error);

    return 0;
}
