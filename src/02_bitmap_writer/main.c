#include "bmp/bmp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void draw_black_bitmap(uint16_t width, uint16_t height);
void draw_gradient_bitmap(uint16_t width, uint16_t height);
void draw_brazil_flag_bitmap(uint16_t width, uint16_t height);
void draw_poland_flag_bitmap(uint16_t width, uint16_t height);
void draw_ireland_flag_bitmap(uint16_t width, uint16_t height);

int main() {
	draw_black_bitmap(10, 10);
	draw_gradient_bitmap(300, 300);
	draw_brazil_flag_bitmap(320, 200);
	draw_poland_flag_bitmap(320, 200);
	draw_ireland_flag_bitmap(320, 200);

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

void draw_poland_flag_bitmap(uint16_t width, uint16_t height) {
	pixel_24_bit_t **matrix = malloc_matrix(height, width);

	pixel_24_bit_t pixel;
	memset(&pixel, 255, sizeof(pixel_24_bit_t)); // white color

	for (uint16_t row = 0; row < height; row++) {
		if (row >= (height/2)) {
			pixel.red = 255;
			pixel.blue = 0;
			pixel.green = 0;
		}

		fill_n_cols_in_row(
			matrix,
			pixel,
			row,
			0,
			width
		);
	}

	FILE *f = fopen("./assets/c_crafted_poland_flag.bmp", "wb");

	write_bmp_file(f, width, height, matrix);

	fclose(f);
	free_matrix(matrix, height);
}

void draw_ireland_flag_bitmap(uint16_t width, uint16_t height) {
	pixel_24_bit_t **matrix = malloc_matrix(height, width);

	pixel_24_bit_t pixel;
	memset(&pixel, 0, sizeof(pixel_24_bit_t));

	uint16_t section_1_end = (width / 3);
	uint16_t section_2_end = section_1_end * 2;

	for (uint16_t col = 0; col < width; col++) {
		if (col <= section_1_end) {
			pixel.red = 0;
			pixel.green = 0x9a;
			pixel.blue = 0x49;
		} else if (col <= section_2_end) {
			pixel.red = 0xff;
			pixel.green = 0xff;
			pixel.blue = 0xff;
		} else {
			pixel.red = 0xff;
			pixel.green = 0x79;
			pixel.blue = 0x00;
		}

		fill_n_rows_in_col(
			matrix,
			pixel,
			col,
			0,
			height
		);
	}

	FILE *f = fopen("./assets/c_crafted_ireland_flag.bmp", "wb");

	write_bmp_file(f, width, height, matrix);

	fclose(f);
	free_matrix(matrix, height);
}

void draw_brazil_flag_bitmap(uint16_t width, uint16_t height) {
	pixel_24_bit_t **matrix = malloc_matrix(height, width);

	/*pixel_24_bit_t red_pixel = {.red=0xff, .green=0x00, .blue=0x00};*/
	pixel_24_bit_t blue_pixel = {.red=0x01, .green=0x21, .blue=0x69};
	pixel_24_bit_t green_pixel = {.red=0x00, .green=0x95, .blue=0x39};
	pixel_24_bit_t white_pixel = {.red=0xff, .green=0xff, .blue=0xff};
	pixel_24_bit_t yellow_pixel = {.red=0xfe, .green=0xdd, .blue=0x00};

	matrix_fill(matrix, green_pixel, height, width); // background

	// todo: manipulate matrix
	coordinate_t vertex_a = {.x= width/2, .y=0};
	coordinate_t vertex_c = {.x= width/2, .y=height-1};

	coordinate_t vertex_b_left = {.x= 0, .y=height/2};
	coordinate_t vertex_b_right = {.x= width-1, .y=height/2};

	// add 10% padding to edge vertexes
	double h_padding = (10.0/100.0) * width;
	double v_padding = (10.0/100.0) * height;

	vertex_a.y += v_padding;
	vertex_c.y -= v_padding;

	vertex_b_left.x += h_padding;
	vertex_b_right.x -= h_padding;

	// first triangle
	{
		coordinate_t *triangle = draw_triangle(
			vertex_a,
			vertex_c,
			vertex_b_left
		);

		uint16_t perimiter = triangle_perimiter(vertex_a, vertex_c, vertex_b_left);

		fill_coordinates(
			matrix,
			triangle,
			perimiter,
			yellow_pixel
		);

		// fill triangle
		scanline_polygon_fill(
			matrix,
			height,
			triangle,
			perimiter,
			yellow_pixel
		);

		// after triangle draw and fill
		free(triangle);
	}

	// second triangle
	{
		coordinate_t *triangle = draw_triangle(
			vertex_a,
			vertex_c,
			vertex_b_right
		);

		uint16_t perimiter = triangle_perimiter(vertex_a, vertex_c, vertex_b_right);

		fill_coordinates(
			matrix,
			triangle,
			perimiter,
			yellow_pixel
		);

		// fill triangle
		scanline_polygon_fill(
			matrix,
			height,
			triangle,
			perimiter,
			yellow_pixel
		);

		// after triangle draw and fill
		free(triangle);
	}

	// draw globe
	{
		// 35% radius kind of
		uint16_t radius = round(((35.0 / 100) * width) / 2);
		coordinate_t centre = {.x = width/2, .y=height/2};

		uint16_t circumference = circle_circumference(radius);
		coordinate_t *circle = draw_circle_bresenham(centre, radius);

		fill_coordinates(
			matrix,
			circle,
			circumference,
			blue_pixel
		);

		scanline_polygon_fill(
			matrix,
			height,
			circle,
			circumference,
			blue_pixel
		);

		// draw curve (globe band)
		uint16_t height = 15;
		for (uint16_t i = 0; i < height; i++) {
			coordinate_t p0 = {.x=centre.x-radius+1, .y=centre.y+i};
			coordinate_t p1 = {.x=centre.x, .y=centre.y-radius+20+i};
			coordinate_t p2 = {.x=centre.x+radius-1, .y=centre.y+i};

			coordinate_t *curve = quadbezier(p0, p1, p2);
			fill_coordinates(
				matrix,
				curve,
				triangle_perimiter(p0, p1, p2),
				white_pixel
			);

			free(curve);
		}

		// write text on band
		FILE *f = fopen("./fonts/zap-vga09.psf", "rb");
		if (is_psf1_file(f)) {
			psf_t psf;

			decode_to_psf1(&psf, f);

			char *content = "ORDEM E PROGRESSO";
			coordinate_t c = { .x=centre.x-radius-9, .y=centre.y+8};

			for (uint16_t i = 0; i < strlen(content); i++) {
				if (content[i] == ' ') {
					c.x += 4;
					/*c.y = i < 6 ? c.y - 4 : c.y + 2;*/
					continue;
				}

				coordinate_t *glyph = write_glyph_to_coordinate(
					c,
					content[i],
					&psf
				);

				fill_coordinates(
					matrix,
					glyph,
					(8 * psf.header.glyph_size),
					green_pixel
				);

				c.x += 8;
				c.y = i < 7 ? c.y - 4 : c.y + 2;
			}

			free(psf.raw_glyphs);
		}

		fclose(f);

		free(circle);

		matrix[0][0] = green_pixel;
	}

	FILE *f = fopen("./assets/c_crafted_brazil_flag.bmp", "wb");

	write_bmp_file(f, width, height, matrix);

	fclose(f);
	free_matrix(matrix, height);
}
