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

#include <argparse.h>
#include <stdbool.h>
#include <stdio.h>
#include <readelf.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

static bool FLAG_HEADER = false;
static bool FLAG_SECTION_HEADERS = false;
static bool FLAG_SYMBOLS = false;
static bool FLAG_DYNAMIC = false;
static bool FLAG_RELOCS = false;

static const struct argparse_option options[] = {
    OPTION_BOOLEAN('H', "help", "show this help message and exit", 
        NULL, argparse_callback_help, 0),
    OPTION_BOOLEAN('h', "file-header", "Display the ELF header", 
        &FLAG_HEADER, NULL, 0),
    OPTION_BOOLEAN('S', "section-headers", "Display the section headers", 
        &FLAG_SECTION_HEADERS, NULL, 0),
    OPTION_BOOLEAN('s', "symbols", "Display the symbol table", 
        &FLAG_SYMBOLS, NULL, 0),
    OPTION_BOOLEAN('d', "dynamic", "Display the dynamic section", 
        &FLAG_DYNAMIC, NULL, 0),
    OPTION_BOOLEAN('r', "relocs", "Display the relocation entries", 
        &FLAG_RELOCS, NULL, 0),
    OPTION_END()
};

static const struct argparse_description desc = {
    ._description = "Display information about the contents of ELF format files",
    ._epilog = NULL,
    ._usage = "readelf [OPTIONS] ELF_FILE",
    ._program_name = "readelf"
};

int elf_struct_init(elf_file_t *elf_file, const char *filename){
    memset(elf_file, 0, sizeof(elf_file_t));
    elf_file->filename = filename;
    elf_file->fd = -1;
    
    elf_file->fd = open(filename, O_RDONLY);
    if (elf_file->fd == -1) {
        return -1;
    }
    return 0;
}

void elf_struct_cleanup(elf_file_t *elf_file){
    if (elf_file->fd != -1) {
        close(elf_file->fd);
        elf_file->fd = -1;
    }
    if (elf_file->elf_class == ELFCLASS64 && elf_file->section.elf64_shdr) {
        free(elf_file->section.elf64_shdr);
        elf_file->section.elf64_shdr = NULL;
    }
    if (elf_file->elf_class == ELFCLASS32 && elf_file->section.elf32_shdr) {
        free(elf_file->section.elf32_shdr);
        elf_file->section.elf32_shdr = NULL;
    }
    if (elf_file->shstrtab) {
        free(elf_file->shstrtab);
        elf_file->shstrtab = NULL;
    }
}

int readelf(int argc, char *argv[]){
    struct argparse argparse;
    
    argparse_init(&argparse, options, &desc);
    argparse_set_flags(&argparse, ARGPARSE_FLAG_IGNORE_UNKNOWN_OPTION);

    if (argc == 0){
        argparse_callback_help(&argparse, options);
        return EXIT_FAILURE;
    }
    argparse_parse(&argparse, argc, argv);

    if (argparse._argc == 0){
        argparse_callback_help(&argparse, options);
        return EXIT_FAILURE;
    } else {
        elf_file_t elf_file;
        if (elf_struct_init(&elf_file, argparse._argv[0]) == -1) {
            perror("readelf");
            return EXIT_FAILURE;
        }
        /*
         * lazy loading: only parse the necessary sections
         * and if there is no flags like -h, -S, -s, -d, -r, 
         * then don't parse the ELF header
         */
        const bool any = FLAG_HEADER || FLAG_SECTION_HEADERS 
            || FLAG_SYMBOLS || FLAG_DYNAMIC || FLAG_RELOCS;
        
        // parse the dependent sections binary data
        if (any) elf_parse_ehdr(&elf_file);
        if (FLAG_SECTION_HEADERS || FLAG_SYMBOLS) elf_parse_shdr(&elf_file);
        if (FLAG_SECTION_HEADERS) elf_parse_shstrtab(&elf_file);
        if (FLAG_SYMBOLS) elf_parse_symtab(&elf_file);

        // print meta information based on the flags
        if (FLAG_HEADER) elf_print_ehdr(&elf_file);
        if (FLAG_SECTION_HEADERS) elf_print_shdr(&elf_file);
        if (FLAG_SYMBOLS) elf_print_symtab(&elf_file);

        elf_struct_cleanup(&elf_file);
    }
    return EXIT_SUCCESS;
}