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


#define _ELF_EHRD_FLAG     (1U << 1)
#define _ELF_SHDR_FLAG     (1U << 2)
#define _ELF_SYMTAB_FLAG   (1U << 3)
#define _ELF_STRTAB_FLAG   (1U << 4)
#define _ELF_SHSTRTAB_FLAG (1U << 5)
#define _ELF_PHDR_FLAG     (1U << 6)
#define _ELF_RELA_FLAG     (1U << 7)
#define _ELF_DYN_FLAG      (1U << 8)

#define ELF_PARSED_EHDR(ELF_FILE)       ((ELF_FILE)->flags & _ELF_EHRD_FLAG)
#define ELF_PARSED_SHDR(ELF_FILE)       ((ELF_FILE)->flags & _ELF_SHDR_FLAG)
#define ELF_PARSED_SYMTAB(ELF_FILE)     ((ELF_FILE)->flags & _ELF_SYMTAB_FLAG)
#define ELF_PARSED_STRTAB(ELF_FILE)     ((ELF_FILE)->flags & _ELF_STRTAB_FLAG)
#define ELF_PARSED_SHSTRTAB(ELF_FILE)   ((ELF_FILE)->flags & _ELF_SHSTRTAB_FLAG)
#define ELF_PARSED_PHDR(ELF_FILE)       ((ELF_FILE)->flags & _ELF_PHDR_FLAG)
#define ELF_PARSED_RELA(ELF_FILE)       ((ELF_FILE)->flags & _ELF_RELA_FLAG)
#define ELF_PARSED_DYN(ELF_FILE)        ((ELF_FILE)->flags & _ELF_DYN_FLAG)

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
#define __DEP_PHDR(ELF_FILE)        \
    if (!ELF_PARSED_PHDR(ELF_FILE)) elf_parse_phdr(ELF_FILE);
#define __DEP_RELA(ELF_FILE)        \
    if (!ELF_PARSED_RELA(ELF_FILE)) elf_parse_rela(ELF_FILE);
#define __DEP_DYN(ELF_FILE)        \
    if (!ELF_PARSED_DYN(ELF_FILE)) elf_parse_dyn(ELF_FILE);

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
#define __GUARD_PHDR(ELF_FILE)      \
    if (ELF_PARSED_PHDR(ELF_FILE)) return;
#define __GUARD_RELA(ELF_FILE)      \
    if (ELF_PARSED_RELA(ELF_FILE)) return;
#define __GUARD_DYN(ELF_FILE)      \
    if (ELF_PARSED_DYN(ELF_FILE)) return;
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
    /* size of the string table */
    uint64_t sz_strtab;
    /* symbol table */
    union {
        Elf64_Sym* symtab64;
        Elf32_Sym* symtab32;
    } symtab;
    /* size of the symbol table */
    uint64_t sz_symtab;
    /* program header table */
    union {
        Elf64_Phdr* phdr64;
        Elf32_Phdr* phdr32;
    } phdr;

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
extern void elf_parse_phdr(_elf_meta *elf_file);
extern void elf_parse_rela(_elf_meta *elf_file);
extern void elf_parse_dyn(_elf_meta *elf_file);

// binary data print functions
extern void elf_print_ehdr(_elf_meta *elf_file);
extern void elf_print_shdr(_elf_meta *elf_file);
extern void elf_print_shstrtab(_elf_meta *elf_file);
extern void elf_print_symtab(_elf_meta *elf_file);
extern void elf_print_strtab(_elf_meta *elf_file);
extern void elf_print_phdr(_elf_meta *elf_file);
extern void elf_print_rela(_elf_meta *elf_file);
extern void elf_print_dyn(_elf_meta *elf_file);
extern void elf_dump_section(_elf_meta *elf_file, const char* sh_name_idx);
#endif