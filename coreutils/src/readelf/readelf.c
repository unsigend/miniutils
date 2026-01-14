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
#include <stdlib.h>
#include <stdnoreturn.h>
#include <readelf.h>

static bool FLAG_HEADER = false;
static bool FLAG_SECTION_HEADERS = false;
static bool FLAG_SYMBOLS = false;
static bool FLAG_DYNAMIC = false;
static bool FLAG_RELOCS = false;

static const struct argparse_option options[] = {
    OPTION_BOOLEAN('H', "help", "show this help message and exit", NULL, argparse_callback_help, 0),
    OPTION_BOOLEAN('h', "file-header", "Display the ELF header", &FLAG_HEADER, NULL, 0),
    OPTION_BOOLEAN('S', "section-headers", "Display the section headers", &FLAG_SECTION_HEADERS, NULL, 0),
    OPTION_BOOLEAN('s', "symbols", "Display the symbol table", &FLAG_SYMBOLS, NULL, 0),
    OPTION_BOOLEAN('d', "dynamic", "Display the dynamic section", &FLAG_DYNAMIC, NULL, 0),
    OPTION_BOOLEAN('r', "relocs", "Display the relocation entries", &FLAG_RELOCS, NULL, 0),
    OPTION_END()
};

static const struct argparse_description desc = {
    ._description = "Display information about the contents of ELF format files",
    ._epilog = NULL,
    ._usage = "readelf [OPTIONS] ELF_FILE",
    ._program_name = "readelf"
};


int readelf(int argc, char *argv[]){
    struct argparse argparse;
    
    argparse_init(&argparse, options, &desc);
    argparse_set_flags(&argparse, ARGPARSE_FLAG_IGNORE_UNKNOWN_OPTION);

    if (argc == 0){
        argparse_callback_help(&argparse, options);
        return 0;
    }
    argparse_parse(&argparse, argc, argv);

    if (argparse._argc == 0){
        argparse_callback_help(&argparse, options);
        return 0;
    } else {
        elf_file_t elf_file;
        elf_file.filename = argparse._argv[0];
        parse_header(&elf_file);

        if (FLAG_HEADER){
            print_header(&elf_file);
        }
    }
    return 0;
}