#ifndef PSF_H
#define PSF_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#pragma pack(1)

#define PSF1_MASK_MODE512 0x01
#define PSF1_MASK_MODEHASTAB 0x02

typedef struct {
	uint8_t signature[2]; // magic bytes (36 04)
	uint8_t font_mode; // bit mask about file
	uint8_t glyph_size; // Glyph size in bytes (storage size)
} psfh_t;

typedef struct {
	psfh_t header; // header
	uint8_t* raw_glyphs;

	// ignoring unicode table
} psf_t;

bool is_psf1_file(FILE *f);
void decode_to_psf1(psf_t *ptr, FILE *f);

void split_bits(uint8_t byte, uint8_t buf[8]);

#endif
