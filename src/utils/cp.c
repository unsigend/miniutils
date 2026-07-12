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

static const char *hint = "cp [-r] SOURCE... DEST";
static noreturn void usage(void)
{
    fprintf(stdout, "usage: %s\n", hint);
    exit(EXIT_SUCCESS);
}

/* Copy a file under a directory, dir must exist. Return 0 on success, -1 on
   error and set errno. */
int cp_ftod(const char *path, const char *dir);

/* Copy a directory under another directory recursively, d1 and d2 must exist.
   Return 0 on success, -1 on error and set errno. */
int cp_dtod(const char *d1, const char *d2);

int main(int argc, char *argv[])
{
    int rec = 0;
    struct argparse ctx;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_BOOL('r', "recursive", "recursive copy", &rec),
        OPT_END(),
    };
    struct argparse_desc desc = {
        .prog = "cp",
        .desc = "copy files and directories",
        .usage = hint,
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
        const char *dst = argparse_getremargv(&ctx)[1];
        int s_isdir = dir_exists(src);
        int d_isdir = dir_exists(dst);
        int s_isfile = file_exists(src);
        int d_isfile = file_exists(dst);
        int d_exist = path_exists(dst);

        struct stat src_st, dst_st;
        if (stat(src, &src_st) != -1 && stat(dst, &dst_st) != -1)
            if (src_st.st_dev == dst_st.st_dev &&
                src_st.st_ino == dst_st.st_ino)
                die("%s: %s and %s are identical (not copied)", argv[0], src,
                    dst);

        /* cp file1 file2*/
        if (s_isfile && d_isfile) {
            if (copy_file(dst, src) == -1)
                die_errno(argv[0]);
        }
        /* cp file dir/ */
        else if (s_isfile && d_isdir) {
            if (cp_ftod(src, dst) == -1)
                die_errno(argv[0]);
        }
        /* cp file newfile */
        else if (s_isfile && !d_exist) {
            if (copy_file(dst, src) == -1)
                die_errno(argv[0]);
        } else if (s_isdir && !rec) {
            die("%s: %s is a directory", argv[0], src);
        } else if (s_isdir && rec) {
            /* cp dir1 dir2/*/
            if (d_isdir && cp_dtod(src, dst) == -1)
                die_errno(argv[0]);

            /* cp dir newdir/ */
            else {
                struct stat st;
                if (stat(src, &st) == -1)
                    die_errno(argv[0]);
                if (mkdirp(dst, st.st_mode) == -1)
                    die_errno(argv[0]);
                if (copy_dir(dst, src) == -1)
                    die_errno(argv[0]);
            }
        } else
            die("%s: %s broken file or symbolic link", argv[0], src);

    } else {
        const char *dst =
            argparse_getremargv(&ctx)[argparse_getremargc(&ctx) - 1];
        if (!dir_exists(dst))
            die("%s: %s is not a directory", argv[0], dst);
        for (size_t i = 0; i < argparse_getremargc(&ctx) - 1; i++) {
            const char *src = argparse_getremargv(&ctx)[i];
            if (file_exists(src)) {
                if (cp_ftod(src, dst) == -1)
                    die_errno(argv[0]);
            } else {
                if (!rec)
                    die("%s: target is not a regular file or directory",
                        argv[0]);
                if (cp_dtod(src, dst) == -1)
                    die_errno(argv[0]);
            }
        }
    }

    argparse_fini(&ctx);
    return 0;
}

int cp_ftod(const char *path, const char *dir)
{
    char bname[PATH_MAX];
    char dstpath[PATH_MAX];
    if (fbasename(path, bname) == NULL)
        return -1;
    if (snprintf(dstpath, sizeof(dstpath), "%s/%s", dir, bname) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }
    if (copy_file(dstpath, path) == -1)
        return -1;
    return 0;
}

int cp_dtod(const char *d1, const char *d2)
{
    char bname[PATH_MAX];
    char dstpath[PATH_MAX];
    if (fbasename(d1, bname) == NULL)
        return -1;
    if (snprintf(dstpath, sizeof(dstpath), "%s/%s", d2, bname) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }
    struct stat st;
    if (stat(d1, &st) == -1)
        return -1;
    if (mkdirp(dstpath, st.st_mode) == -1)
        return -1;
    if (copy_dir(dstpath, d1) == -1)
        return -1;
    return 0;
}