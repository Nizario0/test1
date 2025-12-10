#define _POSIX_C_SOURCE 200809L
#include "q4.h"
#include "q3.h"
#include "q2.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

void q4_run_repl(void) {
    int last_status = 0;
    char line[1024];

    while (1) {
        char prompt[64];
        if (WIFEXITED(last_status))
            snprintf(prompt, sizeof(prompt), "enseash [exit:%d] %% ", WEXITSTATUS(last_status));
        else if (WIFSIGNALED(last_status))
            snprintf(prompt, sizeof(prompt), "enseash [sign:%d] %% ", WTERMSIG(last_status));
        else
            snprintf(prompt, sizeof(prompt), "enseash %% ");

        (void)write(STDOUT_FILENO, prompt, strlen(prompt));

        if (fgets(line, sizeof(line), stdin) == NULL) {
            (void)write(STDOUT_FILENO, "\nGoodbye.\n", 10);
            break;
        }

        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;

        if (strcmp(line, "exit") == 0) break;

       q2_execute_line(line, &last_status);

    }
}


