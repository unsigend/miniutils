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
#include <fileutils.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/stat.h>

static const char *usages[] = {"mkdir [-p] [-m MODE] DIRECTORY..."};
static noreturn void usage(void)
{
    fprintf(stdout, "usage: %s\n", usages[0]);
    exit(EXIT_SUCCESS);
}

#define MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

int main(int argc, char *argv[])
{
    int p = 0;
    const char *modestr = NULL;

    struct argparse ctx;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_BOOL('p', "parents", "create parent directories as needed", &p),
        OPT_STR('m', "mode", "set octal mode", &modestr, OPT_REQUIRED),
        OPT_END(),
    };
    struct argparse_desc desc = {
        .prog = "mkdir",
        .desc = "create directories",
        .usages = usages,
        .epilog = NULL,
        .nusages = sizeof(usages) / sizeof(usages[0]),
    };

    if (argparse_init(&ctx, opts, &desc) == -1)
        die_errno(argv[0]);

    if (argparse_parse(&ctx, argc - 1, argv + 1) == -1)
        die("%s: %s", argv[0], argparse_strerror(&ctx));

    if (argparse_getremargc(&ctx) < 1)
        usage();

    unsigned int mode = MODE;
    if (modestr) {
        char *endstr = NULL;
        errno = 0;
        mode = (unsigned int)strtoul(modestr, &endstr, 8);
        if (errno == ERANGE || errno == EINVAL || *endstr != '\0' ||
            mode > 07777)
            die("%s: invalid mode: %s", argv[0], modestr);
    }

    for (size_t i = 0; i < argparse_getremargc(&ctx); i++) {
        const char *dir = argparse_getremargv(&ctx)[i];
        if (p) {
            if (mkdirp(dir, mode) == -1)
                die_errno(argv[0]);
        } else if (mkdir(dir, mode) == -1)
            die_errno(argv[0]);
    }

    argparse_fini(&ctx);
    return 0;
}