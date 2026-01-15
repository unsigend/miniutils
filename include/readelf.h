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
    /* the section header table */
    union {
        Elf64_Shdr* elf64_shdr;
        Elf32_Shdr* elf32_shdr;
    } section;
    /* the section header string table */
    char* shstrtab;
    /* the symbol table */
    char* strtab;
    
} elf_file_t;

extern int elf_file_init(elf_file_t *elf_file, const char *filename);
extern void elf_file_cleanup(elf_file_t *elf_file);

extern int readelf(int argc, char *argv[]);

extern void parse_ehdr(elf_file_t *elf_file);
extern void parse_shdr(elf_file_t *elf_file);
extern void parse_shstrtab(elf_file_t *elf_file);

extern void print_ehdr(elf_file_t *elf_file);
extern void print_shdr(elf_file_t *elf_file);
extern void print_shstrtab(elf_file_t *elf_file);

#endif