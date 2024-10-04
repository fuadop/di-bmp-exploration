#include "bmp.h"

#define DEFAULT_FRACTION 0.5

pixel_24_bit_t interpolate(pixel_24_bit_t a, pixel_24_bit_t b, double fraction) {
	pixel_24_bit_t c;

	c.red = a.red + (b.red - a.red) * fraction;
	c.blue = a.blue + (b.blue - a.blue) * fraction;
	c.green = a.green + (b.green - a.green) * fraction;

	return c;
}

void fill_n_rows_in_col(
	pixel_24_bit_t **matrix,
	pixel_24_bit_t pixel,
	uint16_t col,
	uint16_t offset,
	uint16_t n
) {
	for (uint16_t i = offset; i < (offset + n); i++) {
		matrix[i][col] = pixel;
	}
}

void linear_gradient_left_to_right(
	pixel_24_bit_t **matrix,
	uint16_t height,
	uint16_t startcol,
	uint16_t endcol
) {
	if (startcol >= endcol) return;

	uint16_t midcol = (endcol - startcol) / 2;

	if (midcol == 0) return;
	if (startcol != 0) midcol += startcol; // offset right division

	for (uint16_t i = 0; i < height; i++) {
		pixel_24_bit_t pixel_a = matrix[i][startcol];
		pixel_24_bit_t pixel_b = matrix[i][endcol];

		matrix[i][midcol] = interpolate(pixel_a, pixel_b, DEFAULT_FRACTION);
	}

	linear_gradient_left_to_right(
		matrix,
		height,
		startcol,
		midcol
	);

	linear_gradient_left_to_right(
		matrix,
		height,
		midcol,
		endcol
	);
}

