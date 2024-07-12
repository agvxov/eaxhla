#ifndef UNIX_H

#include <stdint.h>
#include <string.h>

#define ELF_MAIN_HEADER_SIZE (0x40)
#define ELF_TEXT_SECTOR_SIZE (0x38)
#define ELF_DATA_SECTOR_SIZE (0x38)

#define ELF_HEADER_SIZE (ELF_MAIN_HEADER_SIZE \
                       + ELF_TEXT_SECTOR_SIZE \
                       + ELF_DATA_SECTOR_SIZE)

extern uint8_t elf_main_header_byte [ELF_MAIN_HEADER_SIZE];
extern uint8_t elf_text_sector_byte [ELF_TEXT_SECTOR_SIZE];
extern uint8_t elf_data_sector_byte [ELF_DATA_SECTOR_SIZE];

extern void elf_main_header (uint8_t  has_program,
                             uint8_t  for_linux,
                             uint8_t  for_x86_64,
                             uint64_t entry_point);

extern void elf_text_sector (uint64_t text_size);

extern void elf_data_sector (uint64_t text_size,
                             uint64_t data_size);

#define UNIX_H
#endif
