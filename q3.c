#define _POSIX_C_SOURCE 200809L
#include "q3.h"
#include "q2.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static const char *WELCOME =
    "Bienvenue dans le Shell ENSEA.\n"
    "Pour quitter, tapez 'exit'.\n";

static void safe_write(const char *s) {
    size_t len = strlen(s);
    const char *p = s;
    while (len > 0) {
        ssize_t w = write(STDOUT_FILENO, p, len);
        if (w < 0) { if (errno == EINTR) continue; break; }
        p += w;
        len -= (size_t)w;
    }
}

void q3_run_repl(void) {
    char line[1024];
    int last_status = 0;

    safe_write(WELCOME);

    while (1) {
        (void)write(STDOUT_FILENO, "enseash % ", 10);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            safe_write("\n");
            break; // Ctrl+D exit
        }

        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;

        if (strcmp(line, "exit") == 0) break;

        q2_execute_line(line, &last_status);
    }

    safe_write("Goodbye.\n");
}

