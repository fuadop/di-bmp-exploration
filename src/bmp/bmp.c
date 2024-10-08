#include "bmp.h"
#include <math.h>
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
}

// free (ptr->pixels) and free(ptr->color_table) [if not NULL] must be cleared
void decode_to_bmf_windows_3(bmf_windows_3_t *ptr, FILE *f) {
	fseek(f, 0, SEEK_SET);

	// read file header
	fread(&ptr->file_header, sizeof(bmfh_t), 1, f);

	// read information header
	fread(&ptr->information_header, sizeof(bmih_windows_3_t), 1, f);

	uint32_t bi_width = ptr->information_header.bi_width;
	uint32_t bi_height = ptr->information_header.bi_height;
	uint32_t px_offset = ptr->file_header.bf_pixels_offset;
	uint32_t bi_clr_used = ptr->information_header.bi_clr_used;
	uint16_t bi_bit_count = ptr->information_header.bi_bit_count;
	uint32_t bi_compression = ptr->information_header.bi_compression;

	ptr->color_table = NULL;

	// BI_RLE8
	if (bi_bit_count == 8 && bi_compression == 1) {
		// read color table
		if (bi_clr_used >= 2) {
			bmf_rgbquad_t *color_table = malloc(sizeof(bmf_rgbquad_t) * bi_clr_used);

			memset(color_table, 0, sizeof(bmf_rgbquad_t) * bi_clr_used);

			fread(color_table, sizeof(bmf_rgbquad_t), bi_clr_used, f);

			ptr->color_table = color_table;
		}

		fseek(f, px_offset, SEEK_SET);

		// decode pixels from RLE8 format
		uint8_t *pixels = malloc(bi_width * bi_height); // 1 byte per pixel

		memset(pixels, 0, bi_width * bi_height);

		// read 2 bytes at a time till end seq (00 01)
		uint8_t seq[2];
		uint32_t x, y = 0;

		while (1) {
			if (fread(seq, 2, 1, f) != 1) {
				puts("error reading from input file");

				break;
			}

			// end of image
			if (seq[0] == 0 && seq[1] == 1) break;

			// end of  row
			if (seq[0] == 0 && seq[1] == 0) {
				x = 0;
				y++;

				continue;
			}

			// increment coordinate dx, dy
			if (seq[0] == 0 && seq[1] == 2) {
				if (fread(seq, 2, 1, f) != 1) {
					puts("error reading from input file");

					break;
				}

				x += seq[0];
				y += seq[1];

				continue;
			}

			// read next n pixels
			if (seq[0] == 0 && seq[1] >= 3) {
				uint8_t *nextaddr = pixels + ((bi_width * y) + x);
				if (fread(nextaddr, seq[1], 1, f) != 1) {
					puts("error reading from input file");

					break;
				}

				x += seq[1];
				continue;
			}

			// repeat pixel n times
			if (seq[0] >= 1) {
				uint8_t *nextaddr = pixels + ((bi_width * y) + x);

				memset(nextaddr, seq[1], seq[0]);

				x += seq[0];

				continue;
			}
		}

		ptr->pixels = pixels;

		return;
	}

	// BI_RLE4
	if (bi_bit_count == 4 && bi_compression == 2) {
		// read color table
		if (bi_clr_used >= 2) {
			bmf_rgbquad_t *color_table = malloc(sizeof(bmf_rgbquad_t) * bi_clr_used);

			memset(color_table, 0, sizeof(bmf_rgbquad_t) * bi_clr_used);

			fread(color_table, sizeof(bmf_rgbquad_t), bi_clr_used, f);

			ptr->color_table = color_table;
		}

		fseek(f, px_offset, SEEK_SET);

		uint8_t *pixels = malloc(bi_width * bi_height); // 1 byte per pixel

		memset(pixels, 0, bi_width * bi_height);

		// read 2 bytes at a time till end seq (00 01)
		uint8_t seq[2];
		uint32_t x, y = 0;

		while (1) {
			if (fread(seq, 2, 1, f) != 1) {
				puts("error reading from input file");

				break;
			}

			// end of image
			if (seq[0] == 0 && seq[1] == 1) break;

			// end of  row
			if (seq[0] == 0 && seq[1] == 0) {
				x = 0;
				y++;

				continue;
			}

			// increment coordinate dx, dy
			if (seq[0] == 0 && seq[1] == 2) {
				if (fread(seq, 2, 1, f) != 1) {
					puts("error reading from input file");

					break;
				}

				x += seq[0];
				y += seq[1];

				continue;
			}

			// read next n pixels
			if (seq[0] == 0 && seq[1] >= 3) {
				uint8_t blen = ceil((double)seq[1] / 2);

				uint8_t buffer[blen + (blen%2)];
				if (fread(buffer, blen+(blen%2), 1, f) != 1) {
					puts("error reading from input file");

					break;
				}

				for (uint8_t i = 0; i < seq[1]; i++) {
					size_t index = (bi_width * y) + x + i;
					uint8_t bindex = (i/2), bpixel = (i % 2);

					uint8_t pixel_pair = buffer[bindex];
					uint8_t pixel = bpixel == 0 ? pixel_pair >> 4 : pixel_pair & 0x0f;

					pixels[index] = pixel;
				}

				x += seq[1];

				continue;
			}

			// repeat pixel n times
			if (seq[0] >= 1) {
				uint8_t pixel_a = seq[1] >> 4;
				uint8_t pixel_b = seq[1] & 0x0f;

				for (uint8_t i = 0; i < seq[0]; i++) {
					size_t index = (bi_width * y) + x + i;

					pixels[index] = i % 2 == 0 ? pixel_a : pixel_b;
				}

				x += seq[0];

				continue;
			}
		}

		ptr->pixels = pixels;

		return;
	}

	// read pixels

	// what is the pixels offset?

	// how many pixel bytes total?
	size_t bytes_per_row = round_to_next_multiple_of_4(
		(bi_width * bi_bit_count) / 8
	);

	size_t total_px_bytes = bytes_per_row * bi_height;

	fseek(f, px_offset, SEEK_SET);

	// allocate bytes on heap for pixels
	uint8_t* px_ptr = malloc(total_px_bytes);

	memset(px_ptr, 0, total_px_bytes);
	fread(px_ptr, total_px_bytes, 1, f);

	ptr->pixels = px_ptr;
}

