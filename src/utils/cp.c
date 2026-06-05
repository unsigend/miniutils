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
#include <limits.h>
#include <sys/stat.h>

#include "die.h"
#include "fs.h"

static const char *hint = "cp [-r] SOURCE... DEST";
static noreturn void usage(void)
{
  fprintf(stdout, "usage: %s\n", hint);
  exit(EXIT_SUCCESS);
}

/* cp FILE1 FILE2
   cp FILE1 DIR
   cp FILES... DIR
   cp -r DIR1 DIR2
*/

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
    int src_is_dir = dir_exists(src);
    int dst_is_dir = dir_exists(dst);
    int src_is_file = file_exists(src);
    int dst_is_file = file_exists(dst);
    int dst_exists = path_exists(dst);

    struct stat src_st, dst_st;
    if (stat(src, &src_st) != -1 && stat(dst, &dst_st) != -1)
      if (src_st.st_dev == dst_st.st_dev && src_st.st_ino == dst_st.st_ino)
        die("%s: %s and %s are identical (not copied)", argv[0], src, dst);

    if (src_is_file && dst_is_file) {
      if (copy_file(src, dst) == -1)
        die_errno(argv[0]);
    } else if (src_is_file && dst_is_dir) {
      char bname[PATH_MAX];
      char dstpath[PATH_MAX];
      if (fbasename(src, bname) == NULL)
        die_errno(argv[0]);
      if (snprintf(dstpath, sizeof(dstpath), "%s/%s", dst, bname) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        die_errno(argv[0]);
      }
      if (copy_file(src, dstpath) == -1)
        die_errno(argv[0]);
    } else if (src_is_file && !dst_exists) {
      if (copy_file(src, dst) == -1)
        die_errno(argv[0]);
    } else if (src_is_dir && !rec) {
      die("%s: %s is a directory", argv[0], src);
    } else if (src_is_dir && rec) {
      char bname[PATH_MAX];
      char dstpath[PATH_MAX];
      if (dst_is_dir) {
        if (fbasename(src, bname) == NULL)
          die_errno(argv[0]);
        if (snprintf(dstpath, sizeof(dstpath), "%s/%s", dst, bname) >=
            PATH_MAX) {
          errno = ENAMETOOLONG;
          die_errno(argv[0]);
        }
        struct stat st;
        if (stat(src, &st) == -1)
          die_errno(argv[0]);
        if (mkdirp(dstpath, st.st_mode) == -1)
          die_errno(argv[0]);
        if (copy_dir(src, dstpath) == -1)
          die_errno(argv[0]);
      } else {
        struct stat st;
        if (stat(src, &st) == -1)
          die_errno(argv[0]);
        if (mkdirp(dst, st.st_mode) == -1)
          die_errno(argv[0]);
        if (copy_dir(src, dst) == -1)
          die_errno(argv[0]);
      }
    } else
      die("%s: %s broken file or symbolic link", argv[0], src);

  } else {
    const char *dst = argparse_getremargv(&ctx)[argparse_getremargc(&ctx) - 1];
    if (!dir_exists(dst))
      die("%s: %s is not a directory", argv[0], dst);
    for (size_t i = 0; i < argparse_getremargc(&ctx) - 1; i++) {
      const char *src = argparse_getremargv(&ctx)[i];
      char bname[PATH_MAX];
      char dstpath[PATH_MAX];

      if (file_exists(src)) {
        if (fbasename(src, bname) == NULL)
          die_errno(argv[0]);
        if (snprintf(dstpath, sizeof(dstpath), "%s/%s", dst, bname) >=
            PATH_MAX) {
          errno = ENAMETOOLONG;
          die_errno(argv[0]);
        }
        if (copy_file(src, dstpath) == -1)
          die_errno(argv[0]);
      } else {
        if (!rec)
          die("%s: target is not a regular file or directory", argv[0]);
        struct stat st;
        if (stat(src, &st) == -1)
          die_errno(argv[0]);
        if (fbasename(src, bname) == NULL)
          die_errno(argv[0]);
        if (snprintf(dstpath, sizeof(dstpath), "%s/%s", dst, bname) >=
            PATH_MAX) {
          errno = ENAMETOOLONG;
          die_errno(argv[0]);
        }
        if (mkdirp(dstpath, st.st_mode) == -1)
          die_errno(argv[0]);
        if (copy_dir(src, dstpath) == -1)
          die_errno(argv[0]);
      }
    }
  }

  argparse_fini(&ctx);

  return 0;
}