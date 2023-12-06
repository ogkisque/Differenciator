#include "latex.h"

const int   NUM_PHRASES = 18;
const char* PHRASES[]   = { "Максимально тривиально, что:",
                            "Если спросить у рандомного бомжа на улице, то он будет знать, что:",
                            "Заметим, что:",
                            "Не понимаю тех, кто не знает, что:",
                            "Я сам выпал на этом моменте:",
                            "Если приглядеться, то ты все равно не увидишь, что:",
                            "В садике мне рассказывали, что:",
                            "Если бы меня разбудили в ночь после посвята, то я бы сходу ответил, что:",
                            "Даже ИНБИКСТ знает, что:",
                            "Если бы моя собака умела говорить, то сказала бы, что:",
                            "Петрович закопает того, кто не знает, что:",
                            "Ежу понятно, что:",
                            "Ллойд и Гарри из 'Тупой и ещё тупее' знали, что:",
                            "Невооруженным взглядом видно, что:",
                            "Сложить 2 + 2 эквивалентно по сложности следующему:",
                            "Ньютон перевернулся бы в гробу, если бы узнал, что ты не знаешь, что:",
                            "Я бы перерезал себе вены, если бы не знал, что:",
                            "Сколько бы я не терял память, никогда не забуду, что:"
                          };

void print_latex_begin (FILE* file)
{
    fprintf (file,
    "\\documentclass[a4paper,12pt]{article}\n"
    "\\usepackage[T1]{fontenc}\n"
    "\\usepackage[utf8]{inputenc}\n"
    "\\usepackage[english,russian]{babel}\n"
    "\\usepackage{pdfpages}\n"
    "\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}\n"
    "\\usepackage[left=10mm, top=10mm, right=10mm, bottom=20mm, nohead, nofoot]{geometry}\n"
    "\\usepackage{wasysym}\n"
    "\\author{\\LARGEМерзляков Арсений}\n"
    "\\title{Анализ функции}\n"
    "\\pagestyle {empty}\n"
    "\\begin{document}\n"
    "\\maketitle\n"
    "\\begin{flushleft}\n"
    "\\Large\n");
}

void print_latex_end (FILE* file)
{
    fprintf (file,
    "\\end{flushleft}\n"
    "\\end{document}");
}

void print_latex_trans (const Node* node, FILE* file, const char* var_name, const char* func_name)
{
    fprintf (file, "$%s^{'}(%s) = ", func_name, var_name);
    print_form (node, file);
    fprintf (file, "$\n\n");
}

void print_form (const Node* node, FILE* file)
{
    if (!node)
        return;

    if (node->type == NUM)
    {
        if (node->value < 0)
            fprintf (file, "(%.3lg)", node->value);
        else
            fprintf (file, "%.3lg", node->value);
        return;
    }

    if (node->type == VAR)
    {
        fprintf (file, "%s", node->name);
        return;
    }

    if (node->type == OPER)
    {
        switch ((int) node->value)
        {
            case DIV:
                fprintf (file, " \\dfrac{");
                print_form (node->left, file);
                fprintf (file, "}{");
                print_form (node->right, file);
                fprintf (file, "} ");
                break;
            case MUL:
                print_form (node->left, file);
                fprintf (file, " \\cdot ");
                print_form (node->right, file);
                break;
            case ADD:
                fprintf (file, "(");
                print_form (node->left, file);
                fprintf (file, "+");
                print_form (node->right, file);
                fprintf (file, ")");
                break;
            case SUB:
                fprintf (file, "(");
                print_form (node->left, file);
                fprintf (file, "-");
                print_form (node->right, file);
                fprintf (file, ")");
                break;
            default:
                printf ("Getting in default in latex_print!\n");
                break;
        }
    }
    else if (node->type == FUNC)
    {
        switch ((int) node->value)
        {
            case COS:
                fprintf (file, "\\cos {(");
                print_form (node->right, file);
                fprintf (file, ")}");
                break;
            case SIN:
                fprintf (file, "\\sin {(");
                print_form (node->right, file);
                fprintf (file, ")}");
                break;
            case ARCSIN:
                fprintf (file, "\\arcsin {(");
                print_form (node->right, file);
                fprintf (file, ")}");
                break;
            case ARCCOS:
                fprintf (file, "\\arccos {(");
                print_form (node->right, file);
                fprintf (file, ")}");
                break;
            case TG:
                fprintf (file, "\\tan {(");
                print_form (node->right, file);
                fprintf (file, ")}");
                break;
            case ARCTG:
                fprintf (file, "\\arctan {(");
                print_form (node->right, file);
                fprintf (file, ")}");
                break;
            case LN:
                fprintf (file, "\\ln {(");
                print_form (node->right, file);
                fprintf (file, ")}");
                break;
            case SQRT:
                fprintf (file, "\\sqrt {");
                print_form (node->right, file);
                fprintf (file, "}");
                break;
            case POW:
                fprintf (file, "(");
                print_form (node->left, file);
                fprintf (file, ")^{");
                print_form (node->right, file);
                fprintf (file, "}");
                break;
            default:
                printf ("Getting in default in latex_print!\n");
                break;
        }
    }
}

