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

#define INDEX_STR_ALIGN 4
#define NAME_STR_ALIGN 16
#define TYPE_STR_ALIGN 16
#define ADDRESS_STR_ALIGN 16
#define OFFSET_STR_ALIGN 8
#define SIZE_STR_ALIGN 16
#define ENT_SIZE_STR_ALIGN 16
#define FLAGS_STR_ALIGN 5
#define LINK_STR_ALIGN 4
#define INFO_STR_ALIGN 5
#define ALIGN_STR_ALIGN 4

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

static const char *get_section_type_name(uint32_t type) {
    switch (type) {
        case SHT_NULL: return "NULL";
        case SHT_PROGBITS: return "PROGBITS";
        case SHT_SYMTAB: return "SYMTAB";
        case SHT_STRTAB: return "STRTAB";
        case SHT_RELA: return "RELA";
        case SHT_HASH: return "HASH";
        case SHT_DYNAMIC: return "DYNAMIC";
        case SHT_NOTE: return "NOTE";
        case SHT_NOBITS: return "NOBITS";
        case SHT_REL: return "REL";
        case SHT_SHLIB: return "SHLIB";
        case SHT_DYNSYM: return "DYNSYM";
        case SHT_INIT_ARRAY: return "INIT_ARRAY";
        case SHT_FINI_ARRAY: return "FINI_ARRAY";
        case SHT_PREINIT_ARRAY: return "PREINIT_ARRAY";
        case SHT_GROUP: return "GROUP";
        case SHT_SYMTAB_SHNDX: return "SYMTAB_SHNDX";
        default:
            if (type >= SHT_LOOS && type <= SHT_HIOS) return "OS";
            if (type >= SHT_LOPROC && type <= SHT_HIPROC) return "PROC";
            return "UNKNOWN";
    }
}

static void format_section_flags(char *buf, size_t bufsize, uint32_t flags) {
    size_t pos = 0;
    buf[0] = '\0';

    if (flags & SHF_WRITE) pos += snprintf(buf + pos, bufsize - pos, "W");
    if (flags & SHF_ALLOC) pos += snprintf(buf + pos, bufsize - pos, "A");
    if (flags & SHF_EXECINSTR) pos += snprintf(buf + pos, bufsize - pos, "X");
    if (flags & SHF_MERGE) pos += snprintf(buf + pos, bufsize - pos, "M");
    if (flags & SHF_STRINGS) pos += snprintf(buf + pos, bufsize - pos, "S");
    if (flags & SHF_INFO_LINK) pos += snprintf(buf + pos, bufsize - pos, "I");
    if (flags & SHF_LINK_ORDER) pos += snprintf(buf + pos, bufsize - pos, "L");
    if (flags & SHF_OS_NONCONFORMING) pos += snprintf(buf + pos, bufsize - pos, "O");
    if (flags & SHF_GROUP) pos += snprintf(buf + pos, bufsize - pos, "G");
    if (flags & SHF_TLS) pos += snprintf(buf + pos, bufsize - pos, "T");
    if (flags & SHF_COMPRESSED) pos += snprintf(buf + pos, bufsize - pos, "C");

    if (pos == 0) snprintf(buf, bufsize, "   ");
}

