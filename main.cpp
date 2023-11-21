#include "tree.h"

int main ()
{
    Tree tree = {};
    TREE_CTOR(&tree);

    FILE* file = fopen ("input.txt", "r");
    Error error = nodes_read (&tree, &(tree.root), file);
    fclose (file);

    FILE* file1 = fopen ("output.txt", "w");
    nodes_print (tree.root, file1),
    fclose (file1);

    tree_dump (&tree, error);
    tree_graph_dump (&tree, error);

}
