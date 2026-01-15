/* mini-coreutils - A minimal set of core utilities for Unix-like systems
 * Copyright (C) 2025 Qiu Yixiang
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

#include <readelf.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#define _PANIC() {do {  \
    perror("readelf");  \
    exit(EXIT_FAILURE); \
} while (0); }

void elf_parse_symtab(elf_file_t *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }

    
}

void elf_print_symtab(elf_file_t *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }
}