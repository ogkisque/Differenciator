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

Error nodes_read        (Node** node, ReadStr* str, Vars* vars);
Error get_plus          (Node** node, ReadStr* str, Vars* vars);
Error get_mul           (Node** node, ReadStr* str, Vars* vars);
Error get_brac          (Node** node, ReadStr* str, Vars* vars);
Error get_num           (Node** node, ReadStr* str, bool* is_read);
Error get_var           (Node** node, ReadStr* str, Vars* vars);
Error get_func          (Node** node, ReadStr* str, Vars* vars, bool* is_read);
Error get_pow           (Node** node, ReadStr* str, Vars* vars);

#endif //READ_HEADER
