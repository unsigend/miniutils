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
#include <string.h>

#define PADDING " "
#define __VIS(vis) (vis == STV_DEFAULT ? "DEFAULT" :    \
                  vis == STV_INTERNAL ? "INTERNAL" :    \
                  vis == STV_HIDDEN ? "HIDDEN" :        \
                  vis == STV_PROTECTED ? "PROTECTED" :  \
                  "UNKNOWN")
#define __BIND(bind) (bind == STB_LOCAL ? "LOCAL" :     \
                  bind == STB_GLOBAL ? "GLOBAL" :       \
                  bind == STB_WEAK ? "WEAK" :           \
                  "UNKNOWN")
#define __TYPE(type) (type == STT_NOTYPE ? "NOTYPE" :   \
                  type == STT_OBJECT ? "OBJECT" :       \
                  type == STT_FUNC ? "FUNC" :           \
                  type == STT_SECTION ? "SECTION" :     \
                  type == STT_FILE ? "FILE" :           \
                  "UNKNOWN")
#define __NDX(ndx) (ndx == STN_UNDEF ? "UND" :          \
                  ndx == STN_ABS ? "ABS" :              \
                  ndx == STN_COMMON ? "COMM" :          \
                  ndx == STN_TLS ? "TLS" :              \
                  "UND")

#define _PANIC() {do {  \
    perror("readelf");  \
    exit(EXIT_FAILURE); \
} while (0); }

void elf_parse_symtab(_elf_meta *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }
__GUARD_SYMTAB(elf_file);
__DEP_EHDR(elf_file);
__DEP_SHDR(elf_file);
__DEP_SHSTRTAB(elf_file);

    uint64_t symoff = 0;
    uint64_t symsize = 0;

    if (elf_file->elf_class == ELFCLASS64){
        for (uint16_t i = 1; i < elf_file->ehdr.ehdr64.e_shnum; i++) {
            if (elf_file->shdr.shdr64[i].sh_type == SHT_SYMTAB && 
              !strcmp(elf_file->shstrtab + elf_file->shdr.shdr64[i].sh_name, ".symtab")) {
                symoff = elf_file->shdr.shdr64[i].sh_offset;
                symsize = elf_file->shdr.shdr64[i].sh_size;
                break;
            }
        }
    } else {
        for (uint16_t i = 1; i < elf_file->ehdr.ehdr32.e_shnum; i++) {
            if (elf_file->shdr.shdr32[i].sh_type == SHT_SYMTAB && 
              !strcmp(elf_file->shstrtab + elf_file->shdr.shdr32[i].sh_name, ".symtab")) {
                symoff = elf_file->shdr.shdr32[i].sh_offset;
                symsize = elf_file->shdr.shdr32[i].sh_size;
                break;
            }
        }
    }

    if (symoff == 0 || symsize == 0) {
        elf_file->flags |= _ELF_SYMTAB_FLAG;
        return;
    }

    if (elf_file->elf_class == ELFCLASS64){
        elf_file->symtab.symtab64 = (Elf64_Sym*)malloc(symsize);
        if (!elf_file->symtab.symtab64) _PANIC();
    } else {
        elf_file->symtab.symtab32 = (Elf32_Sym*)malloc(symsize);
        if (!elf_file->symtab.symtab32) _PANIC();
    }

    if (lseek(elf_file->fd, symoff, SEEK_SET) == -1) _PANIC();
    if (elf_file->elf_class == ELFCLASS64){
        ssize_t nb = read(elf_file->fd, elf_file->symtab.symtab64, symsize);
        if (nb == -1 || (size_t)nb != symsize) _PANIC();
    } else {
        ssize_t nb = read(elf_file->fd, elf_file->symtab.symtab32, symsize);
        if (nb == -1 || (size_t)nb != symsize) _PANIC();
    }

    elf_file->symtab_sz = symsize;
    elf_file->flags |= _ELF_SYMTAB_FLAG;
}

