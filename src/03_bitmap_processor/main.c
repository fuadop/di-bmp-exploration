#include "bmp/bmp.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		puts("input file is required");
		return 1;
	}

	char *action = argv[2];
	char *input_file_path = argv[1];

	FILE *inf = fopen(input_file_path, "rb");
	FILE *outf = fopen("./assets/c_bmp_process_output.bmp", "wb");

	if (is_bmp_file(inf) == false) {
		puts("input file is not a bmp file");

		return 1;
	}

	uint32_t bi_size = get_bmp_bi_size(inf);

	// os/2 DIB
	if (bi_size == 12) {
		bmf_os_2_t bmf;

		memset(&bmf, 0, sizeof(bmf));
		decode_to_bmf_os_2(&bmf, inf);

		uint32_t w = bmf.information_header.bi_width;
		uint32_t h = bmf.information_header.bi_height;
		pixel_24_bit_t **matrix = pixel_data_to_matrix_bmf_os_2(&bmf);

		free(bmf.pixels);

		if (strcmp(action, "rotate-forward") == 0) {
			matrix = matrix_rotate_forward(
				matrix,
				bmf.information_header.bi_height,
				bmf.information_header.bi_width
			);

			bmf.information_header.bi_width = h;
			bmf.information_header.bi_height = w;
		}

		if (strcmp(action, "rotate-backward") == 0) {
			matrix = matrix_rotate_backward(
				matrix,
				bmf.information_header.bi_height,
				bmf.information_header.bi_width
			);

			bmf.information_header.bi_width = h;
			bmf.information_header.bi_height = w;
		}

		write_bmp_file(
			outf,
			bmf.information_header.bi_width,
			bmf.information_header.bi_height,
			matrix
		);

		free_matrix(matrix, h);
	}

	// windows 3 DIB
	if (bi_size == 40) {
		bmf_windows_3_t bmf;

		memset(&bmf, 0, sizeof(bmf));
		decode_to_bmf_windows_3(&bmf, inf);

		uint32_t w = bmf.information_header.bi_width;
		uint32_t h = bmf.information_header.bi_height;
		pixel_24_bit_t **matrix = pixel_data_to_matrix_bmf_windows_3(&bmf);

		free(bmf.pixels);

		if (strcmp(action, "rotate-forward") == 0) {
			matrix = matrix_rotate_forward(
				matrix,
				bmf.information_header.bi_height,
				bmf.information_header.bi_width
			);

			bmf.information_header.bi_width = h;
			bmf.information_header.bi_height = w;
		}

		if (strcmp(action, "rotate-backward") == 0) {
			matrix = matrix_rotate_backward(
				matrix,
				bmf.information_header.bi_height,
				bmf.information_header.bi_width
			);

			bmf.information_header.bi_width = h;
			bmf.information_header.bi_height = w;
		}

		write_bmp_file(
			outf,
			bmf.information_header.bi_width,
			bmf.information_header.bi_height,
			matrix
		);

		free_matrix(matrix, h);
	}

	fclose(inf);
	fclose(outf);

	return 0;
}
