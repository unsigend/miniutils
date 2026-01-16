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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <readelf.h>
#include <string.h>

#define STR_TAB ".strtab"
#define SYM_TAB ".symtab"
#define RELA_TAB ".rela"

void elf_print_section(_elf_meta *elf_file, const char* sh_name_idx){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }

__DEP_EHDR(elf_file);
__DEP_SHDR(elf_file);
__DEP_SHSTRTAB(elf_file);

    char* endptr = NULL;
    long sh_idx = strtol(sh_name_idx, &endptr, 10);
    const char* sh_name = NULL;
    if (endptr && *endptr == '\0'){
        uint16_t shnum;
        if (elf_file->elf_class == ELFCLASS64){
            shnum = elf_file->ehdr.ehdr64.e_shnum;
        } else {
            shnum = elf_file->ehdr.ehdr32.e_shnum;
        }
        if (sh_idx == 0)
            fprintf(stderr, "Section '' has no data to dump\n");
        else if (sh_idx <= 0 || sh_idx >= shnum)
            fprintf(stderr, "readelf: Warning: Section '%ld' was not dumped because it does not exist\n", sh_idx);
        else {
            if (elf_file->elf_class == ELFCLASS64){
                sh_name = &elf_file->shstrtab[elf_file->shdr.shdr64[sh_idx].sh_name];
            } else {
                sh_name = &elf_file->shstrtab[elf_file->shdr.shdr32[sh_idx].sh_name];
            }
        }

    }
    if (!sh_name) sh_name = sh_name_idx;
    if (!strcmp(sh_name, STR_TAB)) elf_print_strtab(elf_file);
    else if (!strcmp(sh_name, SYM_TAB)) elf_print_symtab(elf_file);
    else fprintf(stderr, "readelf: Warning: Section '%s' was not dumped because it does not exist\n", sh_name);
}