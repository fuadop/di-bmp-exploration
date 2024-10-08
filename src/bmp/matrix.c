#include "bmp.h"
#include <string.h>
#include <stdlib.h>

// returns pixel_24_bit_t[height][width]
pixel_24_bit_t** pixel_data_to_matrix_bmf_os_2(bmf_os_2_t *ptr) {
	if (ptr->information_header.bi_bit_count != 24) return NULL;

	size_t bytes_per_row = round_to_next_multiple_of_4(
		ptr->information_header.bi_width * sizeof(pixel_24_bit_t)
	);

	pixel_24_bit_t **matrix = malloc_matrix(
		ptr->information_header.bi_height,
		ptr->information_header.bi_width
	);

	for (uint16_t row = 0; row < ptr->information_header.bi_height; row++) {
		size_t row_offset = bytes_per_row * row;

		for (uint16_t col = 0; col < ptr->information_header.bi_width; col++) {
			size_t pixel_offset = row_offset + (col * 3);

			pixel_24_bit_t *pixel = &matrix[row][col];

			pixel->blue = ptr->pixels[pixel_offset];
			pixel->green = ptr->pixels[pixel_offset+1];
			pixel->red = ptr->pixels[pixel_offset+2];
		}
	}

	matrix_reflect_x_axis(matrix, ptr->information_header.bi_height);

	return matrix;
}

// returns pixel_24_bit_t[height][width]
// free(ret) must be called
pixel_24_bit_t** pixel_data_to_matrix_bmf_windows_3(bmf_windows_3_t *ptr) {
	uint32_t bi_width = ptr->information_header.bi_width;
	uint32_t bi_height = ptr->information_header.bi_height;
	uint16_t bi_bit_count = ptr->information_header.bi_bit_count;
	uint32_t bi_compression = ptr->information_header.bi_compression;

	if (bi_bit_count == 8 || (bi_bit_count == 4 && bi_compression == 2)) {
		pixel_24_bit_t **matrix = malloc_matrix(bi_height, bi_width);

		for (uint32_t y = 0; y < bi_height; y++) {
			for (uint32_t x = 0; x < bi_width; x++) {
				pixel_24_bit_t *pixel = &matrix[y][x];

				uint8_t clr_index = ptr->pixels[(bi_width * y) + x];

				if (ptr->color_table != NULL) {
					bmf_rgbquad_t *clr = &ptr->color_table[clr_index];

					pixel->red = clr->red;
					pixel->blue = clr->blue;
					pixel->green = clr->green;

					continue;
				}

				pixel->red = pixel->blue = pixel->green = clr_index;
			}
		}

		matrix_reflect_x_axis(matrix, bi_height);

		return matrix;
	}

	if (bi_bit_count != 24) return NULL;
	if (bi_compression != 0) return NULL;

	size_t bytes_per_row = round_to_next_multiple_of_4(
		bi_width * sizeof(pixel_24_bit_t)
	);

	pixel_24_bit_t **matrix = malloc_matrix(bi_height, bi_width);

	for (uint32_t row = 0; row < bi_height; row++) {
		size_t row_offset = bytes_per_row * row;

		for (uint32_t col = 0; col < bi_width; col++) {
			size_t pixel_offset = row_offset + (col * 3);

			pixel_24_bit_t *pixel = &matrix[row][col];

			pixel->blue = ptr->pixels[pixel_offset];
			pixel->green = ptr->pixels[pixel_offset+1];
			pixel->red = ptr->pixels[pixel_offset+2];
		}
	}

	matrix_reflect_x_axis(matrix, bi_height);

	return matrix;
}

// free(pixels) must be called
uint8_t* matrix_to_pixel_data(pixel_24_bit_t **matrix, uint16_t height, uint16_t width) {
	matrix_reflect_x_axis(matrix, height);

	size_t bytes_per_row = round_to_next_multiple_of_4(width * sizeof(pixel_24_bit_t));

	uint8_t *pixels = malloc(bytes_per_row * height);
	memset(pixels, 0, bytes_per_row * height);

	for (uint16_t row = 0; row < height; row++) {
		size_t row_offset = bytes_per_row * row;

		for (uint16_t col = 0; col < width; col++) {
			size_t col_offset = row_offset + (col * 3);
			pixel_24_bit_t *pixel = &matrix[row][col];

			pixels[col_offset] = pixel->blue;
			pixels[col_offset+1] = pixel->green;
			pixels[col_offset+2] = pixel->red;
		}
	}

	return pixels;
}

