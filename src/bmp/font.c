#include "bmp.h"
#include <stdlib.h>
#include <string.h>

bool is_psf1_file(FILE *f) {
	fseek(f, 0, SEEK_SET);

	uint8_t bf_type[2];
	uint8_t expected_bytes[2] = { 0x36, 0x04 };

	if (fread(bf_type, 2, 1, f) == 1) {
		if (memcmp(bf_type, expected_bytes, sizeof(expected_bytes)) == 0) {
			return true;
		}
	}

	return false;
}

// free(ptr->raw_glyphs) must be called
void decode_to_psf1(psf_t *ptr, FILE *f) {
	fseek(f, 0, SEEK_SET);

	fread(&ptr->header, sizeof(psfh_t), 1, f);

	uint16_t glyphs_count = (ptr->header.font_mode & PSF1_MASK_MODE512) == 0x01
		? 512 : 256;

	uint8_t bytes_per_glyph = ptr->header.glyph_size;

	size_t total_bytes = glyphs_count *  bytes_per_glyph;

	uint8_t *raw_glyph = malloc(total_bytes);
	memset(raw_glyph, 0,total_bytes);

	if (fread(raw_glyph, total_bytes, 1, f) != 1) {
		puts("error reading from psf file");
	}

	ptr->raw_glyphs = raw_glyph;
}

coordinate_t* write_glyph_to_coordinate(
	coordinate_t c,
	uint8_t glyph_offset,
	psf_t *psf
) {
	uint8_t *glyph_ptr = &psf->raw_glyphs[glyph_offset * psf->header.glyph_size];

	coordinate_t *coords = malloc(sizeof(coordinate_t) * (8 * psf->header.glyph_size));

	size_t index = 0;
	coordinate_t p = c;

	// read all rows of glyph
	for (uint8_t y = 0; y < psf->header.glyph_size; y++) {
		uint8_t row[8];
		split_bits(glyph_ptr[y], row); // every byte in row is a pixel

		p.y++;
		p.x = c.x; // carriage return

		for (uint8_t x = 0; x < 8; x++) {
			p.x++;

			if (row[x] == 1) {
				coords[index] = p;

				index++;
			}
		}
	}

	return coords;
}

void split_bits(uint8_t byte, uint8_t buf[8]) {
	for (uint8_t i = 8; i > 0; i--) {
		buf[i-1] = (byte >> (8 - i)) & 0x01;
	}
}
