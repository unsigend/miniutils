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
#include <fcntl.h>
#include <fileutil.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>

#include "die.h"

#define BUFSIZE 4096

static const char *usages[] = {"cat [FILE]..."};

int main(int argc, char *argv[])
{
    struct argparse ctx;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_END(),
    };
    struct argparse_desc desc = {
        .prog = "cat",
        .desc = "concatenate files and print on the standard output",
        .usages = usages,
        .nusages = sizeof(usages) / sizeof(usages[0]),
        .epilog = NULL,
    };

    if (argparse_init(&ctx, opts, &desc) == -1)
        die_errno(argv[0]);

    if (argparse_parse(&ctx, argc - 1, argv + 1) == -1)
        die("%s: %s", argv[0], argparse_strerror(&ctx));

    char buf[BUFSIZE];
    ssize_t n;
    size_t remargc = argparse_getremargc(&ctx);

    if (remargc == 0) /* cat from stdin */
    {
        while ((n = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
            if (write_all(STDOUT_FILENO, buf, n) == -1)
                die_errno(argv[0]);

        if (n == -1)
            die_errno(argv[0]);
    } else {
        const char *path;
        int fd;

        for (size_t i = 0; i < remargc; i++) {
            path = argparse_getremargv(&ctx)[i];
            fd = open(path, O_RDONLY);
            if (fd == -1)
                die_errno(argv[0]);

            while ((n = read(fd, buf, BUFSIZE)) > 0) {
                if (write_all(STDOUT_FILENO, buf, n) == -1)
                    die_errno(argv[0]);
            }
            if (n == -1)
                die_errno(argv[0]);
            if (close(fd) == -1)
                die_errno(argv[0]);
        }
    }

    argparse_fini(&ctx);
    return 0;
}