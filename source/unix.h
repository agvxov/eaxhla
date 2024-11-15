#ifndef UNIX_H
#define UNIX_H

#define ELF_MAIN_HEADER_SIZE (0x40)
#define ELF_TEXT_SECTOR_SIZE (0x38)
#define ELF_DATA_SECTOR_SIZE (0x38)

#define ELF_HEADER_SIZE (ELF_MAIN_HEADER_SIZE + \
                         ELF_TEXT_SECTOR_SIZE + \
                         ELF_DATA_SECTOR_SIZE)

extern unsigned char elf_main_header_byte[ELF_MAIN_HEADER_SIZE];
extern unsigned char elf_text_sector_byte[ELF_TEXT_SECTOR_SIZE];
extern unsigned char elf_data_sector_byte[ELF_DATA_SECTOR_SIZE];

extern void elf_main_header(unsigned char has_program,
                            unsigned char for_linux,
                            unsigned char for_x86_64);

extern void elf_text_sector(unsigned long text_size,
                            unsigned long data_size);

extern void elf_data_sector(unsigned long text_size,
                            unsigned long data_size);

#endif
