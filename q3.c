#define _POSIX_C_SOURCE 200809L
#include "q3.h"
#include "q2.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>   /* IMPORTANT: errno and EINTR */
#include <stdlib.h>

static const char *WELCOME =
    "Bienvenue dans le Shell ENSEA.\n"
    "Pour quitter, tapez 'exit'.\n";
static const char *PROMPT = "enseash % ";

/* safe write (simple reuse) */
static void safe_write(const char *s) {
    size_t len = strlen(s);
    const char *p = s;
    while (len > 0) {
        ssize_t w = write(STDOUT_FILENO, p, len);
        if (w < 0) {
            /* ignore EINTR and continue */
            if (errno == EINTR) continue;
            break;
        }
        p += w;
        len -= (size_t)w;
    }
}

void q3_run_repl(void) {
    char line[1024];

    /* welcome */
    safe_write(WELCOME);

    while (1) {
        safe_write(PROMPT);

        /* read line (fgets returns NULL on EOF or error) */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            /* EOF (Ctrl+D) or error -> exit the REPL */
            safe_write("\n");
            break;
        }

        /* remove newline */
        line[strcspn(line, "\n")] = '\0';

        /* ignore empty lines */
        if (line[0] == '\0') continue;

        /* builtin exit (Q3 requirement) */
        if (strcmp(line, "exit") == 0) {
            break;
        }

        /* delegate to Q2 execution logic (keeps Q2 behavior) */
        q2_execute_line(line);
    }

    safe_write("Goodbye.\n");
}
