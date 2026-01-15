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

#define _PANIC() {do {  \
    perror("readelf");  \
    exit(EXIT_FAILURE); \
} while (0); }

void parse_shdr(elf_file_t *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }

    uint64_t shoff;
    uint16_t shnum;
    
    if (elf_file->elf_class == ELFCLASS64){
        shoff = elf_file->header.elf64_header.e_shoff;
        shnum = elf_file->header.elf64_header.e_shnum;
    } else {
        shoff = elf_file->header.elf32_header.e_shoff;
        shnum = elf_file->header.elf32_header.e_shnum;
    }

    if (shoff == 0 || shnum == 0) {
        if (elf_file->elf_class == ELFCLASS64) {
            elf_file->section.elf64_shdr = NULL;
        } else {
            elf_file->section.elf32_shdr = NULL;
        }
        return;
    }

    if (elf_file->elf_class == ELFCLASS64){
        elf_file->section.elf64_shdr = 
           (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr) * shnum);
        if (!elf_file->section.elf64_shdr) _PANIC();
    } else {
        elf_file->section.elf32_shdr = 
           (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr) * shnum);
        if (!elf_file->section.elf32_shdr) _PANIC();
    }

    if (lseek(elf_file->fd, shoff, SEEK_SET) == -1) _PANIC();

    if (elf_file->elf_class == ELFCLASS64){
        ssize_t nb = read(elf_file->fd, elf_file->section.elf64_shdr,
            sizeof(Elf64_Shdr) * shnum);
        if (nb == -1 || (size_t)nb != sizeof(Elf64_Shdr) * shnum) _PANIC();
    } else {
        ssize_t nb = read(elf_file->fd, elf_file->section.elf32_shdr,
            sizeof(Elf32_Shdr) * shnum);
        if (nb == -1 || (size_t)nb != sizeof(Elf32_Shdr) * shnum) _PANIC();
    }
}

void print_shdr(elf_file_t *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }

    uint16_t shnum;
    uint64_t shoff;

    if (elf_file->elf_class == ELFCLASS64){
        shnum = elf_file->header.elf64_header.e_shnum;
        shoff = elf_file->header.elf64_header.e_shoff;
    } else {
        shnum = elf_file->header.elf32_header.e_shnum;
        shoff = elf_file->header.elf32_header.e_shoff;
    }

    printf("There are %d section headers, starting at offset 0x%lx:\n", 
        shnum, shoff);
    fputc('\n', stdout);
    fputs("Section Headers:\n", stdout);
    
}