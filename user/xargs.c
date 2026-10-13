
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFSIZE 512

int
run(char *args[])
{
    int pid = fork();

    if (pid < 0) {
        fprintf(2, "xargs: fork failed\n");
        return 1;
    }

    if (pid == 0) {
        exec(args[0], args);
        fprintf(2, "xargs: exec failed\n");
        exit(1);
    }

    int status;
    if (wait(&status) < 0)
        return 1;

    return status != 0;
}

int
main(int argc, char *argv[])
{
    char buf[BUFSIZE];
    char *args[MAXARG];

    int limit = 0;
    int start = 1;
    int failed = 0;

    // Check for the optional -n N
    if (argc >= 2 && strcmp(argv[1], "-n") == 0) {
        if (argc < 4) {
            fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
            exit(1);
        }

        for (int i = 0; argv[2][i]; i++) {
            if (argv[2][i] < '0' || argv[2][i] > '9') {
                fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
                exit(1);
            }
        }

        limit = atoi(argv[2]);
        start = 3;

        if (limit < 1) {
            fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
            exit(1);
        }
    }

    if (start >= argc || argc - start >= MAXARG) {
        fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
        exit(1);
    }

    int base = argc - start;

    // Maximum number of additional words
    int capacity = MAXARG - 1 - base;

    if (limit > capacity) {
        fprintf(2, "xargs: too many arguments\n");
        exit(1);
    }

    for (int i = 0; i < base; i++)
        args[i] = argv[start + i];

    int count = base;
    int pos = 0;
    int inword = 0;
    int r;
    char c;

    while ((r = read(0, &c, 1)) == 1) {
        int space = (c == ' ' || c == '\t' || c == '\n');

        if (space) {
            if (inword) {
                buf[pos++] = '\0';
                inword = 0;
            }

            // Execute at a newline in normal mode,
            // or after N words in -n mode
            if ((limit == 0 && c == '\n') ||
                (limit > 0 && count - base == limit)) {

                if (count > base) {
                    args[count] = 0;
                    if (run(args))
                        failed = 1;
                }

                count = base;
                pos = 0;
            }

            continue;
        }

        if (!inword) {
            if (count >= MAXARG - 1 ||
                (limit > 0 && count - base >= limit)) {
                fprintf(2, "xargs: too many arguments\n");
                exit(1);
            }

            args[count++] = &buf[pos];
            inword = 1;
        }

        if (pos >= BUFSIZE - 1) {
            fprintf(2, "xargs: input too long\n");
            exit(1);
        }

        buf[pos++] = c;
    }

    if (r < 0) {
        fprintf(2, "xargs: read failed\n");
        exit(1);
    }

    if (inword)
        buf[pos++] = '\0';

    if (count > base) {
        args[count] = 0;
        if (run(args))
            failed = 1;
    }

    exit(failed);
}