void generate_pdf (const char* file_name)
{
    char text[MAX_TEXT_SIZE] = "";
    sprintf (text, "pdflatex %s", file_name);
    system (text);
}

void print_latex_func_vars (const Node* node, FILE* file, Vars* vars, const char* func_name)
{
    fprintf (file, "$%s(", func_name);
    for (int i = 0; i < vars->num_vars; i++)
    {
        fprintf (file, "%s", vars->vars[i].name);
        if (i != vars->num_vars - 1)
            fprintf (file, ", ");
    }
    fprintf (file, ") = ");
    print_form (node, file);
    fprintf (file, "$\n\n");
}

void print_latex_func (const Node* node, FILE* file, const char* var_name, const char* func_name)
{
    fprintf (file, "$%s(%s) = ", func_name, var_name);
    print_form (node, file);
    fprintf (file, "$\n\n");
}

void print_latex_taylor (Node* node, FILE* file, double x, const char* name, size_t order)
{
    fprintf (file, "Выполним самую тривиальную вещь в курсе математического анализа - разложение функции по формуле Тейлора: \n\n$f(x) = ");
    print_form (node, file);
    if (!is_zero (x))
        fprintf (file, " + o((%s - %.3lf)^{%llu}), %s \\rightarrow %.3lf$\n\n", name, x, order, name, x);
    else
        fprintf (file, " + o(%s^{%llu}), %s \\rightarrow 0$\n\n", name, order, name);
}

void print_latex_graph (FILE* file, const char* file_name)
{
    fprintf (file, "\\includepdf[pages=-]{%s}", file_name);
}

void print_latex_simple (Node* node, FILE* file)
{
    fprintf (file, "После очевиднейших упрощений, которые адекватный человек может сделать ещё в утробе, получаем:\n\n$");
    print_form (node, file);
    fprintf (file, "$\n\n");
}

void print_latex_phrase (FILE* file)
{
    int random = rand () % NUM_PHRASES;
    fprintf (file, "%s\n\n", PHRASES[random]);
}

void print_latex_full_dif (Tree diffs[], Vars* vars, FILE* file_latex)
{
    fprintf (file_latex, "Тогда полный дифференциал будет:\n\n$df = ");

    for (int i = 0; i < vars->num_vars; i++)
    {
        fprintf (file_latex, "\\dfrac{\\partial f}{\\partial %s} \\cdot d%s", vars->vars[i].name, vars->vars[i].name);
        if (i != vars->num_vars - 1)
            fprintf (file_latex, " + ");
    }
    fprintf (file_latex, " = ");

    for (int i = 0; i < vars->num_vars; i++)
    {
        print_form (diffs[i].root, file_latex);
        fprintf (file_latex, " \\cdot d%s", vars->vars[i].name);
        if (i != vars->num_vars - 1)
            fprintf (file_latex, " + ");
    }
    fprintf (file_latex, "$\n\n");
}
