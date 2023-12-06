#include "read.h"
#include "operators.h"

Error nodes_read (Node** node, ReadStr* str, Vars* vars)
{
    Tokens tokens = {};
    tokens.pos = 0;
    Error error = get_tokens (&tokens, str, vars);
    PARSE_ERROR_STR(str, error);

    error = get_expression (node, &tokens);
    PARSE_ERROR_STR(str, error);
    RETURN_ERROR(CORRECT, "");
}

Error get_expression (Node** node, Tokens* tokens)
{
    Node* node_left = NULL;
    Node* node_right = NULL;
    Error error = get_term (&node_left, tokens);
    PARSE_ERROR_STR_NO_DUMP(error);
    *node = node_left;

    while (tokens->tokens[tokens->pos]->type == OPER &&
          ((int) tokens->tokens[tokens->pos]->value == ADD || (int) tokens->tokens[tokens->pos]->value == SUB))
    {
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        error = get_term (&node_right, tokens);
        PARSE_ERROR_STR_NO_DUMP(error);

        (*node)->left = node_left;
        (*node)->right = node_right;

        node_left = *node;
    }
    RETURN_ERROR(CORRECT, "");
}

Error get_term (Node** node, Tokens* tokens)
{
    Node* node_left = NULL;
    Node* node_right = NULL;
    Error error = get_pow (&node_left, tokens);
    PARSE_ERROR_STR_NO_DUMP(error);
    *node = node_left;

    while (tokens->tokens[tokens->pos]->type == OPER &&
          ((int) tokens->tokens[tokens->pos]->value == MUL || (int) tokens->tokens[tokens->pos]->value == DIV))
    {
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        error = get_pow (&node_right, tokens);
        PARSE_ERROR_STR_NO_DUMP(error);

        (*node)->left = node_left;
        (*node)->right = node_right;

        node_left = *node;
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_pow (Node** node, Tokens* tokens)
{
    Node* node_left = NULL;
    Node* node_right = NULL;
    Error error = get_unar_min (&node_left, tokens);
    PARSE_ERROR_STR_NO_DUMP(error);
    *node = node_left;

    while (tokens->tokens[tokens->pos]->type == FUNC &&
           (int) tokens->tokens[tokens->pos]->value == POW)
    {
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        error = get_unar_min (&node_right, tokens);
        PARSE_ERROR_STR_NO_DUMP(error);

        (*node)->left = node_left;
        (*node)->right = node_right;

        node_left = *node;
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_unar_min (Node** node, Tokens* tokens)
{
    Error error = {};
    if (tokens->tokens[tokens->pos]->type == OPER &&
        (int) tokens->tokens[tokens->pos]->value == UNSUB)
    {
        tokens->pos++;
        Node* node_right = NULL;
        error = get_prim_expr (&node_right, tokens);
        PARSE_ERROR_STR_NO_DUMP(error);

        *node = _MUL(_NUM(-1), node_right);
    }
    else
    {
        error = get_prim_expr (node, tokens);
        PARSE_ERROR_STR_NO_DUMP(error);
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_prim_expr (Node** node, Tokens* tokens)
{
    Error error = {};
    if (tokens->tokens[tokens->pos]->type == OPER &&
        (int) tokens->tokens[tokens->pos]->value == LBRAC)
    {
        tokens->pos++;
        error = get_expression (node, tokens);
        if (tokens->tokens[tokens->pos]->type != OPER ||
            (int) tokens->tokens[tokens->pos]->value != RBRAC)
            RETURN_ERROR(SYNTAX_ERR, "Error in reading (). No ')' after '('");

        PARSE_ERROR_STR_NO_DUMP(error);
        tokens->pos++;
        RETURN_ERROR(CORRECT, "");
    }
    else
    {
        bool is_read = false;
        error = get_num (node, tokens, &is_read);
        PARSE_ERROR_STR_NO_DUMP(error);
        if (is_read)
            return error;

        error = get_func (node, tokens, &is_read);
        PARSE_ERROR_STR_NO_DUMP(error);
        if (is_read)
            return error;

        return get_var (node, tokens);
    }
}

Error get_num (Node** node, Tokens* tokens, bool* is_read)
{
    if (tokens->tokens[tokens->pos]->type == NUM)
    {
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        *is_read = true;
    }
    else
    {
        *is_read = false;
    }
    RETURN_ERROR(CORRECT, "");
}

Error get_func (Node** node, Tokens* tokens, bool* is_read)
{
    if (tokens->tokens[tokens->pos]->type == FUNC)
    {
        *is_read = true;
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        return get_prim_expr (&((*node)->right), tokens);
    }
    else
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }
}

Error get_var (Node** node, Tokens* tokens)
{
    if (tokens->tokens[tokens->pos]->type != VAR)
        RETURN_ERROR(SYNTAX_ERR, "No variable. Unknown token");

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;
    RETURN_ERROR(CORRECT, "");
}

Error get_tokens (Tokens* tokens, ReadStr* str, Vars* vars)
{
    tokens->size = 0;
    tokens->tokens = (Node**) calloc (str->size + 1, sizeof (Node*));

    Error error = {};
    while (str->str[str->pos] != '\0')
    {
        skip_spaces (str);
        error = get_token_num (tokens, str);
        PARSE_ERROR_STR(str, error);
        skip_spaces (str);
        error = get_token_oper (tokens, str);
        PARSE_ERROR_STR(str, error);
        skip_spaces (str);
        error = get_token_func (tokens, str);
        PARSE_ERROR_STR(str, error);
        skip_spaces (str);
        error = get_token_var (tokens, str, vars);
        PARSE_ERROR_STR(str, error);
    }
    tokens->tokens[tokens->size] = _END;
    tokens->size++;

    tokens->tokens = (Node**) realloc (tokens->tokens, (tokens->size + 1) * sizeof (Node*));
    RETURN_ERROR(CORRECT, "");
}

Error get_token_num (Tokens* tokens, ReadStr* str)
{
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

    if (pos0 != str->pos)
    {
        tokens->tokens[tokens->size] = _NUM(val);
        tokens->size++;
    }
    RETURN_ERROR(CORRECT, "");
}

Error get_token_oper (Tokens* tokens, ReadStr* str)
{
    char text[2] = "";
    text[0] = str->str[str->pos];

    for (size_t i = 0; i < NUM_OPERS; i++)
    {
        if (strcmp (text, OPERATORS[i].text) == 0)
        {
            int val = OPERATORS[i].value;
            if (OPERATORS[i].type == OPER && OPERATORS[i].value == SUB)
            {
                if (tokens->size == 0)
                {
                    tokens->tokens[0] = _UNSUB(NULL, NULL);
                    tokens->size++;
                    str->pos++;
                    RETURN_ERROR(CORRECT, "");
                }

                Types type_prev = tokens->tokens[tokens->size - 1]->type;
                int val_prev = (int) tokens->tokens[tokens->size - 1]->value;
                if (type_prev == NUM || type_prev == VAR)
                    val = SUB;
                else if (type_prev == OPER)
                    if (val_prev == RBRAC)
                        val = SUB;
                    else if (val_prev == ADD || val_prev == MUL || val_prev == SUB || val_prev == DIV || val_prev == LBRAC)
                        val = UNSUB;
                    else
                        RETURN_ERROR(SYNTAX_ERR, "Unary minus after unary minus");
                else
                    RETURN_ERROR(SYNTAX_ERR, "No bracket after function");
            }

            tokens->tokens[tokens->size] = create_node (OPERATORS[i].type, val, NULL, NULL, NULL);
            tokens->size++;
            str->pos++;
            RETURN_ERROR(CORRECT, "");
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_token_func (Tokens* tokens, ReadStr* str)
{
    char text[MAX_SIZE] = "";
    size_t count = 0;

    while ('a' <= str->str[str->pos] && str->str[str->pos] <= 'z')
    {
        text[count] = str->str[str->pos];
        count++;
        str->pos++;
    }

    for (size_t i = 0; i < NUM_OPERS; i++)
    {
        if (strcmp (text, OPERATORS[i].text) == 0)
        {
            tokens->tokens[tokens->size] = create_node (OPERATORS[i].type, OPERATORS[i].value, NULL, NULL, NULL);
            tokens->size++;
            RETURN_ERROR(CORRECT, "");
        }
    }

    for (size_t i = 0; i < count; i++)
        str->pos--;

    RETURN_ERROR(CORRECT, "");
}

Error get_token_var (Tokens* tokens, ReadStr* str, Vars* vars)
{
    char name[MAX_SIZE] = "";
    size_t count = 0;

    if ('0' <= str->str[str->pos] && str->str[str->pos] <= '9')
        RETURN_ERROR(CORRECT, "");

    while (('a' <= tolower(str->str[str->pos]) && tolower(str->str[str->pos]) <= 'z') ||
           ('0' <= str->str[str->pos] && str->str[str->pos] <= '9') ||
           (str->str[str->pos] == '_'))
    {
        name[count] = str->str[str->pos];
        count++;
        str->pos++;
    }

    if (count != 0)
    {
        tokens->tokens[tokens->size] = _VAR(name);
        tokens->size++;

        if (!found_var (vars, name))
            return append_var (vars, name);
    }

    RETURN_ERROR(CORRECT, "");
}

void skip_spaces (ReadStr* str)
{
    while (str->str[str->pos] == ' ')
        str->pos++;
}
