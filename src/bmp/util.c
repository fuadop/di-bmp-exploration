#include "bmp.h"

void cprint(uint8_t red, uint8_t green, uint8_t blue) {
  printf("\033[48;2;%d;%d;%dm \033[0m", red, green, blue);
}

size_t round_to_next_multiple_of_4(size_t n) {
	if ((n % 4) != 0) {
		size_t next_multiple = (n / 4) + 1;

		return 4 * next_multiple;
	}

	return n;
}
