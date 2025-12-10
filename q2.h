#ifndef Q2_H
#define Q2_H

/* Q2: execute a single command-line (single-word command or builtin)
   returns 0 on success (child ran), non-zero on error (e.g. exec failed).
*/
int q2_execute_line(const char *line);

/* original Q2 REPL (kept for compatibility) */
void q2_run_repl(void);

#endif // Q2_H

