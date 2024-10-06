#include "bmp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define Y_DIRECTION_UP 0
#define Y_DIRECTION_DOWN 1

#define X_DIRECTION_LEFT 0
#define X_DIRECTION_RIGHT 1

uint16_t linelen(coordinate_t a, coordinate_t b) {
	uint16_t dx =  (uint16_t)abs((int)b.x - (int)a.x) + 1;
	uint16_t dy =  (uint16_t)abs((int)b.y - (int)a.y) + 1;

	return dx > dy ? dx : dy;
}

uint16_t circle_circumference(uint16_t r) {
	return round(2 * r * M_PI);
}

uint16_t triangle_perimiter(coordinate_t a, coordinate_t b, coordinate_t c) {
	return linelen(a, b) + linelen(a, c) + linelen(b, c);
}

uint8_t get_x_direction(coordinate_t a, coordinate_t b) {
	if (b.x > a.x) return X_DIRECTION_RIGHT;

	return X_DIRECTION_LEFT;
}

uint8_t get_y_direction(coordinate_t a, coordinate_t b) {
	if (b.y > a.y) return Y_DIRECTION_DOWN;

	return Y_DIRECTION_UP;
}

// free() must be called
coordinate_t* draw_line(coordinate_t a, coordinate_t b) {
	uint8_t x_dir = get_x_direction(a, b);
	uint8_t y_dir = get_y_direction(a, b);

	int dx = (int)b.x - (int)a.x;
	int dy = (int)b.y - (int)a.y;

	double m = dx == 0
		? INFINITY
		: fabs((double)dy/dx);

	coordinate_t bucket;
	coordinate_t *coords = malloc(sizeof(coordinate_t) * linelen(a, b));

	// horizontal line
	if (m == 0) {
		if (x_dir == X_DIRECTION_LEFT) {
			// recurse
			free(coords);
			return draw_line(b, a);
		}

		if (x_dir == X_DIRECTION_RIGHT) {
			// logic
			bucket.y = a.y;

			for (uint16_t x = a.x; x <= b.x; x++) {
				bucket.x = x;
				coords[x - a.x] = bucket;
			}

			return coords;
		}
	}

	// vertical line
	if (m == INFINITY) {
		if (y_dir == Y_DIRECTION_UP) {
			// recurse
			free(coords);
			return draw_line(b, a);
		}

		if (y_dir == Y_DIRECTION_DOWN) {
			// logic
			bucket.x = a.x;

			for (uint16_t y = a.y; y <= b.y; y++) {
				bucket.y = y;
				coords[y - a.y] = bucket;
			}

			return coords;
		}
	}

	// diagonal line
	if (m == 1) {
		if (y_dir == Y_DIRECTION_UP) {
			// recurse
			free(coords);
			return draw_line(b, a);
		}

		if (y_dir == Y_DIRECTION_DOWN) {
			// logic

			if (x_dir == X_DIRECTION_LEFT) {
				// logic
				for (uint16_t x = b.x; x <= a.x; x++) {
					bucket.x = a.x + b.x - x;
					bucket.y = a.y + (x-b.x);
					coords[x-b.x] = bucket;
				}

				return coords;
			}

			if (x_dir == X_DIRECTION_RIGHT) {
				// logic
				for (uint16_t x = a.x; x <= b.x; x++) {
					bucket.x = x;
					bucket.y = a.y + (x-a.x);
					coords[x - a.x] = bucket;
				}

				return coords;
			}
		}
	}

	// DDA case 1
	if (m < 1) {
		if (y_dir == Y_DIRECTION_UP) {
			// recurse
			free(coords);
			return draw_line(b, a);
		}

		if (y_dir == Y_DIRECTION_DOWN) {
			if (x_dir == X_DIRECTION_LEFT) {
				// logic

				double y = a.y;

				for (uint16_t x = b.x; x <= a.x; x++) {
					y += m;

					bucket.x = a.x + b.x - x;
					bucket.y = round(y);

					if (bucket.y > b.y) {
						bucket.x = a.x;
						bucket.y = a.y;
					}

					coords[x - b.x] = bucket;
				}

				return coords;
			}

			if (x_dir == X_DIRECTION_RIGHT) {
				// logic

				double y = a.y;

				for (uint16_t x = a.x; x <= b.x; x++) {
					y += m;

					bucket.x = x;
					bucket.y = round(y);

					if (bucket.y > b.y) {
						bucket.x = b.x;
						bucket.y = b.y;
					}

					coords[x - a.x] = bucket;
				}

				return coords;
			}
		}
	}

	// DDA case 2
	if (m > 1) {
		if (y_dir == Y_DIRECTION_UP) {
			// recurse
			free(coords);
			return draw_line(b, a);
		}

		if (y_dir == Y_DIRECTION_DOWN) {
			if (x_dir == X_DIRECTION_LEFT) {
				// logic

				double x = b.x;

				for (uint16_t y = a.y; y <= b.y; y++) {
					x += 1/m;

					bucket.y = b.y + a.y - y;
					bucket.x = round(x);

					if (bucket.x > a.x) {
						bucket.x = a.x;
						bucket.y = a.y;
					}

					coords[y - a.y] = bucket;
				}

				return coords;
			}

			if (x_dir == X_DIRECTION_RIGHT) {
				// logic

				double x = a.x;

				for (uint16_t y = a.y; y <= b.y; y++) {
					x += 1/m;

					bucket.y = y;
					bucket.x = round(x);

					if (bucket.x > b.x) {
						bucket.x = b.x;
						bucket.y = b.y;
					}

					coords[y - a.y] = bucket;
				}

				return coords;
			}
		}
	}

	// DDA line drawing algorithm
	// if m  = 0 (horizontal line) -> draw horizontal line
	// if m = (xx/0) [vertical line] -> draw vertical line
	// if abs(m) = 1 [diagonal line] -> draw perfectly diagonal line

	// if abs(m) < 1 -> DDA case 1
	// if abs(m) > 1 -> DDA case 2

	// https://medium.com/geekculture/dda-line-drawing-algorithm-be9f069921cf
	// https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)

	return coords;
};

