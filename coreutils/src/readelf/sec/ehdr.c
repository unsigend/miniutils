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

static const char *get_osabi_name(unsigned char osabi) {
  switch (osabi) {
  case ELFOSABI_NONE:
    return "UNIX - System V";
  case ELFOSABI_HPUX:
    return "HP-UX";
  case ELFOSABI_NETBSD:
    return "NetBSD";
  case ELFOSABI_GNU:
    return "GNU";
  case ELFOSABI_SOLARIS:
    return "Solaris";
  case ELFOSABI_AIX:
    return "AIX";
  case ELFOSABI_IRIX:
    return "IRIX";
  case ELFOSABI_FREEBSD:
    return "FreeBSD";
  case ELFOSABI_TRU64:
    return "Tru64";
  case ELFOSABI_MODESTO:
    return "Novell Modesto";
  case ELFOSABI_OPENBSD:
    return "OpenBSD";
  case ELFOSABI_ARM_AEABI:
    return "ARM EABI";
  case ELFOSABI_ARM:
    return "ARM";
  case ELFOSABI_STANDALONE:
    return "Standalone";
  }
  return "Unknown";
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

static const char *get_machine_name(unsigned machine) {
  /**
   * The machine type string is from GNU binutils
   */
  switch (machine) {
  case EM_NONE:
    return "None";
  case EM_M32:
    return "WE32100";
  case EM_SPARC:
    return "Sparc";
  case EM_386:
    return "Intel 80386";
  case EM_68K:
    return "MC68000";
  case EM_88K:
    return "MC88000";
  case EM_IAMCU:
    return "Intel MCU";
  case EM_860:
    return "Intel 80860";
  case EM_MIPS:
    return "MIPS R3000";
  case EM_S370:
    return "IBM System/370";
  case EM_MIPS_RS3_LE:
    return "MIPS R4000 big-endian";
  case EM_SPARCV9:
    return "Sparc v9";
  case EM_PARISC:
    return "HPPA";
  case EM_VPP500:
    return "Fujitsu VPP500";
  case EM_SPARC32PLUS:
    return "Sparc v8+";
  case EM_960:
    return "Intel 80960";
  case EM_PPC:
    return "PowerPC";
  case EM_PPC64:
    return "PowerPC64";
  case EM_S390:
    return "IBM S/390";
  case EM_SPU:
    return "SPU";
  case EM_V800:
    return "Renesas V850 (using RH850 ABI)";
  case EM_FR20:
    return "Fujitsu FR20";
  case EM_RH32:
    return "TRW RH32";
  case EM_ARM:
    return "ARM";
  case EM_ALPHA:
    return "Digital Alpha";
  case EM_SH:
    return "Renesas / SuperH SH";
  case EM_TRICORE:
    return "Siemens Tricore";
  case EM_ARC:
    return "ARC";
  case EM_H8_300:
    return "Renesas H8/300";
  case EM_H8_300H:
    return "Renesas H8/300H";
  case EM_H8S:
    return "Renesas H8S";
  case EM_H8_500:
    return "Renesas H8/500";
  case EM_IA_64:
    return "Intel IA-64";
  case EM_MIPS_X:
    return "Stanford MIPS-X";
  case EM_COLDFIRE:
    return "Motorola Coldfire";
  case EM_68HC12:
    return "Motorola MC68HC12 Microcontroller";
  case EM_MMA:
    return "Fujitsu Multimedia Accelerator";
  case EM_PCP:
    return "Siemens PCP";
  case EM_NCPU:
    return "Sony nCPU embedded RISC processor";
  case EM_NDR1:
    return "Denso NDR1 microprocessor";
  case EM_STARCORE:
    return "Motorola Star*Core processor";
  case EM_ME16:
    return "Toyota ME16 processor";
  case EM_ST100:
    return "STMicroelectronics ST100 processor";
  case EM_TINYJ:
    return "Advanced Logic Corp. TinyJ embedded processor";
  case EM_X86_64:
    return "Advanced Micro Devices X86-64";
  case EM_PDSP:
    return "Sony DSP processor";
  case EM_PDP10:
    return "Digital Equipment Corp. PDP-10";
  case EM_PDP11:
    return "Digital Equipment Corp. PDP-11";
  case EM_FX66:
    return "Siemens FX66 microcontroller";
  case EM_ST9PLUS:
    return "STMicroelectronics ST9+ 8/16 bit microcontroller";
  case EM_ST7:
    return "STMicroelectronics ST7 8-bit microcontroller";
  case EM_68HC16:
    return "Motorola MC68HC16 Microcontroller";
  case EM_68HC11:
    return "Motorola MC68HC11 Microcontroller";
  case EM_68HC08:
    return "Motorola MC68HC08 Microcontroller";
  case EM_68HC05:
    return "Motorola MC68HC05 Microcontroller";
  case EM_SVX:
    return "Silicon Graphics SVx";
  case EM_ST19:
    return "STMicroelectronics ST19 8-bit microcontroller";
  case EM_VAX:
    return "Digital VAX";
  case EM_CRIS:
    return "Axis Communications 32-bit embedded processor";
  case EM_JAVELIN:
    return "Infineon Technologies 32-bit embedded cpu";
  case EM_FIREPATH:
    return "Element 14 64-bit DSP processor";
  case EM_ZSP:
    return "LSI Logic's 16-bit DSP processor";
  case EM_MMIX:
    return "Donald Knuth's educational 64-bit processor";
  case EM_HUANY:
    return "Harvard Universitys's machine-independent object format";
  case EM_PRISM:
    return "Vitesse Prism";
  case EM_AVR:
    return "Atmel AVR 8-bit microcontroller";
  case EM_FR30:
    return "Fujitsu FR30";
  case EM_D10V:
    return "d10v";
  case EM_D30V:
    return "d30v";
  case EM_V850:
    return "Renesas V850";
  case EM_M32R:
    return "Renesas M32R (formerly Mitsubishi M32r)";
  case EM_MN10300:
    return "mn10300";
  case EM_MN10200:
    return "mn10200";
  case EM_PJ:
    return "picoJava";
  case EM_OPENRISC:
    return "OpenRISC 1000";
  case EM_ARC_COMPACT:
    return "ARCompact";
  case EM_XTENSA:
    return "Tensilica Xtensa Processor";
  case EM_VIDEOCORE:
    return "Alphamosaic VideoCore processor";
  case EM_TMM_GPP:
    return "Thompson Multimedia General Purpose Processor";
  case EM_NS32K:
    return "National Semiconductor 32000 series";
  case EM_TPC:
    return "Tenor Network TPC processor";
  case EM_SNP1K:
    return "Trebia SNP 1000 processor";
  case EM_ST200:
    return "STMicroelectronics ST200 microcontroller";
  case EM_IP2K:
    return "Ubicom IP2xxx 8-bit microcontrollers";
  case EM_MAX:
    return "MAX Processor";
  case EM_CR:
    return "National Semiconductor CompactRISC";
  case EM_F2MC16:
    return "Fujitsu F2MC16";
  case EM_MSP430:
    return "Texas Instruments msp430 microcontroller";
  case EM_BLACKFIN:
    return "Analog Devices Blackfin";
  case EM_SE_C33:
    return "S1C33 Family of Seiko Epson processors";
  case EM_SEP:
    return "Sharp embedded microprocessor";
  case EM_ARCA:
    return "Arca RISC microprocessor";
  case EM_UNICORE:
    return "Unicore";
  case EM_EXCESS:
    return "eXcess 16/32/64-bit configurable embedded CPU";
  case EM_DXP:
    return "Icera Semiconductor Inc. Deep Execution Processor";
  case EM_ALTERA_NIOS2:
    return "Altera Nios II";
  case EM_CRX:
    return "National Semiconductor CRX microprocessor";
  case EM_XGATE:
    return "Motorola XGATE embedded processor";
  case EM_C166:
  case EM_M16C:
    return "Renesas M16C series microprocessors";
  case EM_DSPIC30F:
    return "Microchip Technology dsPIC30F Digital Signal Controller";
  case EM_CE:
    return "Freescale Communication Engine RISC core";
  case EM_M32C:
    return "Renesas M32c";
  case EM_TSK3000:
    return "Altium TSK3000 core";
  case EM_RS08:
    return "Freescale RS08 embedded processor";
  case EM_ECOG2:
    return "Cyan Technology eCOG2 microprocessor";
  case EM_DSP24:
    return "New Japan Radio (NJR) 24-bit DSP Processor";
  case EM_VIDEOCORE3:
    return "Broadcom VideoCore III processor";
  case EM_LATTICEMICO32:
    return "Lattice Mico32";
  case EM_SE_C17:
    return "Seiko Epson C17 family";
  case EM_TI_C6000:
    return "Texas Instruments TMS320C6000 DSP family";
  case EM_TI_C2000:
    return "Texas Instruments TMS320C2000 DSP family";
  case EM_TI_C5500:
    return "Texas Instruments TMS320C55x DSP family";
  case EM_TI_PRU:
    return "TI PRU I/O processor";
  case EM_MMDSP_PLUS:
    return "STMicroelectronics 64bit VLIW Data Signal Processor";
  case EM_CYPRESS_M8C:
    return "Cypress M8C microprocessor";
  case EM_R32C:
    return "Renesas R32C series microprocessors";
  case EM_TRIMEDIA:
    return "NXP Semiconductors TriMedia architecture family";
  case EM_QDSP6:
    return "QUALCOMM DSP6 Processor";
  case EM_8051:
    return "Intel 8051 and variants";
  case EM_STXP7X:
    return "STMicroelectronics STxP7x family";
  case EM_NDS32:
    return "Andes Technology compact code size embedded RISC processor "
           "family";
  case EM_ECOG1X:
    return "Cyan Technology eCOG1X family";
  case EM_MAXQ30:
    return "Dallas Semiconductor MAXQ30 Core microcontrollers";
  case EM_XIMO16:
    return "New Japan Radio (NJR) 16-bit DSP Processor";
  case EM_MANIK:
    return "M2000 Reconfigurable RISC Microprocessor";
  case EM_CRAYNV2:
    return "Cray Inc. NV2 vector architecture";
  case EM_RX:
    return "Renesas RX";
  case EM_METAG:
    return "Imagination Technologies Meta processor architecture";
  case EM_MCST_ELBRUS:
    return "MCST Elbrus general purpose hardware architecture";
  case EM_ECOG16:
    return "Cyan Technology eCOG16 family";
  case EM_CR16:
  case EM_MICROBLAZE:
  case EM_ETPU:
    return "Freescale Extended Time Processing Unit";
  case EM_SLE9X:
    return "Infineon Technologies SLE9X core";
  case EM_AARCH64:
    return "AArch64";
  case EM_AVR32:
    return "Atmel Corporation 32-bit microprocessor";
  case EM_STM8:
    return "STMicroeletronics STM8 8-bit microcontroller";
  case EM_TILE64:
    return "Tilera TILE64 multicore architecture family";
  case EM_TILEPRO:
    return "Tilera TILEPro multicore architecture family";
  case EM_CUDA:
    return "NVIDIA CUDA architecture";
  case EM_TILEGX:
    return "Tilera TILE-Gx multicore architecture family";
  case EM_CLOUDSHIELD:
    return "CloudShield architecture family";
  case EM_COREA_1ST:
    return "KIPO-KAIST Core-A 1st generation processor family";
  case EM_COREA_2ND:
    return "KIPO-KAIST Core-A 2nd generation processor family";
  case EM_OPEN8:
    return "Open8 8-bit RISC soft processor core";
  case EM_RL78:
    return "Renesas RL78";
  case EM_VIDEOCORE5:
    return "Broadcom VideoCore V processor";
  case EM_56800EX:
    return "Freescale 56800EX Digital Signal Controller (DSC)";
  case EM_BA1:
    return "Beyond BA1 CPU architecture";
  case EM_BA2:
    return "Beyond BA2 CPU architecture";
  case EM_XCORE:
    return "XMOS xCORE processor family";
  case EM_MCHP_PIC:
    return "Microchip 8-bit PIC(r) family";
  case EM_INTELGT:
    return "Intel Graphics Technology";
  case EM_KM32:
    return "KM211 KM32 32-bit processor";
  case EM_KMX32:
    return "KM211 KMX32 32-bit processor";
  case EM_CDP:
    return "Paneve CDP architecture family";
  case EM_NORC:
    return "Nanoradio Optimized RISC";
  case EM_CSR_KALIMBA:
    return "CSR Kalimba architecture family";
  case EM_Z80:
    return "Zilog Z80";
  case EM_VISIUM:
    return "CDS VISIUMcore processor";
  case EM_FT32:
    return "FTDI Chip FT32";
  case EM_MOXIE:
    return "Moxie";
  case EM_AMDGPU:
    return "AMD GPU";
  case EM_RISCV:
    return "RISC-V";
  case EM_BPF:
    return "Linux BPF";
  case EM_CSKY:
    return "C-SKY";
  case EM_LOONGARCH:
    return "LoongArch";
  default:
    return "Unknown";
  }
}

void elf_parse_ehdr(_elf_meta *elf_file) {
  if (elf_file->fd == -1)
    exitMsg("readelf: file not open");

  __GUARD_EHDR(elf_file);

  unsigned char e_ident[EI_NIDENT];
  if (read(elf_file->fd, e_ident, EI_NIDENT) != EI_NIDENT)
    exitErrno("readelf");

  if (strncmp((char *)e_ident, ELFMAG, SELFMAG) != 0)
    exitMsg("Not an ELF file - it has the wrong magic bytes at the start");

  if (e_ident[EI_CLASS] == ELFCLASS64) {
    elf_file->elf_class = ELFCLASS64;
  } else if (e_ident[EI_CLASS] == ELFCLASS32) {
    elf_file->elf_class = ELFCLASS32;
  } else
    exitMsg("Not an ELF file - it has the wrong class");

  if (lseek(elf_file->fd, 0, SEEK_SET) == -1)
    exitErrno("readelf");

  if (elf_file->elf_class == ELFCLASS64) {
    if (read(elf_file->fd, &elf_file->ehdr.ehdr64, sizeof(Elf64_Ehdr)) !=
        sizeof(Elf64_Ehdr))
      exitErrno("readelf");
  } else if (elf_file->elf_class == ELFCLASS32) {
    if (read(elf_file->fd, &elf_file->ehdr.ehdr32, sizeof(Elf32_Ehdr)) !=
        sizeof(Elf32_Ehdr))
      exitErrno("readelf");
  }
  elf_file->flags |= _ELF_EHRD_FLAG;
}

void elf_print_ehdr(_elf_meta *elf_file) {
  __DEP_EHDR(elf_file);

  unsigned char *e_ident;
  unsigned e_type, e_machine, e_version, e_flags;
  uint64_t e_entry, e_phoff, e_shoff;
  uint16_t e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;

  /**
   * using a compatiable way to get the values of the ELF header
   */
  if (elf_file->elf_class == ELFCLASS64) {
    e_ident = elf_file->ehdr.ehdr64.e_ident;
    e_type = elf_file->ehdr.ehdr64.e_type;
    e_machine = elf_file->ehdr.ehdr64.e_machine;
    e_version = elf_file->ehdr.ehdr64.e_version;
    e_entry = elf_file->ehdr.ehdr64.e_entry;
    e_phoff = elf_file->ehdr.ehdr64.e_phoff;
    e_shoff = elf_file->ehdr.ehdr64.e_shoff;
    e_flags = elf_file->ehdr.ehdr64.e_flags;
    e_ehsize = elf_file->ehdr.ehdr64.e_ehsize;
    e_phentsize = elf_file->ehdr.ehdr64.e_phentsize;
    e_phnum = elf_file->ehdr.ehdr64.e_phnum;
    e_shentsize = elf_file->ehdr.ehdr64.e_shentsize;
    e_shnum = elf_file->ehdr.ehdr64.e_shnum;
    e_shstrndx = elf_file->ehdr.ehdr64.e_shstrndx;
  } else {
    e_ident = elf_file->ehdr.ehdr32.e_ident;
    e_type = elf_file->ehdr.ehdr32.e_type;
    e_machine = elf_file->ehdr.ehdr32.e_machine;
    e_version = elf_file->ehdr.ehdr32.e_version;
    e_entry = elf_file->ehdr.ehdr32.e_entry;
    e_phoff = elf_file->ehdr.ehdr32.e_phoff;
    e_shoff = elf_file->ehdr.ehdr32.e_shoff;
    e_flags = elf_file->ehdr.ehdr32.e_flags;
    e_ehsize = elf_file->ehdr.ehdr32.e_ehsize;
    e_phentsize = elf_file->ehdr.ehdr32.e_phentsize;
    e_phnum = elf_file->ehdr.ehdr32.e_phnum;
    e_shentsize = elf_file->ehdr.ehdr32.e_shentsize;
    e_shnum = elf_file->ehdr.ehdr32.e_shnum;
    e_shstrndx = elf_file->ehdr.ehdr32.e_shstrndx;
  }

  fprintf(stdout, "ELF Header:\n");
  fprintf(stdout, "  Magic:   ");
  for (int i = 0; i < EI_NIDENT; i++) {
    fprintf(stdout, "%02x ", e_ident[i]);
  }
  fprintf(stdout, "\n");

  fprintf(stdout, "  %-35s%s\n",
          "Class:", elf_file->elf_class == ELFCLASS64 ? "ELF64" : "ELF32");
  fprintf(stdout, "  %-35s%s\n", "Data:",
          e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian"
                                          : "2's complement, big endian");
  fprintf(stdout, "  %-35s%d (current)\n", "Version:", e_version);
  fprintf(stdout, "  %-35s%s\n", "OS/ABI:", get_osabi_name(e_ident[EI_OSABI]));
  fprintf(stdout, "  %-35s%d\n", "ABI Version:", e_ident[EI_ABIVERSION]);
  fprintf(stdout, "  %-35s%s\n", "Type:", get_elf_type(e_type));
  fprintf(stdout, "  %-35s%s\n", "Machine:", get_machine_name(e_machine));
  fprintf(stdout, "  %-35s0x%x\n", "Version:", e_version);

  if (elf_file->elf_class == ELFCLASS64) {
    fprintf(stdout, "  %-35s0x%lx\n", "Entry point address:", e_entry);
    fprintf(stdout, "  %-35s%lu (bytes into file)\n",
            "Start of program headers:", e_phoff);
    fprintf(stdout, "  %-35s%lu (bytes into file)\n",
            "Start of section headers:", e_shoff);
    fprintf(stdout, "  %-35s0x%x\n", "Flags:", e_flags);
    fprintf(stdout, "  %-35s%d (bytes)\n", "Size of this header:", e_ehsize);
    fprintf(stdout, "  %-35s%d (bytes)\n",
            "Size of program headers:", e_phentsize);
    fprintf(stdout, "  %-35s%d\n", "Number of program headers:", e_phnum);
    fprintf(stdout, "  %-35s%d (bytes)\n",
            "Size of section headers:", e_shentsize);
    fprintf(stdout, "  %-35s%d\n", "Number of section headers:", e_shnum);
    fprintf(stdout, "  %-35s%d\n",
            "Section header string table index:", e_shstrndx);
  } else {
    fprintf(stdout, "  %-35s0x%x\n", "Entry point address:", (unsigned)e_entry);
    fprintf(stdout, "  %-35s%u (bytes into file)\n",
            "Start of program headers:", (unsigned)e_phoff);
    fprintf(stdout, "  %-35s%u (bytes into file)\n",
            "Start of section headers:", (unsigned)e_shoff);
    fprintf(stdout, "  %-35s0x%x\n", "Flags:", e_flags);
    fprintf(stdout, "  %-35s%d (bytes)\n", "Size of this header:", e_ehsize);
    fprintf(stdout, "  %-35s%d (bytes)\n",
            "Size of program headers:", e_phentsize);
    fprintf(stdout, "  %-35s%d\n", "Number of program headers:", e_phnum);
    fprintf(stdout, "  %-35s%d (bytes)\n",
            "Size of section headers:", e_shentsize);
    fprintf(stdout, "  %-35s%d\n", "Number of section headers:", e_shnum);
    fprintf(stdout, "  %-35s%d\n",
            "Section header string table index:", e_shstrndx);
  }
  putc('\n', stdout);
}