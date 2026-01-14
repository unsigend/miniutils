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

#ifndef _COREUTILS_READELF_H
#define _COREUTILS_READELF_H

#include <elf.h>

/**
 * @brief: the ELF file structure
 */
typedef struct {
    /* the file descriptor of the ELF file */
    int fd;
    /* the filename of the ELF file */
    const char *filename;
    /* the ELF class */
    int elf_class;
    /* the ELF header */
    union {
        Elf64_Ehdr elf64_header;
        Elf32_Ehdr elf32_header;
    } header;

} elf_file_t;

/**
 * @brief: the readelf enrty function
 * @param argc: the number of command line arguments
 * @param argv: the command line arguments
 * @return: the result of the readelf function
 */
extern int readelf(int argc, char *argv[]);

/**
 * @brief: parse the ELF header
 * @param elf_file: the elf file struct
 */
extern void parse_header(elf_file_t *elf_file);

/**
 * @brief: print the ELF header
 * @param elf_file: the elf file struct
 */
extern void print_header(elf_file_t *elf_file);

#endif