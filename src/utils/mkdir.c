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
#include <fileutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/stat.h>

#include "die.h"

static const char *hint = "mkdir [-p] [-m MODE] DIRECTORY...";
static noreturn void usage(void)
{
    fprintf(stdout, "usage: %s\n", hint);
    exit(EXIT_SUCCESS);
}

#define MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

int main(int argc, char *argv[])
{
    int p = 0;
    const char *mode = NULL;
    struct argparse ctx;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_BOOL('p', "parents", "create parent directories as needed", &p),
        OPT_STR('m', "mode", "set octal mode", &mode, OPT_REQUIRED),
        OPT_END(),
    };
    struct argparse_desc desc = {
        .prog = "mkdir",
        .desc = "create directories",
        .usage = "mkdir [OPTION]... DIRECTORY...",
    };

    if (argparse_init(&ctx, opts, &desc) == -1)
        die_errno(argv[0]);

    if (argparse_parse(&ctx, argc - 1, argv + 1) == -1)
        die("%s: %s", argv[0], argparse_strerror(&ctx));

    if (argparse_getremargc(&ctx) < 1)
        usage();

    int m = MODE;
    if (mode) {
        char *e = NULL;
        errno = 0;
        m = (int)strtol(mode, &e, 8);
        if (errno == ERANGE || errno == EINVAL || *e != '\0' || m < 0 ||
            m > 07777)
            die("%s: invalid mode: %s", argv[0], mode);
    }

    for (size_t i = 0; i < argparse_getremargc(&ctx); i++) {
        const char *d = argparse_getremargv(&ctx)[i];
        if (p) {
            if (mkdirp(d, m) == -1)
                die_errno(argv[0]);
        } else {
            if (mkdir(d, m) == -1)
                die_errno(argv[0]);
        }
    }

    argparse_fini(&ctx);
    return 0;
}