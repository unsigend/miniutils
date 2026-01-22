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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _PANIC()          \
  {                       \
    do {                  \
      perror("readelf");  \
      exit(EXIT_FAILURE); \
    } while (0);          \
  }

static const char *get_elf_type(unsigned type) {
  switch (type) {
    case ET_NONE:
      return "NONE (No file type)";
    case ET_REL:
      return "REL (Relocatable file)";
    case ET_EXEC:
      return "EXEC (Executable file)";
    case ET_DYN:
      return "DYN (Shared object file)";
    case ET_CORE:
      return "CORE (Core file)";
    case ET_LOOS:
      return "LOOS (Operating system-specific)";
    case ET_HIOS:
      return "HIOS (Operating system-specific)";
    case ET_LOPROC:
      return "LOPROC (Processor-specific)";
    case ET_HIPROC:
      return "HIPROC (Processor-specific)";
  }
  return "Unknown";
}

void elf_parse_phdr(_elf_meta *elf_file) {
  if (elf_file->fd == -1) {
    perror("readelf: file not open");
    exit(EXIT_FAILURE);
  }
  __GUARD_PHDR(elf_file);
  __DEP_EHDR(elf_file);

  uint64_t phoff;
  uint16_t phnum;
  uint64_t phsize;

  if (elf_file->elf_class == ELFCLASS64) {
    phoff = elf_file->ehdr.ehdr64.e_phoff;
    phnum = elf_file->ehdr.ehdr64.e_phnum;
    phsize = elf_file->ehdr.ehdr64.e_phentsize;
  } else {
    phoff = elf_file->ehdr.ehdr32.e_phoff;
    phnum = elf_file->ehdr.ehdr32.e_phnum;
    phsize = elf_file->ehdr.ehdr32.e_phentsize;
  }

  if (phnum == 0 && phsize == 0) {
    if (elf_file->elf_class == ELFCLASS64)
      elf_file->phdr.phdr64 = NULL;
    else
      elf_file->phdr.phdr32 = NULL;

    elf_file->flags |= _ELF_PHDR_FLAG;
    return;
  }

  if (elf_file->elf_class == ELFCLASS64) {
    elf_file->phdr.phdr64 = (Elf64_Phdr *)malloc(phsize * phnum);
    if (!elf_file->phdr.phdr64) _PANIC();
  } else {
    elf_file->phdr.phdr32 = (Elf32_Phdr *)malloc(phsize * phnum);
    if (!elf_file->phdr.phdr32) _PANIC();
  }

  if (lseek(elf_file->fd, phoff, SEEK_SET) == -1) _PANIC();

  if (elf_file->elf_class == ELFCLASS64) {
    ssize_t nbytes = read(elf_file->fd, elf_file->phdr.phdr64, phsize * phnum);
    if (nbytes == -1 || (size_t)nbytes != phsize * phnum) _PANIC();
  } else {
    ssize_t nbytes = read(elf_file->fd, elf_file->phdr.phdr32, phsize * phnum);
    if (nbytes == -1 || (size_t)nbytes != phsize * phnum) _PANIC();
  }

  elf_file->flags |= _ELF_PHDR_FLAG;
}

static const char *get_phdr_type_name(uint32_t type) {
  switch (type) {
    case PT_NULL:
      return "NULL";
    case PT_LOAD:
      return "LOAD";
    case PT_DYNAMIC:
      return "DYNAMIC";
    case PT_INTERP:
      return "INTERP";
    case PT_NOTE:
      return "NOTE";
    case PT_SHLIB:
      return "SHLIB";
    case PT_PHDR:
      return "PHDR";
    case PT_TLS:
      return "TLS";
    case PT_LOOS:
      return "LOOS";
    case PT_HIOS:
      return "HIOS";
    case PT_LOPROC:
      return "LOPROC";
    case PT_HIPROC:
      return "HIPROC";
    case PT_GNU_EH_FRAME:
      return "GNU_EH_FRAME";
    case PT_GNU_STACK:
      return "GNU_STACK";
    case PT_GNU_RELRO:
      return "GNU_RELRO";
    case PT_GNU_PROPERTY:
      return "GNU_PROPERTY";
    default:
      if (type >= PT_LOOS && type <= PT_HIOS) return "OS";
      if (type >= PT_LOPROC && type <= PT_HIPROC) return "PROC";
      return "UNKNOWN";
  }
}

