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

#define __ELF_INTERNAL__

#include <readelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define _PANIC() {do {  \
    perror("readelf");  \
    exit(EXIT_FAILURE); \
} while (0); }

void elf_parse_shstrtab(_elf_meta *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }
__GUARD_SHSTRTAB(elf_file);
__DEP_EHDR(elf_file);

    uint16_t shstrndx;
    uint64_t shstroff;
    uint64_t shstrsize;
    
    if (elf_file->elf_class == ELFCLASS64){
        shstrndx = elf_file->ehdr.ehdr64.e_shstrndx;
    } else {
        shstrndx = elf_file->ehdr.ehdr32.e_shstrndx;
    }

    if (shstrndx == SHN_UNDEF) {
        elf_file->flags |= _ELF_SHSTRTAB_FLAG;
        return;
    }

    if (elf_file->elf_class == ELFCLASS64){
        if (!elf_file->shdr.shdr64) {
            elf_file->flags |= _ELF_SHSTRTAB_FLAG;
            return;
        }
        shstrsize = elf_file->shdr.shdr64[shstrndx].sh_size;
        shstroff = elf_file->shdr.shdr64[shstrndx].sh_offset;
    } else {
        if (!elf_file->shdr.shdr32) {
            elf_file->flags |= _ELF_SHSTRTAB_FLAG;
            return;
        }
        shstrsize = elf_file->shdr.shdr32[shstrndx].sh_size;
        shstroff = elf_file->shdr.shdr32[shstrndx].sh_offset;
    }
    
    if (shstrsize == 0) {
        elf_file->flags |= _ELF_SHSTRTAB_FLAG;
        return;
    }

    elf_file->shstrtab = (char*)malloc(shstrsize);
    
    if (!elf_file->shstrtab) _PANIC();
    if (lseek(elf_file->fd, shstroff, SEEK_SET) == -1) _PANIC();
    if ((uint64_t)read(elf_file->fd, elf_file->shstrtab, shstrsize)
        != shstrsize) _PANIC();
    
    elf_file->flags |= _ELF_SHSTRTAB_FLAG;
}