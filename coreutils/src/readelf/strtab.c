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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define STR_ALIGN 6

#define _PANIC() {do {  \
    perror("readelf");  \
    exit(EXIT_FAILURE); \
} while (0); }

void elf_parse_strtab(_elf_meta *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }
__GUARD_STRTAB(elf_file)
__DEP_EHDR(elf_file)
__DEP_SHDR(elf_file)
__DEP_SHSTRTAB(elf_file)

    uint64_t strtaboff = 0;
    uint64_t strtabsize = 0;

    if (elf_file->elf_class == ELFCLASS64){
        for (uint16_t i = 1; i < elf_file->ehdr.ehdr64.e_shnum; i++) {
            if (elf_file->shdr.shdr64[i].sh_type == SHT_STRTAB && 
              !strcmp(elf_file->shstrtab + elf_file->shdr.shdr64[i].sh_name, ".strtab")) {
                strtaboff = elf_file->shdr.shdr64[i].sh_offset;
                strtabsize = elf_file->shdr.shdr64[i].sh_size;
                break;
            }
        }
    } else {
        for (uint16_t i = 1; i < elf_file->ehdr.ehdr32.e_shnum; i++) {
            if (elf_file->shdr.shdr32[i].sh_type == SHT_STRTAB && 
              !strcmp(elf_file->shstrtab + elf_file->shdr.shdr32[i].sh_name, ".strtab")) {
                strtaboff = elf_file->shdr.shdr32[i].sh_offset;
                strtabsize = elf_file->shdr.shdr32[i].sh_size;
                break;
            }
        }
    }
    if (strtaboff == 0 || strtabsize == 0) {
        elf_file->flags |= _ELF_STRTAB_FLAG;
        return;
    }

    elf_file->strtab = (char*)malloc(strtabsize);
    if (!elf_file->strtab) _PANIC();
    if (lseek(elf_file->fd, strtaboff, SEEK_SET) == -1) _PANIC();
    if ((uint64_t)read(elf_file->fd, elf_file->strtab, strtabsize)
        != strtabsize) _PANIC();

    elf_file->sz_strtab = strtabsize;
    elf_file->flags |= _ELF_STRTAB_FLAG;
}

void elf_print_strtab(_elf_meta *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }

__DEP_STRTAB(elf_file)

    printf("\nString dump of section '.strtab':\n");
    for (uint32_t off = 0; off < elf_file->sz_strtab; off++) {
        if (elf_file->strtab[off]) {
            printf("  [%6x]  %s\n", off, elf_file->strtab + off);
            off += strlen(elf_file->strtab + off);
        }
    }
    fputc('\n', stdout);
}