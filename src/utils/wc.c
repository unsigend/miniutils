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
#include <ctype.h>
#include <fileutil.h>
#include <stddef.h>
#include <unistd.h>

#include "die.h"

static const char *usages[] = {"wc [-clw] [FILE]..."};

struct counts {
    size_t lines;
    size_t words;
    size_t bytes;
};

static int count_stdin(struct counts *counts); /* count from stdin */
static int count_file(const char *path, struct counts *counts); /* count file */
static void statistics(struct counts *counts, int show_bytes, int show_lines,
                       int show_words, const char *path); /* print statistics */

int main(int argc, char *argv[])
{
    int lines = 0;
    int words = 0;
    int bytes = 0;
    struct counts counts = {0, 0, 0};

    struct argparse ctx;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_BOOL('c', "bytes", "print the byte counts", &bytes),
        OPT_BOOL('l', "lines", "print the newline counts", &lines),
        OPT_BOOL('w', "words", "print the word counts", &words),
        OPT_END(),
    };
    struct argparse_desc desc = {
        .prog = "wc",
        .desc = "print newline, word, and byte counts for each file",
        .usages = usages,
        .nusages = sizeof(usages) / sizeof(usages[0]),
        .epilog = NULL,
    };

    if (argparse_init(&ctx, opts, &desc) == -1)
        die_errno(argv[0]);

    if (argparse_parse(&ctx, argc - 1, argv + 1) == -1)
        die("%s: %s", argv[0], argparse_strerror(&ctx));

    if (!lines && !words && !bytes) /* default: print lines, words, and bytes */
        lines = words = bytes = 1;

    if (argparse_getremargc(&ctx) == 0) /* count from stdin */
    {
        if (count_stdin(&counts) == -1)
            die_errno(argv[0]);
        statistics(&counts, bytes, lines, words, NULL);
    } else {
        for (size_t i = 0; i < argparse_getremargc(&ctx); i++) {
            const char *path = argparse_getremargv(&ctx)[i];
            struct counts file_counts = {0, 0, 0}; /* file local */
            if (count_file(path, &file_counts) == -1)
                die_errno(argv[0]);
            statistics(&file_counts, bytes, lines, words, path);
            counts.lines += file_counts.lines;
            counts.words += file_counts.words;
            counts.bytes += file_counts.bytes;
        }

        if (argparse_getremargc(&ctx) > 1)
            statistics(&counts, bytes, lines, words, "total");
    }

    argparse_fini(&ctx);
    return 0;
}

static int count_stdin(struct counts *counts)
{
    char buf[1024];
    ssize_t n;
    int inword = 0;

    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        counts->bytes += n;
        for (ssize_t i = 0; i < n; i++) {
            int c = ((unsigned char *)buf)[i];
            if (isspace(c)) {
                if (c == '\n')
                    counts->lines++;
                inword = 0;
            } else {
                if (!inword) {
                    inword = 1;
                    counts->words++;
                }
            }
        }
    }

    if (n == -1)
        return -1;

    return 0;
}

static int count_file(const char *path, struct counts *counts)
{

    void *buf;
    size_t buflen;
    int inword = 0;

    if (read_file(path, &buf, &buflen) == -1)
        return -1;

    counts->bytes = buflen;

    for (size_t i = 0; i < buflen; i++) {
        int c = ((unsigned char *)buf)[i];
        if (isspace(c)) {
            if (c == '\n')
                counts->lines++;
            inword = 0;
        } else {
            if (!inword) {
                inword = 1;
                counts->words++;
            }
        }
    }

    free(buf);
    return 0;
}

static void statistics(struct counts *counts, int show_bytes, int show_lines,
                       int show_words, const char *path)
{
    const char *sep = "";

    if (show_lines)
        printf("%s%8zu", sep, counts->lines), sep = " ";
    if (show_words)
        printf("%s%7zu", sep, counts->words), sep = " ";
    if (show_bytes)
        printf("%s%7zu", sep, counts->bytes), sep = " ";
    if (path)
        printf(" %s", path);
    printf("\n");
}
