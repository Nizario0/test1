/* q2.c - simple REPL (Q2) utilisé aussi par Q3
   - commande "yoo" -> "salut"
   - commande "date" -> date précise
   - exécution de commandes simples
   - PAS de exit/Ctrl+D ici (géré dans Q3)
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

/* write complet et sûr */
static void safe_write(const char *s) {
    size_t len = strlen(s);
    const char *p = s;

    while (len > 0) {
        ssize_t w = write(STDOUT_FILENO, p, len);
        if (w < 0) {
            if (errno == EINTR) continue;
            return;
        }
        p += w;
        len -= (size_t)w;
    }
}

/* Exécution d'une commande simple */
static void exec_command(const char *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        execlp(cmd, cmd, (char *)NULL);

        /* si exec échoue */
        char err[256];
        int n = snprintf(err, sizeof(err),
                         "enseash: failed to exec '%s': %s\n",
                         cmd, strerror(errno));

        if (n > 0) {
            ssize_t __w = write(STDERR_FILENO, err,
                                (size_t)(n < (int)sizeof(err) ? n : (int)sizeof(err)));
            (void)__w;
        }
        _exit(127);
    } else {
        waitpid(pid, NULL, 0);
    }
}

/* Fonction principale Q2 (appelée par Q3 ensuite) */
int q2_execute_line(const char *line) {
    if (line[0] == '\0')
        return 0;  // retourner un int

    /* builtin : yoo */
    if (strcmp(line, "yoo") == 0) {
        safe_write("salut\n");
        return 0;
    }

    /* builtin : date */
    if (strcmp(line, "date") == 0) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            execlp("date", "date", "+%Y-%m-%d %H:%M:%S.%N", (char *)NULL);

            char err[256];
            int n = snprintf(err, sizeof(err),
                             "enseash: failed to exec date: %s\n",
                             strerror(errno));

            if (n > 0) {
                ssize_t __w = write(STDERR_FILENO, err,
                                    (size_t)(n < (int)sizeof(err) ? n : (int)sizeof(err)));
                (void)__w;
            }
            _exit(127);
        }
        waitpid(pid, NULL, 0);
        return 0;
    }

    /* sinon exécuter commande */
    exec_command(line);
    return 0;
}

/* Q2 REPL minimal (appelé seulement si tu veux, sinon Q3 remplace) */
void q2_run_repl(void) {
    char line[1024];

    while (1) {
        safe_write(PROMPT);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            safe_write("\n");
            clearerr(stdin);
            continue;
        }

        line[strcspn(line, "\n")] = '\0';
        q2_execute_line(line);
    }
}

