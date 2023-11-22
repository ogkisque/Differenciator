#include "tree.h"

int main ()
{
    Tree tree = {};
    TREE_CTOR(&tree);

    ReadStr str = {};

    FILE* file = fopen ("input.txt", "r");
    read_file (file, &str);
    printf ("<%s>", str.str);
    Error error = nodes_read (&tree, &(tree.root), file);
    fclose (file);

    FILE* file1 = fopen ("output.txt", "w");
    nodes_print (tree.root, file1),
    fclose (file1);

    double value = eval (tree.root, 2.5);
    printf ("value is %.3lf\n", value);
    tree_dump (&tree, error);
    tree_graph_dump (&tree, error);
    return 0;
}
