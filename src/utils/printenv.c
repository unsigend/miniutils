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
#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main(int argc, char *argv[])
{
    if (argc < 2) /* print all environment variables */
    {
        for (char **env = environ; *env; env++) {
            if (puts(*env) == EOF)
                die_errno(argv[0]);
        }
        return 0;
    } else /* print the remaining arguments as environment variables */
    {
        int ok = 1;
        for (int i = 1; i < argc; i++) {
            char *val = getenv(argv[i]);
            if (!val) {
                ok = 0;
                continue;
            }
            if (puts(val) == EOF)
                die_errno(argv[0]);
        }
        return ok ? EXIT_SUCCESS : EXIT_FAILURE;
    }
}