// free() must be called
coordinate_t* draw_triangle(coordinate_t a, coordinate_t b, coordinate_t c) {
	uint16_t len_side_1 = linelen(a, b);
	uint16_t len_side_2 = linelen(a, c);
	uint16_t len_side_3 = linelen(b, c);

	coordinate_t *coords = malloc(
		sizeof(coordinate_t) *
			(len_side_1 + len_side_2 + len_side_3)
	);

	coordinate_t *coords_side_1 = draw_line(a, b);
	coordinate_t *coords_side_2 = draw_line(a, c);
	coordinate_t *coords_side_3 = draw_line(b, c);

	uint16_t offset = 0;
	for (uint16_t i = 0; i < len_side_1; i++) {
		coords[offset + i] = coords_side_1[i];
	}

	offset += len_side_1;
	for (uint16_t i = 0; i < len_side_2; i++) {
		coords[offset + i] = coords_side_2[i];
	}

	offset += len_side_2;
	for (uint16_t i = 0; i < len_side_3; i++) {
		coords[offset + i] = coords_side_3[i];
	}

	free(coords_side_1);
	free(coords_side_2);
	free(coords_side_3);

	return coords;
}

// free() must be called
// https://medium.com/@pushpendrajtp99/bresenhams-circle-drawing-algorithm-in-computer-graphics-1f75349649ed
coordinate_t * draw_circle_bresenham(coordinate_t c, uint16_t r) {
	coordinate_t *coords = malloc(sizeof(coordinate_t) * circle_circumference(r));
	memset(coords, 0, sizeof(coordinate_t) * circle_circumference(r));

	int d = 1-r;
	int x_factor = r;
	int y_factor = 0;

	size_t index = 0;
	coordinate_t bucket;

	while (x_factor >= y_factor) {
		// update all eight octets
		bucket.x = c.x + x_factor;
		bucket.y = c.y + y_factor;
		coords[index] = bucket;
		index++;

		bucket.x = c.x - x_factor;
		bucket.y = c.y + y_factor;
		coords[index] = bucket;
		index++;

		bucket.x = c.x + x_factor;
		bucket.y = c.y - y_factor;
		coords[index] = bucket;
		index++;

		bucket.x = c.x - x_factor;
		bucket.y = c.y - y_factor;
		coords[index] = bucket;
		index++;

		bucket.x = c.x + y_factor;
		bucket.y = c.y + x_factor;
		coords[index] = bucket;
		index++;

		bucket.x = c.x - y_factor;
		bucket.y = c.y + x_factor;
		coords[index] = bucket;
		index++;

		bucket.x = c.x + y_factor;
		bucket.y = c.y - x_factor;
		coords[index] = bucket;
		index++;

		bucket.x = c.x - y_factor;
		bucket.y = c.y - x_factor;
		coords[index] = bucket;
		index++;

		if (d > 0) {
			x_factor--;
			d -= (2 * x_factor) + 1;
		}

		if (d <= 0) {
			y_factor++;
			d += (2 * y_factor) + 1;
		}
	}

	return coords;
}

// free() must be called
// ! sadly it looks like an ellipse
coordinate_t * draw_circle(coordinate_t c, uint16_t r) {
	// circle eqn.: (x-h)^2 + (y-k)^2 = r^2
	// x will always increment/decrement, so let's make y subject of formula
	//
	// y = +/- sqrt(r^2 - (x-h)^2 + k^2) --(1)
	//
	// where (h,k) is the centre of the circle
	// r is the radius of the circle

	coordinate_t *coords = malloc(sizeof(coordinate_t) * circle_circumference(r));
	memset(coords, 0, sizeof(coordinate_t) * circle_circumference(r));

	uint16_t x_left_offset = c.x - r;
	uint16_t x_right_offset = c.x + r;

	size_t index = 0;
	coordinate_t bucket;

	for (uint16_t x = x_left_offset; x < x_right_offset; x++) {
		uint16_t y = round(
			sqrt(pow(r, 2) - pow(x-c.x, 2) + pow(c.y, 2))
		);

		// bottom quadrant
		bucket.x = x;
		bucket.y = y;
		coords[index] = bucket;

		index++;

		// reflect point over the mirror line
		bucket.y = c.y - (y - c.y);
		coords[index] = bucket;

		index++;
	}

	return coords;
}

coordinate_t* quadbezier(coordinate_t a, coordinate_t b, coordinate_t c) {
	coordinate_t *coords = malloc(
		sizeof(coordinate_t) * triangle_perimiter(a, b, c)
	);

	uint16_t index = 0;

	for (double t = 0; t <= 1; t += 0.005) {
		coordinate_t l0 = clerp(a, b, t);
		coordinate_t l1 = clerp(b, c, t);

		coordinate_t q0 = clerp(l0, l1, t);

		coords[index] = q0;

		index++;
	}

	return coords;
}

coordinate_t clerp(coordinate_t a, coordinate_t b, double t) {
	coordinate_t c;

	c.x = a.x + round((double)(b.x - a.x) * t);
	c.y = a.y + round((double)(b.y - a.y) * t);

	return c;
}

