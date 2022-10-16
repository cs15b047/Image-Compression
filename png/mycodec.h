#ifndef MYCODEC_H
#define MYCODEC_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::string compress_image(std::vector<uint8_t>& image, int width, int height);
std::vector<uint8_t> decompress_image(std::string filename);

// RLE encoding
void rle_encode(std::vector<uint8_t> image, int width, int height, std::string filename);
std::vector<uint8_t> rle_decode(std::string filename);
void write_encoded_image(std::vector<std::pair<uint8_t, short int>>& encoded_image, int width, int height, std::string filename);

// Bucket encoding : Bucket intensities and replace intensities with bucket indices
void bucket_encode(std::vector<uint8_t> image, int width, int height, std::string filename);
std::vector<uint8_t> bucket_decode(std::string filename);

#endif // MYCODEC_H