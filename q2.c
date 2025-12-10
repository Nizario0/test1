/* q2.c - simple REPL without exit / Ctrl+D handling (Q2)
   Ctrl+D is ignored (we clear EOF and continue), 'exit' is not handled here.
*/

#define _POSIX_C_SOURCE 200809L
#include "q2.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

static const char *PROMPT = "enseash % ";

/* write whole string, handle EINTR */
static void safe_write(const char *s) {
    size_t len = strlen(s);
    const char *p = s;
    while (len > 0) {
        ssize_t w = write(STDOUT_FILENO, p, len);
        if (w < 0) {
            if (errno == EINTR) continue;
            break;
        }
        p += w;
        len -= (size_t)w;
    }
}

void q2_run_repl(void) {
    char line[256];

    while (1) {
        safe_write(PROMPT);

        /* fgets returns NULL on EOF or error.
           Instead of exiting on EOF, clear EOF and continue so the prompt stays active.
           This allows adding exit handling later in Q3. */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            /* print a newline to keep the prompt tidy, clear EOF state and continue */
            safe_write("\n");
            clearerr(stdin); /* remove EOF so next fgets will block for new input */
            continue;
        }

        /* remove trailing newline */
        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') continue;

        /* builtin simple : "yoo" -> salut */
        if (strcmp(line, "yoo") == 0) {
            safe_write("salut\n");
            continue;
        }

        /* special case: date -> precise format */
        if (strcmp(line, "date") == 0) {
            pid_t pid = fork();
            if (pid < 0) { perror("fork"); continue; }
            if (pid == 0) {
                execlp("date", "date", "+%Y-%m-%d %H:%M:%S.%N", (char *)NULL);
                /* if exec fails */
                char err[128];
                snprintf(err, sizeof(err), "enseash: failed to exec date: %s\n", strerror(errno));
                write(STDERR_FILENO, err, strlen(err));
                _exit(127);
            } else {
                int status; waitpid(pid, &status, 0);
                continue;
            }
        }

        /* otherwise try to execute the single-word command (no args) */
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); continue; }
        if (pid == 0) {
            execlp(line, line, (char *)NULL);
            char err[128];
            snprintf(err, sizeof(err), "enseash: failed to exec '%s': %s\n", line, strerror(errno));
            write(STDERR_FILENO, err, strlen(err));
            _exit(127);
        } else {
            int status; waitpid(pid, &status, 0);
            /* simple REPL: no exit code printed here */
        }
    }

    /* unreachable, but keep function signature consistent */
}

