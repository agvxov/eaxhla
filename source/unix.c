#include "unix.h"

static const uint16_t main_header_size = 0x40;
static const uint16_t text_sector_size = 0x38;
static const uint16_t data_sector_size = 0x38;

static const uint16_t full_header_size = main_header_size
                                       + text_sector_size
                                       + data_sector_size; // 0xb0

static const uint32_t softcore = 0x1000;   // I DON'T KNOW WHAT IS THIS...
static const uint32_t hardcore = 0x400000; // I DON'T KNOW WHAT IS THIS...

uint8_t header_bytes [64] = {
	0X7F, 0X45, 0X4C, 0X46, 0X02, 0X01, 0X01, 0X03,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X02, 0X00, 0X3E, 0X00, 0X01, 0X00, 0X00, 0X00,
	0XB0, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00, // entry_point
	0X40, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X40, 0X00, 0X38, 0X00,
	0X02, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
};

void header (uint8_t  has_program,
             uint8_t  for_linux,
             uint8_t  for_x86_64,
             uint64_t entry_point) {
	/* */
	uint64_t enter = entry_point; // TEST

	header_bytes [16] = (has_program) ? 0x02 : 0x03; // else linkable
	header_bytes [ 7] = (for_linux)   ? 0x03 : 0x00; // else system v
	header_bytes [18] = (for_x86_64)  ? 0x3e : 0x00; // else ?

	if (entry_point != 0) {
		memcpy (& header_bytes [24], & enter, 8);
	}
}
