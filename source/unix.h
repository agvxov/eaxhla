#ifndef UNIX_H
#define UNIX_H

#include <stdint.h>

#define ELF_MAIN_HEADER_SIZE (0x40ul)
#define ELF_TEXT_SECTOR_SIZE (0x38ul)
#define ELF_DATA_SECTOR_SIZE (0x38ul)

#define ELF_HEADER_SIZE (ELF_MAIN_HEADER_SIZE + \
                         ELF_TEXT_SECTOR_SIZE + \
                         ELF_DATA_SECTOR_SIZE)

extern uint8_t elf_main_header_byte[ELF_MAIN_HEADER_SIZE];
extern uint8_t elf_text_sector_byte[ELF_TEXT_SECTOR_SIZE];
extern uint8_t elf_data_sector_byte[ELF_DATA_SECTOR_SIZE];

extern void elf_main_header(uint32_t entry_point,
                            uint8_t  has_program,
                            uint8_t  for_linux,
                            uint8_t  for_x86_64);

extern void elf_text_sector(uint64_t text_size,
                            uint64_t data_size);

extern void elf_data_sector(uint64_t text_size,
                            uint64_t data_size);

#endif
