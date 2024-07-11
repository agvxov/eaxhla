#include <stdint.h>
#include <string.h>

#define MAIN_HEADER_SIZE (0x40)
#define TEXT_SECTOR_SIZE (0x38)
#define DATA_SECTOR_SIZE (0x38)

#define HEADER_SIZE (MAIN_HEADER_SIZE + TEXT_SECTOR_SIZE + DATA_SECTOR_SIZE)

extern uint8_t main_header_byte [MAIN_HEADER_SIZE];
extern uint8_t text_sector_byte [TEXT_SECTOR_SIZE];
extern uint8_t data_sector_byte [DATA_SECTOR_SIZE];

extern void main_header (uint8_t  has_program,
                         uint8_t  for_linux,
                         uint8_t  for_x86_64,
                         uint64_t entry_point);

extern void text_sector (uint64_t text_size);

extern void data_sector (uint64_t text_size,
                         uint64_t data_size);
