#include "refpng.h"

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;

using namespace std;

void read_png_file(const char *filename) {
  FILE *fp = fopen(filename, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  width      = png_get_image_width(png, info);
  height     = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(bit_depth == 16)
    png_set_strip_16(png);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  // if(png_get_valid(png, info, PNG_INFO_tRNS))
  //   png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  // if(color_type == PNG_COLOR_TYPE_RGB ||
  //    color_type == PNG_COLOR_TYPE_GRAY ||
  //    color_type == PNG_COLOR_TYPE_PALETTE)
  //   png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  if (row_pointers) abort();

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for(int y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, row_pointers);
  fclose(fp);
  png_destroy_read_struct(&png, &info, NULL);
}

void write_png_file(const char *filename) {
  int y;
  FILE *fp = fopen(filename, "wb");
  if(!fp) abort();
  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    width, height,
    8,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);

  if (!row_pointers) abort();

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(int y = 0; y < height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);

  png_destroy_write_struct(&png, &info);
}

void copy_row_pointers_to_vector(vector<uint8_t>& image, png_bytep *row_pointers, int width, int height) {
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width * 3; x++) {
            image.push_back(row_pointers[y][x]);
        }
    }
}

png_bytep* allocate_image_memory(int width, int height) {
    png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(width * 3); // assuming 3 channels
    }
    return row_pointers;
}

png_bytep* copy_vector_to_row_pointers(vector<uint8_t>& image, int width, int height) {
    png_bytep* row_pointers = allocate_image_memory(width, height);
    int index = 0;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width * 3; x++) {
            row_pointers[y][x] = image[index];
            index++;
        }
    }
    return row_pointers;
}