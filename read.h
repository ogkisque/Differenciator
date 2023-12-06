#ifndef READ_HEADER
#define READ_HEADER

#include <ctype.h>

#include "tree.h"
#include "diff.h"
#include "error.h"
#include "dump.h"

#define PARSE_ERROR_STR(str, error)     \
        if (error.code != CORRECT)      \
        {                               \
            str_dump (str, error);      \
            return error;               \
        }

#define PARSE_ERROR_STR_NO_DUMP(error)          \
        if (error.code != CORRECT)              \
        {                                       \
            return error;                       \
        }

Error nodes_read        (Node** node, ReadStr* str, Vars* vars);
Error get_expression    (Node** node, Tokens* tokens);
Error get_term          (Node** node, Tokens* tokens);
Error get_prim_expr     (Node** node, Tokens* tokens);
Error get_num           (Node** node, Tokens* tokens, bool* is_read);
Error get_var           (Node** node, Tokens* tokens);
Error get_func          (Node** node, Tokens* tokens, bool* is_read);
Error get_pow           (Node** node, Tokens* tokens);
Error get_unar_min      (Node** node, Tokens* tokens);
Error get_tokens        (Tokens* tokens, ReadStr* str, Vars* vars);
Error get_token_num     (Tokens* tokens, ReadStr* str);
Error get_token_oper    (Tokens* tokens, ReadStr* str);
Error get_token_func    (Tokens* tokens, ReadStr* str);
Error get_token_var     (Tokens* tokens, ReadStr* str, Vars* vars);
void  skip_spaces       (ReadStr* str);

#endif //READ_HEADER