static void format_section_name(char *buf, size_t bufsize, const char *name) {
    size_t len = strlen(name);
    if (len <= NAME_STR_ALIGN) {
        snprintf(buf, bufsize, "%-*s", (int)NAME_STR_ALIGN, name);
    } else {
        snprintf(buf, bufsize, "%.12s[...]", name);
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

    printf("  [Nr] %-*s  %-*s  %-*s  %-*s\n", 
        NAME_STR_ALIGN, "Name", TYPE_STR_ALIGN, "Type", 
        ADDRESS_STR_ALIGN, "Address", OFFSET_STR_ALIGN, "Offset");
    printf("       %-*s  %-*s  %-*s  %-*s  %-*s  %-*s\n", 
        SIZE_STR_ALIGN, "Size", ENT_SIZE_STR_ALIGN, "EntSize", 
        FLAGS_STR_ALIGN, "Flags", LINK_STR_ALIGN, "Link", 
        INFO_STR_ALIGN, "Info", ALIGN_STR_ALIGN, "Align");

    if (!elf_file->section.elf64_shdr 
        && !elf_file->section.elf32_shdr) return;

    uint16_t shstrndx;
    uint64_t shstrsize = 0;
    if (elf_file->elf_class == ELFCLASS64){
        shstrndx = elf_file->header.elf64_header.e_shstrndx;
        if (elf_file->shstrtab && shstrndx < shnum && 
            elf_file->section.elf64_shdr) {
            shstrsize = elf_file->section.elf64_shdr[shstrndx].sh_size;
        }
    } else {
        shstrndx = elf_file->header.elf32_header.e_shstrndx;
        if (elf_file->shstrtab && shstrndx < shnum && 
            elf_file->section.elf32_shdr) {
            shstrsize = elf_file->section.elf32_shdr[shstrndx].sh_size;
        }
    }

    for (uint16_t i = 0; i < shnum; i++) {
        const char *name = "";
        uint32_t sh_type;
        uint64_t sh_addr, sh_offset, sh_size, sh_entsize, sh_addralign;
        uint32_t sh_flags;
        uint32_t sh_link, sh_info;

        if (elf_file->elf_class == ELFCLASS64) {
            Elf64_Shdr *shdr = &elf_file->section.elf64_shdr[i];
            if (elf_file->shstrtab && shdr->sh_name < shstrsize) {
                name = &elf_file->shstrtab[shdr->sh_name];
            }
            sh_type = shdr->sh_type;
            sh_addr = shdr->sh_addr;
            sh_offset = shdr->sh_offset;
            sh_size = shdr->sh_size;
            sh_entsize = shdr->sh_entsize;
            sh_flags = shdr->sh_flags;
            sh_link = shdr->sh_link;
            sh_info = shdr->sh_info;
            sh_addralign = shdr->sh_addralign;
        } else {
            Elf32_Shdr *shdr = &elf_file->section.elf32_shdr[i];
            if (elf_file->shstrtab && shdr->sh_name < shstrsize) {
                name = &elf_file->shstrtab[shdr->sh_name];
            }
            sh_type = shdr->sh_type;
            sh_addr = shdr->sh_addr;
            sh_offset = shdr->sh_offset;
            sh_size = shdr->sh_size;
            sh_entsize = shdr->sh_entsize;
            sh_flags = shdr->sh_flags;
            sh_link = shdr->sh_link;
            sh_info = shdr->sh_info;
            sh_addralign = shdr->sh_addralign;
        }

        const char *type_str = get_section_type_name(sh_type);
        char flags_str[32];
        char name_buf[32];
        format_section_flags(flags_str, sizeof(flags_str), sh_flags);
        format_section_name(name_buf, sizeof(name_buf), name);

        if (elf_file->elf_class == ELFCLASS64) {
            printf("  [%2u] %s  %-*s  %016lx  %08lx\n", 
                i, name_buf, TYPE_STR_ALIGN, type_str, sh_addr, sh_offset);
            printf("       %016lx  %016lx  %-*s  %*u  %*u  %*lu\n",
                sh_size, sh_entsize, 
                FLAGS_STR_ALIGN, flags_str,
                LINK_STR_ALIGN, sh_link, 
                INFO_STR_ALIGN, sh_info,
                ALIGN_STR_ALIGN, sh_addralign);
        } else {
            printf("  [%2u] %s  %-*s  %08x  %08lx\n", 
                i, name_buf, TYPE_STR_ALIGN, type_str, (unsigned)sh_addr, sh_offset);
            printf("       %08lx  %08lx  %-*s  %*u  %*u  %*u\n",
                sh_size, sh_entsize, 
                FLAGS_STR_ALIGN, flags_str, 
                LINK_STR_ALIGN, sh_link, 
                INFO_STR_ALIGN, sh_info, 
                ALIGN_STR_ALIGN, (unsigned)sh_addralign);
        }
    }
}