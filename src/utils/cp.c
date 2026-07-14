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
#include <limits.h>
#include <sys/stat.h>

#include "die.h"

static const char *usages[] = {"cp [-r] SOURCE... DEST"};

static noreturn void usage(void)
{
    fprintf(stdout, "usage: %s\n", usages[0]);
    exit(EXIT_SUCCESS);
}

/* Copy file path into directory dir as dir/<basename(path)>. dir must exist.
   Return 0 on success, -1 on error and set errno. */
static int cp_file_to_dir(const char *path, const char *dir)
{
    char bname[PATH_MAX];
    char destpath[PATH_MAX];

    if (fbasename(path, bname) == NULL)
        return -1;
    if (snprintf(destpath, PATH_MAX, "%s/%s", dir, bname) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }
    if (copy_file(destpath, path) == -1)
        return -1;
    return 0;
}

/* Copy directory src into directory dest as dest/<basename(src)>, creating
   that subdirectory then copying its contents recursively. Both src and dest
   must exist. Return 0 on success, -1 on error and set errno. */
static int cp_dir_to_dir(const char *src, const char *dest)
{
    char bname[PATH_MAX];
    char destpath[PATH_MAX];
    struct stat st;

    if (fbasename(src, bname) == NULL)
        return -1;
    if (snprintf(destpath, PATH_MAX, "%s/%s", dest, bname) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (stat(src, &st) == -1)
        return -1;
    if (mkdirp(destpath, st.st_mode) == -1)
        return -1;
    if (copy_dir(destpath, src) == -1)
        return -1;
    return 0;
}

int main(int argc, char *argv[])
{
    int recursive = 0;

    struct argparse ctx;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_BOOL('r', "recursive", "recursive copy", &recursive),
        OPT_END(),
    };
    struct argparse_desc desc = {
        .prog = "cp",
        .desc = "copy files and directories",
        .usages = usages,
        .nusages = sizeof(usages) / sizeof(usages[0]),
        .epilog = NULL,
    };

    if (argparse_init(&ctx, opts, &desc) == -1)
        die_errno(argv[0]);

    if (argparse_parse(&ctx, argc - 1, argv + 1) == -1)
        die("%s: %s", argv[0], argparse_strerror(&ctx));

    if (argparse_getremargc(&ctx) < 2)
        usage();

    if (argparse_getremargc(&ctx) == 2) {

        const char *src = argparse_getremargv(&ctx)[0];
        const char *dest = argparse_getremargv(&ctx)[1];

        int src_isdir = dir_exists(src);
        int dest_isdir = dir_exists(dest);
        int src_isfile = file_exists(src);
        int dest_isfile = file_exists(dest);
        int dest_exist = path_exists(dest);

        struct stat src_st, dest_st;

        if (stat(src, &src_st) != -1 && stat(dest, &dest_st) != -1)
            if (src_st.st_dev == dest_st.st_dev &&
                src_st.st_ino == dest_st.st_ino)
                die("%s: %s and %s are identical (not copied)", argv[0], src,
                    dest);

        if (src_isfile && dest_isfile) /* cp file1 file2 */
        {
            if (copy_file(dest, src) == -1)
                die_errno(argv[0]);
        }

        else if (src_isfile && dest_isdir) /* cp file dir/ */
        {
            if (cp_file_to_dir(src, dest) == -1)
                die_errno(argv[0]);
        }

        else if (src_isfile && !dest_exist) /* cp file newfile */
        {
            if (copy_file(dest, src) == -1)
                die_errno(argv[0]);
        } else if (src_isdir && !recursive)
            die("%s: %s is a directory", argv[0], src);
        else if (src_isdir && recursive) {

            if (dest_isdir &&
                cp_dir_to_dir(src, dest) == -1) /* cp dir1 dir2/ */
                die_errno(argv[0]);

            else /* cp dir newdir/ */
            {
                struct stat st;
                if (stat(src, &st) == -1)
                    die_errno(argv[0]);
                if (mkdirp(dest, st.st_mode) == -1)
                    die_errno(argv[0]);
                if (copy_dir(dest, src) == -1)
                    die_errno(argv[0]);
            }
        } else
            die("%s: %s broken file or symbolic link", argv[0], src);

    } else {
        const char *dest = argparse_getremargv(
            &ctx)[argparse_getremargc(&ctx) - 1]; /* last one as dest */

        if (!dir_exists(dest))
            die("%s: %s is not a directory", argv[0], dest);
        for (size_t i = 0; i < argparse_getremargc(&ctx) - 1; i++) {
            const char *src = argparse_getremargv(&ctx)[i];
            if (file_exists(src)) {
                if (cp_file_to_dir(src, dest) == -1)
                    die_errno(argv[0]);
            } else {
                if (!recursive)
                    die("%s: target is not a regular file or directory",
                        argv[0]);
                if (cp_dir_to_dir(src, dest) == -1)
                    die_errno(argv[0]);
            }
        }
    }

    argparse_fini(&ctx);
    return 0;
}
