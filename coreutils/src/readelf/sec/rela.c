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

#include <common.h>
#include <readelf.h>

/**
 * @brief: parse the relocation entries from the ELF file
 *
 * @note: not implemented
 * for current version of the readelf, the rela section is not supported
 * Just left the implementation as blank for structure consistency
 */

void elf_parse_rela(_elf_meta *elf_file) {
  if (elf_file->fd == -1)
    exitMsg("readelf: file not open");

  /* Not implemented */
  elf_file->flags |= _ELF_RELA_FLAG;
  return;
}

void elf_print_rela(_elf_meta *elf_file) {
  if (elf_file->fd == -1)
    exitMsg("readelf: file not open");

  /* Not implemented */
}