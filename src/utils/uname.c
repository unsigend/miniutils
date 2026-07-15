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
#include <sys/utsname.h>

static const char *usages[] = {"uname [-amnrsv]"};

int main(int argc, char *argv[])
{
    int s = 0; /* kernel name */
    int n = 0; /* network node name */
    int r = 0; /* kernel release */
    int v = 0; /* kernel version */
    int m = 0; /* machine hardware */
    int a = 0; /* all */

    struct argparse ctx;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_BOOL('a', NULL, "print all available information", &a),
        OPT_BOOL('m', NULL, "print machine hardware name", &m),
        OPT_BOOL('n', NULL, "print network node hostname", &n),
        OPT_BOOL('r', NULL, "print kernel release", &r),
        OPT_BOOL('s', NULL, "print kernel name", &s),
        OPT_BOOL('v', NULL, "print kernel version", &v),
        OPT_END(),
    };
    struct argparse_desc desc = {
        .prog = "uname",
        .desc = "print system information",
        .usages = usages,
        .nusages = sizeof(usages) / sizeof(usages[0]),
        .epilog = NULL,
    };

    if (argparse_init(&ctx, opts, &desc) == -1)
        die_errno(argv[0]);
    if (argparse_parse(&ctx, argc - 1, argv + 1) == -1)
        die("%s: %s", argv[0], argparse_strerror(&ctx));

    if (a)
        s = n = r = v = m = 1;
    if (!a && !s && !n && !r && !v && !m)
        s = 1; /* default to print kernel name */

    struct utsname buf;
    if (uname(&buf) == -1)
        die_errno(argv[0]);

    const char *sep = "";
    if (s)
        fputs(sep, stdout), fputs(buf.sysname, stdout), sep = " ";
    if (n)
        fputs(sep, stdout), fputs(buf.nodename, stdout), sep = " ";
    if (r)
        fputs(sep, stdout), fputs(buf.release, stdout), sep = " ";
    if (v)
        fputs(sep, stdout), fputs(buf.version, stdout), sep = " ";
    if (m)
        fputs(sep, stdout), fputs(buf.machine, stdout), sep = " ";
    putchar('\n');

    argparse_fini(&ctx);
    return 0;
}