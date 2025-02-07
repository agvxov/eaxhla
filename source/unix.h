#ifndef UNIX_H
#define UNIX_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define ELF_MAIN_HEADER_SIZE (0x40)
#define ELF_TEXT_SECTOR_SIZE (0x38)
#define ELF_DATA_SECTOR_SIZE (0x38)

#define ELF_HEADER_SIZE (ELF_MAIN_HEADER_SIZE + \
                         ELF_TEXT_SECTOR_SIZE + \
                         ELF_DATA_SECTOR_SIZE)

extern uint8_t elf_main_header_byte[ELF_MAIN_HEADER_SIZE];
extern uint8_t elf_text_sector_byte[ELF_TEXT_SECTOR_SIZE];
extern uint8_t elf_data_sector_byte[ELF_DATA_SECTOR_SIZE];

extern void elf_main_header(uint32_t entry_point,
                            bool     has_program,
                            bool     for_linux,
                            bool     for_x86_64);

extern void elf_text_sector(uint32_t text_size,
                            uint32_t data_size);

extern void elf_data_sector(uint32_t text_size,
                            uint32_t data_size);

#endif
