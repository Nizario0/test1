#define _POSIX_C_SOURCE 200809L
#include "q2.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

/* safe write */
static void safe_write(const char *s) {
    size_t len = strlen(s);
    const char *p = s;
    while (len > 0) {
        ssize_t w = write(STDOUT_FILENO, p, len);
        if (w < 0) { if (errno == EINTR) continue; return; }
        p += w;
        len -= (size_t)w;
    }
}

/* exécution d’une commande externe */
static int exec_command(const char *cmd, int *status) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); if (status) *status = 1; return -1; }
    if (pid == 0) {
        execlp(cmd, cmd, NULL);
        char err[256];
        int n = snprintf(err, sizeof(err),
                         "enseash: failed to exec '%s': %s\n",
                         cmd, strerror(errno));
        if (n > 0) (void)write(STDERR_FILENO, err, (size_t)(n < (int)sizeof(err) ? n : (int)sizeof(err)));
        _exit(127);
    } else {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (status) *status = wstatus;
    }
    return 0;
}

int q2_execute_line(const char *line, int *status) {
    if (line[0] == '\0') return 0;

    /* builtin "yoo" */
    if (strcmp(line, "yoo") == 0) {
        safe_write("salut\n");
        if (status) *status = 0;
        return 0;
    }

    /* builtin "date" */
    if (strcmp(line, "date") == 0) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); if(status) *status = 1; return 1; }
        if (pid == 0) {
            execlp("date","date","+%Y-%m-%d %H:%M:%S.%N",NULL);
            _exit(127);
        } else {
            int wstatus;
            waitpid(pid, &wstatus, 0);
            if (status) *status = wstatus;
            return 0;
        }
    }

    /* autres commandes */
    exec_command(line, status);
    return 0;
}

/* REPL minimal Q2 */
void q2_run_repl(void) {
    char line[1024];
    int status = 0;
    while (1) {
        (void)write(STDOUT_FILENO, "enseash % ", 10);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            safe_write("\n");
            clearerr(stdin);
            continue;
        }
        line[strcspn(line, "\n")] = '\0';
        q2_execute_line(line, &status);
    }
}