#define PERM_READ "R"
#define PERM_WRITE "W"
#define PERM_EXEC "E"
static void format_phdr_flags(char *buf, size_t size, uint32_t flags) {
  buf[0] = '\0';
  if (flags & PF_R) strncat(buf, PERM_READ, size - strlen(buf) - 1);
  if (flags & PF_W) strncat(buf, PERM_WRITE, size - strlen(buf) - 1);
  if (flags & PF_X) {
    if (buf[0] != '\0') strncat(buf, " ", size - strlen(buf) - 1);
    strncat(buf, PERM_EXEC, size - strlen(buf) - 1);
  }
  if (buf[0] == '\0') strncpy(buf, "  ", size - 1);
}

void elf_print_phdr(_elf_meta *elf_file) {
  if (elf_file->fd == -1) {
    perror("readelf: file not open");
    exit(EXIT_FAILURE);
  }
  __DEP_EHDR(elf_file);
  __DEP_PHDR(elf_file);

  putc('\n', stdout);
  if (!elf_file->phdr.phdr64 && !elf_file->phdr.phdr32) {
    puts("There are no program headers in this file.\n");
    return;
  }
  uint16_t e_type;
  if (elf_file->elf_class == ELFCLASS64)
    e_type = elf_file->ehdr.ehdr64.e_type;
  else
    e_type = elf_file->ehdr.ehdr32.e_type;

  printf("Elf file type is %s\n", get_elf_type(e_type));
  printf("Entry point 0x%lx\n", elf_file->elf_class == ELFCLASS64
                                    ? elf_file->ehdr.ehdr64.e_entry
                                    : (uint64_t)elf_file->ehdr.ehdr32.e_entry);
  printf("There are %d program headers, starting at offset %d\n",
         elf_file->elf_class == ELFCLASS64
             ? elf_file->ehdr.ehdr64.e_phnum
             : (unsigned)elf_file->ehdr.ehdr32.e_phnum,
         elf_file->elf_class == ELFCLASS64
             ? (unsigned)elf_file->ehdr.ehdr64.e_phoff
             : (unsigned)elf_file->ehdr.ehdr32.e_phoff);

  putc('\n', stdout);
  puts("Program Headers:");
  printf("  Type           Offset             VirtAddr           PhysAddr\n");
  printf(
      "                 FileSiz            MemSiz              Flags  "
      "Align\n");

  uint16_t phnum;
  if (elf_file->elf_class == ELFCLASS64) {
    phnum = elf_file->ehdr.ehdr64.e_phnum;
  } else {
    phnum = elf_file->ehdr.ehdr32.e_phnum;
  }

  // print program headers
  for (uint16_t i = 0; i < phnum; i++) {
    uint32_t p_type;
    uint64_t p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_align;
    uint32_t p_flags;

    if (elf_file->elf_class == ELFCLASS64) {
      Elf64_Phdr *phdr = &elf_file->phdr.phdr64[i];
      p_type = phdr->p_type;
      p_offset = phdr->p_offset;
      p_vaddr = phdr->p_vaddr;
      p_paddr = phdr->p_paddr;
      p_filesz = phdr->p_filesz;
      p_memsz = phdr->p_memsz;
      p_flags = phdr->p_flags;
      p_align = phdr->p_align;
    } else {
      Elf32_Phdr *phdr = &elf_file->phdr.phdr32[i];
      p_type = phdr->p_type;
      p_offset = phdr->p_offset;
      p_vaddr = phdr->p_vaddr;
      p_paddr = phdr->p_paddr;
      p_filesz = phdr->p_filesz;
      p_memsz = phdr->p_memsz;
      p_flags = phdr->p_flags;
      p_align = phdr->p_align;
    }

    const char *type_name = get_phdr_type_name(p_type);
    char flags_str[4];
    format_phdr_flags(flags_str, sizeof(flags_str), p_flags);

    printf("  %-14s 0x%016lx 0x%016lx 0x%016lx\n", type_name, p_offset, p_vaddr,
           p_paddr);
    printf("                 0x%016lx 0x%016lx %-6s 0x%lx\n", p_filesz, p_memsz,
           flags_str, p_align);
  }
  putc('\n', stdout);
}