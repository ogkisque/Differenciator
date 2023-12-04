#include "read.h"

Error nodes_read (Node** node, ReadStr* str, Vars* vars)
{
    Error error = get_plus (node, str, vars);
    PARSE_ERROR_STR(str, error);

    if (str->str[str->pos] != '\0')
    {
        error = GET_ERROR(SYNTAX_ERR, "No '\\0' in the end of str");
        str_dump (str, error);
        return error;
    }
    RETURN_ERROR(CORRECT, "");
}

Error get_plus (Node** node, ReadStr* str, Vars* vars)
{
    Node* node_right = NULL;
    Error error = get_mul (node, str, vars);
    PARSE_ERROR_STR(str, error);

    while (str->str[str->pos] == '+' || str->str[str->pos] == '-')
    {
        char oper = str->str[str->pos];
        str->pos++;
        error = get_mul (&node_right, str, vars);
        PARSE_ERROR_STR(str, error);

        switch (oper)
        {
            case '+':
                *node = _ADD(*node, node_right);
                break;
            case '-':
                *node = _SUB(*node, node_right);
                break;
            default:
                RETURN_ERROR(SYNTAX_ERR, "Unknown oper, not *, not /");
                break;
        }
    }
    RETURN_ERROR(CORRECT, "");
}

Error get_mul (Node** node, ReadStr* str, Vars* vars)
{
    Node* node_right = NULL;
    Error error = get_pow (node, str, vars);
    PARSE_ERROR_STR(str, error);

    while (str->str[str->pos] == '*' || str->str[str->pos] == '/')
    {
        char oper = str->str[str->pos];
        str->pos++;
        error = get_pow (&node_right, str, vars);
        PARSE_ERROR_STR(str, error);

        switch (oper)
        {
            case '*':
                *node = _MUL(*node, node_right);
                break;
            case '/':
                *node = _DIV(*node, node_right);
                break;
            default:
                RETURN_ERROR(SYNTAX_ERR, "Unknown oper, not +, not -");
                break;
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_pow (Node** node, ReadStr* str, Vars* vars)
{
    Node* node_right = NULL;
    Error error = get_brac (node, str, vars);
    PARSE_ERROR_STR(str, error);

    while (str->str[str->pos] == '^')
    {
        str->pos++;
        error = get_brac (&node_right, str, vars);
        PARSE_ERROR_STR(str, error);

        *node = _POW(*node, node_right);
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_brac (Node** node, ReadStr* str, Vars* vars)
{
    Error error = {};
    if (str->str[str->pos] == '(')
    {
        str->pos++;
        error = get_plus (node, str, vars);
        if (str->str[str->pos] != ')')
            RETURN_ERROR(SYNTAX_ERR, "Error in reading (). No ')' after '('");

        PARSE_ERROR_STR(str, error);
        str->pos++;
        RETURN_ERROR(CORRECT, "");
    }
    else
    {
        bool is_read = false;
        error = get_num (node, str, &is_read);
        PARSE_ERROR_STR(str, error);
        if (is_read)
            return error;

        error = get_func (node, str, vars, &is_read);
        PARSE_ERROR_STR(str, error);
        if (is_read)
            return error;

        return get_var (node, str, vars);
    }
}

Error get_num (Node** node, ReadStr* str, bool* is_read)
{
    int sign = 1;
    if (str->str[str->pos] == '-')
    {
        sign = -1;
        str->pos++;
        if (!('0' <= str->str[str->pos] && str->str[str->pos] <= '9'))
            RETURN_ERROR(SYNTAX_ERR, "No number after '-'");
    }

    int val1 = 0;
    size_t pos0 = str->pos;
    while ('0' <= str->str[str->pos] && str->str[str->pos] <= '9')
    {
        val1 = val1 * 10 + (str->str[str->pos] - '0');
        str->pos++;
    }

    int val2 = 0;
    size_t count = 0;
    double val = (double) val1;
    if (str->str[str->pos] == '.')
    {
        str->pos++;
        while ('0' <= str->str[str->pos] && str->str[str->pos] <= '9')
        {
            val2 = val2 * 10 + (str->str[str->pos] - '0');
            str->pos++;
            count++;
        }

        if (count == 0)
            RETURN_ERROR(SYNTAX_ERR, "No numbers after '.'");

        int st = 1;
        for (size_t i = 0; i < count; i++)
            st *= 10;
        val += (double) val2 / (double) st;
    }


    if (pos0 == str->pos)
        *is_read = false;
    else
        *is_read = true;

    *node = _NUM(val * sign);
    RETURN_ERROR(CORRECT, "");
}

Error get_func (Node** node, ReadStr* str, Vars* vars, bool* is_read)
{
    char text[MAX_SIZE] = "";
    size_t count = 0;

    while ('a' <= str->str[str->pos] && str->str[str->pos] <= 'z')
    {
        text[count] = str->str[str->pos];
        count++;
        str->pos++;
    }

    *is_read = false;
    if (strcmp (text, "cos") == 0)
    {
        *is_read = true;
        *node = _COS(NULL, NULL);
    }
    else if (strcmp (text, "sin") == 0)
    {
        *is_read = true;
        *node = _SIN(NULL, NULL);
    }
    else if (strcmp (text, "sqrt") == 0)
    {
        *is_read = true;
        *node = _SQRT(NULL, NULL);
    }
    else if (strcmp (text, "ln") == 0)
    {
        *is_read = true;
        *node = _LN(NULL, NULL);
    }
    else if (strcmp (text, "tg") == 0)
    {
        *is_read = true;
        *node = _TG(NULL, NULL);
    }
    else if (strcmp (text, "arcsin") == 0)
    {
        *is_read = true;
        *node = _ARCSIN(NULL, NULL);
    }
    else if (strcmp (text, "arccos") == 0)
    {
        *is_read = true;
        *node = _ARCCOS(NULL, NULL);
    }
    else if (strcmp (text, "arctg") == 0)
    {
        *is_read = true;
        *node = _ARCTG(NULL, NULL);
    }
    else
    {
        *is_read = false;
        for (size_t i = 0; i < count; i++)
            str->pos--;

        RETURN_ERROR(CORRECT, "");
    }

    return get_brac (&((*node)->right), str, vars);
}

Error get_var (Node** node, ReadStr* str, Vars* vars)
{
    char name[MAX_SIZE] = "";
    size_t count = 0;

    while (('a' <= tolower(str->str[str->pos]) && tolower(str->str[str->pos]) <= 'z') ||
           ('0' <= str->str[str->pos] && str->str[str->pos] <= '9') ||
           (str->str[str->pos] == '_'))
    {
        name[count] = str->str[str->pos];
        count++;
        str->pos++;
    }

    *node = _VAR(name);

    if (!found_var (vars, name))
        return append_var (vars, name);
    RETURN_ERROR(CORRECT, "");
}
