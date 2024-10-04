#include "bmp/bmp.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv) { 
	if (argc < 2) {
		puts("input file path is required");

		return 1;
	}

	char* fpth = argv[1];
	FILE *fptr = fopen(fpth, "rb");

	if (is_bmp_file(fptr) == false) {
		puts("input file is not a bmp file");

		return 1;
	}

	uint32_t bi_size = get_bmp_bi_size(fptr);

	// IBM os/2 version
	if (bi_size == 12) {
		bmf_os_2_t bmf;

		memset(&bmf, 0, sizeof(bmf));
		decode_to_bmf_os_2(&bmf, fptr);

		terminal_write_headers_bmf_os_2(&bmf);
		terminal_print_bmf_os_2(&bmf);

		free(bmf.pixels);
	}

	// Windows 3 version
	if (bi_size == 40) {
		bmf_windows_3_t bmf;

		memset(&bmf, 0, sizeof(bmf));
		decode_to_bmf_windows_3(&bmf, fptr);

		terminal_write_headers_bmf_windows_3(&bmf);
		terminal_print_bmf_windows_3(&bmf);

		free(bmf.pixels);
	}

	fclose(fptr);
	
	return 0; 
}
