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

#include <die.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>

static const char *usages[] = {"sleep SECONDS"};

int main(int argc, char *argv[])
{
    char *endstr;
    unsigned long v;
    unsigned int left;

    if (argc != 2)
        usage(usages[0]);
    if (argv[1][0] == '-') /* reject negative numbers */
        die("%s: invalid time interval '%s'", argv[0], argv[1]);

    errno = 0;
    v = strtoul(argv[1], &endstr, 10);
    if (endstr == argv[1] || *endstr != '\0' || errno == ERANGE || v > UINT_MAX)
        die("%s: invalid time interval '%s'", argv[0], argv[1]);

    left = (unsigned int)v;

    while (left) /* sleep might be interrupted by signal */
        left = sleep(left);

    return EXIT_SUCCESS;
}