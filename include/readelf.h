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

/* To these macros needs define __ELF_INTERNAL__ */
#if defined (__ELF_INTERNAL__)

#define _ELF_EHRD_FLAG     (1U << 1)
#define _ELF_SHDR_FLAG     (1U << 2)
#define _ELF_SYMTAB_FLAG   (1U << 3)
#define _ELF_STRTAB_FLAG   (1U << 4)
#define _ELF_SHSTRTAB_FLAG (1U << 5)

#define ELF_PARSED_EHDR(ELF_FILE)       ((ELF_FILE)->flags & _ELF_EHRD_FLAG)
#define ELF_PARSED_SHDR(ELF_FILE)       ((ELF_FILE)->flags & _ELF_SHDR_FLAG)
#define ELF_PARSED_SYMTAB(ELF_FILE)     ((ELF_FILE)->flags & _ELF_SYMTAB_FLAG)
#define ELF_PARSED_STRTAB(ELF_FILE)     ((ELF_FILE)->flags & _ELF_STRTAB_FLAG)
#define ELF_PARSED_SHSTRTAB(ELF_FILE)   ((ELF_FILE)->flags & _ELF_SHSTRTAB_FLAG)

#define __DEP_EHDR(ELF_FILE)        \
    if (!ELF_PARSED_EHDR(ELF_FILE)) elf_parse_ehdr(ELF_FILE);
#define __DEP_SHDR(ELF_FILE)        \
    if (!ELF_PARSED_SHDR(ELF_FILE)) elf_parse_shdr(ELF_FILE);
#define __DEP_SHSTRTAB(ELF_FILE)    \
    if (!ELF_PARSED_SHSTRTAB(ELF_FILE)) elf_parse_shstrtab(ELF_FILE);
#define __DEP_SYMTAB(ELF_FILE)      \
    if (!ELF_PARSED_SYMTAB(ELF_FILE)) elf_parse_symtab(ELF_FILE);
#define __DEP_STRTAB(ELF_FILE)      \
    if (!ELF_PARSED_STRTAB(ELF_FILE)) elf_parse_strtab(ELF_FILE);

#define __GUARD_EHDR(ELF_FILE)      \
    if (ELF_PARSED_EHDR(ELF_FILE)) return;
#define __GUARD_SHDR(ELF_FILE)      \
    if (ELF_PARSED_SHDR(ELF_FILE)) return;
#define __GUARD_SHSTRTAB(ELF_FILE)   \
    if (ELF_PARSED_SHSTRTAB(ELF_FILE)) return;
#define __GUARD_SYMTAB(ELF_FILE)    \
    if (ELF_PARSED_SYMTAB(ELF_FILE)) return;
#define __GUARD_STRTAB(ELF_FILE)    \
    if (ELF_PARSED_STRTAB(ELF_FILE)) return;

#endif

/**
 * @brief: the ELF file structure
 */
typedef struct {
    /* file descriptor of the ELF file */
    int fd;
    /* filename of the ELF file */
    const char *filename;
    /* ELF class */
    int elf_class;
    /* flags of the ELF file for parser */
    uint32_t flags;
    /* ELF header */
    union {
        Elf64_Ehdr ehdr64;
        Elf32_Ehdr ehdr32;
    } ehdr;
    /* section header table */
    union {
        Elf64_Shdr* shdr64;
        Elf32_Shdr* shdr32;
    } shdr;
    /* section header string table */
    char* shstrtab;
    /* string table */
    char* strtab;
    /* symbol table */
    union {
        Elf64_Sym* sym64;
        Elf32_Sym* sym32;
    } symtab;
    
} _elf_meta;

extern int elf_struct_init(_elf_meta *elf_file, const char *filename);
extern void elf_struct_cleanup(_elf_meta *elf_file);

// main function
extern int readelf(int argc, char *argv[]);

// parser functions
extern void elf_parse_ehdr(_elf_meta *elf_file);
extern void elf_parse_shdr(_elf_meta *elf_file);
extern void elf_parse_shstrtab(_elf_meta *elf_file);
extern void elf_parse_symtab(_elf_meta *elf_file);
extern void elf_parse_strtab(_elf_meta *elf_file);

// binary data print functions
extern void elf_print_ehdr(_elf_meta *elf_file);
extern void elf_print_shdr(_elf_meta *elf_file);
extern void elf_print_shstrtab(_elf_meta *elf_file);
extern void elf_print_symtab(_elf_meta *elf_file);
extern void elf_print_strtab(_elf_meta *elf_file);
#endif