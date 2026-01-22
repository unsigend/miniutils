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
#include <fcntl.h>
#include <readelf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static bool FLAG_HEADER = false;
static bool FLAG_SECTION_HEADERS = false;
static bool FLAG_SYMBOLS = false;
static bool FLAG_DYNAMIC = false;
static bool FLAG_RELOCS = false;
static bool FLAG_ALL = false;
static bool FLAG_PROGRAM_HEADERS = false;
static const char *FLAG_STRING_DUMP = NULL;

static const struct argparse_option options[] = {
    OPTION_BOOLEAN('H', "help", "show this help message and exit", NULL,
                   argparse_callback_help, 0),
    OPTION_BOOLEAN('a', "all", "Display all information", &FLAG_ALL, NULL, 0),
    OPTION_BOOLEAN('h', "file-header", "Display the ELF header", &FLAG_HEADER,
                   NULL, 0),
    OPTION_BOOLEAN('S', "section-headers", "Display the section headers",
                   &FLAG_SECTION_HEADERS, NULL, 0),
    OPTION_BOOLEAN('s', "symbols", "Display the symbol table", &FLAG_SYMBOLS,
                   NULL, 0),
    OPTION_BOOLEAN('d', "dynamic", "Display the dynamic section", &FLAG_DYNAMIC,
                   NULL, 0),
    OPTION_BOOLEAN('r', "relocs", "Display the relocation entries",
                   &FLAG_RELOCS, NULL, 0),
    OPTION_BOOLEAN('l', "program-headers", "Display the program headers",
                   &FLAG_PROGRAM_HEADERS, NULL, 0),
    OPTION_STRING('p', "string-dump", "Dump the contents of section",
                  &FLAG_STRING_DUMP, NULL, 0),
    OPTION_END()};

static const struct argparse_description desc = {
    ._description =
        "Display information about the contents of ELF format files",
    ._epilog = NULL,
    ._usage = "readelf [OPTIONS] ELF_FILE",
    ._program_name = "readelf"};

int elf_struct_init(_elf_meta *elf_file, const char *filename) {
  memset(elf_file, 0, sizeof(_elf_meta));
  elf_file->filename = filename;
  elf_file->fd = -1;

  elf_file->fd = open(filename, O_RDONLY);
  if (elf_file->fd == -1) {
    return -1;
  }
  return 0;
}

void elf_struct_cleanup(_elf_meta *elf_file) {
  if (elf_file->fd != -1) {
    close(elf_file->fd);
    elf_file->fd = -1;
  }
  if (elf_file->elf_class == ELFCLASS64 && elf_file->shdr.shdr64) {
    free(elf_file->shdr.shdr64);
    elf_file->shdr.shdr64 = NULL;
  }
  if (elf_file->elf_class == ELFCLASS32 && elf_file->shdr.shdr32) {
    free(elf_file->shdr.shdr32);
    elf_file->shdr.shdr32 = NULL;
  }
  if (elf_file->shstrtab) {
    free(elf_file->shstrtab);
    elf_file->shstrtab = NULL;
  }
}

int readelf(int argc, char *argv[]) {
  struct argparse argparse;

  argparse_init(&argparse, options, &desc);
  argparse_set_flags(&argparse, ARGPARSE_FLAG_IGNORE_UNKNOWN_OPTION);

  if (argc == 0) {
    argparse_callback_help(&argparse, options);
    return EXIT_FAILURE;
  }
  argparse_parse(&argparse, argc, argv);

  if (argparse._argc == 0) {
    argparse_callback_help(&argparse, options);
    return EXIT_FAILURE;
  } else {
    _elf_meta elf_file;
    if (elf_struct_init(&elf_file, argparse._argv[0]) == -1) {
      perror("readelf");
      return EXIT_FAILURE;
    }
    if (FLAG_ALL) {
      FLAG_HEADER = true;
      FLAG_SYMBOLS = true;
      FLAG_DYNAMIC = true;
      FLAG_RELOCS = true;
      FLAG_SECTION_HEADERS = true;
      FLAG_PROGRAM_HEADERS = true;
    }
    if (FLAG_HEADER) elf_print_ehdr(&elf_file);
    if (FLAG_SECTION_HEADERS) elf_print_shdr(&elf_file);
    if (FLAG_PROGRAM_HEADERS) elf_print_phdr(&elf_file);
    if (FLAG_DYNAMIC) elf_print_dyn(&elf_file);
    if (FLAG_RELOCS) elf_print_rela(&elf_file);
    if (FLAG_SYMBOLS) elf_print_symtab(&elf_file);
    if (FLAG_STRING_DUMP) elf_dump_section(&elf_file, FLAG_STRING_DUMP);

    elf_struct_cleanup(&elf_file);
  }
  return EXIT_SUCCESS;
}