static const char* get_section_index_str(uint16_t ndx) {
    if (ndx == SHN_UNDEF) return "UND";
    if (ndx == SHN_ABS) return "ABS";
    if (ndx == SHN_COMMON) return "COMM";
    return NULL;
}

void elf_print_symtab(_elf_meta *elf_file){
    if (elf_file->fd == -1) {
        perror("readelf: file not open");
        exit(EXIT_FAILURE);
    }
__DEP_SYMTAB(elf_file);
__DEP_STRTAB(elf_file);

    if (!elf_file->symtab.symtab64 && !elf_file->symtab.symtab32) {
        return;
    }

    uint64_t symentnum = 0;
    if (elf_file->elf_class == ELFCLASS64){
        symentnum = elf_file->symtab_sz / sizeof(Elf64_Sym);
    } else {
        symentnum = elf_file->symtab_sz / sizeof(Elf32_Sym);
    }
    
    printf("\nSymbol table '.symtab' contains %lu entries:\n", symentnum);
    printf("   Num:    Value          Size Type    Bind   Vis      Ndx Name\n");
    
    for (uint64_t i = 0; i < symentnum; i++) {
        uint64_t st_value;
        uint64_t st_size;
        uint8_t st_info;
        uint8_t st_other;
        uint16_t st_shndx;
        uint32_t st_name;
        const char* name = "";
        
        if (elf_file->elf_class == ELFCLASS64) {
            Elf64_Sym* sym = &elf_file->symtab.symtab64[i];
            st_value = sym->st_value;
            st_size = sym->st_size;
            st_info = sym->st_info;
            st_other = sym->st_other;
            st_shndx = sym->st_shndx;
            st_name = sym->st_name;
        } else {
            Elf32_Sym* sym = &elf_file->symtab.symtab32[i];
            st_value = sym->st_value;
            st_size = sym->st_size;
            st_info = sym->st_info;
            st_other = sym->st_other;
            st_shndx = sym->st_shndx;
            st_name = sym->st_name;
        }
        
        uint8_t st_bind, st_type, st_vis;
        if (elf_file->elf_class == ELFCLASS64) {
            st_bind = ELF64_ST_BIND(st_info);
            st_type = ELF64_ST_TYPE(st_info);
            st_vis = ELF64_ST_VISIBILITY(st_other);
        } else {
            st_bind = ELF32_ST_BIND(st_info);
            st_type = ELF32_ST_TYPE(st_info);
            st_vis = ELF32_ST_VISIBILITY(st_other);
        }
        
        if (st_type == STT_SECTION && st_shndx != SHN_UNDEF && st_shndx != SHN_ABS) {
            if (elf_file->shstrtab) {
                uint16_t shnum;
                if (elf_file->elf_class == ELFCLASS64) {
                    shnum = elf_file->ehdr.ehdr64.e_shnum;
                } else {
                    shnum = elf_file->ehdr.ehdr32.e_shnum;
                }
                if (st_shndx < shnum) {
                    uint32_t sh_name;
                    if (elf_file->elf_class == ELFCLASS64) {
                        sh_name = elf_file->shdr.shdr64[st_shndx].sh_name;
                    } else {
                        sh_name = elf_file->shdr.shdr32[st_shndx].sh_name;
                    }
                    if (elf_file->shstrtab[sh_name]) {
                        name = &elf_file->shstrtab[sh_name];
                    }
                }
            }
        } else if (elf_file->strtab && st_name < elf_file->strtab_sz) {
            name = &elf_file->strtab[st_name];
        }
        
        const char* ndx_str = get_section_index_str(st_shndx);
        
        printf("%6lu: %016lx %5lu %-7s %-6s %-7s", 
            i, st_value, st_size, __TYPE(st_type), __BIND(st_bind), __VIS(st_vis));
        
        if (ndx_str) printf(" %4s", ndx_str);
        else printf(" %4u", st_shndx);
        
        if (name && *name) {
            size_t name_len = strlen(name);
            if (name_len > 21) printf(" %.16s[...]", name);
            else printf(" %s", name);
        } else printf(" ");
        printf("\n");
    }
    putc('\n', stdout);
}