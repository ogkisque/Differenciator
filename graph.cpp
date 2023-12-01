#include "graph.h"

void get_graph_script (Node* node, double x, double delta, const char* name_graph, const char* name_script, const char* name_pdf)
{
    FILE* file = fopen (name_script, "w");
    fprintf (file,
            "set term pdfcairo enhanced size 20cm,15cm font ',15' linewidth 3\n"
            "set output '%s'\n"
            "set grid xtics ytics mxtics mytics\n"
            "set xlabel 'x' font 'Sans,20'\n"
            "set ylabel 'f(x)' font 'Sans,20'\n"
            "set title '%s' font 'Sans,25'\n", name_pdf, name_graph);
    fprintf (file, "plot [%lf:%lf] ", x - delta, x + delta);
    print_nodes_graph (file, node);
    fprintf (file, "\nset term pop\n");
    fclose (file);
}

void get_graph_pdf (Node* node, double x, double delta, const char* name_graph, const char* name_script, const char* name_pdf)
{
    get_graph_script (node, x, delta, name_graph, name_script, name_pdf);
    char text[MAX_TEXT_SIZE] = "";
    sprintf (text, "gnuplot -c %s", name_script);
    system (text);
}

void print_nodes_graph (FILE* file, Node* node)
{
    if (!node)
        return;

    fprintf (file, "(");
    print_nodes_graph (file, node->left);

    char str[MAX_STR_SIZE] = "";
    val_to_str_graph (node, str);

    fprintf (file, "%s", str);

    print_nodes_graph (file, node->right);
    fprintf (file, ")");
}

void val_to_str_graph (const Node* node, char* str)
{
    if (node->type == NUM)
    {
        sprintf (str, "%.3lf", node->value);
        return;
    }

    if (node->type == VAR)
    {
        sprintf (str, "x");
        return;
    }

    if (node->type == FUNC)
    {
        switch ((int) node->value)
        {
            case COS:
                sprintf (str, "cos");
                return;
            case SIN:
                sprintf (str, "sin");
                return;
            case SQRT:
                sprintf (str, "sqrt");
                return;
            case POW:
                sprintf (str, "**");
                return;
            case LN:
                sprintf (str, "log");
                return;
            case TG:
                sprintf (str, "tan");
                return;
            case ARCSIN:
                sprintf (str, "asin");
                return;
            case ARCCOS:
                sprintf (str, "acos");
                return;
            case ARCTG:
                sprintf (str, "atan");
                return;
            default:
                printf ("Unknown function!\n");
                return;
        }
    }

    if (node->type == OPER)
    {
        switch ((int) node->value)
        {
            case ADD:
                sprintf (str, "+");
                return;
            case MUL:
                sprintf (str, "*");
                return;
            case SUB:
                sprintf (str, "-");
                return;
            case DIV:
                sprintf (str, "/");
                return;
            default:
                return;
        }
    }
}

void get_two_graph_script (Node* node1, Node* node2, double x, double delta, const char* name_graph, const char* name_script, const char* name_pdf)
{
    FILE* file = fopen (name_script, "w");
    fprintf (file,
            "set term pdfcairo enhanced size 20cm,15cm font ',15' linewidth 3\n"
            "set output '%s'\n"
            "set grid xtics ytics mxtics mytics\n"
            "set xlabel 'x' font 'Sans,20'\n"
            "set ylabel 'f(x)' font 'Sans,20'\n"
            "set title '%s' font 'Sans,25'\n", name_pdf, name_graph);
    fprintf (file, "plot [%lf:%lf] ", x - delta, x + delta);
    print_nodes_graph (file, node1);
    fprintf (file, ", ");
    print_nodes_graph (file, node2);
    fprintf (file, "\nset term pop\n");
    fclose (file);
}

void get_two_graph_pdf (Node* node1, Node* node2, double x, double delta, const char* name_graph, const char* name_script, const char* name_pdf)
{
    get_two_graph_script (node1, node2, x, delta, name_graph, name_script, name_pdf);
    char text[MAX_TEXT_SIZE] = "";
    sprintf (text, "gnuplot -c %s", name_script);
    system (text);
}
