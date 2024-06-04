#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdlib>

#include "execute.h"
#include "lexer.h"
#include "parser.h"

struct InstructionNode *parse_generate_intermediate_representation()
{
    Parser parser;
    struct InstructionNode *program = parser.parse_program();
    return program;
}
