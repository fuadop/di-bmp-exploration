#include "bmp.h"
#include <limits.h>

#define DEFAULT_FRACTION 0.5

pixel_24_bit_t rgblerp(pixel_24_bit_t a, pixel_24_bit_t b, double fraction) {
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

void fill_n_cols_in_row(
	pixel_24_bit_t **matrix,
	pixel_24_bit_t pixel,
	uint16_t row,
	uint16_t offset,
	uint16_t n
) {
	for (uint16_t i = offset; i < (offset + n); i++) {
		matrix[row][i] = pixel;
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

		matrix[i][midcol] = rgblerp(pixel_a, pixel_b, DEFAULT_FRACTION);
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

void linear_gradient_top_to_bottom(
	pixel_24_bit_t **matrix,
	uint16_t width,
	uint16_t startrow,
	uint16_t endrow
) {
	if (startrow >= endrow) return;

	uint16_t midrow = (endrow - startrow) / 2;

	if (midrow == 0) return;
	if (startrow != 0) midrow += startrow; // offset bottom division

	for (uint16_t i = 0; i < width; i++) {
		pixel_24_bit_t pixel_a = matrix[startrow][i];
		pixel_24_bit_t pixel_b = matrix[endrow][i];

		matrix[midrow][i] = rgblerp(pixel_a, pixel_b, DEFAULT_FRACTION);
	}

	linear_gradient_top_to_bottom(
		matrix,
		width,
		startrow,
		midrow
	);

	linear_gradient_top_to_bottom(
		matrix,
		width,
		midrow,
		endrow
	);
}

void fill_coordinates(
	pixel_24_bit_t **matrix,
	coordinate_t* coords,
	uint16_t coordslen,
	pixel_24_bit_t pixel
) {
	coordinate_t c;

	for (uint16_t i = 0; i < coordslen; i++) {
		c = coords[i];

		/*printf("x=%d y=%d\n", c.x, c.y);*/
		matrix[c.y][c.x] = pixel;
	}
	/*puts("end");*/
}

void scanline_polygon_fill(
	pixel_24_bit_t **matrix,
	uint16_t height,
	coordinate_t* polygon,
	uint16_t polygonlen,
	pixel_24_bit_t pixel
) {
	for (uint16_t y = 0; y < height; y++) {
		uint16_t x_low = UINT16_MAX;
		uint16_t x_high = 0;

		for (uint16_t i = 0; i < polygonlen; i++) {
			coordinate_t *p = &polygon[i];
			if (p->y == y) {
				if (p->x < x_low) x_low = p->x;
				if (p->x > x_high) x_high = p->x;
			}
		}

		if (x_low >= x_high) continue;

		for (uint16_t x = x_low+1; x < x_high; x++) {
			matrix[y][x] = pixel;
		}
	}
}

