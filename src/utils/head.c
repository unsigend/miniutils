/* miniutils - A minimal GNU coreutils implementation
 * Copyright (C) 2025 - 2026 Qiu Yixiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <argparse.h>
#include <die.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#define DEFAULT_LINES 10
#define NOTSET INT_MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static const char *usages[] = {"head [-n LINES] [-c BYTES] [FILE...]"};

static int bytes_mode(FILE *fp, int bytes)
{
    char buf[BUFSIZ];
    size_t nread, total = 0, rem;

    rem = bytes - total;
    while ((nread = fread(buf, 1, MIN(rem, BUFSIZ), fp)) > 0 && rem > 0) {
        if (fwrite(buf, 1, nread, stdout) < nread)
            return -1;
        total += nread;
        rem = bytes - total;
    }

    if (ferror(fp))
        return -1;

    return 0;
}

static int lines_mode(FILE *fp, int lines)
{
    char *buf = NULL;
    size_t buflen = 0;
    ssize_t len;

    while ((len = getline(&buf, &buflen, fp)) != -1 && lines > 0) {
        if (fputs(buf, stdout) == EOF) {
            free(buf);
            return -1;
        }
        lines--;
    }

    if (len == -1 && ferror(fp)) {
        free(buf);
        return -1;
    }

    free(buf);

    return 0;
}

int main(int argc, char *argv[])
{
    int n = NOTSET;
    int c = NOTSET;

    struct argparse ctx;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_INT('n', "lines", "print the first LINES lines", &n, OPT_REQUIRED),
        OPT_INT('c', "bytes", "print the first BYTES bytes", &c, OPT_REQUIRED),
        OPT_END(),
    };
    struct argparse_desc desc = {
        .prog = "head",
        .desc = "print the first part of files",
        .usages = usages,
        .nusages = sizeof(usages) / sizeof(usages[0]),
        .epilog = NULL,
    };

    if (argparse_init(&ctx, opts, &desc) == -1)
        die_errno(argv[0]);
    if (argparse_parse(&ctx, argc - 1, argv + 1) == -1)
        die("%s: %s", argv[0], argparse_strerror(&ctx));

    if ((n != NOTSET && n < 0) || (c != NOTSET && c < 0))
        die("%s: invalid argument", argv[0]);
    if (n != NOTSET && c != NOTSET)
        die("%s: cannot combine -n and -c", argv[0]);
    if (n == NOTSET && c == NOTSET)
        n = DEFAULT_LINES;

    if (c != NOTSET) /* bytes mode */
    {
        if (argparse_getremargc(&ctx) < 1) /* from stdin */
        {
            if (bytes_mode(stdin, c) == -1)
                die_errno(argv[0]);

        } else /* from files */
        {
            for (size_t i = 0; i < argparse_getremargc(&ctx); i++) {
                FILE *fp = NULL;
                const char *filename = argparse_getremargv(&ctx)[i];

                fp = fopen(filename, "r");
                if (!fp)
                    die_errno(argv[0]);

                if (bytes_mode(fp, c) == -1)
                    die_errno(argv[0]);

                fclose(fp);
            }
        }
    } else /* lines mode */
    {
        if (argparse_getremargc(&ctx) < 1) /* from stdin */
        {
            if (lines_mode(stdin, n) == -1)
                die_errno(argv[0]);

        } else /* from files */
        {
            for (size_t i = 0; i < argparse_getremargc(&ctx); i++) {
                FILE *fp = NULL;
                const char *filename = argparse_getremargv(&ctx)[i];

                fp = fopen(filename, "r");
                if (!fp)
                    die_errno(argv[0]);

                if (lines_mode(fp, n) == -1)
                    die_errno(argv[0]);

                fclose(fp);
            }
        }
    }

    argparse_fini(&ctx);
    return 0;
}