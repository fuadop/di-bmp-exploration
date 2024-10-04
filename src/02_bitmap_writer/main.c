#include "bmp/bmp.h"
#include <stdlib.h>
#include <string.h>

void draw_black_bitmap(uint16_t width, uint16_t height);
void draw_gradient_bitmap(uint16_t width, uint16_t height);

int main() {
	draw_black_bitmap(10, 10);
	draw_gradient_bitmap(300, 300);

	return 0;
}

void draw_black_bitmap(uint16_t width, uint16_t height) {
	pixel_24_bit_t **matrix = malloc_matrix(height, width);

	// fill with black pixels
	pixel_24_bit_t pixel;
	memset(&pixel, 0 ,sizeof(pixel_24_bit_t));
	matrix_fill(matrix, pixel, height, width);

	FILE *f = fopen("./assets/10_x_10_black_square.bmp", "wb");

	write_bmp_file(f, width, height, matrix);

	fclose(f);
	free_matrix(matrix, height);
}

void draw_gradient_bitmap(uint16_t width, uint16_t height) {
	pixel_24_bit_t **matrix = malloc_matrix(height, width);

	// draw gradient pixels
	pixel_24_bit_t pixel_1; // black (top-left)
	pixel_24_bit_t pixel_2; // red (top-right)
	pixel_24_bit_t pixel_3; // blue (bottom-left)
	pixel_24_bit_t pixel_4; // black (bottom-right)
	memset(&pixel_1, 0 ,sizeof(pixel_24_bit_t));
	memset(&pixel_2, 0 ,sizeof(pixel_24_bit_t));
	memset(&pixel_3, 0 ,sizeof(pixel_24_bit_t));
	memset(&pixel_4, 0 ,sizeof(pixel_24_bit_t));

	pixel_2.red = 255;
	pixel_3.blue = 255;

	// top-left
	fill_n_rows_in_col(
		matrix,
		pixel_1,
		0,
		0,
		height/2
	);

	// bottom-left
	fill_n_rows_in_col(
		matrix,
		pixel_3,
		0,
		height/2,
		height/2
	);

	// top-right
	fill_n_rows_in_col(
		matrix,
		pixel_2,
		width-1,
		0,
		height/2
	);

	// bottom-right
	fill_n_rows_in_col(
		matrix,
		pixel_4,
		width-1,
		height/2,
		height/2
	);

	// gradients
	linear_gradient_left_to_right(
		matrix,
		height,
		0,
		width-1
	);

	linear_gradient_top_to_bottom(
		matrix,
		width,
		0,
		height-1
	);

	FILE *f = fopen("./assets/c_crafted_square_gradient.bmp", "wb");

	write_bmp_file(f, width, height, matrix);

	fclose(f);
	free_matrix(matrix, height);
}
