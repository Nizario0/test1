#include <unistd.h>
#include <string.h>
#include "q1.h"

static const char *WELCOME =
    "Bienvenue dans le Shell ENSEA.\n"
    "Pour quitter, tapez 'exit'.\n";
static const char *PROMPT = "enseash % ";

void q1_print(void) {
    write(STDOUT_FILENO, WELCOME, strlen(WELCOME));
    write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}
