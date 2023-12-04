#ifndef LATEX_HEADER
#define LATEX_HEADER

#include "tree.h"

void    print_form                  (const Node* node, FILE* file);
void    print_latex_begin           (FILE* file);
void    print_latex_end             (FILE* file);
void    print_latex_trans           (const Node* node, FILE* file, const char* var_name, const char* func_name);
void    print_latex_func            (const Node* node, FILE* file, const char* var_name, const char* func_name);
void    generate_pdf                (const char* file_name);
void    print_latex_taylor          (Node* node, FILE* file, double x, const char* name, size_t order);
void    print_latex_graph           (FILE* file, const char* file_name);
void    print_latex_simple          (Node* node, FILE* file);
void    print_latex_phrase          (FILE* file);
void    print_latex_func_vars       (const Node* node, FILE* file, Vars* vars, const char* func_name);
void    print_latex_full_dif        (Tree diffs[], Vars* vars, FILE* file_latex);

#endif //LATEX_HEADER
