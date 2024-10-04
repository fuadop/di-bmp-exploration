#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool is_bmp_file(FILE *f) {
	fseek(f, 0, SEEK_SET);

	uint8_t bf_type[2];
	uint8_t expected_bytes[2] = { 'B', 'M' };

	if (fread(bf_type, 2, 1, f) == 1) {
		if (memcmp(bf_type, expected_bytes, sizeof(expected_bytes)) == 0) {
			return true;
		}
	}

	return false;
}

// Gets the BMP Information header size.
// 12 - os/2 version DIB.
// 40 - windows 3 version DIB.
uint32_t get_bmp_bi_size(FILE *f) {
	fseek(f, sizeof(bmfh_t), SEEK_SET);

	uint32_t bi_size;
	fread(&bi_size, sizeof(bi_size), 1, f);

	return bi_size;
}

// free(ptr.pixels) must be called.
void decode_to_bmf_os_2(bmf_os_2_t *ptr, FILE *f) {
	fseek(f, 0, SEEK_SET);

	// read file header
	fread(&ptr->file_header, sizeof(bmfh_t), 1, f);

	// read information header
	fread(&ptr->information_header, sizeof(bmih_os_2_t), 1, f);

	// read pixels

	// what is the pixels offset?
	uint32_t px_offset = ptr->file_header.bf_pixels_offset;

	// how many pixel bytes total?
	size_t bytes_per_row = round_to_next_multiple_of_4(
		(ptr->information_header.bi_width * ptr->information_header.bi_bit_count) / 8
	);

	size_t total_px_bytes = bytes_per_row * ptr->information_header.bi_height;

	fseek(f, px_offset, SEEK_SET);

	// allocate bytes on heap for pixels
	uint8_t* px_ptr = malloc(total_px_bytes);

	memset(px_ptr, 0, total_px_bytes);
	fread(px_ptr, total_px_bytes, 1, f);

	ptr->pixels = px_ptr;
	ptr->pxlen = total_px_bytes;
}

void decode_to_bmf_windows_3(bmf_windows_3_t *ptr, FILE *f) {
	fseek(f, 0, SEEK_SET);

	// read file header
	fread(&ptr->file_header, sizeof(bmfh_t), 1, f);

	// read information header
	fread(&ptr->information_header, sizeof(bmih_windows_3_t), 1, f);

	// read pixels

	// what is the pixels offset?
	uint32_t px_offset = ptr->file_header.bf_pixels_offset;

	// how many pixel bytes total?
	size_t bytes_per_row = round_to_next_multiple_of_4(
		(ptr->information_header.bi_width * ptr->information_header.bi_bit_count) / 8
	);

	size_t total_px_bytes = bytes_per_row * ptr->information_header.bi_height;

	fseek(f, px_offset, SEEK_SET);

	// allocate bytes on heap for pixels
	uint8_t* px_ptr = malloc(total_px_bytes);

	memset(px_ptr, 0, total_px_bytes);
	fread(px_ptr, total_px_bytes, 1, f);

	ptr->pixels = px_ptr;
	ptr->pxlen = total_px_bytes;
}

void terminal_write_headers_bmf_os_2(bmf_os_2_t *ptr) {
	// print headers to console
	printf("file_size=%d\n", ptr->file_header.bf_size);
	printf("file_signature=%c%c\n", ptr->file_header.bf_type[0], ptr->file_header.bf_type[1]);

	printf("width=%d\n", ptr->information_header.bi_width);
	printf("height=%d\n", ptr->information_header.bi_height);
	printf("bits_per_pixel=%d\n", ptr->information_header.bi_bit_count);

	printf("pxlen_bytes=%zu\n", ptr->pxlen);
}

void terminal_write_headers_bmf_windows_3(bmf_windows_3_t *ptr) {
	// print headers to console
	printf("file_size=%d\n", ptr->file_header.bf_size);
	printf("file_signature=%c%c\n", ptr->file_header.bf_type[0], ptr->file_header.bf_type[1]);

	printf("width=%u\n", ptr->information_header.bi_width);
	printf("height=%u\n", ptr->information_header.bi_height);
	printf("bits_per_pixel=%d\n", ptr->information_header.bi_bit_count);
	printf("bi_compression=%d\n", ptr->information_header.bi_compression);
	printf("bi_size_image=%d\n", ptr->information_header.bi_size_image);

	printf("pxlen_bytes=%zu\n", ptr->pxlen);
}


void terminal_print_bmf_os_2(bmf_os_2_t *ptr) {
	puts("bits_per_pixel value of 24 supported only");

	size_t bytes_per_row = round_to_next_multiple_of_4(
		(ptr->information_header.bi_width * ptr->information_header.bi_bit_count) / 8
	);

	if (ptr->information_header.bi_bit_count == 24) {
		for (uint16_t row = ptr->information_header.bi_height - 1; row >= 0; row--) {
			size_t row_offset = ptr->pxlen - 
				((ptr->information_header.bi_height - row) * bytes_per_row);

			for (uint16_t col = 0; col < ptr->information_header.bi_width; col++) {
				size_t color_byte_offset = row_offset + (col * 3);

				uint8_t blue = ptr->pixels[color_byte_offset];
				uint8_t green = ptr->pixels[color_byte_offset + 1];
				uint8_t red = ptr->pixels[color_byte_offset + 2];

				cprint(red, green, blue);
			}

			printf("\n");
			if (row == 0) break;
		}
	}
}

void terminal_print_bmf_windows_3(bmf_windows_3_t *ptr) {
	puts("only support bits_per_pixel=24 & bi_compression=BI_RGB");

	size_t bytes_per_row = round_to_next_multiple_of_4(
		(ptr->information_header.bi_width * ptr->information_header.bi_bit_count) / 8
	);

	if (ptr->information_header.bi_bit_count == 24 && ptr->information_header.bi_compression == 0) {
		for (uint16_t row = ptr->information_header.bi_height - 1; row >= 0; row--) {
			size_t row_offset = ptr->pxlen - 
				((ptr->information_header.bi_height - row) * bytes_per_row);

			for (uint16_t col = 0; col < ptr->information_header.bi_width; col++) {
				size_t color_byte_offset = row_offset + (col * 3);

				uint8_t blue = ptr->pixels[color_byte_offset];
				uint8_t green = ptr->pixels[color_byte_offset + 1];
				uint8_t red = ptr->pixels[color_byte_offset + 2];

				cprint(red, green, blue);
			}

			printf("\n");
			if (row == 0) break;
		}
	}
}

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
