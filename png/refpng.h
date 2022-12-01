#ifndef REFPNG_H
#define REFPNG_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <png.h>
#include <stdint.h>

extern int width, height;
extern png_byte color_type;
extern png_byte bit_depth;
extern png_bytep *row_pointers;

void read_png_file(const char *filename);
void write_png_file(const char *filename);

void copy_row_pointers_to_vector(std::vector<uint8_t>& image, png_bytep *row_pointers, int width, int height);
png_bytep* copy_vector_to_row_pointers(std::vector<uint8_t>& image, int width, int height);

#endif // REFPNG_H
