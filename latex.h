#ifndef LATEX_HEADER
#define LATEX_HEADER

#include "tree.h"

void    print_form                  (const Node* node, FILE* file);
void    print_latex_begin           (FILE* file);
void    print_latex_end             (FILE* file);
void    print_latex_trans           (const Node* node, FILE* file);
void    print_latex_func            (const Node* node, FILE* file);
void    generate_pdf                (const char* file_name);
void    print_latex_taylor          (Node* node, FILE* file, double x, size_t order);
void    print_latex_graph           (FILE* file, const char* file_name);
void    print_latex_simple          (Node* node, FILE* file);
void    print_latex_phrase          (FILE* file);

#endif //LATEX_HEADER
