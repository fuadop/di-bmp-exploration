#ifndef BMP_H
#define BMP_H

#include "psf.h"

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
	uint32_t bf_size; // entire size of file
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
	uint32_t bi_x_pels_per_meter; // 0
	uint32_t bi_y_pels_per_meter; // 0
	uint32_t bi_clr_used; // number of colors used
	uint32_t bi_clr_important; // number of important colors
} bmih_windows_3_t;

/** FILE STRUCTURES */

// file structure for os/2 bmf variant
typedef struct {
	bmfh_t file_header;
	bmih_os_2_t information_header;
	uint8_t *pixels;
} bmf_os_2_t;

// file structure for windows 3 bmf variant
typedef struct {
	bmfh_t file_header;
	bmih_windows_3_t information_header;
	// todo: color table
	uint8_t *pixels;
} bmf_windows_3_t;

/** UTILITY STRUCTURES */
typedef struct {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} pixel_24_bit_t;

typedef struct {
	uint16_t x;
	uint16_t y;
} coordinate_t;

/** DI-BMP METHODS */
bool is_bmp_file(FILE *f);
uint32_t get_bmp_bi_size(FILE *f);

void decode_to_bmf_os_2(bmf_os_2_t *ptr, FILE *f);
void decode_to_bmf_windows_3(bmf_windows_3_t *ptr, FILE *f);

void terminal_print_bmf_os_2(bmf_os_2_t *ptr);
void terminal_print_bmf_windows_3(bmf_windows_3_t *ptr);

void terminal_write_headers_bmf_os_2(bmf_os_2_t *ptr);
void terminal_write_headers_bmf_windows_3(bmf_windows_3_t *ptr);

void write_bmp_file(
	FILE *f,
	uint16_t width,
	uint16_t height,
	pixel_24_bit_t **matrix
);

/** MATRIX METHODS */
pixel_24_bit_t** pixel_data_to_matrix_bmf_os_2(bmf_os_2_t *ptr);
pixel_24_bit_t** pixel_data_to_matrix_bmf_windows_3(bmf_windows_3_t *ptr);

uint8_t* matrix_to_pixel_data(pixel_24_bit_t **matrix, uint16_t height, uint16_t width);

void matrix_fill(pixel_24_bit_t **matrix, pixel_24_bit_t pixel, uint16_t height, uint16_t width);

void free_matrix(pixel_24_bit_t **matrix, uint16_t height);
pixel_24_bit_t** malloc_matrix(uint16_t height, uint16_t width);

void matrix_reflect_x_axis(pixel_24_bit_t **matrix, uint16_t height);
void matrix_reflect_y_axis(pixel_24_bit_t **matrix, uint16_t height, uint16_t width);

// void matrix_rotate_forward(deg);
// void matrix_rotate_backward(deg);

void fill_n_rows_in_col(
	pixel_24_bit_t **matrix,
	pixel_24_bit_t pixel,
	uint16_t col,
	uint16_t offset,
	uint16_t n
);

void fill_n_cols_in_row(
	pixel_24_bit_t **matrix,
	pixel_24_bit_t pixel,
	uint16_t row,
	uint16_t offset,
	uint16_t n
);

void linear_gradient_left_to_right(
	pixel_24_bit_t **matrix,
	uint16_t height,
	uint16_t startcol,
	uint16_t endcol
);

void linear_gradient_top_to_bottom(
	pixel_24_bit_t **matrix,
	uint16_t width,
	uint16_t startrow,
	uint16_t endrow
);

pixel_24_bit_t rgblerp(pixel_24_bit_t a, pixel_24_bit_t b, double fraction);

void fill_coordinates(
	pixel_24_bit_t **matrix,
	coordinate_t* coords,
	uint16_t coordslen,
	pixel_24_bit_t pixel
);

void scanline_polygon_fill(
	pixel_24_bit_t **matrix,
	uint16_t height,
	coordinate_t* polygon,
	uint16_t polygonlen,
	pixel_24_bit_t pixel
);

/** VECTORS */
coordinate_t* draw_line(coordinate_t a, coordinate_t b);
coordinate_t* draw_triangle(coordinate_t a, coordinate_t b, coordinate_t c);

coordinate_t * draw_circle(coordinate_t c, uint16_t r);
coordinate_t * draw_circle_bresenham(coordinate_t c, uint16_t r);

uint16_t circle_circumference(uint16_t r);
uint16_t linelen(coordinate_t a, coordinate_t b);
uint16_t triangle_perimiter(coordinate_t a, coordinate_t b, coordinate_t c);

coordinate_t clerp(coordinate_t a, coordinate_t b, double t);
coordinate_t* quadbezier(coordinate_t a, coordinate_t b, coordinate_t c);

/** GLYPHS */
coordinate_t* write_glyph_to_coordinate(
	coordinate_t c,
	uint8_t glyph,
	psf_t *psf
);

/** UTILITY METHODS */
size_t round_to_next_multiple_of_4(size_t n);
void cprint(uint8_t red, uint8_t green, uint8_t blue);

#endif
