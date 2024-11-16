#ifndef UNIX_H
#define UNIX_H

#define ELF_MAIN_HEADER_SIZE (0x40)
#define ELF_TEXT_SECTOR_SIZE (0x38)
#define ELF_DATA_SECTOR_SIZE (0x38)

#define ELF_HEADER_SIZE (ELF_MAIN_HEADER_SIZE + \
                         ELF_TEXT_SECTOR_SIZE + \
                         ELF_DATA_SECTOR_SIZE)

extern char elf_main_header_byte[ELF_MAIN_HEADER_SIZE];
extern char elf_text_sector_byte[ELF_TEXT_SECTOR_SIZE];
extern char elf_data_sector_byte[ELF_DATA_SECTOR_SIZE];

extern void elf_main_header(int entry_point,
                            int has_program,
                            int for_linux,
                            int for_x86_64);

extern void elf_text_sector(int text_size,
                            int data_size);

extern void elf_data_sector(int text_size,
                            int data_size);

#endif
