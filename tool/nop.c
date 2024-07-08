#include <xolatile/xtandard.c>

int main (int argc, char * * argv) {
	int file   = -1;
	int size   = 0;
	int offset = 0;

	unsigned char * buffer = NULL;

	file = file_open (argv [1], O_RDONLY);
	size = file_size (argv [1]);

	buffer = allocate (size);

	file_read (file, buffer, size);

	file = file_close (file);

	offset = 64 + 2 * 56 + 3;

	do {
		int byte = (int) buffer [offset];

		if (byte == 0X90) {
			echo      ("\n");
			echo_byte ((int) buffer [offset]);
		} else {
			echo_byte (buffer [offset]);
		}

		++offset;
	} while (offset != size);

	echo ("\n");

	buffer = deallocate (buffer);

	return (EXIT_SUCCESS);
}