void matrix_reflect_x_axis(pixel_24_bit_t **matrix, uint16_t height) {
	uint16_t a = 0;
	uint16_t b = height - 1;
	pixel_24_bit_t *bucket = NULL;

	while (a < b) {
		bucket = matrix[a];

		matrix[a] = matrix[b];
		matrix[b] = bucket;

		// move inwards
		a++;
		b--;
	}
}

void matrix_reflect_y_axis(pixel_24_bit_t **matrix, uint16_t height, uint16_t width) {
	for (uint16_t y = 0; y < height; y++) {
		pixel_24_bit_t *row = matrix[y];

		uint16_t a = 0;
		uint16_t b = width - 1;

		pixel_24_bit_t bucket;
		memset(&bucket, 0, sizeof(bucket));


		while (a < b) {
			bucket = row[a];

			row[a] = row[b];
			row[b] = bucket;

			// move inwards
			a++;
			b--;
		}
	}
}

pixel_24_bit_t** matrix_rotate_forward(
	pixel_24_bit_t **matrix,
	uint16_t height,
	uint16_t width
) {
	uint16_t new_width = height;
	uint16_t new_height = width;

	pixel_24_bit_t **new_matrix = malloc_matrix(new_height, new_width);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			new_matrix[x][height-y-1] = matrix[y][x];
		}
	}

	free_matrix(matrix, height); // release the old matrix

	return new_matrix;
}

pixel_24_bit_t** matrix_rotate_backward(
	pixel_24_bit_t **matrix,
	uint16_t height,
	uint16_t width
) {
	// same as 3 * matrix_rotate_forward
	pixel_24_bit_t **rt0 = matrix_rotate_forward(matrix, height, width);

	pixel_24_bit_t **rt1 = matrix_rotate_forward(rt0, width, height);

	return matrix_rotate_forward(
		rt1,
		height,
		width
	);
}

pixel_24_bit_t** matrix_extract_single_channel(
	pixel_24_bit_t **matrix,
	uint16_t height,
	uint16_t width,
	uint8_t channel
) {
	pixel_24_bit_t **new_matrix = malloc_matrix(height, width);

	for (uint16_t y = 0; y < height; y++) {
		for (uint16_t x = 0; x < width; x++) {
			pixel_24_bit_t pixel = matrix[y][x];

			if (channel == CHANNEL_RED) pixel.blue = pixel.green = pixel.red;
			if (channel == CHANNEL_BLUE) pixel.green = pixel.red = pixel.blue;
			if (channel == CHANNEL_GREEN) pixel.blue = pixel.red = pixel.green;

			new_matrix[y][x] = pixel;
		}
	}

	return new_matrix;
}

// free_matrix() must be called
pixel_24_bit_t** matrix_convert_to_grayscale(
	pixel_24_bit_t **matrix,
	uint16_t height,
	uint16_t width
) {
	pixel_24_bit_t **new_matrix = malloc_matrix(height, width);

	for (uint16_t y = 0; y < height; y++) {
		for (uint16_t x = 0; x < width; x++) {
			pixel_24_bit_t pixel = matrix[y][x];

			uint8_t grayscale = (pixel.red + pixel.blue + pixel.green) / 3;

			pixel.red = pixel.blue = pixel.green = grayscale;

			new_matrix[y][x] = pixel;
		}
	}

	free_matrix(matrix, height);

	return new_matrix;
}

void free_matrix(pixel_24_bit_t **matrix, uint16_t height) {
	for (uint16_t i = 0; i < height; i++) {
		free(matrix[i]);
	}

	free(matrix);
}

void matrix_fill(pixel_24_bit_t **matrix, pixel_24_bit_t pixel, uint16_t height, uint16_t width) {
	for (uint16_t y = 0; y < height; y++) {
		for (uint16_t x = 0; x < width; x++) {
			matrix[y][x] = pixel;
		}
	}
}

pixel_24_bit_t** malloc_matrix(uint16_t height, uint16_t width) {
	pixel_24_bit_t** matrix = malloc(height * sizeof(pixel_24_bit_t*));

	for (uint16_t i = 0; i < height; i++) {
		matrix[i] = malloc(width * sizeof(pixel_24_bit_t));
	}

	return matrix;
}
