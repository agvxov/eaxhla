#include "unix.h"

#include <string.h>

uint8_t elf_main_header_byte[ELF_MAIN_HEADER_SIZE] = {
    0X7F, 0X45, 0X4C, 0X46, 0X02, 0X01, 0X01, 0X03,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X02, 0X00, 0X3E, 0X00, 0X01, 0X00, 0X00, 0X00,
    0XB0, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X40, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X40, 0X00, 0X38, 0X00,
    0X02, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00
};

uint8_t elf_text_sector_byte[ELF_TEXT_SECTOR_SIZE] = {
    0X01, 0X00, 0X00, 0X00, 0X05, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00
};

uint8_t elf_data_sector_byte[ELF_DATA_SECTOR_SIZE] = {
    0X01, 0X00, 0X00, 0X00, 0X06, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X10, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X10, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00
};

void elf_main_header(uint32_t entry_point,
                     uint8_t  has_program,
                     uint8_t  for_linux,
                     uint8_t  for_x86_64) {
    uint32_t enter = entry_point + 0x4000b0u;

    elf_main_header_byte[16] = (has_program) ? 0x02 : 0x03;
    elf_main_header_byte[ 7] = (for_linux)   ? 0x03 : 0x00;
    elf_main_header_byte[18] = (for_x86_64)  ? 0x3e : 0x00;

    memcpy(&elf_main_header_byte[24], &enter, sizeof(enter));
}

void elf_text_sector(uint64_t text_size,
                     uint64_t data_size) {
    unsigned long text = ELF_HEADER_SIZE + text_size - data_size;

    memcpy(&elf_text_sector_byte[32], &text, sizeof(text));
    memcpy(&elf_text_sector_byte[40], &text, sizeof(text));
}

void elf_data_sector(uint64_t text_size,
                     uint64_t data_size) {
    uint64_t data = data_size;
    uint64_t core = ELF_HEADER_SIZE + text_size - data_size;
    uint64_t move = 0x401000ul + core;

    memcpy(&elf_data_sector_byte[ 8], &core, sizeof(core));
    memcpy(&elf_data_sector_byte[16], &move, sizeof(move));
    memcpy(&elf_data_sector_byte[24], &move, sizeof(move));
    memcpy(&elf_data_sector_byte[32], &data, sizeof(data));
    memcpy(&elf_data_sector_byte[40], &data, sizeof(data));
}
