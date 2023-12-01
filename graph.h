#ifndef GRAPH_HEADER
#define GRAPH_HEADER

#include "tree.h"

void    get_graph_script            (Node* node, double x, double delta, const char* name_graph, const char* name_script, const char* name_pdf);
void    get_graph_pdf               (Node* node, double x, double delta, const char* name_graph, const char* name_script, const char* name_pdf);
void    val_to_str_graph            (const Node* node, char* str);
void    print_nodes_graph           (FILE* file, Node* node);
void    get_two_graph_script        (Node* node1, Node* node2, double x, double delta, const char* name_graph, const char* name_script, const char* name_pdf);
void    get_two_graph_pdf           (Node* node1, Node* node2, double x, double delta, const char* name_graph, const char* name_script, const char* name_pdf);

#endif //GRAPH_HEADER
