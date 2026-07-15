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

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    while (1) {
        if (argc < 2) {
            if (fputs("y\n", stdout) == EOF)
                return EXIT_FAILURE;
        } else {
            for (int i = 1; i < argc; i++) {
                if (fputs(argv[i], stdout) == EOF)
                    return EXIT_FAILURE;
                if (i < argc - 1) {
                    if (fputc(' ', stdout) == EOF)
                        return EXIT_FAILURE;
                }
            }
            if (fputc('\n', stdout) == EOF)
                return EXIT_FAILURE;
        }
    }
    return 0;
}