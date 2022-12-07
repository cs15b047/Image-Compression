#ifndef MYCODEC_H
#define MYCODEC_H

#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <fftw3.h>
#include <stdint.h>
#include <bitset>
#include <omp.h>

std::string compress_image(std::vector<uint8_t>& image, int width, int height, std::string filename);
std::vector<uint8_t> decompress_image(std::string filename);

// RLE encoding
template <typename T>
std::vector<std::pair<T, short int>> rle_encode(std::vector<T>& image);
template <typename T>
std::vector<T> rle_decode(std::vector<std::pair<T, short int>>& encoded_image);
void write_encoded_image(std::vector<std::pair<uint8_t, short int>>& encoded_image, int width, int height, std::string filename);

// Delta encoding
template <typename T>
std::vector<T> delta_encode(std::vector<T>& image);
template <typename T>
std::vector<T> delta_decode(std::vector<T>& encoded_image);

// Bucket encoding : Bucket intensities and replace intensities with bucket indices
std::vector<uint8_t> bucket_rgb(std::vector<uint8_t>& image, int width, int height, std::string filename);
std::vector<uint8_t> restore_bucket(std::vector<uint8_t>& encoded_image);
void bucket_encode(std::vector<uint8_t> image, int width, int height, std::string filename);
std::vector<uint8_t> bucket_decode(std::string filename);
// Bucket encoding for YCbCr
std::vector<uint8_t> bucket_ycbcr(std::vector<uint8_t>& image, int width, int height);
std::vector<uint8_t> restore_bucket_ycbcr(std::vector<uint8_t>& image);


// Color transform: RGB to YCbCr
std::vector<uint8_t> rgb2ycbcr(std::vector<uint8_t>& image);
std::vector<uint8_t> ycbcr2rgb(std::vector<uint8_t>& image);

// DCT
std::vector<float> apply_dct(std::vector<uint8_t>& image_, int width, int height);
std::vector<uint8_t> apply_idct(std::vector<float>& image_, int width, int height);

#endif // MYCODEC_H