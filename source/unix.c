#include "unix.h"

static const uint32_t softcore = 0x1000;   // I DON'T KNOW WHAT IS THIS...
static const uint32_t hardcore = 0x400000; // I DON'T KNOW WHAT IS THIS...

uint8_t main_header_byte [MAIN_HEADER_SIZE] = {
	0X7F, 0X45, 0X4C, 0X46, 0X02, 0X01, 0X01, 0X03,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X02, 0X00, 0X3E, 0X00, 0X01, 0X00, 0X00, 0X00,
	0XB0, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00, // entry_point
	0X40, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X40, 0X00, 0X38, 0X00,
	0X02, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
};

uint8_t text_sector_byte [TEXT_SECTOR_SIZE]; = {
	0X01, 0X00, 0X00, 0X00, 0X05, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, // HDR + TXT
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, // HDR + TXT
	0X00, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};

uint8_t data_sector_byte [DATA_SECTOR_SIZE] = {
	0X01, 0X00, 0X00, 0X00, 0X06, 0X00, 0X00, 0X00,
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, // HDR + TXT
	0X00, 0X10, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00, // +++ ^
	0X00, 0X10, 0X40, 0X00, 0X00, 0X00, 0X00, 0X00, // +++ ^
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, // DAT
	0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, // DAT
	0X00, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};

void main_header (uint8_t  has_program,
                  uint8_t  for_linux,
                  uint8_t  for_x86_64,
                  uint64_t entry_point) {
	/* */
	uint64_t enter = entry_point; // TEST

	main_header_bytes [16] = (has_program) ? 0x02 : 0x03; // else linkable
	main_header_bytes [ 7] = (for_linux)   ? 0x03 : 0x00; // else system v
	main_header_bytes [18] = (for_x86_64)  ? 0x3e : 0x00; // else ?

	if (entry_point != 0) {
		memcpy (& main_header_bytes [24], & enter, sizeof (enter));
	}
}

void text_sector (uint64_t text_size) {
	/* */
	uint64_t text = text_size; // TEST

	memcpy (& text_sector_byte [32], & text, sizeof (text));
	memcpy (& text_sector_byte [40], & text, sizeof (text));
}

void data_sector (uint64_t text_size,
                  uint64_t data_size) {
	/* */
	uint64_t data = data_size; // TEST
	uint64_t main = HEADER_SIZE + text_size;
	uint64_t move = 0x401000 + main;

	memcpy (& data_sector_byte [ 8], & main, sizeof (main));
	memcpy (& data_sector_byte [16], & move, sizeof (move));
	memcpy (& data_sector_byte [24], & move, sizeof (move));
	memcpy (& data_sector_byte [32], & data, sizeof (data));
	memcpy (& data_sector_byte [40], & data, sizeof (data));
}
