#include "bmp/bmp.h"
#include <stdlib.h>
#include <string.h>

void draw_black_square();
void draw_square_gradient();

int main() {
	draw_black_square();
	draw_square_gradient();

	return 0;
}

void draw_square_gradient() {
	bmf_windows_3_t bmf;
	memset(&bmf, 0, sizeof(bmf));

	bmfh_t *fhdr = &bmf.file_header;
	bmih_windows_3_t *ihdr = &bmf.information_header;

	fhdr->bf_type[0] = 'B';
	fhdr->bf_type[1] = 'M';

	// pixels starts immediately after the headers
	fhdr->bf_pixels_offset = sizeof(bmfh_t) + sizeof(bmih_windows_3_t);

	ihdr->bi_size = 40;
	ihdr->bi_width = 100;
	ihdr->bi_height = 100;
	ihdr->bi_planes = 1;
	ihdr->bi_bit_count = 24;

	pixel_24_bit_t **matrix = malloc_matrix(ihdr->bi_height, ihdr->bi_width);

	// draw gradient pixels
	pixel_24_bit_t pixel_1;
	pixel_24_bit_t pixel_2;
	memset(&pixel_1, 0 ,sizeof(pixel_24_bit_t));
	memset(&pixel_2, 0 ,sizeof(pixel_24_bit_t));

	pixel_1.red = 255;
	pixel_2.blue = 255;

	fill_n_rows_in_col(
		matrix,
		pixel_1,
		0,
		0,
		ihdr->bi_height
	);

	fill_n_rows_in_col(
		matrix,
		pixel_2,
		ihdr->bi_width - 1,
		0,
		ihdr->bi_height
	);

	linear_gradient_left_to_right(
		matrix,
		ihdr->bi_height,
		0,
		ihdr->bi_width - 1
	);

	bmf.pixels = matrix_to_pixel_data(matrix, ihdr->bi_height, ihdr->bi_width);

	// calculate file size
	size_t bytes_per_row = round_to_next_multiple_of_4(ihdr->bi_width * sizeof(pixel_24_bit_t));
	fhdr->bf_size = sizeof(bmfh_t) + sizeof(bmih_windows_3_t) + (bytes_per_row * ihdr->bi_height);

	FILE *f = fopen("./assets/c_crafted_square_gradient.bmp", "wb");

	// write file header
	if (fwrite(fhdr, sizeof(bmfh_t), 1, f) != 1) {
		puts("error writing file");
	}

	// write info header
	if (fwrite(ihdr, sizeof(bmih_windows_3_t), 1, f) != 1) {
		puts("error writing file");
	}

	// write pixel data
	if (fwrite(bmf.pixels, bytes_per_row * ihdr->bi_height, 1, f) != 1) {
		puts("error writing file");
	}

	fclose(f);
	free(bmf.pixels);
	free_matrix(matrix, ihdr->bi_height);
}

// 10x10 black square
void draw_black_square() {
	bmf_windows_3_t bmf;
	memset(&bmf, 0, sizeof(bmf));

	bmfh_t *fhdr = &bmf.file_header;
	bmih_windows_3_t *ihdr = &bmf.information_header;

	fhdr->bf_type[0] = 'B';
	fhdr->bf_type[1] = 'M';

	// pixels starts immediately after the headers
	fhdr->bf_pixels_offset = sizeof(bmfh_t) + sizeof(bmih_windows_3_t);

	ihdr->bi_size = 40;
	ihdr->bi_width = 10;
	ihdr->bi_height = 10;
	ihdr->bi_planes = 1;
	ihdr->bi_bit_count = 24;

	pixel_24_bit_t **matrix = malloc_matrix(ihdr->bi_height, ihdr->bi_width);

	// fill with black pixel
	pixel_24_bit_t pixel;
	memset(&pixel, 0 ,sizeof(pixel_24_bit_t));
	matrix_fill(matrix, pixel, ihdr->bi_height, ihdr->bi_width);

	bmf.pixels = matrix_to_pixel_data(matrix, ihdr->bi_height, ihdr->bi_width);

	// calculate file size
	size_t bytes_per_row = round_to_next_multiple_of_4(ihdr->bi_width * sizeof(pixel_24_bit_t));
	fhdr->bf_size = sizeof(bmfh_t) + sizeof(bmih_windows_3_t) + (bytes_per_row * ihdr->bi_height);

	FILE *f = fopen("./assets/10_x_10_black_square.bmp", "wb");

	// write file header
	if (fwrite(fhdr, sizeof(bmfh_t), 1, f) != 1) {
		puts("error writing file");
	}

	// write info header
	if (fwrite(ihdr, sizeof(bmih_windows_3_t), 1, f) != 1) {
		puts("error writing file");
	}

	// write pixel data
	if (fwrite(bmf.pixels, bytes_per_row * ihdr->bi_height, 1, f) != 1) {
		puts("error writing file");
	}

	fclose(f);
	free(bmf.pixels);
	free_matrix(matrix, ihdr->bi_height);
}
