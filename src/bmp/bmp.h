#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#pragma pack(1)

// 4 bytes
#define BMIH_SIZE 4

/** FILE HEADER */

// file header - same for all versions of bmf
typedef struct {
	uint8_t bf_type[2]; // file signature "BM"
	uint32_t bf_size; // entire file of size
	uint16_t bf_reserved_1; // 0
	uint16_t bf_reserved_2; // 0
	uint32_t bf_pixels_offset; // offset where the actual pixels start
} bmfh_t;

/** INFORMATION HEADERS */

// information header for os/2 bmf variant
typedef struct {
	uint32_t bi_size; // size of the information header 12 | 40
	uint16_t bi_width; // number of pixels per rows
	uint16_t bi_height; // number of rows
	uint16_t bi_planes; // 1
	uint16_t bi_bit_count; // bits per pixel (1, 4, 8, 24)
} bmih_os_2_t;

// information header for windows 3 bmf variant
typedef struct {
	uint32_t bi_size; // size of the information header 12 | 40
	uint32_t bi_width; // number of pixels per rows
	uint32_t bi_height; // number of rows
	uint16_t bi_planes; // 1
	uint16_t bi_bit_count; // bits per pixel (1, 4, 8, 16, 24, 32)
	uint32_t bi_compression; // one of (BI_RGB, BI_RLE8,)
	uint32_t bi_size_image; // zero for BI_RGB
	uint64_t bi_x_pels_per_meter; // 0
	uint64_t bi_y_pels_per_meter; // 0
	uint32_t bi_clr_used; // number of colors used
	uint32_t bi_clr_important; // number of important colors
} bmih_windows_3_t;

/** FILE STRUCTURES */

// file structure for os/2 bmf variant
typedef struct {
	bmfh_t file_header;
	bmih_os_2_t information_header;
	size_t pxlen; // size of pixels buffer
	uint8_t *pixels;
} bmf_os_2_t;

// file structure for windows 3 bmf variant
typedef struct {
	bmfh_t file_header;
	bmih_windows_3_t information_header;
	// todo: color table
	size_t pxlen; // size of pixels buffer
	uint8_t *pixels;
} bmf_windows_3_t;

bool is_bmp_file(FILE *f);
uint32_t get_bmp_bi_size(FILE *f);

void decode_to_bmf_os_2(bmf_os_2_t *ptr, FILE *f);
void decode_to_bmf_windows_3(bmf_windows_3_t *ptr, FILE *f);

void terminal_print_bmf_os_2(bmf_os_2_t *ptr);
void terminal_print_bmf_windows_3(bmf_windows_3_t *ptr);

void terminal_write_headers_bmf_os_2(bmf_os_2_t *ptr);
void terminal_write_headers_bmf_windows_3(bmf_windows_3_t *ptr);

size_t round_to_next_multiple_of_4(size_t n);
void cprint(uint8_t red, uint8_t green, uint8_t blue);

#endif
