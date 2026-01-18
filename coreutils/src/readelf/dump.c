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
#include <unistd.h>
#include <ctype.h>

#define _PANIC() {do {  \
    perror("readelf");  \
    exit(EXIT_FAILURE); \
} while (0); }

static void dump_sec(_elf_meta *elf_file, const char* name, uint64_t offset, uint64_t size){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }

    if (size == 0) {
        fprintf(stderr, "readelf: Warning: Section '%s' has no data to dump\n", name);
        return;
    }

    char* data = (char*)malloc(size);
    if (!data) _PANIC();
    if (lseek(elf_file->fd, offset, SEEK_SET) == -1) _PANIC();
    if ((uint64_t)read(elf_file->fd, data, size) != size) _PANIC();

    printf("\nString dump of section '%s':\n", name);
    
    // readelf -p : scan the printable strings
    uint32_t off = 0;
    while (off < size) {
        // skip null bytes
        while (off < size && data[off] == '\0') {
            off++;
        }
        if (off >= size) break;
        // printable character
        if (isprint((unsigned char)data[off])) {
            uint32_t start = off;
            while (off < size && data[off] != '\0') {
                off++;
            }
            
            if (off > start) {
                printf("  [%6x]  ", start);
                for (uint32_t i = start; i < off && i < size; i++) {
                    unsigned char c = (unsigned char)data[i];
                    if (c == '\0') break;
                    if (isprint(c)) {
                        putchar(c);
                    }
                    // control character
                    else if (c < 0x20) {
                        putchar('^');
                        putchar(c + 0x40);
                    }
                }
                putchar('\n');
            }
            if (off < size && data[off] == '\0') off++;
            
        } 
        else off++;
    }
    fputc('\n', stdout);

    free(data);
}

void elf_dump_section(_elf_meta *elf_file, const char* sh_name_idx){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }

__DEP_EHDR(elf_file);
__DEP_SHDR(elf_file);
__DEP_SHSTRTAB(elf_file);

    uint16_t shnum;
    if (elf_file->elf_class == ELFCLASS64){
        shnum = elf_file->ehdr.ehdr64.e_shnum;
    } else {
        shnum = elf_file->ehdr.ehdr32.e_shnum;
    }

    // get shstrtab size for bounds checking
    uint16_t shstrndx;
    uint64_t shstrsize = 0;
    if (elf_file->elf_class == ELFCLASS64){
        shstrndx = elf_file->ehdr.ehdr64.e_shstrndx;
        if (elf_file->shstrtab && shstrndx < shnum && elf_file->shdr.shdr64) {
            shstrsize = elf_file->shdr.shdr64[shstrndx].sh_size;
        }
    } else {
        shstrndx = elf_file->ehdr.ehdr32.e_shstrndx;
        if (elf_file->shstrtab && shstrndx < shnum && elf_file->shdr.shdr32) {
            shstrsize = elf_file->shdr.shdr32[shstrndx].sh_size;
        }
    }

    char* endptr = NULL;
    long sh_idx = strtol(sh_name_idx, &endptr, 10);
    const char* sh_name = NULL;
    uint64_t sh_offset = 0;
    uint64_t sh_size = 0;

    // numeric index
    if (endptr && *endptr == '\0'){
        if (sh_idx == 0)
            fprintf(stderr, "Section '' has no data to dump\n");
        else if (sh_idx <= 0 || sh_idx >= shnum)
            fprintf(stderr, "readelf: Warning: Section '%ld' was not dumped because it does not exist\n", sh_idx);
        else {
            if (elf_file->elf_class == ELFCLASS64){
                if (elf_file->shstrtab && elf_file->shdr.shdr64[sh_idx].sh_name < shstrsize) {
                    sh_name = &elf_file->shstrtab[elf_file->shdr.shdr64[sh_idx].sh_name];
                }
                sh_offset = elf_file->shdr.shdr64[sh_idx].sh_offset;
                sh_size = elf_file->shdr.shdr64[sh_idx].sh_size;
            } else {
                if (elf_file->shstrtab && elf_file->shdr.shdr32[sh_idx].sh_name < shstrsize) {
                    sh_name = &elf_file->shstrtab[elf_file->shdr.shdr32[sh_idx].sh_name];
                }
                sh_offset = elf_file->shdr.shdr32[sh_idx].sh_offset;
                sh_size = elf_file->shdr.shdr32[sh_idx].sh_size;
            }
        }
    }
    // section name 
    else {
        for (uint16_t i = 0; i < shnum; i++) {
            const char* current_sh_name = NULL;
            if (elf_file->elf_class == ELFCLASS64){
                if (elf_file->shstrtab && elf_file->shdr.shdr64[i].sh_name < shstrsize) {
                    current_sh_name = &elf_file->shstrtab[elf_file->shdr.shdr64[i].sh_name];
                }
                if (current_sh_name && !strcmp(current_sh_name, sh_name_idx)) {
                    sh_name = current_sh_name;
                    sh_offset = elf_file->shdr.shdr64[i].sh_offset;
                    sh_size = elf_file->shdr.shdr64[i].sh_size;
                    break;
                }
            } else {
                if (elf_file->shstrtab && elf_file->shdr.shdr32[i].sh_name < shstrsize) {
                    current_sh_name = &elf_file->shstrtab[elf_file->shdr.shdr32[i].sh_name];
                }
                if (current_sh_name && !strcmp(current_sh_name, sh_name_idx)) {
                    sh_name = current_sh_name;
                    sh_offset = elf_file->shdr.shdr32[i].sh_offset;
                    sh_size = elf_file->shdr.shdr32[i].sh_size;
                    break;
                }
            }
        }
    }

    if (!sh_name || sh_offset == 0) {
        fprintf(stderr, "readelf: Warning: Section '%s' was not dumped because it does not exist\n", 
                sh_name ? sh_name : sh_name_idx);
        return;
    }

    dump_sec(elf_file, sh_name, sh_offset, sh_size);
}