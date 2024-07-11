#include <stdint.h>
#include <string.h>

extern uint8_t elf_header_bytes [64];

extern void header (uint8_t  has_program,
                    uint8_t  for_linux,
                    uint8_t  for_x86_64,
                    uint64_t entry_point);