void terminal_write_headers_bmf_os_2(bmf_os_2_t *ptr) {
	// print headers to console
	printf("file_size=%d\n", ptr->file_header.bf_size);
	printf("file_signature=%c%c\n", ptr->file_header.bf_type[0], ptr->file_header.bf_type[1]);

	printf("width=%d\n", ptr->information_header.bi_width);
	printf("height=%d\n", ptr->information_header.bi_height);
	printf("bits_per_pixel=%d\n", ptr->information_header.bi_bit_count);
}

void terminal_write_headers_bmf_windows_3(bmf_windows_3_t *ptr) {
	// print headers to console
	printf("file_size=%d\n", ptr->file_header.bf_size);
	printf("pixels_offset=%x\n", ptr->file_header.bf_pixels_offset);
	printf("file_signature=%c%c\n", ptr->file_header.bf_type[0], ptr->file_header.bf_type[1]);

	printf("width=%u\n", ptr->information_header.bi_width);
	printf("height=%u\n", ptr->information_header.bi_height);
	printf("bits_per_pixel=%d\n", ptr->information_header.bi_bit_count);
	printf("bi_compression=%d\n", ptr->information_header.bi_compression);
	printf("bi_size_image=%d\n", ptr->information_header.bi_size_image);
}

void terminal_print_bmf_os_2(bmf_os_2_t *ptr) {
	pixel_24_bit_t **matrix = pixel_data_to_matrix_bmf_os_2(ptr);

	if (matrix == NULL) {
		puts("only support bits_per_pixel=24");

		return;
	}

	for (uint16_t y = 0; y < ptr->information_header.bi_height; y++) {
		for (uint16_t x = 0; x < ptr->information_header.bi_width; x++) {
			pixel_24_bit_t *pixel = &matrix[y][x];

			cprint(pixel->red, pixel->green, pixel->blue);
		}

		printf("\n");
	}

	free_matrix(matrix, ptr->information_header.bi_height);
}

void terminal_print_bmf_windows_3(bmf_windows_3_t *ptr) {
	pixel_24_bit_t **matrix = pixel_data_to_matrix_bmf_windows_3(ptr);

	if (matrix == NULL) {
		puts("only support bits_per_pixel=24/8 & bi_compression=BI_RGB/BI_RLEN8");

		return;
	}

	for (uint16_t y = 0; y < ptr->information_header.bi_height; y++) {
		for (uint16_t x = 0; x < ptr->information_header.bi_width; x++) {
			pixel_24_bit_t *pixel = &matrix[y][x];

			cprint(pixel->red, pixel->green, pixel->blue);
		}

		printf("\n");
	}

	if (ptr->color_table != NULL) free(ptr->color_table);
	free_matrix(matrix, ptr->information_header.bi_height);
}

void write_bmp_file(FILE *f, uint16_t width, uint16_t height, pixel_24_bit_t **matrix) {
	bmf_windows_3_t bmf;
	memset(&bmf, 0, sizeof(bmf));

	bmfh_t *fhdr = &bmf.file_header;
	bmih_windows_3_t *ihdr = &bmf.information_header;

	fhdr->bf_type[0] = 'B';
	fhdr->bf_type[1] = 'M';

	// pixels starts immediately after the headers
	fhdr->bf_pixels_offset = sizeof(bmfh_t) + sizeof(bmih_windows_3_t);

	ihdr->bi_size = 40;
	ihdr->bi_width = width;
	ihdr->bi_height = height;
	ihdr->bi_planes = 1;
	ihdr->bi_bit_count = 24;

	bmf.pixels = matrix_to_pixel_data(matrix, height, width);
	size_t bytes_per_row = round_to_next_multiple_of_4(width * sizeof(pixel_24_bit_t));

	fhdr->bf_size = sizeof(bmfh_t) + sizeof(bmih_windows_3_t) + (bytes_per_row * height);

	// write file header
	if (fwrite(fhdr, sizeof(bmfh_t), 1, f) != 1) {
		puts("error writing file");
	}

	// write info header
	if (fwrite(ihdr, sizeof(bmih_windows_3_t), 1, f) != 1) {
		puts("error writing file");
	}

	// write pixel data
	if (fwrite(bmf.pixels, bytes_per_row * height, 1, f) != 1) {
		puts("error writing file");
	}

	free(bmf.pixels);
}

